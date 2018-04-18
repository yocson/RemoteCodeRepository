/////////////////////////////////////////////////////////////////////////////
// Checkin.cpp - check in new files into repository                        //
// ver 1.6                                                                 //
// ----------------------------------------------------------------------- //
// Language:    C++, Visual Studio 2017                                    //
// Platform:    Macbook Pro 15, Windows 10 on Parallel Virtual Machine     //
// Application: Project2, Repository,                                      //
//				CSE687, Object Oriented Design, Spring 2018                //
// Author:      Cheng Wang,                                                //
//              cwang76@sry.edu                                            //
/////////////////////////////////////////////////////////////////////////////

#include "CheckIn.h"

namespace Repository
{
	/*----< accept a file by filepath and name and namespace, check author to follow single user reponsibility  >-----------------------*/
    bool CheckIn::acceptNewFile(const FilePath& filepath, const std::string filename, const std::string& namesp, const std::string& author)
    {
		reset();
        VersionInfo versioninfo = getVersionInfo(namesp, filename);
		if (versioninfo.first == 0) {
			std::cout << "First time created" << std::endl;
			file_.name(filename);
			int versionNum = versioninfo.first + 1;
			file_.payLoad().author(author);
			file_.payLoad().namesp(namesp);
			file_.payLoad().filePath(filepath);
			file_.payLoad().version(versionNum);
			ver_.updateVersion(namesp, filename, versionNum);
		} else {
			Key key = namesp + "::" + filename + "." + std::to_string(versioninfo.first);
			file_ = repo_.getElem(key);
			//check ownership
			if (author != file_.payLoad().author()) {
				std::cout << "This user has no access to the file." << std::endl;
				return false;
			}
			file_.payLoad().filePath(filepath);
			if (versioninfo.second == "closed") {
				int versionNum = versioninfo.first + 1;
				file_.payLoad().version(versionNum);
				ver_.updateVersion(namesp, filename, versionNum);
			}
			if (versioninfo.second == "close-pending") {
				std::cout << "This is a close-pending file, please close it first!" << std::endl;
				return false;
			}
		}
		return true;
    }

	/*----< get version number and status >-----------------------*/

	CheckIn::VersionInfo CheckIn::getVersionInfo(const std::string& namesp, const std::string& filename)
    {
        return ver_.checkVersion(namesp + "::" + filename);
    }

    void CheckIn::saveToDb()
    {
        //generate key
        Key key = file_.payLoad().namesp() + "::" + file_.name() + "." + std::to_string(file_.payLoad().version());
        repo_.addElem(key, file_);
		if (repo_.missFile(key)) {
			repo_.notMissAnyMore(key);
		}
    }

	/*----< save file to destination folder with new filename >-----------------------*/

    void CheckIn::saveToDestination(const FilePath& destination)
    {
        //always save to destination first, because we need to update filepath of file
		std::string src = file_.payLoad().filePath() + "/" + file_.name();
		std::string des = "";
		std::string fp = "";
		std::string path = destination + "/";

		std::stack<std::string> pathStack;

		while (path != "../") {
			pathStack.push(path);
			path.pop_back();
			path = FileSystem::Path::getPath(path);
		}

		while (!pathStack.empty()) {
			path = pathStack.top();
			pathStack.pop();
			FileSystem::Directory::create(path);
		}

		des = destination + "/" + file_.payLoad().namesp() + "_" + file_.name() + "." + std::to_string(file_.payLoad().version());
		fp = destination;

		std::cout << "  * Save file: " << std::endl;
		std::cout << "  * From: " << src << std::endl;
		std::cout << "  * To: " << des << std::endl;
		FileSystem::File::copy(src, des);
        file_.payLoad().filePath(fp);
    }

	/*----< wrap saveTodb and saveToDestination >-----------------------*/
    void CheckIn::saveFile(const FilePath& destination)
    {
        //check dependencies
        saveToDestination(destination);
        saveToDb();
    }

	/*----< call repositorycall's checkdependency to see whether all dependencies exsit >-----------------------*/
    bool CheckIn::checkDependency() 
    {
        return repo_.checkDependency(file_);
    }

	/*----< display file information >-----------------------*/
	void CheckIn::showFile()
	{
		NoSqlDb::showHeader();
		NoSqlDb::showElem(file_);
		std::cout << std::endl;
	}

	/*----< reflush the file_ being processed >-----------------------*/

	void CheckIn::reset()
	{
		DbElement<FileInfo> newElem;
		file_ = newElem;
	}

	/*----< add file description >-----------------------*/

	CheckIn& CheckIn::addDescription(const std::string& descrip)
    {
        file_.descrip(descrip);
        return *this;
    }

	/*----< add file dependency >-----------------------*/

    CheckIn& CheckIn::addDependency(const std::string& depend)
    {
		file_.children().push_back(depend);
        return *this;
    }

	/*----< add file category >-----------------------*/

    CheckIn& CheckIn::addCategory(const std::string& category)
    {
		file_.payLoad().addCate(category);
        return *this;
    }
}


#ifdef TEST_CHECKIN

int main()
{
	Repository::RepositoryCore repo;
	Repository::CheckIn checkin(repo);

	if (checkin.acceptNewFile("../Source", "Test.h", "TestName", "Yocson")) {
		checkin.addDescription("First file without any depend").addCategory("Test").addCategory("Myfile");
		checkin.showFile();
		checkin.checkDependency();
		checkin.saveFile("../RepositoryContents");
		repo.showDb();
	}

	if (checkin.acceptNewFile("../Source", "Test.h", "TestName", "SomeOne")) {
		std::cout << "should not be printed" << std::endl;
	}
	repo.showDb();

	repo.closeFile("TestName", "Test.h");

	if (checkin.acceptNewFile("../Source", "Test.h", "TestName", "Yocson")) {
		checkin.saveFile("../RepositoryContents");
	}
	getchar();
	getchar();
	return 0;
}

#endif // TEST_CHECKIN
