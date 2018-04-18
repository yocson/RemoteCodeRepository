#pragma once
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