#include <expat.h>

#include <stddef.h>
#include <stdio.h>

static int Check(char const* const id, int const passed, char const* const detail)
{
   printf("SMOKE|CHECK|%s|%s|%s\n", id, passed ? "PASS" : "FAIL", detail);
   return passed;
}

int main(void)
{
   static char const xml[] = "<root><value>BuildEngine</value></root>";
   int success = 1;

   XML_Parser const parser = XML_ParserCreate(NULL);
   success = Check("create", parser != NULL, "parser created") && success;
   if(parser == NULL) {
      printf("SMOKE|RESULT|FAIL|expat consumer unusable\n");
      return 1;
   }

   enum XML_Status const status =
      XML_Parse(parser, xml, (int)(sizeof(xml) - 1U), XML_TRUE);

   success = Check("parse", status == XML_STATUS_OK, "XML parse completed") && success;
   XML_ParserFree(parser);

   success = Check("version", XML_ExpatVersion() != NULL, "version available") && success;

   printf("SMOKE|RESULT|%s|expat consumer usable\n", success ? "PASS" : "FAIL");
   return success ? 0 : 1;
}
