#pragma once

namespace NoSqlDb
{
#include "../XmlDocument/XmlElement/XmlElement.h"

    using Xml = std::string;
    using Sptr = std::shared_ptr<XmlProcessing::AbstractXmlElement>;

    // template<typename P>
    // struct IPayLoad
    // {
    //     virtual Sptr toXmlElement() = 0;
    //     static P fromXmlElement(Sptr elem);
    //     virtual ~IPayLoad() {};
    // };

	template<typename P>
	struct IPayLoad
	{
		// deal with XML format
		virtual void fromXML(Sptr payloadTag) = 0;
		virtual Sptr toXML() = 0;

		// for cout overloading and for compatibility with saveToXML function
		virtual std::string toString() const = 0;
		virtual operator std::string() = 0;
		virtual ~IPayLoad() {};
	};
}