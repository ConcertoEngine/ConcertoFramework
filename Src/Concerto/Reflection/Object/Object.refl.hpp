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

#define CCT_OBJECT(className)                      \
public:                                            \
	static const cct::refl::Class* GetClass()      \
	{                                              \
		return m_class;                            \
	}                                              \
                                                   \
private:                                           \
	inline static const cct::refl::Class* m_class; \
	friend class Internal##className##Class

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
	};

	class CCT_REFL_CLASS() CCT_REFLECTION_API Int8 : public cct::refl::Object
	{
	public:
		virtual ~Int8() = default;
		CCT_OBJECT(Int8);
	};

	class CCT_REFL_CLASS() CCT_REFLECTION_API Int16 : public cct::refl::Object
	{
	public:
		virtual ~Int16() = default;
		CCT_OBJECT(Int16);
	};

#define CCT_STRINGIFY(x) #x
#define CCT_QUOTE(x) "\"" CCT_STRINGIFY(x) "\""

	class CCT_REFL_CLASS() CCT_REFLECTION_API Int32 : public cct::refl::Object
	{
	public:
		Int32() :
			m_value(0)
		{
			m_dynamicClass = m_class;
		}
		Int32(cct::Int32 value) :
			m_value(value)
		{
		}
		~Int32() override = default;

		bool operator==(const Int32& other) const
		{
			return other.m_value == m_value;
		}

		bool operator!=(const Int32& other) const
		{
			return !(other == m_value);
		}

		CCT_OBJECT(Int32);

	protected:
		cct::Int32 m_value;
	};

	class CCT_REFL_CLASS() CCT_REFLECTION_API Int64 : public cct::refl::Object
	{
	public:
		virtual ~Int64() = default;
		CCT_OBJECT(Int64);
	};

	class CCT_REFL_CLASS() CCT_REFLECTION_API ClampedInt32 : public cct::refl::Int32
	{
	public:
		~ClampedInt32() override = default;
		CCT_OBJECT(ClampedInt32);
		void SetMin(cct::Int32 min)
		{
			m_min = min;
			Set(m_value);
		}
		void SetMax(cct::Int32 max)
		{
			m_max = max;
			Set(m_value);
		}

		void Set(cct::Int32 value)
		{
			if (!m_registry)
			{
				//				OnValueChangeEvent(*this, ChangeType::Set);
				m_value = value;
				return;
			}
			// m_registry->NotifyPreChange(*this, ChangeType::Set, CCT_STRINGIFY(m_value));
			// m_value = value;
			// m_registry->NotifyPostChange(*this, ChangeType::Set, CCT_STRINGIFY(m_value));
		}

		[[nodiscard]] cct::Int32 Get() const
		{
			return m_value;
		}

	private:
		// CCT_REFL_METHOD("ChangedEvent = " CCT_QUOTE(m_value))
		// void OnValueChangeEvent(const Object& oldValue, EnumFlags<ChangeType> changeType)
		//{
		//	m_value = std::max(std::min(m_min, m_value), m_max);
		// }
		cct::Int32 m_min;
		cct::Int32 m_max;
	};
} // namespace cct::refl

#include "Concerto/Reflection/Object/Object.inl"
#endif // CONCERTO_REFLECTION_OBJECT_HPP