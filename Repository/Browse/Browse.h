#pragma once

/////////////////////////////////////////////////////////////////////////////
// Browse.h - locate file and display content                              //
// ver 1.7                                                                 //
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
* This module provides Browse class.
*
* Browse class use Process package to pop up windows displaying file content.
* It can accept one exact key to show one file. Or it can display multiple 
* file at the same time from a key vector. It can accept a set of key from a 
* query result of the nosqldb.
*
* Public Interface:
* =================
* void getKeys(const std::vector<std::string>& keys) { keys_ = keys; }
* void displayAllFiles();
* void displayFileContent(const std::string& namesp, const std::string& filename, int versionnum);
*
* Required Files:
* ===============
* Process.h, Process.cpp
* RepositoryCore.h, RepositoryCore.cpp
* CheckIn.h, CheckIn.cpp
*
* Build Process:
* --------------
* devenv Repository.sln /rebuild debug
* 
* Maintenance History:
* ====================
* ver 1.7 : 5 Mar 2018
* - fix bugs in display function
* ver 1.6 : 3 Mar 2018
* - change information shown
* ver 1.5 : 28 Feb 2018
* - support browse from keys 
* ver 1.4 : 26 Feb 2018
* - add test stub
* ver 1.3 : 25 Feb 2018
* - fix bugs in display function
* ver 1.2 : 24 Feb 2018
* - add interface to access db and version
* ver 1.1 : 23 Feb 2018
* - create basic functions
* ver 1.0 : 20 Feb 2018
* - first release
*/



#include "../RepositoryCore/RepositoryCore.h"
#include "../Process/Process/Process.h"
#include "../CheckIn/CheckIn.h"

namespace Repository
{
    class Browse
    {
    public:
        Browse(RepositoryCore& repo):repo_(repo) {}
        
		void getKeys(const std::vector<std::string>& keys) { keys_ = keys; }
		void displayAllFiles();
		void displayFileContent(const std::string& namesp, const std::string& filename, int versionnum);

    private:
        RepositoryCore& repo_;
		std::vector<std::string> keys_;
		
    };

	/*----< display file content with a certain key value and version >-----------------------*/

	inline void Browse::displayFileContent(const std::string& namesp, const std::string& filename, int versionnum)
	{
		std::string key = namesp + "::" + filename + "." + std::to_string(versionnum);
		std::cout << std::endl;
		std::cout << "  * " << key;
		NoSqlDb::DbElement<NoSqlDb::FileInfo> file = repo_.getElem(key);
		NoSqlDb::showHeader();
		NoSqlDb::showElem(file);
		std::cout << std::endl;
		std::string filepath = file.payLoad().filePath();
		std::string filesrc = filepath + "/" + namesp + "_" + filename + "." + std::to_string(versionnum);
		Process p;
		std::string appPath = "c:/windows/system32/notepad.exe";
		p.application(appPath);

		std::string cmdLine = "/A " + filesrc;
		p.commandLine(cmdLine);
		p.create();
	}

	/*----< display all files in the private key vector >-----------------------*/
	inline void Browse::displayAllFiles()
	{
		for (auto key : keys_) {
			std::string nsandpk = "";
			int versionnum = 0;
			size_t pos = key.find_last_of('.');
			versionnum = std::stoi(key.substr(pos + 1));
			nsandpk = key.substr(0, pos);
			pos = nsandpk.find_last_of(':');
			std::string namesp = nsandpk.substr(0, pos - 1);
			std::string filename = nsandpk.substr(pos + 1);
			displayFileContent(namesp, filename, versionnum);
		}
	}
}