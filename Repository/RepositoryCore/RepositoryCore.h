#pragma once

/////////////////////////////////////////////////////////////////////////////
// Repository.h - Store file information and manipulate files              //
// ver 1.6                                                                 //
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
* This module provides Repository class
*
* Repository class maintain a NoSqlDb and a version object. It provides interface 
* to access db elements, check dependencies and close file through nosqldb and 
* version.
* It also has a 'missingfile' set storing the files needed to be added later on.
* Other packages like Checkin and Checkout store a reference to RepositoryCore.
* 
*
* Public Interface:
* =================
* Version& version() { return ver_; }
* Version version() const { return ver_; }
* 
* NoSqlDb::DbCore<NoSqlDb::FileInfo>& database() { return db_; }
* NoSqlDb::DbCore<NoSqlDb::FileInfo> database() const { return db_; }
* 
* File& getElem(Key key);
* void addElem(const Key& key, const File& f);
* 
* bool checkDependency(const NoSqlDb::DbElement<NoSqlDb::FileInfo>& file);
* bool closeFile(const std::string& namesp, const std::string & filename);
* 
* void showDb();
* 
* bool missFile(Key key);
* void notMissAnyMore(Key key);
* 
* void reset();
*
* Required Files:
* ===============
* NoSql.h, NoSql.cpp
* PayLoad.h, PayLoad.cpp
* Version.h Version.cpp
*
* Build Process:
* --------------
* devenv Repository.sln /rebuild debug
*
*
* Maintenance History:
* ====================
* ver 1.6 : 3 Mar 2018
* - change closefile(), check mututal dependency
* ver 1.5 : 28 Feb 2018
* - add show db
* ver 1.4 : 26 Feb 2018
* - add test stub
* ver 1.3 : 25 Feb 2018
* - fix bugs
* ver 1.2 : 24 Feb 2018
* - add interface to access db and version
* ver 1.1 : 23 Feb 2018
* - create basic functions
* ver 1.0 : 20 Feb 2018
* - first release
*/


#include "../NoSqlDb/Executive/NoSql.h"
#include "../NoSqlDb/PayLoad/PayLoad.h"
#include "../Version/Version.h"

namespace Repository
{
    class RepositoryCore
    {
    public:
        using File = NoSqlDb::DbElement<NoSqlDb::FileInfo>;
        using Key = std::string;

		RepositoryCore() {}

		Version& version() { return ver_; }
		Version version() const { return ver_; }

		NoSqlDb::DbCore<NoSqlDb::FileInfo>& database() { return db_; }
		NoSqlDb::DbCore<NoSqlDb::FileInfo> database() const { return db_; }

        File& getElem(Key key);
        void addElem(const Key& key, const File& f);

        bool checkDependency(const NoSqlDb::DbElement<NoSqlDb::FileInfo>& file);
		bool closeFile(const std::string& namesp, const std::string & filename);

		void showDb();

		bool missFile(Key key);
		void notMissAnyMore(Key key);

		void reset();

	private:
		NoSqlDb::DbCore<NoSqlDb::FileInfo> db_;
        Version ver_;
		std::unordered_set<std::string> missingfiles_;
		bool isMutualDepend(const std::string& depend, const std::string& parent);
	};
}