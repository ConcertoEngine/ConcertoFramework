//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_OBJECT_HPP
#define CONCERTO_REFLECTION_OBJECT_HPP

#include <memory>
#include <string_view>

#include <Concerto/Core/EnumFlags/EnumFlags.hpp>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Core/Signal/Signal.hpp"

#define CCT_OBJECT(className)                                                           \
public:                                                                                 \
	static const cct::refl::Class* GetClass()                                           \
	{                                                                                   \
		return m_class;                                                                 \
	}                                                                                   \
                                                                                        \
private:                                                                                \
	inline static const cct::refl::Class* m_class;                                      \
	friend class Internal##className##Class;                                            \
	friend class Internal##className##GenericClass;                                     \
	int PrivateReflInitClass##className = (this->InitReflection(m_class), 0)

struct CCT_REFL_PACKAGE("version = \"1.0.0\"", "description = \"Concerto Reflection Standard Package\"") ConcertoReflection
{
};

namespace cct::refl
{
	enum class ChangeType
	{
		PreChange,
		PostChange,
		Set,
		Insert,
		Delete
	};
}

CCT_ENABLE_ENUM_FLAGS(cct::refl::ChangeType)

namespace cct::refl
{
	class Class;
	class Registry;

	class CCT_REFL_CLASS() CCT_REFLECTION_API Object
	{
	public:
		Object();
		virtual ~Object() = default;

		/// Emitted whenever the value of this object changes.
		/// Connect with a lambda or a member function pointer:
		///   obj.OnValueChanged.Connect([](){ ... });
		///   obj.OnValueChanged.Connect(this, &MyClass::OnChanged);
		Signal<> OnValueChanged;

		Object(const Object& other);
		Object(Object&& other) noexcept;

		Object& operator=(const Object& other);
		Object& operator=(Object&& other) noexcept;

		static inline bool IsInstanceOf(const Class& klass);

		[[nodiscard]] inline const cct::refl::Object* GetMemberVariable(std::size_t index) const;
		[[nodiscard]] inline cct::refl::Object* GetMemberVariable(std::size_t index);

		[[nodiscard]] inline const cct::refl::Object* GetMemberVariable(std::string_view name) const;
		[[nodiscard]] inline cct::refl::Object* GetMemberVariable(std::string_view name);

		template<typename T>
		[[nodiscard]] T* GetMemberVariable(std::size_t index);
		template<typename T>
		[[nodiscard]] const T* GetMemberVariable(std::size_t index) const;

		template<typename T>
		[[nodiscard]] T* GetMemberVariable(std::string_view name);
		template<typename T>
		[[nodiscard]] const T* GetMemberVariable(std::string_view name) const;

		template<typename T>
		[[nodiscard]] T* GetNativeMemberVariable(std::size_t index);
		template<typename T>
		[[nodiscard]] const T* GetNativeMemberVariable(std::size_t index) const;

		template<typename T>
		[[nodiscard]] T* GetNativeMemberVariable(std::string_view name);
		template<typename T>
		[[nodiscard]] const T* GetNativeMemberVariable(std::string_view name) const;

		[[nodiscard]] inline const cct::refl::Class* GetDynamicClass() const;
		inline void SetDynamicClass(const Class* klass);

		[[nodiscard]] bool HasRegistry() const;
		[[nodiscard]] Registry* GetRegistry();
		[[nodiscard]] const Registry* GetRegistry() const;

		inline void InitializeMemberVariables();

		CCT_OBJECT(Object);

	protected:
		const cct::refl::Class* m_dynamicClass;
		Registry* m_registry;

		/* 
		* Default member initializer, evaluated by the compiler for every constructor of
		* className (including make_shared<T>, new T, etc.), in member declaration order.
		* Because CCT_OBJECT is placed last in the class body, this runs after all other
		* members are initialized but before the constructor body. In an inheritance chain
		* each class level calls InitReflection with its own m_class, so the most-derived
		* call wins and m_dynamicClass ends up set to the concrete type.
		*/
		inline void InitReflection(const Class* cls) noexcept;
	};
} // namespace cct::refl

#include "Concerto/Reflection/Object/Object.inl"
#endif // CONCERTO_REFLECTION_OBJECT_HPP