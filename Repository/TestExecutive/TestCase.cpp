/////////////////////////////////////////////////////////////////////////////
// TestCase.h - all test cases for repository project                      //
// ver 1.3                                                                 //
// ----------------------------------------------------------------------- //
// Language:    C++, Visual Studio 2017                                    //
// Platform:    Macbook Pro 15, Windows 10 on Parallel Virtual Machine     //
// Application: Project2, Repository,                                      //
//				CSE687, Object Oriented Design, Spring 2018                //
// Author:      Cheng Wang,                                                //
//              cwang76@sry.edu                                            //
/////////////////////////////////////////////////////////////////////////////

#include "TestCase.h"
#include "../Browse/Browse.h"

namespace Repository
{

	auto putLine = [](size_t n = 1, std::ostream& out = std::cout)
	{
		Utilities::putline(n, out);
	};

	auto printLine = [](std::string content)
	{
		std::cout << " * " << content << std::endl;
	};

	// declare static member data
	Repository::RepositoryCore TestCase::repo_;

	bool TestCase::testR1()
	{
		Utilities::title("Demonstrating Requirement #1");
		std::cout << "\n   *" << typeid(std::function<bool()>).name()
			<< ", declared in this function, "
			<< "\n   *is only valid for C++11 and later versions.";
		putLine();
		return true; // would not compile unless C++11
	}

	bool TestCase::testR2()
	{
		Utilities::title("Demonstrating Requirement #2");
		putLine();
		std::cout << "\n   *A visual examination of all the submitted code "
			<< "will show only\n   *use of streams and operators new and delete.";
		putLine();
		return true;
	}

	bool TestCase::testR3a()
	{
		Utilities::title("Demonstrating Requirement #3 - packages");
		putLine();
		printLine("This Project has following packages:");
		printLine("RepositoryCore, CheckIn, CheckOut, Browse, Version, TestExecutive, Process, NoSql, PayLoad, FileSystem");
		printLine("This project is based on the project1 - NoSqlDb.");
		return true;
	}

	bool TestCase::testR3b()
	{
		Utilities::title("Demonstrating Requirement #3b - construct db inside repository");
		putLine();
		printLine("Create a dummy record to show db is working");
		RepositoryCore::File file;
		file.name("HelloWorld.h");
		file.descrip("Hello World");
		NoSqlDb::FileInfo f;
		f.addCate("Test");
		f.author("Cheng");
		f.version(1);
		f.namesp("Project");
		f.filePath("C://test");
		file.payLoad(f);
		std::string key = "Dummy::Test.h.1";
		repo_.addElem(key, file);
		repo_.showDb();

		return true;
	}

	bool TestCase::testR3c()
	{
		Utilities::title("Demonstrating Requirement #3c - checkin file");
		putLine();
		Repository::CheckIn checkin(repo_);
		repo_.reset();
		printLine("To create two files:");
		printLine("Project::Test.h.1 and Project::Test.cpp.1");
		printLine("These two are mutual depended, when we checkin one file first and then another, we store the missing dependency");
		if (checkin.acceptNewFile("../Source", "Test.h", "Project", "Cheng")) {
			checkin.addDescription("First file").addCategory("Test").addCategory("Project2").addDependency("Project::Test.cpp.1");
			checkin.showFile();
			checkin.checkDependency();
			checkin.saveFile("../RepositoryContents/Project2/Test");
			repo_.showDb();
		}
		putLine();

		// demonstrate single user policy
		printLine("Checkin Project::Test.h.1 again using another user,");
		printLine("Failed because the single user policy");
		if (checkin.acceptNewFile("../Source", "Test.h", "Project", "SomeOne")) {
			return false;
		}
		repo_.showDb();
		putLine();

		printLine("We store the missing dependency in a set");
		if (repo_.missFile("Project::Test.cpp.1")) {
			printLine("Miss one dependency file Project::Test.cpp.1");
		}

		printLine("Then checkin Project::Test.cpp.1");
		if (checkin.acceptNewFile("../Source", "Test.cpp", "Project", "Cheng")) {
			checkin.addDescription("Second file").addCategory("Test").addCategory("Project2").addDependency("Project::Test.h.1");
			checkin.checkDependency();
			checkin.showFile();
			checkin.saveFile("../RepositoryContents/Project2/Test");
			repo_.showDb();
		}

		if (repo_.missFile("Project::Test.cpp.1")) {
			printLine("Now check missing file set again, there is no Project::Test.cpp.1 any more");
			return false;
		}
		return true;
	}

	bool TestCase::testR3d()
	{
		Utilities::title("Demonstrating Requirement #3d - checkout file");
		putLine();

		Repository::CheckOut checkout(repo_);

		printLine("Checkout Project::Test.cpp.1 with dependency");
		printLine("Two files will be checked out in 3d");
		printLine("This function will run again after 5, so 3 files will be checked out.");
		checkout.checkOutFile("Project", "Test.cpp", 0);
		checkout.copyFiles("../CheckOutFiles", "../RepositoryContents");

		return true;
	}

	bool TestCase::testR3e()
	{
		Utilities::title("Demonstrating Requirement #3d - Demonstrate Version core features");
		putLine();

		printLine("Version can check whether file is existed.");
		Repository::Version v;
		if (!v.hasFile("nothing")) {
			printLine("There is no package called nothing inside");
		}
		printLine("Version can update file number.");
		v.updateVersion("NoSql", "DbCore.h", 1);
		Repository::Version::VersionInfo vi = v.checkVersion("NoSql::DbCore.h");
		printLine("NoSql::DbCore.h's version is");
		std::cout << vi.first << std::endl;
		printLine("NoSql::DbCore.h'' status is");
		printLine(vi.second);

		std::string depend = "NoSql::DbCore.h.1";
		printLine("Version can check dependency's status");
		printLine("Dependency is " + depend);
		if (v.isOpen(depend)) printLine("Dependency is open");

		if (v.hasFile("NoSql::DbCore.h", 1)) printLine("Has NoSql::DbCore.h.1");
		v.closeFile("NoSql", "DbCore.h", true);
		printLine("Dependency is " + depend);
		if (v.isOpen(depend)) printLine("Dependency is open");
		else printLine("Dependency is closed");

		return true;
	}

	bool TestCase::testR3f()
	{
		Utilities::title("Demonstrating Requirement #3f - basic Browse");
		putLine();

		printLine("More detailed demonstration about browse will be shown in requirement 5");

 		Repository::Browse browse(repo_);
 		
		printLine("Here display one file with exact key");
		browse.displayFileContent("Project", "Test.h", 1);

		return true;
	}

	bool TestCase::testR4a()
	{
		Utilities::title("Demonstrating Requirement #4 - CheckIn part a check in and close mutual dependency");
		putLine();

		printLine("In requirement 3c, we have created two files:");
		printLine("Project::Test.h.1 and Project::Test.cpp.1");
		printLine("Now we close this two files");

		if (repo_.closeFile("Project", "Test.h")) { printLine("Project::Test.h.1 closed");}
		else { printLine("Project::Test.h.1 is not closed because cpp is open");}
		printLine("Project::Test.h.1 is now:");
		printLine(repo_.version().checkVersion("Project::Test.h").second);
		if (repo_.closeFile("Project", "Test.cpp")) { printLine("Project::Test.cpp.1 is closed"); }
		else { printLine("Project::Test.cpp.1 is open"); }

		if (repo_.version().isOpen("Project::Test.h.1") || repo_.version().isOpen("Project::Test.cpp.1")) {
			printLine("Both files are closed, because Project::Test.h.1 is closed when we close Project::Test.cpp.1");
			return false;
		}

		CheckIn checkin(repo_);

		printLine("Checkin Project::Test.h and .cpp again, now there will be Project::Test.h.2 and Project::Test.cpp.2");

		if (checkin.acceptNewFile("../Source", "Test.h", "Project", "Cheng")) {
			checkin.addDescription("test h version 2");
			checkin.checkDependency();
			checkin.saveFile("../RepositoryContents/Project2/Test");
		}
		if (checkin.acceptNewFile("../Source", "Test.cpp", "Project", "Cheng")) {
			checkin.addDescription("test cpp version 2");
			checkin.checkDependency();
			checkin.saveFile("../RepositoryContents/Project2/Test");
		}
		repo_.showDb();

		/*with this two files open, we modify the cpp file.*/
		printLine("Demonstrate changing file information without altering version number");
		if (checkin.acceptNewFile("../Source", "Test.cpp", "Project", "Cheng")) {
			printLine("Add dependency: Utility::Utility.h.1");
			checkin.addDescription("Change description").addDependency("Utility::Utility.h.1");
			checkin.saveFile("../RepositoryContents/Project2/Test");
		}
		repo_.showDb();
		return true;
	}

	bool TestCase::testR4b()
	{
		Utilities::title("Demonstrating Requirement #4b - CheckIn part b Checkin and close");
		putLine();

		CheckIn checkin(repo_);

		printLine("Check in Utility::Utility.h");
		if (checkin.acceptNewFile("../Source", "Utility.h", "Utility", "Cheng")) {
			checkin.addDescription("A utility file");
			checkin.checkDependency();
			checkin.saveFile("../RepositoryContents/Utility");
		}
		repo_.showDb();

		printLine("Now try to close Project::Test.cpp.2");
		if (repo_.closeFile("Project", "Test.cpp")) { printLine("Project::Test.cpp.2 closed"); }
		else {
			printLine("Project::Test.cpp.2 can not be closed");
			printLine("Project::Test.cpp.2 has a open dependcy Utility::Utility.h.1");
		}
		printLine("Project::Test.cpp.2 's status is ");
		printLine(repo_.version().checkVersion("Project::Test.cpp").second);

		printLine("Close Utility::Utility.h.1");
		if (repo_.closeFile("Utility", "Utility.h")) { printLine("Utility::Utility.h.1 closed"); }
		printLine("Now we can close Project::Test.cpp.2");
		if (repo_.closeFile("Project", "Test.cpp")) { printLine("Project::Test.cpp.2 closed"); }
		printLine("Project::Test.cpp.2 is " + repo_.version().checkVersion("Utility::Utility.h").second);
		printLine("Utility::Utility.h.1 " + repo_.version().checkVersion("Project::Test.cpp").second);

		return true;
	}

	bool TestCase::testR4c()
	{
		Utilities::title("Demonstrating Requirement #4c - CheckIn part c, more files");
		putLine();

		CheckIn checkin(repo_);

		printLine("Checkin Project::main.cpp");
		if (checkin.acceptNewFile("../Source", "main.cpp", "Project", "Cheng")) {
			checkin.addDescription("main file");
			checkin.addCategory("Executive").addDependency("Project::Test.h.2").addDependency("Utility::Utility.h.1");
			checkin.checkDependency();
			checkin.saveFile("../RepositoryContents/Project2/");
		}

		repo_.showDb();

		printLine("Checkin Project::PayLoad.cpp");
		if (checkin.acceptNewFile("../Source", "PayLoad.cpp", "Project", "Cheng")) {
			checkin.addDescription("main file");
			checkin.addCategory("Project1").addDependency("Project::PayLoad.h.1").addDependency("Project::IPayLoad.h.1");
			checkin.checkDependency();
			checkin.saveFile("../RepositoryContents/Project1/PayLoad");
		}

		printLine("Checkin Project::PayLoad.h");
		if (checkin.acceptNewFile("../Source", "PayLoad.h", "Project", "Cheng")) {
			checkin.addDescription("main file");
			checkin.addCategory("Project1").addDependency("Project::PayLoad.cpp.1");
			checkin.checkDependency();
			checkin.saveFile("../RepositoryContents/Project1/PayLoad");
		}

		printLine("Checkin Project::IPayLoad.h");
		if (checkin.acceptNewFile("../Source", "IPayLoad.h", "Project", "Cheng")) {
			checkin.addDescription("main file");
			checkin.checkDependency();
			checkin.saveFile("../RepositoryContents/Project1/PayLoad");
		}

		if (repo_.closeFile("Project", "IPayLoad.h")) { printLine("Project::IPayLoad.h is closed"); }
		else { return false; }

		return true;
	}

	bool TestCase::testR5()
	{
		Utilities::title("Demonstrating Requirement #5 - Browse");
		putLine();

		Repository::Browse browse(repo_);
		printLine("Display Project::Test.cpp");
		browse.displayFileContent("Project", "Test.cpp", 2);
		NoSqlDb::Query<NoSqlDb::FileInfo> q(repo_.database());

		NoSqlDb::Condition c1;
		c1.name(".*Test.*");
		q.select(c1).show();
		putLine();

		printLine("Display all files with 'Test' in the name");
		browse.getKeys(q.keys());
		browse.displayAllFiles();

		std::string category = "Executive";
		printLine( "\n  select on payload categories for \"" + category);

		auto hasCategory = [&category](DbElement<FileInfo>& elem) { return (elem.payLoad()).hasCategory(category); };

		q.reset();
		q.select(hasCategory).show();

		putLine();
		printLine("Display all files with category 'Executive'");
		browse.getKeys(q.keys());
		browse.displayAllFiles();

		std::string fp = "../RepositoryContents/Utility";
		auto fileP = [&fp](DbElement<FileInfo>& elem) { return (elem.payLoad()).filePath() == fp; };

		q.reset();
		q.select(fileP).show();
		putLine();
		putLine();
		printLine("Display all files in directory '../RepositoryContents/Utility'");

		browse.getKeys(q.keys());
		browse.displayAllFiles();

		return true;
	}

	bool TestCase::testR6()
	{
		Utilities::title("Demonstrating Requirement #6 - Provide test files");
		putLine();
		
		printLine("This project is tested with following files:");
		printLine("../RepositoryContents/Project2/Test/Project::Test.h.1 " + repo_.version().getStatus("Project::Test.h.1"));
		printLine("../RepositoryContents/Project2/Test/Project::Test.cpp.1 " + repo_.version().getStatus("Project::Test.cpp.1"));
		printLine("../RepositoryContents/Project2/Test/Project::Test.h.2 " + repo_.version().getStatus("Project::Test.h.1"));
		printLine("../RepositoryContents/Project2/Test/Project::Test.cpp.2 " + repo_.version().getStatus("Project::Test.cpp.2"));
		printLine("../RepositoryContents/Utility::Utility.h.1 " + repo_.version().getStatus("Utility::Utility.h.1"));
		printLine("../RepositoryContents/Project1/PayLoad/Project::PayLoad.h.1 " + repo_.version().getStatus("Project::PayLoad.h.1"));
		printLine("../RepositoryContents/Project1/PayLoad/Project::PayLoad.cpp.1 " + repo_.version().getStatus("Project::PayLoad.cpp.1"));
		printLine("../RepositoryContents/Project1/PayLoad/Project::IPayLoad.h.1 " + repo_.version().getStatus("Project::IPayLoad.h.1"));
		printLine("Files are checked in from '../Source'");
		printLine("Repository root folder is '../RepositoryContents'");
		printLine("Files are checked out to '../CheckOutFiles'");

		return true;
	}

}