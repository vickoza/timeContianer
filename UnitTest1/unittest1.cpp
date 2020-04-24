#include "stdafx.h"
#include <vector>
#include <thread>
#include "CppUnitTest.h"
#include "data_ledger.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono_literals;

namespace UnitTest1
{		
	TEST_CLASS(data_ledgerTest)
	{
	public:
		
		TEST_METHOD(data_ledgerIntGen1)
		{
			data_ledger<int, std::chrono::steady_clock, std::chrono::time_point<std::chrono::steady_clock>, std::deque<std::pair<int, std::chrono::time_point<std::chrono::steady_clock>>>> leg(0);
			Assert::AreEqual(leg.getCurrent(), 0, L"mismatch initialization");
			leg.update(2);
			std::this_thread::sleep_for(1ms);
			leg.update(12);
			std::this_thread::sleep_for(1ms);
			leg.update(-4);
			std::this_thread::sleep_for(1ms);
			leg.update(3);
			std::this_thread::sleep_for(1ms);
			leg.update(7);
			std::this_thread::sleep_for(1ms);
			leg.update(4);
			std::this_thread::sleep_for(1ms);
			Assert::AreEqual(leg.getCurrent(), 24, L"mismatch current state");
			Assert::IsTrue(leg.validate(), L"corrupted ledger state");
		}

		TEST_METHOD(data_ledgerPartPercent)
		{
			data_ledger<int> leg(0);
			leg.update(2);
			std::this_thread::sleep_for(1ms);
			leg.update(12);
			std::this_thread::sleep_for(1ms);
			leg.update(-4);
			std::this_thread::sleep_for(1ms);
			leg.update(3);
			std::this_thread::sleep_for(1ms);
			leg.update(7);
			std::this_thread::sleep_for(1ms);
			leg.update(4);
			std::this_thread::sleep_for(1ms);
			auto leg2 = leg.partition(50.0); //need to figure out a way to you with a defferent container
			Assert::AreEqual(leg.getIntial(), 10, L"mismatch intial state ledger 1");
			Assert::AreEqual(leg.getCurrent(), 24, L"mismatch current state ledger 1");
			Assert::IsTrue(leg.validate(), L"corrupted ledger 1 state");
			Assert::AreEqual(leg2.getIntial(), 0, L"mismatch intial state ledger 2");
			Assert::AreEqual(leg2.getCurrent(), 10, L"mismatch current state ledger 2");
			Assert::IsTrue(leg2.validate(), L"corrupted ledger 1 state");
		}

	};

	TEST_CLASS(state_data_ledgerTest)
	{
		enum class action{intialized, add, sub, sqr};
	public:

		TEST_METHOD(state_data_ledgerIntGen1)
		{
			state_data_ledger<int, action> leg{ [](int& value, const action& act)
			{
				switch (act)
				{
				case action::intialized:
					value = 0;
					break;
				case action::add:
					++value;
					break;
				case action::sub:
					--value;
					break;
				case action::sqr:
					value *= value;
				//case default:
					break;
				}
			}, 0 };
			Assert::AreEqual(leg.getCurrent(), 0, L"mismatch initialization");
			leg.update(action::intialized);
			std::this_thread::sleep_for(1ms);
			leg.update(action::add);
			std::this_thread::sleep_for(1ms);
			leg.update(action::add);
			std::this_thread::sleep_for(1ms);
			leg.update(action::add);
			std::this_thread::sleep_for(1ms);
			leg.update(action::sqr);
			std::this_thread::sleep_for(1ms);
			leg.update(action::sub);
			std::this_thread::sleep_for(1ms);
			Assert::AreEqual(leg.getCurrent(), 8, L"mismatch current state");
			Assert::IsTrue(leg.validate(), L"corrupted ledger state");
		}

		TEST_METHOD(state_data_ledgerPartPercent)
		{
			state_data_ledger<int, action> leg{ [](int& value, const action& act)
			{
				switch (act)
				{
				case action::intialized:
					value = 0;
					break;
				case action::add:
					++value;
					break;
				case action::sub:
					--value;
					break;
				case action::sqr:
					value *= value;
					//case default:
						break;
					}
				}, 0 };
			Assert::AreEqual(leg.getCurrent(), 0, L"mismatch initialization");
			leg.update(action::intialized);
			std::this_thread::sleep_for(1ms);
			leg.update(action::add);
			std::this_thread::sleep_for(1ms);
			leg.update(action::add);
			std::this_thread::sleep_for(1ms);
			leg.update(action::add);
			std::this_thread::sleep_for(1ms);
			leg.update(action::sqr);
			std::this_thread::sleep_for(1ms);
			leg.update(action::sub);
			std::this_thread::sleep_for(1ms);
			leg.update(action::sqr);
			std::this_thread::sleep_for(1ms);
			leg.update(action::add);
			std::this_thread::sleep_for(1ms);
			auto leg2 = leg.partition(50.0); //need to figure out a way to you with a defferent container
			Assert::AreEqual(leg.getIntial(), 3, L"mismatch intial state ledger 1");
			Assert::AreEqual(leg.getCurrent(), 65, L"mismatch current state ledger 1");
			Assert::IsTrue(leg.validate(), L"corrupted ledger 1 state");
			Assert::AreEqual(leg2.getIntial(), 0, L"mismatch intial state ledger 2");
			Assert::AreEqual(leg2.getCurrent(), 3, L"mismatch current state ledger 2");
			Assert::IsTrue(leg2.validate(), L"corrupted ledger 1 state");
		}

	};
}