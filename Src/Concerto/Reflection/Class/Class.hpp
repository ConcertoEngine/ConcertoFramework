//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_CLASS_HPP
#define CONCERTO_REFLECTION_CLASS_HPP

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class NativeMemberVariable;
	class Object;
	class Namespace;
	class MemberVariable;
	class Method;

	class CCT_REFLECTION_API Class
	{
	public:
		Class(Namespace* nameSpace, std::string name, const Class* baseClass);
		virtual ~Class() = default;

		Class(const Class&) = delete;
		Class(Class&&) = default;

		Class& operator=(const Class&) = delete;
		Class& operator=(Class&&) = default;

		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] std::string_view GetNamespaceName() const;
		[[nodiscard]] const Namespace& GetNamespace() const;
		[[nodiscard]] std::size_t GetHash() const;

		[[nodiscard]] std::size_t GetMemberVariableCount() const;
		[[nodiscard]] std::size_t GetNativeMemberVariableCount() const;
		[[nodiscard]] std::size_t GetMethodCount() const;

		[[nodiscard]] const Class* GetBaseClass() const;

		[[nodiscard]] std::span<std::unique_ptr<MemberVariable>> GetMemberVariables();
		[[nodiscard]] std::span<const std::unique_ptr<MemberVariable>> GetMemberVariables() const;

		[[nodiscard]] std::span<std::unique_ptr<NativeMemberVariable>> GetNativeMemberVariables();
		[[nodiscard]] std::span<const std::unique_ptr<NativeMemberVariable>> GetNativeMemberVariables() const;

		[[nodiscard]] std::span<std::unique_ptr<Method>> GetMethods();
		[[nodiscard]] std::span<const std::unique_ptr<Method>> GetMethods() const;

		[[nodiscard]] const MemberVariable* GetMemberVariable(std::size_t index) const;
		[[nodiscard]] const MemberVariable* GetMemberVariable(std::string_view name) const;
		[[nodiscard]] const NativeMemberVariable* GetNativeMemberVariable(std::size_t index) const;
		[[nodiscard]] const NativeMemberVariable* GetNativeMemberVariable(std::string_view name) const;

		[[nodiscard]] virtual cct::refl::Object* GetMemberVariable(std::size_t index, const cct::refl::Object& self) const = 0;
		[[nodiscard]] cct::refl::Object* GetMemberVariable(std::string_view name, const cct::refl::Object& self) const;
		[[nodiscard]] virtual void* GetNativeMemberVariable(std::size_t index, const cct::refl::Object& self) const = 0;
		[[nodiscard]] void* GetNativeMemberVariable(std::string_view name, const cct::refl::Object& self) const;

		[[nodiscard]] const Method* GetMethod(std::size_t index) const;
		[[nodiscard]] const Method* GetMethod(std::string_view name) const;

		[[nodiscard]] bool HasMemberVariable(std::string_view name) const;
		[[nodiscard]] bool HasMethod(std::string_view name) const;

		[[nodiscard]] bool InheritsFrom(const Class& other) const;
		[[nodiscard]] bool InheritsFrom(std::string_view name) const;

		bool HasAttribute(std::string_view attribute) const;
		std::string_view GetAttribute(std::string_view attribute);

		bool operator==(const Class& other) const;
		bool operator!=(const Class& other) const;

		virtual std::unique_ptr<cct::refl::Object> CreateDefaultObject() const = 0;

		template<typename T>
			requires(std::is_base_of_v<cct::refl::Object, T> && std::is_polymorphic_v<T>)
		std::unique_ptr<T> CreateDefaultObject() const;

		[[nodiscard]] virtual bool IsTemplateClass() const;

		[[nodiscard]] virtual bool IsGenericClass() const
		{
			return false;
		}

		// should be private
		virtual void Initialize() = 0;

	protected:
		void AddMemberVariable(std::string_view name, const Class* type);
		void AddNativeMemberVariable(std::string_view name, UInt64 typeId);
		void AddMemberFunction(std::unique_ptr<Method> method);
		void AddAttribute(std::string name, std::string value);
		void SetNamespace(Namespace* nameSpace);
		void SetBaseClass(const Class* klass);

	private:
		std::string m_name;
		Namespace* m_namespace;
		std::vector<std::unique_ptr<MemberVariable>> m_memberVariables;
		std::vector<std::unique_ptr<NativeMemberVariable>> m_nativeMemberVariables;
		std::vector<std::unique_ptr<Method>> m_methods;
		const Class* m_baseClass;
		std::unordered_map<std::string /*name*/, std::string /*value*/> m_attributes;

		std::size_t m_hash;
	};
	CCT_REFLECTION_API const Class* GetClassByName(std::string_view nameSpaceName, std::string_view name);
	CCT_REFLECTION_API const Class* GetClassByName(std::span<std::string_view> nameSpaceNames, std::string_view name);

	/**
	 *
	 * @param name The name of the class, it can be prefixed with the namespace, GetClassByName("cct::refl::Object")
	 * @return nullptr if the class could not be found
	 */
	CCT_REFLECTION_API const Class* GetClassByName(std::string_view name);

} // namespace cct::refl

#include "Concerto/Reflection/Class/Class.inl"

#endif // CONCERTO_REFLECTION_CLASS_HPP