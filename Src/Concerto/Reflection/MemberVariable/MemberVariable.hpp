//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_MEMBERVARIABLE_HPP
#define CONCERTO_REFLECTION_MEMBERVARIABLE_HPP

#include <string>
#include <string_view>
#include <unordered_map>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Class;
	class CCT_REFLECTION_API MemberVariable
	{
	public:
		MemberVariable(std::string name, const Class* type, std::size_t index);
		~MemberVariable() = default;

		MemberVariable(const MemberVariable&) = delete;
		MemberVariable(MemberVariable&&) = default;

		MemberVariable& operator=(const MemberVariable&) = delete;
		MemberVariable& operator=(MemberVariable&&) = default;

		[[nodiscard]] inline std::string_view GetName() const;
		[[nodiscard]] inline std::size_t GetIndex() const;
		[[nodiscard]] inline const Class* GetType() const;

		[[nodiscard]] bool HasAttribute(std::string_view name) const;
		[[nodiscard]] std::string_view GetAttribute(std::string_view name) const;
		void AddAttribute(std::string name, std::string value);
		[[nodiscard]] const std::unordered_map<std::string, std::string>& GetAttributes() const;

	private:
		std::string m_name;
		std::size_t m_index;
		const Class* m_type;
		std::unordered_map<std::string, std::string> m_attributes;
	};

	class CCT_REFLECTION_API NativeMemberVariable
	{
	public:
		NativeMemberVariable(std::string name, cct::UInt64 typeId, std::size_t index);
		~NativeMemberVariable() = default;

		NativeMemberVariable(const NativeMemberVariable&) = delete;
		NativeMemberVariable(NativeMemberVariable&&) = default;

		NativeMemberVariable& operator=(const NativeMemberVariable&) = delete;
		NativeMemberVariable& operator=(NativeMemberVariable&&) = default;

		[[nodiscard]] inline std::string_view GetName() const;
		[[nodiscard]] inline std::size_t GetIndex() const;
		[[nodiscard]] inline cct::UInt64 GetTypeId() const;

		[[nodiscard]] bool HasAttribute(std::string_view name) const;
		[[nodiscard]] std::string_view GetAttribute(std::string_view name) const;
		void AddAttribute(std::string name, std::string value);
		[[nodiscard]] const std::unordered_map<std::string, std::string>& GetAttributes() const;

	private:
		std::string m_name;
		std::size_t m_index;
		cct::UInt64 m_typeId;
		std::unordered_map<std::string, std::string> m_attributes;
	};
} // namespace cct::refl

#include "Concerto/Reflection/MemberVariable/MemberVariable.inl"

#endif // CONCERTO_REFLECTION_MEMBERVARIABLE_HPP