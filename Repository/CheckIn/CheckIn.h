#pragma once

/////////////////////////////////////////////////////////////////////////////
// Checkin.h - check in new files into repository                          //
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
* This module provides Checkin class
*
* Checkin class can provide functionality to checkin a new file. First, it 
* accept file from one source path, with filename and namespace information 
* given. Also, user information need to be supplied because of the single 
* responsibility principle, which means if the user is not the user inside 
* file information then this file cannot be checked in.
* After accepting a file, user can add dependency, category, and description
* to the file, other metadata are automatically generated during checking in.
* Then, class will check whether dependencies are existed. If not, the missing
* one will be stored in the missing file set in repository core. It is because 
* the mutual dependency between h and cpp files that we need to allow checking 
* in files with missing dependencies.
* After checking dependencies, the file can be saved to database and be copied 
* to destination folder.
*
* Public Interface:
* =================
* bool acceptNewFile(const FilePath& filepath, const std::string filename, const std::string& namesp, const std::string& author);
* VersionInfo getVersionInfo(const std::string& namesp, const std::string& filename);
*
* CheckIn& addDescription(const std::string& descrip);
* CheckIn& addDependency(const std::string& depend);
* CheckIn& addCategory(const std::string& category);
*
* void saveToDestination(const FilePath& destination);
* void saveToDb();
* void saveFile(const FilePath& destination);
*
* bool checkDependency();
*
* void showFile();
*
*
* Required Files:
* ===============
* FileSystem.h, FileSystem.cpp
* RepositoryCore.h, RepositoryCore.cpp
*
* Build Process:
* --------------
* devenv Repository.sln /rebuild debug
* 
* Maintenance History:
* ====================
* ver 1.6 : 1 Mar 2018
* - fix bug in saveTodestination()
* ver 1.5 : 28 Feb 2018
* - allow checkin with missing dependency
* ver 1.4 : 26 Feb 2018
* - Fixed bug in checkDependency
* ver 1.3 : 25 Feb 2018
* - save file into db and destination folder
* ver 1.2 : 23 Feb 2018
* - fix bug in acceptFile()
* ver 1.1 : 21 Feb 2018
* - create basic functions
* ver 1.0 : 20 Feb 2018
* - first release
*/



#include "../RepositoryCore/RepositoryCore.h"
#include "../FileSystem/FileSystemDemo/FileSystem.h"

namespace Repository
{
    using namespace NoSqlDb;
    
    class CheckIn
    {
    public:
        using File = DbElement<NoSqlDb::FileInfo>;
		using FilePath = std::string;
        using PackageNumber = int;
        using PackageStatus = std::string;
		using Key = std::string;
        using VersionInfo = std::pair<PackageNumber, PackageStatus>;

        CheckIn(RepositoryCore& repo):repo_(repo), ver_(repo.version()){}

        bool acceptNewFile(const FilePath& filepath, const std::string filename, const std::string& namesp, const std::string& author);
        VersionInfo getVersionInfo(const std::string& namesp, const std::string& filename);

        CheckIn& addDescription(const std::string& descrip);
        CheckIn& addDependency(const std::string& depend);
        CheckIn& addCategory(const std::string& category);

		void saveToDestination(const FilePath& destination);
		void saveToDb();
        void saveFile(const FilePath& destination);

        bool checkDependency();

		void showFile();

    private:
        RepositoryCore& repo_;
        Version& ver_;
        File file_;
		void reset();
    };
}

