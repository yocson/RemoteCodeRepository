/////////////////////////////////////////////////////////////////////////////
// Version.cpp - Manage version information in repository                  //
// ver 1.4                                                                 //
// ----------------------------------------------------------------------- //
// Language:    C++, Visual Studio 2017                                    //
// Platform:    Macbook Pro 15, Windows 10 on Parallel Virtual Machine     //
// Application: Project2, Repository,                                      //
//				CSE687, Object Oriented Design, Spring 2018                //
// Author:      Cheng Wang,                                                //
//              cwang76@sry.edu                                            //
/////////////////////////////////////////////////////////////////////////////

#include "Version.h"

namespace Repository
{
	/*----< check whether the package exist >-----------------------*/
    // if so, return its versioninfo with version number and status
    // otherwise, return version = 0, and closed
    Repository::Version::VersionInfo Version::checkVersion(const std::string& nsandpk)
	{
        if (versions_.find(nsandpk) != versions_.end()) {
            return versions_[nsandpk];
        } else {
            Version::VersionInfo newPak;
            newPak.first = 0;
            newPak.second = "closed";
            return newPak;
        }
    }

	/*----< add version information to version's map >-----------------------*/
    void Version::updateVersion(const std::string& namesp, const std::string& filename, int vernum)
    {
		std::string nsandpk;
		nsandpk = namesp + "::" + filename;
        versions_[nsandpk] = std::make_pair(vernum, "open");
    }

	/*----< check whether the respository has the false >-----------------------*/

    bool Version::hasFile(const std::string& nsandpk, int versionnum)
    {
		VersionInfo versioninfo = checkVersion(nsandpk);
		if (versioninfo.first != 0 && versionnum <= versioninfo.first) {
			return true;
		}
		return false;
    }

	/*----< close file, either becomes closed or close-pending >-----------------------*/

    void Version::closeFile(const std::string& namesp, const std::string& filename, bool canbeclosed)
    {
        std::string nsandpk = namesp + "::" + filename;
		if (canbeclosed) { versions_[nsandpk].second = "closed"; }
		else { versions_[nsandpk].second = "close-pending"; }
    }

	/*----< close file, either becomes closed or close-pending >-----------------------*/

	void Version::closeFile(const std::string& nsandpk, bool canbeclosed /*= true*/)
	{
		if (canbeclosed) { versions_[nsandpk].second = "closed"; }
		else { versions_[nsandpk].second = "close-pending"; }
	}

	/*----< check whether a file is open or not >-----------------------*/

	bool Version::isOpen(const std::string& depend)
    {
		//no need to check if exist
        std::string nsandpk = "";
        int versionnum = 0;
        size_t pos = depend.find_last_of('.');
        versionnum = std::stoi(depend.substr(pos + 1));
        nsandpk = depend.substr(0, pos);
        VersionInfo versioninfo = versions_[nsandpk];
        if (versioninfo.first == versionnum){
            return (versioninfo.second == "open");
        } else {
            return false;
        }
    }

	/*----< get the file status, open, closed or close-pending >-----------------------*/

	std::string Version::getStatus(const std::string& depend)
	{
		std::string nsandpk = "";
		int versionnum = 0;
		size_t pos = depend.find_last_of('.');
		versionnum = std::stoi(depend.substr(pos + 1));
		nsandpk = depend.substr(0, pos);
		VersionInfo versioninfo = versions_[nsandpk];
		if (versioninfo.first == versionnum) {
			return versioninfo.second;
		}
		else {
			return "closed";
		}
	}


}

#ifdef TEST_VERSION

int main()
{
	Repository::Version v;
	if (!v.hasFile("nothing")) {
		std::cout << "There is no package called nothing inside" << std::endl;
	}
	v.updateVersion("NoSql", "DbCore.h", 2);
	Repository::Version::VersionInfo vi = v.checkVersion("NoSql");
	std::cout << vi.first << std::endl;
	std::cout << vi.second << std::endl;

	std::string depend = "NoSql::DbCore.h.2";
	std::cout << "Dependency is " << depend << std::endl;
	if (v.isOpen(depend)) std::cout << "Dependency is open" << std::endl;

	if (v.hasFile("NoSql::DbCore")) std::cout << "Has NoSql::DbCore" << std::endl;
	v.closeFile("NoSql", "DbCore.h");
	std::cout << "Dependency is " << depend << std::endl;
	if (v.isOpen(depend)) std::cout << "Dependency is open" << std::endl;
	else std::cout << "Dependency is closed" << std::endl;

	getchar();
	getchar();

	return 0;
}

#endif // TEST_VERSION
