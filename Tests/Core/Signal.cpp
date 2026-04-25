//
// Created by arthur on 27/02/2026.
//

#define CATCH_CONFIG_RUNNER
#include <catch2/catch_test_macros.hpp>

#include <Concerto/Core/Signal/Connection.hpp>
#include <Concerto/Core/Signal/Signal.hpp>

// Helper: tracks how many times a slot was called and captures the last value
struct CallTracker
{
	int callCount = 0;
	std::string lastValue;

	void OnChanged() { ++callCount; }
	void OnStringChanged(const std::string& value)
	{
		++callCount;
		lastValue = value;
	}
};

SCENARIO("Signal - basic connectivity")
{
	GIVEN("A Signal<> with no connections")
	{
		cct::Signal<> signal;

		THEN("It has no connections")
		{
			CHECK(signal.GetConnectionCount() == 0);
			CHECK_FALSE(signal.HasConnections());
		}

		WHEN("A lambda is connected")
		{
			int callCount = 0;
			auto conn = signal.Connect([&callCount]() { ++callCount; });

			THEN("GetConnectionCount reflects it")
			{
				CHECK(signal.GetConnectionCount() == 1);
				CHECK(signal.HasConnections());
				CHECK(conn.IsConnected());
			}

			AND_WHEN("The signal is emitted")
			{
				signal.Emit();
				THEN("The lambda is called once") { CHECK(callCount == 1); }
			}

			AND_WHEN("The signal is emitted via operator()")
			{
				signal();
				THEN("The lambda is called once") { CHECK(callCount == 1); }
			}

			AND_WHEN("The connection is manually disconnected")
			{
				conn.Disconnect();
				signal.Emit();
				THEN("The lambda is NOT called and connection is marked disconnected")
				{
					CHECK(callCount == 0);
					CHECK_FALSE(conn.IsConnected());
					CHECK(signal.GetConnectionCount() == 0);
				}
			}

			AND_WHEN("DisconnectAll() is called")
			{
				signal.DisconnectAll();
				signal.Emit();
				THEN("The lambda is NOT called")
				{
					CHECK(callCount == 0);
					CHECK(signal.GetConnectionCount() == 0);
				}
			}
		}

		WHEN("Multiple lambdas are connected")
		{
			int a = 0, b = 0, c = 0;
			auto connA = signal.Connect([&a]() { ++a; });
			auto connB = signal.Connect([&b]() { ++b; });
			auto connC = signal.Connect([&c]() { ++c; });

			signal.Emit();
			THEN("All three are called")
			{
				CHECK(a == 1);
				CHECK(b == 1);
				CHECK(c == 1);
				CHECK(signal.GetConnectionCount() == 3);
			}

			connB.Disconnect();
			signal.Emit();
			THEN("Only A and C are called after B is disconnected")
			{
				CHECK(a == 2);
				CHECK(b == 1);
				CHECK(c == 2);
				CHECK(signal.GetConnectionCount() == 2);
			}
		}
	}
}

SCENARIO("Signal - member function Connect(obj, &Class::Method)")
{
	GIVEN("A Signal<> and a CallTracker instance")
	{
		cct::Signal<> signal;
		CallTracker tracker;

		WHEN("Connected via (obj, method) two-argument form")
		{
			auto conn = signal.Connect(&tracker, &CallTracker::OnChanged);

			THEN("The method is called on Emit()")
			{
				signal.Emit();
				CHECK(tracker.callCount == 1);
				signal.Emit();
				CHECK(tracker.callCount == 2);
			}

			AND_WHEN("Disconnected")
			{
				conn.Disconnect();
				signal.Emit();
				THEN("The method is NOT called") { CHECK(tracker.callCount == 0); }
			}
		}
	}

	GIVEN("A Signal<const std::string&> and a CallTracker")
	{
		cct::Signal<const std::string&> signal;
		CallTracker tracker;

		WHEN("Connected via (obj, method) two-argument form")
		{
			auto conn = signal.Connect(&tracker, &CallTracker::OnStringChanged);
			signal.Emit("hello");

			THEN("The method receives the correct argument")
			{
				CHECK(tracker.callCount == 1);
				CHECK(tracker.lastValue == "hello");
			}
		}
	}
}

SCENARIO("Signal - ScopedConnection RAII")
{
	GIVEN("A Signal<> and a call counter")
	{
		cct::Signal<> signal;
		int callCount = 0;

		WHEN("A ScopedConnection goes out of scope")
		{
			{
				cct::ScopedConnection sc{signal.Connect([&callCount]() { ++callCount; })};
				CHECK(sc.IsConnected());
				signal.Emit();
				CHECK(callCount == 1);
			} // auto-disconnect

			signal.Emit();
			THEN("The lambda is NOT called after scope exit")
			{
				CHECK(callCount == 1);
				CHECK(signal.GetConnectionCount() == 0);
			}
		}

		WHEN("A ScopedConnection is moved")
		{
			cct::ScopedConnection sc1{signal.Connect([&callCount]() { ++callCount; })};
			cct::ScopedConnection sc2 = std::move(sc1);

			CHECK_FALSE(sc1.IsConnected());
			CHECK(sc2.IsConnected());

			signal.Emit();
			THEN("Only the moved-into connection fires") { CHECK(callCount == 1); }
		}

		WHEN("Release() is called on a ScopedConnection")
		{
			cct::Connection conn;
			{
				cct::ScopedConnection sc{signal.Connect([&callCount]() { ++callCount; })};
				conn = sc.Release();
				CHECK_FALSE(sc.IsConnected());
				CHECK(conn.IsConnected());
			} // no auto-disconnect

			signal.Emit();
			THEN("The slot is still active after scope exit") { CHECK(callCount == 1); }

			conn.Disconnect();
			signal.Emit();
			THEN("After manual disconnect, slot is gone") { CHECK(callCount == 1); }
		}
	}
}

SCENARIO("Signal - reentrancy: disconnect inside Emit()")
{
	GIVEN("A Signal<> with a slot that disconnects itself during emission")
	{
		cct::Signal<> signal;
		int callCount = 0;
		cct::Connection selfConn;

		selfConn = signal.Connect([&]()
								  {
			++callCount;
			selfConn.Disconnect();
		});
		int bCount = 0;
		signal.Connect([&bCount]() { ++bCount; });

		signal.Emit();
		THEN("No crash, self-disconnected slot called once, other slot called")
		{
			CHECK(callCount == 1);
			CHECK(bCount == 1);
			CHECK(signal.GetConnectionCount() == 1);
		}

		signal.Emit();
		THEN("After second emit, only b fires")
		{
			CHECK(callCount == 1);
			CHECK(bCount == 2);
		}
	}
}

SCENARIO("Signal - lifetime safety: Signal destroyed before Connection")
{
	GIVEN("A Connection that outlives its Signal")
	{
		cct::Connection conn;
		{
			cct::Signal<> signal;
			int callCount = 0;
			conn = signal.Connect([&callCount]() { ++callCount; });
			CHECK(conn.IsConnected());
		} // signal destroyed here

		THEN("IsConnected() returns false and Disconnect() is a no-op")
		{
			CHECK_FALSE(conn.IsConnected());
			conn.Disconnect(); // must not crash
		}
	}
}

SCENARIO("Signal - copy and move semantics")
{
	GIVEN("A Signal<> with one connection")
	{
		int callCount = 0;
		cct::Signal<> original;
		auto conn = original.Connect([&callCount]() { ++callCount; });

		WHEN("The Signal is copied")
		{
			cct::Signal<> copy = original;
			copy.Emit();
			THEN("The copy has NO connections (fresh)")
			{
				CHECK(copy.GetConnectionCount() == 0);
				CHECK(callCount == 0);
			}

			original.Emit();
			THEN("The original still fires its connection") { CHECK(callCount == 1); }
		}

		WHEN("The Signal is moved")
		{
			cct::Signal<> moved = std::move(original);
			moved.Emit();
			THEN("The moved signal fires the original connection")
			{
				CHECK(callCount == 1);
				CHECK(conn.IsConnected());
			}
		}
	}
}
