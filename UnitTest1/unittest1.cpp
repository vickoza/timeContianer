#include "stdafx.h"
#include <thread>
#include "CppUnitTest.h"
#include "data_ledger.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono_literals;

namespace UnitTest1
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(data_ledgerIntGen1)
		{
			data_ledger<int> leg(0);
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

	};
}