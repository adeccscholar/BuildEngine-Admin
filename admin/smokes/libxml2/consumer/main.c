#include <libxml/parser.h>

#include <stdio.h>
#include <string.h>

int main(void)
{
   char const* szXml = "<invoice><id>42</id></invoice>";
   xmlDocPtr theDocument = xmlReadMemory(szXml, (int)strlen(szXml), "invoice.xml", NULL, XML_PARSE_NONET);

   if (theDocument == NULL)
   {
      printf("SMOKE|CHECK|libxml2-parse|FAIL|xmlReadMemory returned no document\n");
      printf("SMOKE|RESULT|FAIL|libxml2 consumer validation failed\n");
      return 2;
   }

   xmlFreeDoc(theDocument);
   xmlCleanupParser();

   printf("SMOKE|CHECK|libxml2-parse|PASS|XML document parsed with network access disabled\n");
   printf("SMOKE|RESULT|PASS|libxml2 consumer linked and executed successfully\n");
   return 0;
}
