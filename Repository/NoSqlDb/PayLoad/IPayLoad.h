#pragma once
/////////////////////////////////////////////////////////////////////
// IPayLoad.h - Implements Payload Class Interface              //
// ver 1.3                                                         //
// Cheng Wang, CSE687 - Object Oriented Design, Spring 2018        //
// cwang76@syr.edu												   //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides payload base interface:
*
* Required Files:
* ---------------
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

#include "../XmlDocument/XmlElement/XmlElement.h"

namespace NoSqlDb
{
	template<typename P>
	struct IPayLoad
	{
		using Xml = std::string;
		using Sptr = std::shared_ptr<XmlProcessing::AbstractXmlElement>;

		// deal with XML format
		virtual void fromXML(Sptr payloadTag) = 0;
		virtual Sptr toXML() = 0;

		// for cout overloading and for compatibility with saveToXML function
		virtual std::string toString() const = 0;
		virtual operator std::string() = 0;
		virtual ~IPayLoad() {};
	};
}