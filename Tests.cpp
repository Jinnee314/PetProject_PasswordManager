#include "Tests.hpp"
#include "MyUnitTestFreimwork.hpp"
#include "PasswordManager.hpp"

#include <iterator>
#include <utility>

void testReadDataFromStorage()
{
	{
		PasswordManager ps("testStorage");

		ASSERT_EQUAL(ps.getRecords().size(), 0);

		ps.AddRecord("f", "l", "p");
		ps.AddRecord("s", "l", "p");
	}

	{
		PasswordManager ps("testStorage");
		Record exp1{ "f", "l", "p" };
		Record exp2{ "s", "l", "p" };

		auto records = ps.getRecords();
		ASSERT_EQUAL(*(records.begin()), exp1);
		ASSERT_EQUAL(*(std::next(records.begin())), exp2);
	}
}

void testAddRecord()
{
	PasswordManager ps("testStorage");
	
	ps.AddRecord("first", "log", "pass");
	ps.AddRecord("second", "log", "pass");

	auto rec = ps.getRecordByName("first");
	Record exp{ "first", "log", "pass", "" };

	ASSERT_EQUAL(rec, exp);
	
	auto rec2 = ps.getRecordByNumber(0);
	Record exp2{ "second", "log", "pass", "" };
	ASSERT_EQUAL(rec2, exp2);

	auto isInserted = ps.AddRecord("", "", "");
	ASSERT(!isInserted);

	isInserted = ps.AddRecord("first", "log2", "pass2");
	ASSERT(!isInserted);

	Record r3{ "third", "log", "pass" };
	Record exp3 = r3;
	ps.AddRecord(std::move(r3));
	ASSERT_EQUAL(ps.getRecordByName("third"), exp3);
}



void testAll()
{
	MyUnitTest::TestRunner tr;
	RUN_TEST(tr, testReadDataFromStorage);
	RUN_TEST(tr, testAddRecord);
}