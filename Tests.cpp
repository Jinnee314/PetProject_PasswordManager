#include "Tests.hpp"
#include "MyUnitTestFreimwork.hpp"
#include "PasswordManager.hpp"

#include <iterator>
#include <utility>

std::filesystem::path file = "testStorage";
std::string mKey = "BestMasterKeyEveryTime";

void testConstructAndDestruct()
{
	{
		PasswordManager ps;
		ps.readDataFromFile(file);
		ps.decryptData(mKey);

		ASSERT_EQUAL(ps.numRecords(), 0);

		ps.addRecord("f", "l", "p");
		ps.addRecord("s", "l", "p");
	}

	{
		PasswordManager ps;
		ps.readDataFromFile(file);
		ps.decryptData(mKey);

		Record exp1{ "f", "l", "p" };
		Record exp2{ "s", "l", "p" };

		ASSERT_EQUAL(ps.getRecordByName(exp1.name).value(), exp1);
		ASSERT_EQUAL(ps.getRecordByName(exp2.name).value(), exp2);
	}
}

void testGetNames()
{
	PasswordManager ps;
	ps.readDataFromFile(file);
	ps.decryptData(mKey);

	auto names = ps.getNames();

	std::vector<std::string_view> exp{ "f", "s" };

	ASSERT_EQUAL(names, exp);
}

void testAddRecord()
{
	PasswordManager ps;
	ps.readDataFromFile(file);
	ps.decryptData(mKey);
	
	ps.addRecord("first", "log", "pass");
	ps.addRecord("second", "log", "pass");

	auto rec = ps.getRecordByName("first");
	Record exp{ "first", "log", "pass", "" };

	ASSERT_EQUAL(*rec, exp);
	
	auto rec2 = ps.getRecordByNumber(0);
	Record exp2{ "f", "l", "p", "" };
	ASSERT_EQUAL(*rec2, exp2);

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
	ASSERT_EQUAL(ps.getRecordByName("third").value(), exp3);
	ASSERT_EQUAL(ps.getRecordByName("ssssssssss").value(), exp4);

	Record r5{ "someText", "rrrr", "tttt", "ddd" };
	Record r6{ "AaAaAaAaAa", "asdfga", "ssss", "dddd" };
	ps.addRecord(r5);
	ps.addRecord(r6);
	ASSERT_EQUAL(ps.getRecordByName("someText").value(), r5);
	ASSERT_EQUAL(ps.getRecordByName("AaAaAaAaAa").value(), r6);
}

void testGetRecordBySome()
{
	PasswordManager ps;
	ps.readDataFromFile(file);
	ps.decryptData(mKey);

	{
		Record exp{ "AaAaAaAaAa", "asdfga", "ssss", "dddd" };
		auto rec = ps.getRecordByNumber(0);
		ASSERT_EQUAL(*rec, exp);
	}

	{
		Record exp{ "third", "log", "pass", "" };
		auto rec = ps.getRecordByNumber(ps.numRecords() - 1);
		ASSERT_EQUAL(*rec, exp);
	}
}

void testChangeRecord()
{
	PasswordManager ps;
	ps.readDataFromFile(file);
	ps.decryptData(mKey);

	{
		Record exp{ "test name","test login", "test pass", "test desription" };
		auto rec = ps.getRecordByNumber(1);
		ps.changeLoginRecord("test login");
		ps.changeDescriptionRecord("test desription");
		ps.changeNameRecord("test name");
		ps.changePasswordRecord("test pass");

		rec = ps.getRecordByName("test name");
		ASSERT_EQUAL(*rec, exp);
	}

	{
		auto rec = ps.getRecordByName("test name");
		auto name = rec.value().name;
		ps.changeNameRecord("");
		ASSERT_EQUAL(ps.getRecordByName(name).value().name, name);
	}
}

void testWrongKey()
{
	{
		PasswordManager ps;
		ps.readDataFromFile(file);
		ASSERT_EQUAL(ps.decryptData("WrongKey"), 1);
		ASSERT_EQUAL(ps.decryptData("BestMasterKeyEvery"), 1);
		ASSERT_EQUAL(ps.decryptData("BestMasterKeuEveryTime"), 1);
		ASSERT_EQUAL(ps.decryptData(""), 1);
	}
	{
		PasswordManager ps;
		ps.readDataFromFile(file);
	}
	ASSERT(static_cast<bool>(std::filesystem::file_size(file)));
}

void testSave()
{
	{
		PasswordManager ps;
		ps.readDataFromFile(file);
		ps.decryptData(mKey);

		Record r{ "testSave", "sdlfj", "savePass", "saveDes" };
		ps.addRecord(std::move(r));

		ps.saveDataInFile(false);

		{
			PasswordManager ps2;
			ps2.readDataFromFile(file);
			ps2.decryptData(mKey);

			auto rec = ps2.getRecordByName("testSave");
			r = { "testSave", "sdlfj", "savePass", "saveDes" };
			ASSERT_EQUAL(rec.value(), r);
		}
	}

	{
		PasswordManager ps;
		ps.readDataFromFile(file);
		ps.decryptData(mKey);

		auto rec = ps.getRecordByName("testSave");
		Record r{ "testSave", "sdlfj", "savePass", "saveDes" };
		ASSERT_EQUAL(rec.value(), r);
	}
}

void testDelete()
{
	{
		PasswordManager ps;
		ps.readDataFromFile(file);
		ps.decryptData(mKey);

		{
			auto numRec = ps.numRecords();
			ps.deleteRecordByName("qwertyuioasdfghjk");
			ASSERT_EQUAL(ps.numRecords(), numRec)
		}

		{
			Record exp{};
			auto recName = ps.getRecordByNumber(1).value().name;
			ps.deleteRecordByNumber(1);
			auto res = ps.getRecordByName(recName);
			ASSERT_EQUAL(*res, exp);
		}

		{
			Record exp{};
			ps.deleteRecordByName("test name");
			auto res = ps.getRecordByName("test name");
			ASSERT_EQUAL(*res, exp);
		}

		ps.clearData();
		ASSERT_EQUAL(ps.numRecords(), 0);
	}
	ASSERT(!static_cast<bool>(std::filesystem::file_size(file)));
}

void testAll()
{
	MyUnitTest::TestRunner tr;
	RUN_TEST(tr, testConstructAndDestruct);
	RUN_TEST(tr, testGetNames);
	RUN_TEST(tr, testAddRecord);
	RUN_TEST(tr, testGetRecordBySome);
	RUN_TEST(tr, testChangeRecord);
	RUN_TEST(tr, testWrongKey);
	RUN_TEST(tr, testSave);
	RUN_TEST(tr, testDelete);
}