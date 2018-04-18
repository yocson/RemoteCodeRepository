#pragma once

/////////////////////////////////////////////////////////////////////////////
// Checkout.h - checkout file and its dependency to destination            //
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
* This module provides Checkout class
* 
* Checkout class allows users to check out files either itself or with all
* its dependencies. Use BFS to get all dependencies in retireveAllDepend()
* function. Use copyFiles() to copy all files whose key is stored in the private
* member keys_.
*
* Public Interface:
* =================
* CheckOut(RepositoryCore& repo): repo_(repo), ver_(repo.version()){}
* void checkOutFile(const std::string& namesp, const std::string & filename, int versionnum, bool withDepend = true);
* void copyFiles(const FilePath& destination, const std::string& reporootdir = "../RepositoryContents");
* void retrieveAllDepend(const std::string& key);
*
* Required Files:
* ===============
* FileSystem.h, FileSystem.cpp
* RepositoryCore.h RepositoryCore.cpp
*
** Build Process:
* --------------
* devenv Repository.sln /rebuild debug
*
* Maintenance History:
* ====================
* ver 1.6 : 3 Mar 2018
* - add checkout with all dependencies
* ver 1.5 : 28 Feb 2018
* - fix bug in retrieveAllDepend()
* ver 1.4 : 26 Feb 2018
* - add test stub
* ver 1.3 : 25 Feb 2018
* - copy file into destination folder
* ver 1.2 : 23 Feb 2018
* - fix bug in checkoutFile()
* ver 1.1 : 22 Feb 2018
* - create basic functions
* ver 1.0 : 20 Feb 2018
* - first release
*/


#include <string>
#include <queue>
#include "../RepositoryCore/RepositoryCore.h"
#include "../FileSystem/FileSystemDemo/FileSystem.h"



namespace Repository
{
    class CheckOut
    {
    public:
        using Key = std::string;
        using Keys = std::vector<Key>;
		using File = NoSqlDb::DbElement<NoSqlDb::FileInfo>;
        using FilePath = std::string;
        using PackageNumber = int;
        using PackageStatus = std::string;
        using VersionInfo = std::pair<PackageNumber, PackageStatus>;

        CheckOut(RepositoryCore& repo): repo_(repo), ver_(repo.version()){}
        void checkOutFile(const std::string& namesp, const std::string & filename, int versionnum, bool withDepend = true);
        void copyFiles(const FilePath& destination, const std::string& reporootdir = "../RepositoryContents");
		void retrieveAllDepend(const std::string& key);

    private:
        RepositoryCore& repo_;
        Version& ver_;
        Keys keys_;
    };
}