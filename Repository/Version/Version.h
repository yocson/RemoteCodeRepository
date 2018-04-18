#pragma once

/////////////////////////////////////////////////////////////////////////////
// Version.h - Manage version information in repository                    //
// ver 1.4                                                                 //
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
* This module provides version class.
*
* Version Class is used to maintain version information. With a set of namespace
* plus filename and version number, it provides updating version and checking 
* versions. Closing file is implemented through version class. One file can be 
* closed or close-pending. Close-pending is same with close except that it needs
* to wait for all its dependencies to be closed and no new check-in is accepted 
* while it is close-pending.
*
* Public Interface:
* =================
* VersionInfo checkVersion(const std::string& nsandpk);
* void updateVersion(const std::string& namesp, const std::string& filename, int vernum);
* bool hasFile(const std::string& nsandpk, int versionnum = 0);
* void closeFile(const std::string& nsandpk, bool canbeclosed = true);
* void closeFile(const std::string& namesp, const std::string& filename, bool canbeclosed = true);
* bool isOpen(const std::string& depend);
* std::string getStatus(const std::string& depend);
*
* Required Files:
* ===============
* No file required
*
* Build Process:
* --------------
* devenv Repository.sln /rebuild debug
*
* Maintenance History:
* ====================
* ver 1.4 : 26 Feb 2018
* - Fixed bug in hasFile
* ver 1.3 : 25 Feb 2018
* - change versioninfo to hold namespace plus filename
* ver 1.2 : 23 Feb 2018
* - add closeFile()
* ver 1.1 : 22 Feb 2018
* - create basic functions
* ver 1.0 : 20 Feb 2018
* - first release
*/

#include <string>
#include <unordered_map>
#include <iostream>

namespace Repository
{
    class Version
    {
    public:
        using VersionNumber = int;
        using FileStatus = std::string;
        using VersionInfo = std::pair<VersionNumber, FileStatus>;

		Version() { }

        VersionInfo checkVersion(const std::string& nsandpk);
        void updateVersion(const std::string& namesp, const std::string& filename, int vernum);
        bool hasFile(const std::string& nsandpk, int versionnum = 0);
		void closeFile(const std::string& nsandpk, bool canbeclosed = true);
        void closeFile(const std::string& namesp, const std::string& filename, bool canbeclosed = true);
        bool isOpen(const std::string& depend);
		std::string getStatus(const std::string& depend);

	private:
        std::unordered_map<std::string, VersionInfo> versions_;
    };
}