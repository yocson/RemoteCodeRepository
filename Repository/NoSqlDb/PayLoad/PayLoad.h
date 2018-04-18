#pragma once
/////////////////////////////////////////////////////////////////////
// PayLoad.h - Implements user-defined Payload Class               //
// ver 1.3                                                         //
// Cheng Wang, CSE687 - Object Oriented Design, Spring 2018        //
// cwang76@syr.edu												   //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides payload base class and a use-defined FileInfo
* class derived from base payload class:
* - Base class Payload is an abstract class. It asks every derived class
*	to implement two functions, fromXML and toXML to interact with XML file
*	, and compare function to realize the general querybyPayload function.
*	toString is essential too because we want to use it in the cout. Also 
*	we want to take the payload as a second parameter of makeTaggedElement,
*	so the cast to string function must be implemented by the derived class.
* - FileInfo class is the class defined in requirement 9, derived from
*	payload class. It has a filepath string and a vector of categories.
*	FileInfo implemented all functions from Payload class so it can be
*	manipulated in dbcore regardless of its inner structures.
*

* Required Files:
* ---------------
* DbCore.h, DbCore.cpp
* DateTime.h, DateTime.cpp
* Utilities.h, Utilities.cpp
* XmlParser.h, XmlParser.cpp
* XmlDocument.h, XmlDocument.cpp
*
* Maintenance History:
* --------------------
* ver 1.3 :Feb 5, 2018
* - added cast operator to string
* ver 1.2 :Feb 4, 2018
* - added abstract payload class
* ver 1.1 : Feb 3, 2018
* - overload <<
* ver 1.0 : Feb 2, 2018
* - first release
*/
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>
#include "../XmlDocument/XmlDocument/XmlDocument.h"
#include "../XmlDocument/XmlParser/XmlParser.h"
#include "IPayLoad.h"

namespace NoSqlDb {
	class FileInfo : public IPayLoad<FileInfo>
	{
	public:
		using FilePath = std::string;
		using Category = std::unordered_set<std::string>;
		using Sptr = std::shared_ptr<XmlProcessing::AbstractXmlElement>;
		using Author = std::string;

		FileInfo() {}
		FileInfo(std::shared_ptr<XmlProcessing::AbstractXmlElement> payloadTag);
		FileInfo(std::string str);

		virtual void fromXML(Sptr payloadTag);
		virtual Sptr toXML();

		virtual std::string toString() const;
		virtual operator std::string();
		// for showDb
		friend std::ostream& operator<<(std::ostream& out, const FileInfo& f);

		// bool compare(const IPayLoad & p);

		FilePath& filePath() { return filepath_; }
		FilePath filePath() const { return filepath_; }
		void filePath(const FilePath& fp) { filepath_ = fp; }

		Category& category() { return category_; }
		Category category() const { return category_; }
		void category(const Category& cate) { category_ = cate; }
		void addCate(const std::string &cat);

		bool hasCategory(const std::string& cat)
		{
			return std::find(category().begin(), category().end(), cat) != category().end();
		}

// 		bool fileStatus() { return filestatus_ ; }
// 		void fileStatus(bool fs) { filestatus_ = fs; }

		int version() { return ver_; }
		void version(int ver) { ver_ = ver; }

		Author author() { return author_; }
		void author(const Author& a) { author_ = a; }

		std::string namesp() { return namesp_; }
		void namesp(const std::string& n) { namesp_ = n; }

	private:
		FilePath filepath_;
		Category category_;
		//bool filestatus_;
		int ver_;
		Author author_;
		std::string namesp_;
	};
}
