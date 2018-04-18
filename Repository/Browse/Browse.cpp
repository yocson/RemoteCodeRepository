/////////////////////////////////////////////////////////////////////////////
// Browse.cpp - locate file and display content                            //
// ver 1.7                                                                 //
// ----------------------------------------------------------------------- //
// Language:    C++, Visual Studio 2017                                    //
// Platform:    Macbook Pro 15, Windows 10 on Parallel Virtual Machine     //
// Application: Project2, Repository,                                      //
//				CSE687, Object Oriented Design, Spring 2018                //
// Author:      Cheng Wang,                                                //
//              cwang76@sry.edu                                            //
/////////////////////////////////////////////////////////////////////////////

#include "Browse.h"


#ifdef TEST_BROWSE

int main()
{
	Repository::RepositoryCore repo;
	Repository::CheckIn checkin(repo);

	if (checkin.acceptNewFile("../Source", "Test.h", "TestName", "Yocson")) {
		checkin.addDescription("First file without any depend").addCategory("Test").addCategory("Myfile");
		checkin.showFile();
		if (checkin.checkDependency()) {
			std::cout << "save file now" << std::endl;
			checkin.saveFile("../RepositoryContents");
		}
		repo.showDb();
	}

	Repository::Browse browse(repo);
	browse.displayFileContent("TestName", "Test.h", 1);

	getchar();
	getchar();
	return 0;
}

#endif // TEST_BROWSE
