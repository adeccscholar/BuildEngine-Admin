#include <podofo/podofo.h>

#include <exception>
#include <print>
#include <string>

int main() {
   try {
      PoDoFo::PdfMemDocument theDocument;
      std::println("SMOKE|CHECK|podofo-document|PASS|PoDoFo created an in-memory PDF document");

      PoDoFo::PdfXMPPacket theTemplate;
      PoDoFo::PdfMetadataStore theMetadata;
      theMetadata.PdfaLevel = PoDoFo::PdfALevel::L1B;
      theTemplate.SetMetadata(theMetadata);

      std::string strXmp = theTemplate.ToString();
      std::string const strClosing = "</rdf:Description>";
      std::size_t const uInsert = strXmp.find(strClosing);
      if(uInsert == std::string::npos) {
         std::println("SMOKE|CHECK|podofo-xmp|FAIL|Generated XMP has no rdf:Description closing element");
         std::println("SMOKE|RESULT|FAIL|PoDoFo consumer validation failed");
         return 3;
         }

      strXmp.insert(
         uInsert,
         "<invalid:BuildEngine xmlns:invalid=\"urn:adecc:invalid\">invalid</invalid:BuildEngine>");

      for(std::size_t uRun = 0; uRun < 32U; ++uRun) {
         auto pPacket = PoDoFo::PdfXMPPacket::Create(strXmp);
         if(pPacket == nullptr) {
            std::println("SMOKE|CHECK|podofo-xmp|FAIL|PdfXMPPacket::Create returned null at run {}", uRun);
            std::println("SMOKE|RESULT|FAIL|PoDoFo consumer validation failed");
            return 4;
            }

         std::size_t uWarnings = 0U;
         pPacket->PruneAndValidate(
            PoDoFo::PdfALevel::L1B,
            [&uWarnings](PoDoFo::PdfXMPProperty const&) {
               ++uWarnings;
               });

         std::string const strPruned = pPacket->ToString();
         if(uWarnings == 0U || strPruned.find("urn:adecc:invalid") != std::string::npos) {
            std::println(
               "SMOKE|CHECK|podofo-xmp|FAIL|Invalid XMP property was not pruned at run {}",
               uRun);
            std::println("SMOKE|RESULT|FAIL|PoDoFo consumer validation failed");
            return 5;
            }
         }

      std::println(
         "SMOKE|CHECK|podofo-xmp|PASS|Installed PoDoFo DLL pruned invalid XMP and completed 32 create-validate-destroy cycles");
      std::println("SMOKE|RESULT|PASS|PoDoFo consumer linked and executed successfully");
      return 0;
      }
   catch(std::exception const& theException) {
      std::println("SMOKE|CHECK|podofo-xmp|FAIL|{}", theException.what());
      std::println("SMOKE|RESULT|FAIL|PoDoFo consumer validation failed");
      return 2;
      }
   }
