#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XercesVersion.hpp>

#include <print>
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

[[nodiscard]] bool Check(std::string_view const svId,
                         bool const bPassed,
                         std::string_view const svDetail) {
   std::println("SMOKE|CHECK|{}|{}|{}", svId, bPassed ? "PASS" : "FAIL", svDetail);
   return bPassed;
}

[[nodiscard]] bool XmlEquals(XMLCh const* const pValue, char const* const pExpected) {
   char* pText = xercesc::XMLString::transcode(pValue);
   if(!pText) return false;
   bool const bResult = std::string_view { pText } == pExpected;
   xercesc::XMLString::release(&pText);
   return bResult;
}

[[nodiscard]] bool CheckDomRuntime() {
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
   bool bSuccess = true;

   bool const bVersion = XERCES_VERSION_MAJOR == 3 && XERCES_VERSION_MINOR == 3;
   bSuccess = Check("package", bVersion, "Xerces-C 3.3 public headers available") && bSuccess;

   try {
      bool const bRuntime = CheckDomRuntime();
      bSuccess = Check("runtime", bRuntime, "initialize, in-memory parse and DOM traversal") && bSuccess;
   }
   catch(xercesc::XMLException const&) {
      // CheckDomRuntime owns Xerces initialization. During stack unwinding its
      // runtime guard has already called Terminate(), so do not invoke Xerces
      // conversion APIs from this outer catch block.
      bSuccess = Check("runtime", false, "Xerces XMLException") && bSuccess;
   }
   catch(...) {
      bSuccess = Check("runtime", false, "unknown exception") && bSuccess;
   }

   std::println("SMOKE|RESULT|{}|Xerces-C consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
