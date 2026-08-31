#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include <iostream>
#include <string_view>

namespace {

class XercesRuntime final {
public:
   XercesRuntime() {
      xercesc::XMLPlatformUtils::Initialize();
   }

   ~XercesRuntime() {
      xercesc::XMLPlatformUtils::Terminate();
   }

   XercesRuntime(XercesRuntime const&) = delete;
   XercesRuntime& operator=(XercesRuntime const&) = delete;
};

[[nodiscard]] bool XmlEquals(XMLCh const* const pValue, char const* const pExpected) {
   char* pText = xercesc::XMLString::transcode(pValue);
   if(!pText) return false;
   bool const bResult = std::string_view { pText } == pExpected;
   xercesc::XMLString::release(&pText);
   return bResult;
}

[[nodiscard]] bool RunSmoke() {
   XercesRuntime const theRuntime;

   static constexpr char arrXml[] =
      "<?xml version=\"1.0\"?><buildengine><value>42</value></buildengine>";

   xercesc::MemBufInputSource theSource {
      reinterpret_cast<XMLByte const*>(arrXml),
      sizeof(arrXml) - 1U,
      "buildengine-xerces-smoke",
      false
   };

   xercesc::XercesDOMParser theParser;
   theParser.setValidationScheme(xercesc::XercesDOMParser::Val_Never);
   theParser.setDoNamespaces(false);
   theParser.setDoSchema(false);
   theParser.setLoadExternalDTD(false);
   theParser.parse(theSource);

   xercesc::DOMDocument const* const pDocument = theParser.getDocument();
   if(!pDocument) return false;

   xercesc::DOMElement const* const pRoot = pDocument->getDocumentElement();
   if(!pRoot || !XmlEquals(pRoot->getTagName(), "buildengine")) return false;

   xercesc::DOMNode const* pChild = pRoot->getFirstChild();
   while(pChild && pChild->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
      pChild = pChild->getNextSibling();
   if(!pChild || !XmlEquals(pChild->getNodeName(), "value")) return false;

   xercesc::DOMNode const* const pText = pChild->getFirstChild();
   return pText && XmlEquals(pText->getNodeValue(), "42");
}

}

int main() {
   try {
      bool const bSuccess = RunSmoke();
      std::cout << "Xerces-C DOM consumer: " << (bSuccess ? "PASS" : "FAIL") << '\n';
      return bSuccess ? 0 : 1;
   }
   catch(xercesc::XMLException const& theException) {
      char* pMessage = xercesc::XMLString::transcode(theException.getMessage());
      std::cerr << "Xerces-C XMLException: " << (pMessage ? pMessage : "<unavailable>") << '\n';
      xercesc::XMLString::release(&pMessage);
      return 2;
   }
   catch(...) {
      std::cerr << "Xerces-C consumer: unknown exception\n";
      return 3;
   }
}
