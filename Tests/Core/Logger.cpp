//
// Created by arthur on 02/03/2024.
//

#include <catch2/catch_test_macros.hpp>

#include <Concerto/Core/Logger/Logger.hpp>

using namespace cct;

namespace CCT_ANONYMOUS_NAMESPACE
{
	class ScoppedCoutRedirector
	{
	public:
		ScoppedCoutRedirector() : _old(nullptr)
		{
			std::cout.clear();
			_old = std::cout.rdbuf(_buffer.rdbuf());
		}

		~ScoppedCoutRedirector()
		{
			std::cout.rdbuf(_old);
		}

		const std::string& Str() const
		{
			return _buffer.str();
		}
	private:
		const std::ostringstream _buffer;
		std::streambuf* _old;
	};

	//SCENARIO("Logger - Debug output")
	//{
	//	GIVEN("A redirected cout")
	//	{
	//		ScoppedCoutRedirector redirector;
	//		Logger::Debug("Test string {}", 25);
	//
	//		auto currentLocation = std::source_location::current();
	//		const std::string content = Terminal::Color::CYAN + std::string(currentLocation.function_name()) + ":" + std::to_string(currentLocation.line()) + " message: Test string 25" + Terminal::Color::DEFAULT;
	//		THEN("The output matches") { CHECK(redirector.Str() == "Test string 25"); }
	//	}
	//}
} // namespace CCT_ANONYMOUS_NAMESPACE
