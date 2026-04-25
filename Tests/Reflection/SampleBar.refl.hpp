#pragma once
#include <string>

#include <Concerto/Core/FunctionRef/FunctionRef.hpp>
#include <Concerto/Core/Types/Types.hpp>
#include <Concerto/Reflection/Defines.hpp>
#include <Concerto/Reflection/Object/Object.refl.hpp>

#include "Defines.hpp"

struct CCT_PACKAGE("version = \"1.0\"", "description = \"Sample Package description\"", "serialize = [\"JSON\", \"YML\"]") ConcertoReflectionTests
{
};

namespace cct::sample
{
	enum class CCT_REFL_TESTS_ENUM() SampleEnum : cct::UInt32
	{
		Foo = 0,
		Bar = 1
	};

	class CCT_REFL_TESTS_CLASS("Test=\"test\"") SampleBar : public cct::refl::Object
	{
	public:
		SampleBar() :
			m_bar(42),
			m_nativeType(42)
		{
		}
		CCT_REFL_TESTS_METHOD("Test=\"test\"")
		int* Foo(const cct::refl::Int32& bar)
		{
			m_bar = bar;
			return nullptr;
		}

		CCT_REFL_TESTS_METHOD("Test=\"test\"")
		cct::refl::Int32 Bar(const cct::refl::Int32& bar1, const cct::refl::Int32& bar2, const cct::refl::Int32& bar3)
		{
			return 42;
		}

		CCT_REFL_TESTS_METHOD("Delegate = \"m_customDelegatePtr\"")
		cct::refl::Int32 CustomDelegate();

		CCT_OBJECT(SampleBar);

	private:
		CCT_REFL_TESTS_MEMBER("Test=\"test\"")
		cct::refl::Int32 m_bar;

		CCT_REFL_TESTS_NATIVE_MEMBER()
		cct::Int32 m_nativeType;

		cct::FunctionRef<cct::refl::Int32()> m_customDelegatePtr = []()
		{
			return refl::Int32();
		};
	};

	// Templated class with actual generic members
	template<typename K, typename V>
	class CCT_REFL_TESTS_CLASS() TemplatedPair : public cct::refl::Object
	{
	public:
		TemplatedPair() :
			m_key(),
			m_value()
		{
		}

		CCT_REFL_TESTS_NATIVE_MEMBER()
		K m_key;

		CCT_REFL_TESTS_NATIVE_MEMBER()
		V m_value;

		CCT_REFL_TESTS_METHOD()
		void Set(K k, V v)
		{
			m_key = k;
			m_value = v;
		}

		CCT_REFL_TESTS_METHOD()
		K GetKey() const
		{
			return m_key;
		}

		CCT_REFL_TESTS_METHOD()
		V GetValue() const
		{
			return m_value;
		}

		CCT_OBJECT(TemplatedPair);
	};

	template class TemplatedPair<int, float>;
	template class TemplatedPair<std::string, std::string>;

	// Generic class with multiple type parameters (runtime parameterized)
	class CCT_REFL_TESTS_CLASS() CCT_GENERIC_CLASS() GenericPair : public cct::refl::Object
	{
	public:
		GenericPair() :
			m_keyType(nullptr),
			m_valueType(nullptr)
		{
		}

		CCT_REFL_TESTS_MEMBER()
		CCT_REFL_TESTS_GENERIC_TYPE()
		const cct::refl::Class* m_keyType;

		CCT_REFL_TESTS_MEMBER()
		CCT_REFL_TESTS_GENERIC_TYPE()
		const cct::refl::Class* m_valueType;

		CCT_REFL_TESTS_METHOD()
		const cct::refl::Class* GetKeyType() const
		{
			return m_keyType;
		}

		CCT_REFL_TESTS_METHOD()
		const cct::refl::Class* GetValueType() const
		{
			return m_valueType;
		}

		CCT_OBJECT(GenericPair);
	};
} // namespace cct::sample
