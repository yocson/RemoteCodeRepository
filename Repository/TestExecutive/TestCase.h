#pragma once

/////////////////////////////////////////////////////////////////////////////
// TestCase.h - all test cases for repository project                      //
// ver 1.3                                                                 //
// ----------------------------------------------------------------------- //
// Language:    C++, Visual Studio 2017                                    //
// Platform:    Macbook Pro 15, Windows 10 on Parallel Virtual Machine     //
// Application: Project2, Repository,                                      //
//				CSE687, Object Oriented Design, Spring 2018                //
// Author:      Cheng Wang,                                                //
//              cwang76@sry.edu                                            //
/////////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
* This module provides Testcases class.
*
* This class provides several test cases for the repository project. It maintains
* a private repositorycore object so that all test functions can access one same 
* repository.
*
* Required Files:
* ===============
* RepositoryCore.h, RepositoryCore.cpp
* CheckIn.h, CheckIn.cpp
* CheckOut.h, CheckOut.cpp
*
* Build Process:
* --------------
* devenv Repository.sln /rebuild debug
*
* Maintenance History:
* ====================
* ver 1.3 : 3 Mar 2018
* - add tests for requirement 6
* ver 1.2 : 28 Feb 2018
* - add tests
* ver 1.1 : 22 Feb 2018
* - create basic tests
* ver 1.0 : 25 Feb 2018
* - first release
*/


#include "../RepositoryCore/RepositoryCore.h"
#include "../CheckIn/CheckIn.h"
#include "../CheckOut/CheckOut.h"



namespace Repository
{
	class TestCase
	{
	public:
		static bool testR1();
		static bool testR2();
		static bool testR3a();
		static bool testR3b();
		static bool testR3c();
		static bool testR3d();
		static bool testR3e();
		static bool testR3f();
		static bool testR4a();
		static bool testR4b();
		static bool testR4c();
		static bool testR5();
		static bool testR6();

	private:
		static Repository::RepositoryCore repo_;
	};
}