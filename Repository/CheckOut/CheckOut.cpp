/////////////////////////////////////////////////////////////////////////////
// Checkout.cpp - checkout file and its dependency to destination          //
// ver 1.6                                                                 //
// ----------------------------------------------------------------------- //
// Language:    C++, Visual Studio 2017                                    //
// Platform:    Macbook Pro 15, Windows 10 on Parallel Virtual Machine     //
// Application: Project2, Repository,                                      //
//				CSE687, Object Oriented Design, Spring 2018                //
// Author:      Cheng Wang,                                                //
//              cwang76@sry.edu                                            //
/////////////////////////////////////////////////////////////////////////////

#include "CheckOut.h"
#include "../CheckIn/CheckIn.h"

namespace Repository
{
	/*----< check out file with or without dependencies >-----------------------*/
	// if versionnum is 0, give the latest version

    void CheckOut::checkOutFile(const std::string& namesp, const std::string & filename, int versionnum, bool withDepend /*= true*/)
    {
        std::string nsandpk = namesp + "::" + filename;
        if(ver_.hasFile(nsandpk)) {
            CheckOut::VersionInfo verinfo = ver_.checkVersion(nsandpk);
			// if versionnum is 0, give the latest version
			// otherwise get the versionnum
			std::string ver = (versionnum == 0) ? std::to_string(verinfo.first) : std::to_string(versionnum);
			Key temp_key = namesp + "::" + filename + + "." + ver;    
			if (withDepend) {
				retrieveAllDepend(temp_key);
			} else { keys_.push_back(temp_key); }
        } else {
            std::cout << "No file" << std::endl;
        }
    }

	/*----< copy files in keys_ to destination folder >-----------------------*/

    void CheckOut::copyFiles(const FilePath& destination, const std::string& reporootdir)
    {
        // use file system to copy files from keys_
        // change filename at the same time
        for(auto key: keys_) {
            File file = repo_.getElem(key);

			std::string src = file.payLoad().filePath() + "/" + file.payLoad().namesp() + "_" + file.name() + "." + std::to_string(file.payLoad().version());
			std::string path = file.payLoad().filePath();
			
			path = destination + path.substr(reporootdir.size()) + "/";

			std::string des = path + file.name();

			std::stack<std::string> pathStack;

			while (path != "../") {
				pathStack.push(path);
				path.pop_back();
				path = FileSystem::Path::getPath(path);
			}

			// create all folders
			while (!pathStack.empty()) {
				path = pathStack.top();
				pathStack.pop();
				FileSystem::Directory::create(path);
			}

			std::cout << "  * Check out file:" << std::endl;
			std::cout << "  * From" << src << std::endl;
			std::cout << "  * To" << des << std::endl;

            FileSystem::File::copy(src, des);
        }
    }

	/*----< using DFS to get all depenedencies and store them in keys_ >-----------------------*/

	void CheckOut::retrieveAllDepend(const std::string& key)
	{
		std::unordered_set<std::string> visited;

		std::queue<std::string> bfsque;

		bfsque.push(key);
		
		while (!bfsque.empty())
		{
			std::string key = bfsque.front();
			bfsque.pop();
			keys_.push_back(key);
			DbElement<FileInfo> file = repo_.getElem(key);
			for (auto depend : file.children()) {
				if (visited.count(depend) == 0) { 
					bfsque.push(depend); 
				}
			}
			visited.insert(key);
		}
	}

}


#ifdef TEST_CHECKOUT

int main() 
{
	Repository::RepositoryCore repo;
	Repository::CheckIn checkin(repo);

	if (checkin.acceptNewFile("../Source", "Test.h", "TestName", "Yocson")) {
		checkin.addDescription("First file without any depend").addCategory("Test").addCategory("Myfile");
		checkin.showFile();
		if (checkin.checkDependency()) {
			checkin.addDependency("TestName::Test.cpp.1");
			std::cout << "save file now" << std::endl;
			checkin.saveFile("../RepositoryContents");
		}
		repo.showDb();
	}

	checkin.acceptNewFile("../Source", "Test.cpp", "TestName", "Yocson");
	checkin.addDescription("Second file").addCategory("Test").addDependency("TestName::Test.h.1");
	if (checkin.checkDependency()) {
		checkin.showFile();
		std::cout << "save file now" << std::endl;
		checkin.saveFile("../RepositoryContents");
	}
	repo.showDb();

	Repository::CheckOut checkout(repo);

	checkout.checkOutFile("TestName", "Test.cpp", 0);
	checkout.copyFiles("../CheckOutFiles");

	getchar();
	getchar();
}
#endif // TEST_CHECKOUT
