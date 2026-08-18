#ifndef CONCERTO_REFLECTION_HTTP_HTTPDISPATCH_HPP
#define CONCERTO_REFLECTION_HTTP_HTTPDISPATCH_HPP

#include <string>
#include <string_view>
#include <unordered_map>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Object;
}

namespace cct::refl::http
{
	struct Request
	{
		std::string_view method;
		std::string_view path;
		std::string_view query;
		std::string_view body;
	};

	struct Response
	{
		int status = 404;
		std::string body;
	};

	CCT_REFLECTION_API std::string UrlDecode(std::string_view in);
	CCT_REFLECTION_API bool MatchRoute(std::string_view pattern, std::string_view path,
									   std::unordered_map<std::string, std::string>& caps);
	CCT_REFLECTION_API void ApplyField(Object& obj, std::string_view name, std::string_view value);
	CCT_REFLECTION_API void ApplyQuery(Object& obj, std::string_view query);
	CCT_REFLECTION_API void ApplyCaptures(Object& obj, const std::unordered_map<std::string, std::string>& caps);
	CCT_REFLECTION_API void ApplyJsonBody(Object& obj, std::string_view body);
} // namespace cct::refl::http

#endif // CONCERTO_REFLECTION_HTTP_HTTPDISPATCH_HPP
