#pragma once
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <sstream>

// INTERFACE

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec);

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::set<T>& s);

template<typename K, typename V>
std::ostream& operator<<(std::ostream& out, const std::map<K, V>& m);

namespace MyUnitTest
{
	template<typename T, typename U>
	void AssertEqual(const T& t, const U& u, const std::string& hint = {});

	void Assert(bool b, const std::string& hint = {});

	class TestRunner
	{
	private:
		uint32_t failCount = 0;
	public:

		~TestRunner();

		template<class TestFunc>
		void RunTest(TestFunc func, const std::string& test_name);
	};
}

//IMPLEMENTATION

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
	out << "[";

	bool first = true;
	for (const auto& elem : vec)
	{
		if (!first)
		{
			out << ", ";
		}
		first = false;
		out << elem;
	}
	out << "]";

	return out;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::set<T>& s)
{
	out << "{";

	bool first = true;
	for (const auto& elem : s)
	{
		if (!first)
		{
			out << ", ";
		}
		first = false;
		out << elem;
	}
	out << "}";

	return out;
}

template<typename K, typename V>
std::ostream& operator <<(std::ostream& out, const std::map<K, V>& m)
{
	out << "{";

	bool first = true;
	for (const auto& [key, value] : m)
	{
		if (!first)
		{
			out << ", ";
		}
		first = false;
		out << key << ": " << value;
	}
	out << "}";

	return out;
}


template<typename T, typename U>
void MyUnitTest::AssertEqual(const T& t, const U& u, const std::string& hint)
{
	if (t != u)
	{
		std::ostringstream os;
		os << "Assertion failed:\n" << t << " != " << u << ";\nHint " << hint << "\n";
		throw std::runtime_error(os.str());
	}
}

template<class TestFunc>
void MyUnitTest::TestRunner::RunTest(TestFunc func, const std::string& test_name) {
	try {
		func();
		std::cerr << test_name << " OK" << std::endl;
	}
	catch (std::exception& e) {
		++failCount;
		std::cerr << test_name << " fail: " << e.what() << std::endl;
	}
	catch (...) {
		++failCount;
		std::cerr << "Unknown exception caught" << std::endl;
	}
}

#define ASSERT_EQUAL(x, y)						\
	{											\
		std::ostringstream testOut;					\
		testOut << #x << " != " << #y				\
		<< ", " << __FILE__ << ":"				\
		<< __LINE__;							\
		MyUnitTest::AssertEqual(x,y, testOut.str());	\
	}

#define ASSERT(x)								\
	{											\
		std::ostringstream testOut;					\
		testOut << #x << " is false"				\
		<< ", " << __FILE__ << ":"				\
		<< __LINE__;							\
		MyUnitTest::Assert(x, testOut.str());		\
	}

#define RUN_TEST(tr, func) tr.RunTest(func, #func);