/////////////////////////////////////////////////////////////////////////////
// Repository.cpp - Store file information and manipulate files            //
// ver 1.6                                                                 //
// ----------------------------------------------------------------------- //
// Language:    C++, Visual Studio 2017                                    //
// Platform:    Macbook Pro 15, Windows 10 on Parallel Virtual Machine     //
// Application: Project2, Repository,                                      //
//				CSE687, Object Oriented Design, Spring 2018                //
// Author:      Cheng Wang,                                                //
//              cwang76@sry.edu                                            //
/////////////////////////////////////////////////////////////////////////////

#include "RepositoryCore.h"

namespace Repository
{
	/*----< get database element using a key >-----------------------*/

    RepositoryCore::File& RepositoryCore::getElem(RepositoryCore::Key key)
    {

		return db_[key];
        
    }

	/*----< add element into database >-----------------------*/

    void RepositoryCore::addElem(const RepositoryCore::Key& key, const RepositoryCore::File& f)
    {
        db_[key] = f;
    }


	/*----< close file with namespace and filename >-----------------------*/

    bool RepositoryCore::closeFile(const std::string& namesp, const std::string & filename)
    {
        // check whether the dependencies are all closed
		// if there is open dependency, change to close-pending
		// if there is only one close-pending file, check its dependencies.
        Key key = namesp + "::" + filename + "." + std::to_string(ver_.checkVersion(namesp + "::" + filename).first);
        File file = getElem(key);
		ver_.closeFile(namesp, filename, false);
		bool canbeclosed = true;
		std::string filestatus = "";
        for (auto depend: file.children()) {
			if (missFile(depend)) {
				// if it has some missing depend, can not be closed or even close-pending
				return false;
			}
			filestatus = ver_.getStatus(depend);

            if (filestatus == "close-pending") { 
				// go to check whether there is only one open close which is this file
				if (!isMutualDepend(depend, key)) { canbeclosed = false; }
			}
			if (filestatus == "open") { 
				canbeclosed = false; }
        }
		ver_.closeFile(namesp, filename, canbeclosed);
		return canbeclosed;
    }

	/*----< check whether all dependencies are in the repository already >-----------------------*/
	// store the missing dependency into missingfiles_

    bool RepositoryCore::checkDependency(const NoSqlDb::DbElement<NoSqlDb::FileInfo>& file)
    {
		bool noMissDependency = true;
        for (auto depend: file.children())
        {
            std::string nsandpk = "";
            int versionnum = 0;
            size_t pos = depend.find_last_of('.');
            versionnum = std::stoi(depend.substr(pos + 1));
            nsandpk = depend.substr(0, pos);
			if (!ver_.hasFile(nsandpk, versionnum)) {
				missingfiles_.insert(depend);
				std::cout << "Missing Dependency: " << depend << std::endl;
				noMissDependency = false;
			}
        }
        return noMissDependency;
    }


	/*----< show the database >-----------------------*/

	void RepositoryCore::showDb()
	{
		NoSqlDb::showDb<NoSqlDb::FileInfo>(db_);
		std::cout << std::endl;
	}

	/*----< check whether two files are mututal depended, if so, close th current one >-----------------------*/

	bool RepositoryCore::isMutualDepend(const std::string& depend, const std::string& parent)
	{
		// check whether depend has only one close-pending dependency which is parent.
		// if true, return true, and close depend
		// else, return false
		RepositoryCore::File file = getElem(depend);
		std::vector<std::string> close_pending;
		std::string nsandpk = "";
		for (auto dependency : file.children()) {
			size_t pos = depend.find_last_of('.');
			nsandpk = depend.substr(0, pos);

			if (ver_.getStatus(dependency) == "close-pending") { 
				close_pending.push_back(dependency);
			}
		}
		if (close_pending.size() == 1 && close_pending[0] == parent) {
			ver_.closeFile(nsandpk);
			return true; 
		}
		return false;
	}

	/*----< check whether the file given is in the missing list >-----------------------*/

	bool RepositoryCore::missFile(Key key)
	{
		return (missingfiles_.count(key) == 1);
	}

	/*----< remove file from the missing list >-----------------------*/

	void RepositoryCore::notMissAnyMore(Key key)
	{
		missingfiles_.erase(key);
	}

	/*----< reflush the database and version object >-----------------------*/

	void RepositoryCore::reset()
	{
		NoSqlDb::DbCore<NoSqlDb::FileInfo> newdb;
		db_ = newdb;
		Version newver;
		ver_ = newver;
	}

}

#ifdef TEST_REPO

using namespace Repository;

int main()
{
	RepositoryCore repo;
	RepositoryCore::File file;
	file.name("first");
	file.descrip("Hello World");
	std::string key = "NoSql.h.1";
	repo.addElem(key, file);
	
	repo.showDb();

	getchar();
	getchar();

	return 0;
}

#endif