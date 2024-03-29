///////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages//
// ver 1.8                                                           //
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

Repository::RepositoryCore Server::repo_;
Repository::CheckIn Server::checkin_(Server::repo_);
Repository::CheckOut Server::checkout_(Server::repo_);
Repository::Browse Server::browse_(Server::repo_);

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

//----< convert file format to key, from namespace_filename to namespace::filename >-----------------------------------------

auto file_to_key = [](const std::string& filename) {
	std::string key = filename;
	size_t i = key.find_first_of('_');
	key[i] = ':';
	key.insert(i, ":");
	return key;
};

//----< convert key to file format, from namespace::filename to namespace_filename >-----------------------------------------

auto key_to_file = [](const std::string& key) {
	std::string filename = key;
	size_t i = filename.find_first_of(':');
	filename[i] = '_';
	filename.erase(i + 1, 1);
	return filename;
};

//----< serverproc for checkIn >-----------------------------------------

std::function<Msg(Msg)> checkAuthor = [](Msg msg) {
	Msg reply = msg; 
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("checkAuthor");
	bool passAuthor = Server::checkAuthor(msg.value("author"),  msg.value("namesp"), msg.value("filename"));
	reply.attribute("pass", std::to_string(passAuthor));
	std::cout << "\n check author " << passAuthor;
	return reply;
};

//----<  serverproc for checkInfile >-----------------------------------------

std::function<Msg(Msg)> checkInFile = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to()); 
	reply.command("checkInFile");
	reply.attribute("filename", msg.value("filename"));
	reply.attribute("namesp", msg.value("namesp"));
	
	bool canbeCheckIn = Server::checkInFile(msg.value("author"), msg.value("namesp"), msg.value("filename"), msg.value("subdir"), msg.value("description"), msg);
	VersionInfo versioninfo = Server::getVersion(msg.value("namesp"), msg.value("filename"));
	reply.attribute("versionnum", std::to_string(versioninfo.first));
	// if can't, is a close-pending file, need to be closed first
	reply.attribute("canbeCheckIn", std::to_string(canbeCheckIn));
	std::cout << "\n receive a checkin file msg \n";
	return reply;
};
//----<  get version information >-----------------------------------------

VersionInfo Server::getVersion(const std::string& namesp, const std::string& filename) {
	VersionInfo versioninfo = checkin_.getVersionInfo(namesp, filename);
	return versioninfo;
}
//----<  check single author responsiblity >-----------------------------------------

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
//----<  check in, save to database and save file >-----------------------------------------

bool Server::checkInFile(const std::string& author, const std::string& namesp, const std::string& filename, const std::string& filepath, const std::string& descrip, Msg msg) {
	bool canbeCheckIn = checkin_.acceptNewFile("../SaveFiles", filename, namesp, author);
	if (canbeCheckIn) {
		if (descrip != "") checkin_.addDescription(descrip);
		int cate_num = std::stoi(msg.value("cate_num"));
		for (int i = 0; i < cate_num; ++i) {
			checkin_.addCategory(msg.value("cate" + std::to_string(i)));
		}
		checkin_.saveFile("../Storage" + filepath);
	}
	return canbeCheckIn;
}

//----<  serverproc for addDepend >-----------------------------------------

std::function<Msg(Msg)> addDepend = [](Msg msg) {
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("addDepend");
	int num = std::stoi(msg.value("num"));

	Key key = file_to_key(msg.value("parent"));
	std::cout << key;

	FileElem fileelem = Server::getElem(key);
	std::vector<Key> depends = fileelem.children();

	Server::getElem(key).children().clear();
	for (int i = 0; i < num; ++i) {
		std::cout << file_to_key(msg.value("depend" + std::to_string(i)));
		Server::getElem(key).children().push_back(file_to_key(msg.value("depend" + std::to_string(i))));
	}
	std::cout << "\n new depends size: " << depends.size();

	return reply;
};

//----<  serverproc for getDepend >-----------------------------------------

std::function<Msg(Msg)> getDepend = [](Msg msg) {
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getDepend");
	Key key = file_to_key(msg.value("filename"));
	FileElem fileElem = Server::getElem(key);
	std::vector<std::string> depends = fileElem.children();
	std::cout << "\n Key: " << key;
	reply.attribute("dependnum", std::to_string(depends.size()));
	for (size_t i = 0; i < depends.size(); ++i) {
		reply.attribute("depend" + std::to_string(i), key_to_file(depends[i]));
	}
	return reply;
};
//----<  serverproc for viewMetaData >-----------------------------------------

std::function<Msg(Msg)> viewMetaData = [](Msg msg) {
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("viewMetaData");
	std::string key = file_to_key(msg.value("filename"));

	// make sure the file is already in the db
	FileElem fileElem = Server::getElem(key);
	reply.attribute("description", fileElem.descrip());
	reply.attribute("namesp", fileElem.payLoad().namesp());
	reply.attribute("author", fileElem.payLoad().author());
	reply.attribute("filename", fileElem.name());
	bool open =  Server::isOpen(key);
	reply.attribute("version", std::to_string(fileElem.payLoad().version()));
	reply.attribute("status", (open?"Open":"Closed"));
	reply.attribute("source", msg.value("source"));

	std::unordered_set<std::string> cates = fileElem.payLoad().category();
	int count = 0;
	reply.attribute("cate_num", std::to_string(cates.size()));
	for (std::string cate : cates) {
		reply.attribute("cate" + std::to_string(count), cate);
		++count;
	}

	std::vector<std::string> depends = fileElem.children();
	std::cout << "\n Key: " << key;
	reply.attribute("dependnum", std::to_string(depends.size()));
	for (size_t i = 0; i < depends.size(); ++i) {
		reply.attribute("depend" + std::to_string(i), depends[i]);
	}
	return reply;
};
//----<  return whethter a file is open or not >-----------------------------------------

bool Server::isOpen(const Key& key) {
	return repo_.version().isOpen(key);
}
//----<  get a db element >-----------------------------------------

FileElem& Server::getElem(const Key& key) {
	return repo_.getElem(key);
}
//----<  serverproc for closeFile >-----------------------------------------

std::function<Msg(Msg)> closeFile = [](Msg msg) {
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	std::string file = msg.value("filename");
	FileElem fileelem = Server::getElem(file_to_key(file));
	std::string namesp = fileelem.payLoad().namesp();
	std::string filename = fileelem.name();
	bool closed = true;
	VersionInfo versioninfo = Server::getVersion(namesp, filename);
	size_t i = file.find_last_of(".");
	std::string versionnumstr = file.substr(i + 1, 100);
	int versionnum = std::stoi(versionnumstr);
	if (versionnum == versioninfo.first) {
		closed = Server::closeFile(namesp, filename);
	}
	reply.command("closeFile");
	reply.attribute("closed", std::to_string(closed));
	return reply;
};

//----<  close a file in the database >-----------------------------------------

bool Server::closeFile(const std::string& namesp, const std::string&filename) {
	return repo_.closeFile(namesp, filename);
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
	std::string key = file_to_key(msg.value("filename"));

	FileElem fileelem = Server::getElem(key);
	std::string namesp = fileelem.payLoad().namesp();
	std::string filename = fileelem.name();
	std::string path = msg.value("filepath");
	reply.file(filename);
	reply.attribute("filepath", msg.value("filepath"));
	bool withDepend = (msg.value("with_depend") == "True") ? true : false;
	reply.attribute("with_depend", msg.value("with_depend"));
	size_t i = key.find_last_of(".");
	std::string versionnumstr = key.substr(i + 1, 100);
	int versionnum = std::stoi(versionnumstr);
	reply.attribute("filename", msg.value("filename"));
	std::vector<Key> allDepends = Server::checkOut(namesp, filename, withDepend, versionnum);
	if (withDepend) {
		reply.attribute("dependnum", std::to_string(allDepends.size()));
		for (size_t i = 0; i < allDepends.size(); ++i) {
			std::string dependKey = key_to_file(allDepends[i]);
			std::cout << dependKey;
			reply.attribute("depend" + std::to_string(i), dependKey);
		}
	}
	return reply;
};

//----<  check out a file >-----------------------------------------

std::vector<Key> Server::checkOut(const std::string& namesp, const std::string& filename, bool withdepend, int versionnum) {
	checkout_.checkOutFile(namesp, filename, versionnum, withdepend);
	std::cout << "\n Checkout Files";
	checkout_.copyFiles("../SendFiles", "../Storage");
	return checkout_.keys();
}

//----< serverproc for query >-----------------------------------------

std::function<Msg(Msg)> query = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("query");
	std::vector<Key> result;
	if (msg.value("isAndQuery") == "True") {
		result = Server::andQuery(msg);
	}
	else {
		result = Server::orQuery(msg);
	}
	reply.attribute("num", std::to_string(result.size()));
	for (size_t i = 0; i < result.size(); ++i) {
		reply.attribute("result" + std::to_string(i), result[i]);
	}
	return reply;
};
//----<  do add query by conditions >-----------------------------------------

std::vector<Key> Server::andQuery(Msg msg) {
	int num = std::stoi(msg.value("num"));
	NoSqlDb::Query<NoSqlDb::FileInfo> q(repo_.database());

	if (msg.value("noparent") == "True") {
		q.from(Server::getFileWithoutParents());
	}
	NoSqlDb::Condition c;
	for (int i = 0; i < num; ++i) {
		std::string ConName = msg.value("ConName" + std::to_string(i));
		std::string matchCon = msg.value("MatchCon" + std::to_string(i));
		std::string condi = msg.value("Condition" + std::to_string(i));
		
		if (matchCon == "includes") { condi = ".*" + condi + ".*"; }
		if (ConName == "Name") {
			c.name(condi);
			q.select(c);
		}
		if (ConName == "Description") {
			c.description(condi);
			q.select(c);
		}
		if (ConName == "Author") {
			auto authorS = [&condi](DbElement<NoSqlDb::FileInfo>& elem) { return (elem.payLoad().author() == condi); };
			q.select(authorS);
		}
		if (ConName == "Category") {
			auto hasCategory = [&condi](DbElement<NoSqlDb::FileInfo>& elem) { return (elem.payLoad()).hasCategory(condi); };
			q.select(hasCategory);
		}
		if (ConName == "Namespace") {
			auto namespS = [&condi](DbElement<NoSqlDb::FileInfo>& elem) { return (elem.payLoad().namesp() == condi); };
			q.select(namespS);
		}
		if (ConName == "Dependency") {
			auto dependS = [&condi](DbElement<NoSqlDb::FileInfo>& elem) { return elem.hasDepend(condi); };
			q.select(dependS);
		}
	}
	return q.keys();
}
//----<  do or query by conditions >-----------------------------------------

std::vector<Key> Server::orQuery(Msg msg) {
	int num = std::stoi(msg.value("num"));
	NoSqlDb::Query<NoSqlDb::FileInfo> q1(repo_.database());
	NoSqlDb::Query<NoSqlDb::FileInfo> q2(repo_.database());
	if (msg.value("noparent") == "True") {
		q1.from(Server::getFileWithoutParents());
		q2.from(Server::getFileWithoutParents());
	}
	std::vector<Key> empty_key;
	q2.from(empty_key);
	NoSqlDb::Condition c;
	for (int i = 0; i < num; ++i) {
		std::string ConName = msg.value("ConName" + std::to_string(i));
		std::string matchCon = msg.value("MatchCon" + std::to_string(i));
		std::string condi = msg.value("Condition" + std::to_string(i));

		if (matchCon == "includes") { condi = ".*" + condi + ".*"; }
		if (ConName == "Name") {
			c.name(condi);
			q1.select(c);
		}
		if (ConName == "Description") {
			c.description(condi);
			q1.select(c);
		}
		if (ConName == "Author") {
			auto authorS = [&condi](DbElement<NoSqlDb::FileInfo>& elem) { return (elem.payLoad().author() == condi); };
			q1.select(authorS);
		}
		if (ConName == "Category") {
			auto hasCategory = [&condi](DbElement<NoSqlDb::FileInfo>& elem) { return (elem.payLoad()).hasCategory(condi); };
			q1.select(hasCategory);
		}
		if (ConName == "Namespace") {
			auto namespS = [&condi](DbElement<NoSqlDb::FileInfo>& elem) { return (elem.payLoad().namesp() == condi); };
			q1.select(namespS);
		}
		q2.unionFrom(q1.keys(), q2.keys());
		q1.reset();
	}
	return q2.keys();
}
//----<  get file collection with no parent >-----------------------------------------

std::vector<Key> Server::getFileWithoutParents() {
	NoSqlDb::DbCore<NoSqlDb::FileInfo> db = repo_.database();
	std::vector<Key> res = db.keys();
	NoSqlDb::DbCore<NoSqlDb::FileInfo>::iterator it = db.begin();
	while (it != db.end()) {
		for (Key child : it->second.children()) {
			std::vector<Key>::iterator vit = find(res.begin(), res.end(), child);
			if (vit != res.end()) {
				res.erase(vit);
			}
		}
		it++;
	}
	return res;

}

//----< serverproc for sendfile >-----------------------------------------

std::function<Msg(Msg)> sendFile = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
 	reply.command("sendFile");
	std::string filename = "";
	std::string path = "";
	if (msg.value("source") == "querylist") {
		filename = key_to_file(msg.value("key"));
		path = Server::getElem(msg.value("key")).payLoad().filePath();
	}
	else {
		filename = msg.attributes()["filename"];
		path = msg.attributes()["path"];
	}
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
//----< serverproc for showdb, a one-way msg >-----------------------------------------

std::function<Msg(Msg)> showdb = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("showdb");
	Server::showDb();
	return reply;
};

//----< show database >-----------------------------------------

void Server::showDb() {
	repo_.showDb();
}

int main()
{
  std::cout << "\n  Testing Server Prototype";
  std::cout << "\n ==========================";
  std::cout << "\n";

	//StaticLogger<1>::attach(&std::cout);
	//StaticLogger<1>::start();
  // for test
  FileSystem::File::remove("../Storage/SOCKET_Message.cpp.2");
  Server server(serverEndPoint, "ServerPrototype");
  server.start();
  SetConsoleTitleA("Project#4 Server console");
  std::cout << "\n  testing getFiles and getDirs methods";
  std::cout << "\n --------------------------------------";
  Files files = server.getFiles();
  show(files, "Files:");
  Dirs dirs = server.getDirs();
  show(dirs, "Dirs:");
  std::cout << "\n";



  std::cout << "\n  testing message processing";
  std::cout << "\n ----------------------------";
  server.addMsgProc("echo", echo);
  server.addMsgProc("getFiles", getFiles);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("serverQuit", echo);
  server.addMsgProc("checkAuthor", checkAuthor);
  server.addMsgProc("checkInFile", checkInFile);
  server.addMsgProc("checkOut", checkOut);
  server.addMsgProc("query", query);
  server.addMsgProc("connect", connected);
  server.addMsgProc("sendFile", sendFile);
  server.addMsgProc("addDepend", addDepend);
  server.addMsgProc("viewMetaData", viewMetaData);
  server.addMsgProc("closeFile", closeFile);
  server.addMsgProc("getDepend", getDepend);
  server.addMsgProc("showdb", showdb);
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

