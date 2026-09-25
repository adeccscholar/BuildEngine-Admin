#include <podofo/podofo.h>

#include <exception>
#include <print>

int main() {
   try {
      PoDoFo::PdfMemDocument theDocument;

      std::println("SMOKE|CHECK|podofo-document|PASS|PoDoFo created an in-memory PDF document");

      PoDoFo::PdfXMPPacket theSourcePacket;
      PoDoFo::PdfMetadataStore theMetadata;
      theSourcePacket.SetMetadata(theMetadata);

      std::string const strXmp = theSourcePacket.ToString();
      auto pPacket = PoDoFo::PdfXMPPacket::Create(strXmp);
      if(pPacket == nullptr) {
         std::println("SMOKE|CHECK|podofo-xmp|FAIL|PdfXMPPacket::Create returned null");
         std::println("SMOKE|RESULT|FAIL|PoDoFo consumer validation failed");
         return 3;
         }

      pPacket->PruneAndValidate(PoDoFo::PdfALevel::L1B);
      pPacket.reset();

      std::println("SMOKE|CHECK|podofo-xmp|PASS|Installed PoDoFo DLL created, validated and destroyed an XMP packet");
      std::println("SMOKE|RESULT|PASS|PoDoFo consumer linked and executed successfully");
      return 0;
      }
   catch (std::exception const& theException) {
      std::println("SMOKE|CHECK|podofo-document|FAIL|{}", theException.what());
      std::println("SMOKE|RESULT|FAIL|PoDoFo consumer validation failed");
      return 2;
      }
   }
