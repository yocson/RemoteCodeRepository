#pragma once
///////////////////////////////////////////////////////////////////////
// ServerPrototype.h - Console App that processes incoming messages  //
// ver 1.8                                                           //
// Platform:    Macbook Pro 15, Windows 10 on Parallel Virtual Machine//
// Application: Project3, RemoteRepositoryPrototypes,                //
//				CSE687, Object Oriented Design, Spring 2018          //
// Author:      Cheng Wang,                                          //
//              cwang76@sry.edu                                      //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018 //
///////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* ---------------------
*  Package contains one class, Server, that contains a Message-Passing Communication
*  facility. It processes each message by invoking an installed callable object
*  defined by the message's command key.
*
*  Message handling runs on a child thread, so the Server main thread is free to do
*  any necessary background processing like checkin, checkout, browse.
*
*  Required Files:
* -----------------
*  ServerPrototype.h, ServerPrototype.cpp
*  Comm.h, Comm.cpp, IComm.h
*  Message.h, Message.cpp
*  FileSystem.h, FileSystem.cpp
*  Utilities.h
*
*  Build Process:
*  --------------
*  devenv RemoteRepositoryPrototypes.sln /rebuild debug
*
*  Maintenance History:
* ----------------------
* ver 1.8 : 24 Apr 2018
* - add showdb
* ver 1.7 : 23 Apr 2018
* - fix bugs
* ver 1.6 : 22 Apr 2018
* - Add functionality for query
* ver 1.5 : 21 Apr 2018
* - Add functionality for browse
* ver 1.4 : 20 Apr 2018
* - Add functionality for checkin, checkout
* ver 1.3 : 15 Apr 2018
* - Add process functions
* ver 1.2 : 9 Apr 2018
* - Add process functions
*  ver 1.1 : 4/3/2018
*  - add process functions
*  ver 1.0 : 3/27/2018
*  - first release
*/
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <thread>
#include "../CppCommWithFileXfer/Message/Message.h"
#include "../CppCommWithFileXfer/MsgPassingComm/Comm.h"
#include <windows.h>
#include <tchar.h>
#include "../Repository/Repository/Repository.h"
#include "../Repository/NoSqlDb/PayLoad/PayLoad.h"

namespace Repository
{
  using File = std::string;
  using Files = std::vector<File>;
  using FileElem = NoSqlDb::DbElement<NoSqlDb::FileInfo>;
  using Dir = std::string;
  using Dirs = std::vector<Dir>;
  using SearchPath = std::string;
  using Key = std::string;
  using Msg = MsgPassingCommunication::Message;
  using ServerProc = std::function<Msg(Msg)>;
  using MsgDispatcher = std::unordered_map<Key,ServerProc>;
  using PackageNumber = int;
  using PackageStatus = std::string;
  using Key = std::string;
  using VersionInfo = std::pair<PackageNumber, PackageStatus>;
  
  const SearchPath storageRoot = "../Storage";  // root for all server file storage
  const MsgPassingCommunication::EndPoint serverEndPoint("localhost", 8080);  // listening endpoint

  class Server
  {
  public:
    Server(MsgPassingCommunication::EndPoint ep, const std::string& name);
    void start();
    void stop();
    void addMsgProc(Key key, ServerProc proc);
    void processMessages();
    void postMessage(MsgPassingCommunication::Message msg);
    MsgPassingCommunication::Message getMessage();
	static void showDb();
	static std::vector<Key> checkOut(const std::string& namesp, const std::string& filename, bool withdepend, int versionnum);
	static bool closeFile(const std::string& namesp, const std::string& filename);
	static FileElem& getElem(const Key& key);
	static bool addDependency(const Key& key, const std::string& depend);
	static VersionInfo getVersion(const std::string& namesp, const std::string& filename);
	static bool isOpen(const Key& key);
	static bool checkAuthor(const std::string& author, const std::string& namesp, const std::string& filename);
	static bool checkInFile(const std::string& author, const std::string& namesp, const std::string& filename, const std::string& filepath, const std::string& descrip, Msg msg);
    static Dirs getDirs(const SearchPath& path = storageRoot);
    static Files getFiles(const SearchPath& path = storageRoot);
	static std::vector<Key> andQuery(Msg msg);
	static std::vector<Key> orQuery(Msg msg);
	static std::vector<Key> getFileWithoutParents();

  private:
	MsgPassingCommunication::Comm comm_;
	MsgDispatcher dispatcher_;
	std::thread msgProcThrd_;
	static Repository::RepositoryCore repo_;
	static Repository::CheckIn checkin_;
	static Repository::CheckOut checkout_;
	static Repository::Browse browse_;
  };
  //----< initialize server endpoint and give server a name >----------

  inline Server::Server(MsgPassingCommunication::EndPoint ep, const std::string& name)
    : comm_(ep, name) {}

  //----< start server's instance of Comm >----------------------------

  inline void Server::start()
  {
    comm_.start();
  }
  //----< stop Comm instance >-----------------------------------------

  inline void Server::stop()
  {
    if(msgProcThrd_.joinable())
      msgProcThrd_.join();
    comm_.stop();
  }
  //----< pass message to Comm for sending >---------------------------

  inline void Server::postMessage(MsgPassingCommunication::Message msg)
  {
    comm_.postMessage(msg);
  }
  //----< get message from Comm >--------------------------------------

  inline MsgPassingCommunication::Message Server::getMessage()
  {
    Msg msg = comm_.getMessage();
    return msg;
  }
  //----< add ServerProc callable object to server's dispatcher >------

  inline void Server::addMsgProc(Key key, ServerProc proc)
  {
    dispatcher_[key] = proc;
  }
  //----< start processing messages on child thread >------------------

  inline void Server::processMessages()
  {
    auto proc = [&]()
    {
      if (dispatcher_.size() == 0)
      {
        std::cout << "\n  no server procs to call";
        return;
      }
      while (true)
      {
        Msg msg = getMessage();
        std::cout << "\n  received message: " << msg.command() << " from " << msg.from().toString();
        if (msg.containsKey("verbose"))
        {
          std::cout << "\n";
          msg.show();
        }
        if (msg.command() == "serverQuit")
          break;
        Msg reply = dispatcher_[msg.command()](msg);
        if (msg.to().port != msg.from().port)  // avoid infinite message loop
        {
          postMessage(reply);
          msg.show();
          reply.show();
        }
        else
          std::cout << "\n  server attempting to post to self";
      }
      std::cout << "\n  server message processing thread is shutting down";
    };
    std::thread t(proc);
    //SetThreadPriority(t.native_handle(), THREAD_PRIORITY_HIGHEST);
    std::cout << "\n  starting server thread to process messages";
    msgProcThrd_ = std::move(t);
  }
}