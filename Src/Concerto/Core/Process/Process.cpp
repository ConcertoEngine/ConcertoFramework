#include "Concerto/Core/Process/Process.hpp"

namespace cct
{
	std::string_view ToString(ProcessOutcome outcome) noexcept
	{
		using namespace std::string_view_literals;

		switch (outcome)
		{
			case ProcessOutcome::Exited:
				return "Exited"sv;
			case ProcessOutcome::Crashed:
				return "Crashed"sv;
			case ProcessOutcome::TimedOut:
				return "TimedOut"sv;
			case ProcessOutcome::SpawnFailed:
				return "SpawnFailed"sv;
		}

		return "Unknown"sv;
	}
} // namespace cct
