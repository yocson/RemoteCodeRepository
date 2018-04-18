/////////////////////////////////////////////////////////////////////////////
// Executive.cpp - register test functions and run                         //
// ver 1.0                                                                 //
// ----------------------------------------------------------------------- //
// Language:    C++, Visual Studio 2017                                    //
// Platform:    Macbook Pro 15, Windows 10 on Parallel Virtual Machine     //
// Application: Project2, Repository,                                      //
//				CSE687, Object Oriented Design, Spring 2018                //
// Author:      Cheng Wang,                                                //
//              cwang76@sry.edu                                            //
/////////////////////////////////////////////////////////////////////////////
// This file register all test functions and run them

#include "../NoSqlDb/NoSql/NoSql.h"
#include "../NoSqlDb/PayLoad/PayLoad.h"
#include "../NoSqlDb/Utilities/TestUtilities/TestUtilities.h"
#include "TestCase.h"

#ifdef TESTEXE

using namespace NoSqlDb;

int main()
{
	Utilities::Title("Testing Repository");
	std::cout << "\n  Cheng Wang, cwang76@syr.edu";
	Utilities::putline();
	TestExecutive ex;

	// define test structures with test function and message
	TestExecutive::TestStr ts1{ Repository::TestCase::testR1, "Use C++11" };
	TestExecutive::TestStr ts2{ Repository::TestCase::testR2, "Use streams and new and delete" };
	TestExecutive::TestStr ts3a{ Repository::TestCase::testR3a, "Showing packages" };
	TestExecutive::TestStr ts3b{ Repository::TestCase::testR3b, "Testing Repository" };
	TestExecutive::TestStr ts3c{ Repository::TestCase::testR3c, "Checkin" };
	TestExecutive::TestStr ts3d{ Repository::TestCase::testR3d, "Checkout" };
	TestExecutive::TestStr ts3e{ Repository::TestCase::testR3e, "Version" };
	TestExecutive::TestStr ts3f{ Repository::TestCase::testR3f, "Browse" };
	TestExecutive::TestStr ts4a{ Repository::TestCase::testR4a, "CheckinA" };
	TestExecutive::TestStr ts4b{ Repository::TestCase::testR4b, "CheckinB" };
	TestExecutive::TestStr ts4c{ Repository::TestCase::testR4c, "CheckinC" };
	TestExecutive::TestStr ts5{ Repository::TestCase::testR5, "Browse" };
	TestExecutive::TestStr ts6{ Repository::TestCase::testR6, "Test with files" };
	// register test structures with TestExecutive instance, ex
	ex.registerTest(ts1);
	ex.registerTest(ts2);
	ex.registerTest(ts3a);
	ex.registerTest(ts3b);
	ex.registerTest(ts3c);
	ex.registerTest(ts3d);
	ex.registerTest(ts3e);
	ex.registerTest(ts3f);
	ex.registerTest(ts4a);
	ex.registerTest(ts4b);
	ex.registerTest(ts4c);
	ex.registerTest(ts5);
	ex.registerTest(ts3d);
	ex.registerTest(ts6);

	// run tests
	bool result = ex.doTests();
	if (result == true)
		std::cout << "\n  all tests passed";
	else
		std::cout << "\n  at least one test failed";
	Utilities::putline(2);
	getchar();
	getchar();
	return 0;
}

#endif