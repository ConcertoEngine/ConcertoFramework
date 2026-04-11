//
// Created by arthur on 24/12/2023.
//

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <Concerto/Core/TypeInfo/TypeInfo.hpp>

enum class EnumClass{};
enum struct EnumStruct{};
class Class{};
struct Struct{};

namespace CCT_ANONYMOUS_NAMESPACE
{
	SCENARIO("TypeInfo - TypeName")
	{
		GIVEN("Various types")
		{
			THEN("TypeName<int> returns \"int\"")
			{
				constexpr auto typeName = cct::TypeName<int>();
				REQUIRE(typeName == "int");
			}

			THEN("TypeName<std::shared_ptr<float>> returns \"std::shared_ptr<float>\"")
			{
				constexpr auto typeName = cct::TypeName<std::shared_ptr<float>>();
				REQUIRE(typeName == "std::shared_ptr<float>");
			}

			THEN("TypeName<EnumClass> returns \"EnumClass\"")
			{
				constexpr auto typeName = cct::TypeName<EnumClass>();
				REQUIRE(typeName == "EnumClass");
			}

			THEN("TypeName<EnumStruct> returns \"EnumStruct\"")
			{
				constexpr auto typeName = cct::TypeName<EnumStruct>();
				REQUIRE(typeName == "EnumStruct");
			}

			THEN("TypeName<Class> returns \"Class\"")
			{
				constexpr auto typeName = cct::TypeName<Class>();
				REQUIRE(typeName == "Class");
			}

			THEN("TypeName<Struct> returns \"Struct\"")
			{
				constexpr auto typeName = cct::TypeName<Struct>();
				REQUIRE(typeName == "Struct");
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
