/**
 * @file Tests/ThreadPool.cpp
 * @brief Unit tests for ThreadPool
 * @date 2025-10-31
 */

#include <atomic>
#include <chrono>
#include <future>
#include <string>
#include <thread>
#include <vector>

#include <Concerto/Core/ThreadPool/ThreadPool.hpp>

#include <catch2/catch_test_macros.hpp>

using cct::ThreadPool;
using namespace std::chrono_literals;

// ---------------------------------------------------------------------------
SCENARIO("ThreadPool - initialization")
{
	GIVEN("A default-constructed ThreadPool")
	{
		ThreadPool pool;
		THEN("Worker count is greater than 0") { CHECK(pool.GetWorkerCount() > 0u); }
	}

	GIVEN("A ThreadPool with 4 threads")
	{
		ThreadPool pool(4);
		THEN("Worker count is 4") { CHECK(pool.GetWorkerCount() == 4u); }
	}

	GIVEN("A ThreadPool with 1 thread")
	{
		ThreadPool pool(1);
		THEN("Worker count is 1") { CHECK(pool.GetWorkerCount() == 1u); }
	}
}

// ---------------------------------------------------------------------------
SCENARIO("ThreadPool - AddTask")
{
	GIVEN("A ThreadPool with 4 workers")
	{
		ThreadPool pool(4);

		WHEN("A single task is submitted via AddTask")
		{
			std::atomic<bool> executed{false};
			pool.AddTask([&executed]() { executed.store(true, std::memory_order_relaxed); });

			THEN("The task executes within timeout")
			{
				REQUIRE(pool.WaitFor(1000ms));
				CHECK(executed.load());
			}
		}

		WHEN("100 tasks are submitted via AddTask")
		{
			std::atomic<int> counter{0};
			constexpr int numTasks = 100;
			for (int i = 0; i < numTasks; ++i)
				pool.AddTask([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });

			THEN("All tasks execute within timeout")
			{
				REQUIRE(pool.WaitFor(5000ms));
				CHECK(counter.load() == numTasks);
			}
		}

		WHEN("50 tasks each add their index to a shared sum")
		{
			std::atomic<int> sum{0};
			constexpr int numTasks = 50;
			for (int i = 1; i <= numTasks; ++i)
				pool.AddTask([&sum, i]() { sum.fetch_add(i, std::memory_order_relaxed); });

			THEN("The sum is n*(n+1)/2")
			{
				REQUIRE(pool.WaitFor(5000ms));
				CHECK(sum.load() == (numTasks * (numTasks + 1)) / 2);
			}
		}
	}
}

// ---------------------------------------------------------------------------
SCENARIO("ThreadPool - Submit")
{
	GIVEN("A ThreadPool with 4 workers")
	{
		ThreadPool pool(4);

		WHEN("A task returning int 42 is submitted")
		{
			auto future = pool.Submit([]() { return 42; });
			THEN("The future returns 42")
			{
				REQUIRE(pool.WaitFor(1000ms));
				CHECK(future.get() == 42);
			}
		}

		WHEN("A task returning a string is submitted")
		{
			auto future = pool.Submit([]() { return std::string("Hello, ThreadPool!"); });
			THEN("The future returns the correct string")
			{
				REQUIRE(pool.WaitFor(1000ms));
				CHECK(future.get() == "Hello, ThreadPool!");
			}
		}

		WHEN("A task computing a sum 1..100 is submitted")
		{
			auto future = pool.Submit([]() {
				int sum = 0;
				for (int i = 1; i <= 100; ++i)
					sum += i;
				return sum;
			});
			THEN("The future returns 5050")
			{
				REQUIRE(pool.WaitFor(1000ms));
				CHECK(future.get() == 5050);
			}
		}

		WHEN("20 tasks each returning i*i are submitted")
		{
			std::vector<std::future<int>> futures;
			constexpr int numTasks = 20;
			for (int i = 0; i < numTasks; ++i)
				futures.push_back(pool.Submit([i]() { return i * i; }));

			THEN("Each future returns the correct squared value")
			{
				REQUIRE(pool.WaitFor(5000ms));
				for (int i = 0; i < numTasks; ++i)
					CHECK(futures[i].get() == i * i);
			}
		}
	}
}

// ---------------------------------------------------------------------------
SCENARIO("ThreadPool - exception handling")
{
	GIVEN("A ThreadPool with 4 workers")
	{
		ThreadPool pool(4);

		WHEN("A task throws std::runtime_error")
		{
			auto future = pool.Submit([]() -> int { throw std::runtime_error("Test exception"); });
			THEN("WaitFor succeeds and future.get() rethrows the exception")
			{
				CHECK(pool.WaitFor(1000ms));
				CHECK_THROWS_AS(future.get(), std::runtime_error);
			}
		}

		WHEN("10 tasks alternate between throwing and returning")
		{
			std::vector<std::future<int>> futures;
			for (int i = 0; i < 10; ++i)
			{
				futures.push_back(pool.Submit([i]() -> int {
					if (i % 2 == 0)
						throw std::runtime_error("Even number");
					return i;
				}));
			}

			THEN("Even futures throw, odd futures return their index")
			{
				REQUIRE(pool.WaitFor(5000ms));
				for (int i = 0; i < 10; ++i)
				{
					if (i % 2 == 0)
						CHECK_THROWS_AS(futures[i].get(), std::runtime_error);
					else
						CHECK(futures[i].get() == i);
				}
			}
		}

		WHEN("A first task throws, then a second succeeds")
		{
			auto future1 = pool.Submit([]() -> int { throw std::runtime_error("First exception"); });
			REQUIRE(pool.WaitFor(1000ms));
			CHECK_THROWS_AS(future1.get(), std::runtime_error);

			auto future2 = pool.Submit([]() { return 42; });
			THEN("The pool continues to work and returns 42")
			{
				REQUIRE(pool.WaitFor(1000ms));
				CHECK(future2.get() == 42);
			}
		}
	}
}

// ---------------------------------------------------------------------------
SCENARIO("ThreadPool - WaitFor / Wait")
{
	GIVEN("A ThreadPool with 4 workers")
	{
		ThreadPool pool(4);

		WHEN("10 tasks are submitted and WaitFor is called")
		{
			std::atomic<int> counter{0};
			for (int i = 0; i < 10; ++i)
				pool.AddTask([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });

			THEN("All tasks complete and WaitFor returns true")
			{
				REQUIRE(pool.WaitFor(5000ms));
				CHECK(counter.load() == 10);
			}
		}

		WHEN("A slow task (500ms) is submitted")
		{
			pool.AddTask([]() { std::this_thread::sleep_for(500ms); });

			THEN("WaitFor(100ms) returns false but WaitFor(1000ms) returns true")
			{
				CHECK_FALSE(pool.WaitFor(100ms));
				CHECK(pool.WaitFor(1000ms));
			}
		}

		WHEN("A slow task (500ms) is submitted and Wait() with deadlines is used")
		{
			pool.AddTask([]() { std::this_thread::sleep_for(500ms); });

			THEN("Wait with short deadline returns false; with long deadline returns true")
			{
				auto deadline1 = std::chrono::steady_clock::now() + 200ms;
				CHECK_FALSE(pool.Wait(deadline1));
				auto deadline2 = std::chrono::steady_clock::now() + 1000ms;
				CHECK(pool.Wait(deadline2));
			}
		}

		WHEN("50 tasks are submitted, WaitFor completes, then WaitFor is called again")
		{
			std::atomic<int> counter{0};
			for (int i = 0; i < 50; ++i)
				pool.AddTask([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });

			THEN("All 50 tasks execute and the second WaitFor also returns true")
			{
				REQUIRE(pool.WaitFor(5000ms));
				CHECK(counter.load() == 50);
				CHECK(pool.WaitFor(100ms));
			}
		}
	}
}

// ---------------------------------------------------------------------------
SCENARIO("ThreadPool - RequestStop")
{
	GIVEN("A ThreadPool with 4 workers that has completed all tasks")
	{
		ThreadPool pool(4);
		std::atomic<int> counter{0};
		for (int i = 0; i < 10; ++i)
			pool.AddTask([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });
		REQUIRE(pool.WaitFor(5000ms));

		WHEN("RequestStop is called")
		{
			pool.RequestStop();
			THEN("All 10 tasks have executed") { CHECK(counter.load() == 10); }
		}
	}

	GIVEN("A ThreadPool with 4 workers")
	{
		ThreadPool pool(4);

		WHEN("RequestStop is called three times")
		{
			THEN("No crash (idempotent)")
			{
				pool.RequestStop();
				pool.RequestStop();
				pool.RequestStop();
			}
		}

		WHEN("RequestStop is called before adding a task")
		{
			pool.RequestStop();
			std::atomic<int> counter{0};
			pool.AddTask([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });
			std::this_thread::sleep_for(100ms);
			THEN("The task does not execute") { CHECK(counter.load() == 0); }
		}

		WHEN("RequestStop is called, then Submit is called")
		{
			pool.RequestStop();
			auto future = pool.Submit([]() { return 42; });
			THEN("The future is valid") { CHECK(future.valid()); }
		}
	}
}

// ---------------------------------------------------------------------------
SCENARIO("ThreadPool - destruction")
{
	WHEN("A ThreadPool is destroyed after all tasks complete")
	{
		std::atomic<int> counter{0};
		{
			ThreadPool pool(4);
			for (int i = 0; i < 10; ++i)
				pool.AddTask([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });
			REQUIRE(pool.WaitFor(5000ms));
		}
		THEN("All 10 tasks executed before destruction") { CHECK(counter.load() == 10); }
	}

	WHEN("A ThreadPool is destroyed with pending tasks")
	{
		std::atomic<int> counter{0};
		{
			ThreadPool pool(4);
			for (int i = 0; i < 100; ++i)
			{
				pool.AddTask([&counter]() {
					std::this_thread::sleep_for(10ms);
					counter.fetch_add(1, std::memory_order_relaxed);
				});
			}
		}
		THEN("Counter is non-negative (some tasks may not have run)") { CHECK(counter.load() >= 0); }
	}
}

// ---------------------------------------------------------------------------
SCENARIO("ThreadPool - concurrent operations")
{
	GIVEN("A ThreadPool with 8 workers")
	{
		ThreadPool pool(8);

		WHEN("10 threads each submit 100 AddTask calls concurrently")
		{
			std::atomic<int> counter{0};
			constexpr int numThreads = 10;
			constexpr int tasksPerThread = 100;

			std::vector<std::thread> threads;
			for (int t = 0; t < numThreads; ++t)
			{
				threads.emplace_back([&pool, &counter]() {
					for (int i = 0; i < tasksPerThread; ++i)
						pool.AddTask([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });
				});
			}
			for (auto& thread : threads)
				thread.join();

			THEN("All tasks execute")
			{
				REQUIRE(pool.WaitFor(10000ms));
				CHECK(counter.load() == numThreads * tasksPerThread);
			}
		}

		WHEN("10 threads each submit 50 Submit calls concurrently")
		{
			constexpr int numThreads = 10;
			constexpr int tasksPerThread = 50;

			std::vector<std::thread> threads;
			std::vector<std::vector<std::future<int>>> allFutures(numThreads);

			for (int t = 0; t < numThreads; ++t)
			{
				threads.emplace_back([&pool, &allFutures, t]() {
					for (int i = 0; i < tasksPerThread; ++i)
						allFutures[t].push_back(pool.Submit([i]() { return i; }));
				});
			}
			for (auto& thread : threads)
				thread.join();

			THEN("Each future returns the correct value")
			{
				REQUIRE(pool.WaitFor(10000ms));
				for (int t = 0; t < numThreads; ++t)
					for (int i = 0; i < tasksPerThread; ++i)
						CHECK(allFutures[t][i].get() == i);
			}
		}

		WHEN("5 threads call WaitFor concurrently while 100 tasks run")
		{
			std::atomic<int> counter{0};
			for (int i = 0; i < 100; ++i)
			{
				pool.AddTask([&counter]() {
					std::this_thread::sleep_for(10ms);
					counter.fetch_add(1, std::memory_order_relaxed);
				});
			}

			std::vector<std::thread> threads;
			std::atomic<int> waitSuccessCount{0};
			for (int t = 0; t < 5; ++t)
			{
				threads.emplace_back([&pool, &waitSuccessCount]() {
					if (pool.WaitFor(10000ms))
						waitSuccessCount.fetch_add(1, std::memory_order_relaxed);
				});
			}
			for (auto& thread : threads)
				thread.join();

			THEN("All 100 tasks ran and all 5 WaitFor calls succeeded")
			{
				CHECK(counter.load() == 100);
				CHECK(waitSuccessCount.load() == 5);
			}
		}
	}
}

// ---------------------------------------------------------------------------
SCENARIO("ThreadPool - edge cases")
{
	GIVEN("A ThreadPool with 4 workers")
	{
		ThreadPool pool(4);

		WHEN("A task adds another task from within itself")
		{
			std::atomic<int> counter{0};
			pool.AddTask([&pool, &counter]() {
				counter.fetch_add(1, std::memory_order_relaxed);
				pool.AddTask([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });
			});

			THEN("Both the original and the nested task execute")
			{
				REQUIRE(pool.WaitFor(5000ms));
				CHECK(counter.load() == 2);
			}
		}

		WHEN("WaitFor is called on an empty pool")
		{
			THEN("It returns immediately with true") { CHECK(pool.WaitFor(100ms)); }
		}

		WHEN("RequestStop is called immediately after construction, then a task is added")
		{
			std::atomic<int> counter{0};
			pool.RequestStop();
			pool.AddTask([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });
			std::this_thread::sleep_for(100ms);
			THEN("The task does not execute") { CHECK(counter.load() == 0); }
		}
	}

	GIVEN("A ThreadPool with 100 workers")
	{
		ThreadPool pool(100);

		WHEN("1000 tasks are submitted")
		{
			std::atomic<int> counter{0};
			for (int i = 0; i < 1000; ++i)
				pool.AddTask([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });

			THEN("All 1000 tasks execute")
			{
				REQUIRE(pool.WaitFor(10000ms));
				CHECK(counter.load() == 1000);
			}
		}
	}

	GIVEN("A ThreadPool with 4 workers and 20 tasks with varying durations")
	{
		ThreadPool pool(4);
		std::atomic<int> counter{0};

		for (int i = 0; i < 20; ++i)
		{
			pool.AddTask([&counter, i]() {
				if (i % 2 == 0)
					std::this_thread::sleep_for(10ms);
				else
					std::this_thread::sleep_for(50ms);
				counter.fetch_add(1, std::memory_order_relaxed);
			});
		}

		THEN("All 20 tasks complete")
		{
			REQUIRE(pool.WaitFor(10000ms));
			CHECK(counter.load() == 20);
		}
	}
}

// ---------------------------------------------------------------------------
SCENARIO("ThreadPool - stress tests")
{
	GIVEN("A ThreadPool with 8 workers")
	{
		ThreadPool pool(8);

		WHEN("10000 tasks are submitted via AddTask")
		{
			std::atomic<int> counter{0};
			constexpr int numTasks = 10000;
			for (int i = 0; i < numTasks; ++i)
				pool.AddTask([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });

			THEN("All tasks complete")
			{
				REQUIRE(pool.WaitFor(30000ms));
				CHECK(counter.load() == numTasks);
			}
		}

		WHEN("1000 operations alternate between AddTask and Submit")
		{
			std::atomic<int> addTaskCounter{0};
			std::vector<std::future<int>> futures;
			constexpr int numOperations = 1000;

			for (int i = 0; i < numOperations; ++i)
			{
				if (i % 2 == 0)
					pool.AddTask([&addTaskCounter]() { addTaskCounter.fetch_add(1, std::memory_order_relaxed); });
				else
					futures.push_back(pool.Submit([i]() { return i; }));
			}

			THEN("AddTask counter is 500 and Submit futures return correct values")
			{
				REQUIRE(pool.WaitFor(30000ms));
				CHECK(addTaskCounter.load() == numOperations / 2);
				for (size_t i = 0; i < futures.size(); ++i)
					CHECK(futures[i].get() == static_cast<int>(i * 2 + 1));
			}
		}
	}
}

// ---------------------------------------------------------------------------
SCENARIO("ThreadPool - thread safety")
{
	GIVEN("A ThreadPool with 4 workers")
	{
		ThreadPool pool(4);

		WHEN("10000 increments on a shared atomic are submitted")
		{
			std::atomic<int> counter{0};
			constexpr int numIncrements = 10000;
			for (int i = 0; i < numIncrements; ++i)
				pool.AddTask([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });

			THEN("The final count is exact")
			{
				REQUIRE(pool.WaitFor(30000ms));
				CHECK(counter.load() == numIncrements);
			}
		}

		WHEN("10 threads each call GetWorkerCount() 100 times concurrently")
		{
			std::vector<std::thread> threads;
			for (int t = 0; t < 10; ++t)
			{
				threads.emplace_back([&pool]() {
					for (int i = 0; i < 100; ++i)
					{
						volatile size_t count = pool.GetWorkerCount();
						(void)count;
					}
				});
			}
			for (auto& thread : threads)
				thread.join();
			THEN("No crash") { CHECK(true); }
		}
	}
}
