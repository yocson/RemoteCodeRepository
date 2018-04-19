///////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 1.2                                                           //
// Platform:    Macbook Pro 15, Windows 10 on Parallel Virtual Machine//
// Application: Project3, RemoteRepositoryPrototypes,                //
//				CSE687, Object Oriented Design, Spring 2018          //
// Author:      Cheng Wang,                                          //
//              cwang76@sry.edu                                      //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018 //
///////////////////////////////////////////////////////////////////////

#include "ServerPrototype.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include <chrono>

namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

//----< getfiles for given filepath >-----------------------------------------

Files Server::getFiles(const Repository::SearchPath& path)
{
	return Directory::getFiles(path);
}
//----< getdirs for given filepath >-----------------------------------------

Dirs Server::getDirs(const Repository::SearchPath& path)
{
	return Directory::getDirectories(path);
}

//----< show all contents of msg >-----------------------------------------

template<typename T>
void show(const T& t, const std::string& msg)
{
	std::cout << "\n  " << msg.c_str();
	for (auto item : t)
	{
		std::cout << "\n    " << item.c_str();
	}
}

bool Server::checkAuthor(const std::string& author, const std::string& namesp, const std::string& filename) {
	if (author == "" || namesp == "" || filename == "") return false;
	VersionInfo versioninfo = checkin_.getVersionInfo(namesp, filename);
	std::cout << versioninfo.first;
	if (versioninfo.first != 0) {
		Key key = namesp + "::" + filename + "." + std::to_string(versioninfo.first);
		NoSqlDb::DbElement<NoSqlDb::FileInfo> fileelem = repo_.getElem(key);
		if (author != fileelem.payLoad().author()) {
			std::cout << "This user has no access to the file." << std::endl;
			return false;
		}
	}
	return true;
}

//----< serverproc for echo >-----------------------------------------

std::function<Msg(Msg)> echo = [](Msg msg) {
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	return reply;
};

//----< serverproc for getfiles >-----------------------------------------

std::function<Msg(Msg)> getFiles = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getFiles");
	std::string path = msg.value("path");
	reply.attribute("listname", msg.value("listname"));
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Files files = Server::getFiles(searchPath);
		size_t count = 0;
		for (auto item : files)
		{
			std::string countStr = Utilities::Converter<size_t>::toString(++count);
			reply.attribute("file" + countStr, item);
		}
	}
	else
	{
		std::cout << "\n  getFiles message did not define a path attribute";
	}
	return reply;
};

//----< serverproc for getDirs >-----------------------------------------

std::function<Msg(Msg)> getDirs = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getDirs");
	std::string path = msg.value("path");
	reply.attribute("listname", msg.value("listname"));
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Files dirs = Server::getDirs(searchPath);
		size_t count = 0;
		for (auto item : dirs)
		{
			if (item != ".." && item != ".")
			{
				std::string countStr = Utilities::Converter<size_t>::toString(++count);
				reply.attribute("dir" + countStr, item);
			}
		}
	}
	else
	{
		std::cout << "\n  getDirs message did not define a path attribute";
	}
	return reply;
};


//----< serverproc for checkOut >-----------------------------------------

std::function<Msg(Msg)> checkOut = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("checkOut");
	std::cout << "\n check out";
	return reply;
};
//----< serverproc for browse >-----------------------------------------

std::function<Msg(Msg)> browse = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("browse");
	std::cout << "\n browse";
	return reply;
};
//----< serverproc for sendfile >-----------------------------------------

std::function<Msg(Msg)> sendFile = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
 	reply.command("sendFile");
	std::string filename = msg.attributes()["filename"];
	std::cout << filename;
	std::string path = msg.attributes()["path"];
/*	std::cout << path;*/
	FileSystem::File::copy(path + "/" + filename, "../SendFiles/" + filename);
	reply.file(filename);
	std::cout << "\n sendFile";
	return reply;
};
//----< serverproc for connected >-----------------------------------------

std::function<Msg(Msg)> connected = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("connect");
	std::cout << "\n connect";
	return reply;
};
//----< serverproc for viewdata >-----------------------------------------

std::function<Msg(Msg)> viewdata = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("viewdata");
	std::cout << "\n viewdata";
	return reply;
};


int main()
{
  std::cout << "\n  Testing Server Prototype";
  std::cout << "\n ==========================";
  std::cout << "\n";

	//StaticLogger<1>::attach(&std::cout);
	//StaticLogger<1>::start();

  Server server(serverEndPoint, "ServerPrototype");
  server.start();
  SetConsoleTitleA("Project#3 Server console");
  std::cout << "\n  testing getFiles and getDirs methods";
  std::cout << "\n --------------------------------------";
  Files files = server.getFiles();
  show(files, "Files:");
  Dirs dirs = server.getDirs();
  show(dirs, "Dirs:");
  std::cout << "\n";

  //----< serverproc for checkIn >-----------------------------------------

  std::function<Msg(Msg)> checkIn = [&server](Msg msg) {
	  Msg reply;
	  reply.to(msg.from());
	  reply.from(msg.to());
	  reply.command("checkIn");
	  bool passAuthor = server.checkAuthor(msg.value("author"), msg.value("filename"), msg.value("namesp"));
	  std::cout << "\n check author " << passAuthor;
	  return reply;
  };

  std::cout << "\n  testing message processing";
  std::cout << "\n ----------------------------";
  server.addMsgProc("echo", echo);
  server.addMsgProc("getFiles", getFiles);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("serverQuit", echo);
  server.addMsgProc("checkIn", checkIn);
  server.addMsgProc("checkOut", checkOut);
  server.addMsgProc("browse", browse);
  server.addMsgProc("connect", connected);
  server.addMsgProc("sendFile", sendFile);
  server.addMsgProc("viewdata", viewdata);
  server.processMessages();
  
  Msg msg(serverEndPoint, serverEndPoint);  // send to self

  std::cout << "\n  press enter to exit";
  std::cin.get();
  std::cout << "\n";

  msg.command("serverQuit");
  server.postMessage(msg);
  server.stop();
  return 0;
}

