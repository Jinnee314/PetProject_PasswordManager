#include "Tests.hpp"
#include "MyUnitTestFreimwork.hpp"
#include "PasswordManager.hpp"

#include <iterator>
#include <utility>

void testConstructAndDestruct()
{
	{
		PasswordManager ps("testStorage");

		ASSERT_EQUAL(ps.numRecords(), 0);

		ps.addRecord("f", "l", "p");
		ps.addRecord("s", "l", "p");
	}

	{
		PasswordManager ps("testStorage");
		Record exp1{ "f", "l", "p" };
		Record exp2{ "s", "l", "p" };

		ASSERT_EQUAL(ps.getRecordByName(exp1.name), exp1);
		ASSERT_EQUAL(ps.getRecordByName(exp2.name), exp2);
	}
}

void testGetNames()
{
	PasswordManager ps("testStorage");

	auto names = ps.getNames();

	std::vector<std::string_view> exp{ "f", "s" };

	ASSERT_EQUAL(names, exp);
}

void testAddRecord()
{
	PasswordManager ps("testStorage");
	
	ps.addRecord("first", "log", "pass");
	ps.addRecord("second", "log", "pass");

	auto rec = ps.getRecordByName("first");
	Record exp{ "first", "log", "pass", "" };

	ASSERT_EQUAL(rec, exp);
	
	auto rec2 = ps.getRecordByNumber(0);
	Record exp2{ "second", "log", "pass", "" };
	ASSERT_EQUAL(rec2, exp2);

	auto size = ps.numRecords();
	ps.addRecord("", "", "");
	ASSERT_EQUAL(ps.numRecords(), size);

	ps.addRecord("first", "log2", "pass2");
	ASSERT_EQUAL(ps.numRecords(), size);

	Record r3{ "third", "log", "pass", ""};
	Record r4{ "ssssssssss", "zzzz", "bbbb", "" };
	Record exp3 = r3;
	Record exp4 = r4;
	ps.addRecord(std::move(r3));
	ps.addRecord(std::move(r4));
	ASSERT_EQUAL(ps.getRecordByName("third"), exp3);
	ASSERT_EQUAL(ps.getRecordByName("ssssssssss"), exp4);

	Record r5{ "someText", "rrrr", "tttt", "ddd" };
	Record r6{ "AaAaAaAaAa", "asdfga", "ssss", "dddd" };
	ps.addRecord(r5);
	ps.addRecord(r6);
	ASSERT_EQUAL(ps.getRecordByName("someText"), r5);
	ASSERT_EQUAL(ps.getRecordByName("AaAaAaAaAa"), r6);
}

void testChangeRecord()
{
	PasswordManager ps("testStorage");

	{
		Record exp{ "test name","test login", "test pass", "test desription" };
		auto rec = ps.getRecordByNumber(1);
		ps.changeLoginRecord("test login");
		ps.changeDescriptionRecord("test desription");
		ps.changeNameRecord("test name");
		ps.changePasswordRecord("test pass");

		rec = ps.getRecordByName("test name");
		ASSERT_EQUAL(rec, exp);
	}

	{
		auto rec = ps.getRecordByName("test name");
		auto name = rec.name;
		ps.changeNameRecord("");
		ASSERT_EQUAL(ps.getRecordByName(name).name, name);
	}
}

void testDelete()
{
	PasswordManager ps("testStorage");

	{
		auto numRec = ps.numRecords();
		ps.deleteRecordByName("qwertyuioasdfghjk");
		ASSERT_EQUAL(ps.numRecords(), numRec)
	}

	{
		Record exp{};
		auto recName = ps.getRecordByNumber(1).name;
		ps.deleteRecordByNumber(1);
		Record res;
		try
		{
			res = ps.getRecordByName(recName);
		}
		catch (const std::exception&)
		{
			res = Record{};
		}
		ASSERT_EQUAL(res, exp);
	}

	{
		Record exp{};
		ps.deleteRecordByName("test name");
		Record res;
		try
		{
			res = ps.getRecordByName("test name");
		}
		catch (const std::exception&)
		{
			res = Record{};
		}
		ASSERT_EQUAL(res, exp);
	}

	ps.clearData();
	ASSERT_EQUAL(ps.numRecords(), 0);
}

void testAll()
{
	MyUnitTest::TestRunner tr;
	RUN_TEST(tr, testConstructAndDestruct);
	RUN_TEST(tr, testGetNames);
	RUN_TEST(tr, testAddRecord);
	RUN_TEST(tr, testChangeRecord);
	RUN_TEST(tr, testDelete);
}