#include "MyUnitTestFreimwork.hpp"

MyUnitTest::TestRunner::~TestRunner()
{
	if (failCount > 0)
	{
		std::cerr << failCount << " tests failed. Terminate\n";
		exit(1);
	}
}

void MyUnitTest::Assert(bool b, const std::string& hint)
{
	AssertEqual(b, true, hint);
}
