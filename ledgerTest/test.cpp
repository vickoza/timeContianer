#include "pch.h"
#include <data_ledger_old.h>
#include <thread>

using namespace std::chrono_literals;

TEST(TestCaseName, TestName) 
{
	data_ledger<int> leg(0);
	EXPECT_EQ(1, 1);
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
	EXPECT_TRUE(true);
}