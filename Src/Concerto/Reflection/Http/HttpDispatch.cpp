#include "Concerto/Reflection/Http/HttpDispatch.hpp"

#include <cstdlib>
#include <vector>

#include "Concerto/Reflection/Boolean/Boolean.refl.hpp"
#include "Concerto/Reflection/Cast.hpp"
#include "Concerto/Reflection/Int32/Int32.refl.hpp"
#include "Concerto/Reflection/Json/Json.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"
#include "Concerto/Reflection/String/String.refl.hpp"
#include "Concerto/Reflection/UInt32/UInt32.refl.hpp"

namespace cct::refl::http
{
	namespace
	{
		int HexNibble(char c)
		{
			if (c >= '0' && c <= '9')
				return c - '0';
			if (c >= 'a' && c <= 'f')
				return c - 'a' + 10;
			if (c >= 'A' && c <= 'F')
				return c - 'A' + 10;
			return -1;
		}

		std::vector<std::string_view> SplitPath(std::string_view path)
		{
			std::vector<std::string_view> parts;
			std::size_t i = 0;
			while (i < path.size())
			{
				if (path[i] == '/')
				{
					++i;
					continue;
				}
				const auto end = path.find('/', i);
				parts.push_back(path.substr(i, end == std::string_view::npos ? path.size() - i : end - i));
				i = end == std::string_view::npos ? path.size() : end;
			}
			return parts;
		}
	} // namespace

	std::string UrlDecode(std::string_view in)
	{
		std::string out;
		out.reserve(in.size());
		for (std::size_t i = 0; i < in.size(); ++i)
		{
			if (in[i] == '+')
			{
				out.push_back(' ');
				continue;
			}
			if (in[i] == '%' && i + 2 < in.size())
			{
				const int hi = HexNibble(in[i + 1]);
				const int lo = HexNibble(in[i + 2]);
				if (hi >= 0 && lo >= 0)
				{
					out.push_back(static_cast<char>((hi << 4) | lo));
					i += 2;
					continue;
				}
			}
			out.push_back(in[i]);
		}
		return out;
	}

	bool MatchRoute(std::string_view pattern, std::string_view path,
					std::unordered_map<std::string, std::string>& caps)
	{
		const auto pat = SplitPath(pattern);
		const auto got = SplitPath(path);
		if (pat.size() != got.size())
			return false;
		caps.clear();
		for (std::size_t i = 0; i < pat.size(); ++i)
		{
			if (pat[i].size() >= 2 && pat[i].front() == '{' && pat[i].back() == '}')
			{
				const auto name = pat[i].substr(1, pat[i].size() - 2);
				caps.emplace(name.empty() ? "id" : std::string(name), std::string(got[i]));
				continue;
			}
			if (pat[i] != got[i])
				return false;
		}
		return true;
	}

	void ApplyField(Object& obj, std::string_view name, std::string_view value)
	{
		Object* member = obj.GetMemberVariable(name);
		if (member == nullptr)
			return;
		if (auto* s = Cast<String>(member))
		{
			s->Set(value);
			return;
		}
		if (auto* i = Cast<Int32>(member))
		{
			i->Set(static_cast<cct::Int32>(std::strtol(std::string(value).c_str(), nullptr, 10)));
			return;
		}
		if (auto* u = Cast<UInt32>(member))
		{
			u->Set(static_cast<cct::UInt32>(std::strtoul(std::string(value).c_str(), nullptr, 10)));
			return;
		}
		if (auto* b = Cast<Boolean>(member))
			b->Set(value == "true" || value == "1" || value == "yes");
	}

	void ApplyQuery(Object& obj, std::string_view query)
	{
		std::size_t start = 0;
		while (start < query.size())
		{
			const auto amp = query.find('&', start);
			const auto part = query.substr(start, amp == std::string_view::npos ? query.size() - start : amp - start);
			const auto eq = part.find('=');
			const auto k = eq == std::string_view::npos ? part : part.substr(0, eq);
			const auto v = eq == std::string_view::npos ? std::string_view{} : part.substr(eq + 1);
			if (!k.empty())
				ApplyField(obj, k, UrlDecode(v));
			if (amp == std::string_view::npos)
				break;
			start = amp + 1;
		}
	}

	void ApplyCaptures(Object& obj, const std::unordered_map<std::string, std::string>& caps)
	{
		for (const auto& [name, value] : caps)
			ApplyField(obj, name, UrlDecode(value));
	}

	void ApplyJsonBody(Object& obj, std::string_view body)
	{
		if (!body.empty())
			Json::FromJson(obj, body);
	}
} // namespace cct::refl::http
