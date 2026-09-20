#include <expat.h>

#include <stddef.h>

int main(void)
{
   static char const xml[] = "<root><value>BuildEngine</value></root>";

   XML_Parser const parser = XML_ParserCreate(NULL);
   if(parser == NULL)
      return 1;

   enum XML_Status const status =
      XML_Parse(parser, xml, (int)(sizeof(xml) - 1U), XML_TRUE);

   XML_ParserFree(parser);

   if(status != XML_STATUS_OK)
      return 2;

   return XML_ExpatVersion() != NULL ? 0 : 3;
}
