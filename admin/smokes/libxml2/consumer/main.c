#include <libxml/parser.h>
#include <libxml/relaxng.h>
#include <libxml/tree.h>

#include <stdio.h>
#include <string.h>

static void IgnoreStructuredError(void* pContext, const xmlError* pError)
{
   (void)pContext;
   (void)pError;
}

static int ValidateElementWithRecovery(xmlRelaxNGValidCtxtPtr pContext, xmlDocPtr pDocument, xmlNodePtr pElement)
{
   int iResult = xmlRelaxNGValidatePushElement(pContext, pDocument, pElement);

   if(iResult == 0)
      {
      iResult = xmlRelaxNGValidateFullElement(pContext, pDocument, pElement);
      if(iResult == 1)
         return 1;

      xmlRelaxNGValidCtxtClearErrors(pContext);
      xmlResetLastError();
      return 0;
      }

   if(iResult == 1)
      {
      xmlNodePtr pChild = xmlFirstElementChild(pElement);
      while(pChild != NULL)
         {
         if(!ValidateElementWithRecovery(pContext, pDocument, pChild))
            return 0;

         pChild = xmlNextElementSibling(pChild);
         }

      if(xmlRelaxNGValidatePopElement(pContext, pDocument, pElement) == 1)
         return 1;
      }

   xmlRelaxNGValidCtxtClearErrors(pContext);
   xmlResetLastError();
   return 0;
}

static int TestRelaxNGRecovery(void)
{
   static char const szSchema[] =
      "<element xmlns=\"http://relaxng.org/ns/structure/1.0\" name=\"root\">"
      "<zeroOrMore>"
      "<element name=\"ok\"><text/></element>"
      "</zeroOrMore>"
      "</element>";

   static char const szDocument[] =
      "<root><ok>one</ok><bad>invalid</bad><ok>two</ok></root>";

   xmlDocPtr pSchemaDocument = xmlReadMemory(
      szSchema, (int)strlen(szSchema), "schema.rng", NULL, XML_PARSE_NONET);
   if(pSchemaDocument == NULL)
      return 0;

   xmlRelaxNGParserCtxtPtr pParser = xmlRelaxNGNewDocParserCtxt(pSchemaDocument);
   if(pParser == NULL)
      {
      xmlFreeDoc(pSchemaDocument);
      return 0;
      }

   xmlRelaxNGPtr pSchema = xmlRelaxNGParse(pParser);
   xmlRelaxNGFreeParserCtxt(pParser);
   xmlFreeDoc(pSchemaDocument);

   if(pSchema == NULL)
      return 0;

   xmlDocPtr pDocument = xmlReadMemory(
      szDocument, (int)strlen(szDocument), "document.xml", NULL, XML_PARSE_NONET);
   if(pDocument == NULL)
      {
      xmlRelaxNGFree(pSchema);
      return 0;
      }

   xmlRelaxNGValidCtxtPtr pValidation = xmlRelaxNGNewValidCtxt(pSchema);
   if(pValidation == NULL)
      {
      xmlFreeDoc(pDocument);
      xmlRelaxNGFree(pSchema);
      return 0;
      }

   xmlRelaxNGSetValidStructuredErrors(
      pValidation, IgnoreStructuredError, NULL);

   xmlNodePtr pRoot = xmlDocGetRootElement(pDocument);
   int iPushRoot = xmlRelaxNGValidatePushElement(
      pValidation, pDocument, pRoot);

   int iResult = 0;
   if(iPushRoot == 1)
      {
      xmlNodePtr pFirst = xmlFirstElementChild(pRoot);
      xmlNodePtr pInvalid = pFirst != NULL ? xmlNextElementSibling(pFirst) : NULL;
      xmlNodePtr pLast = pInvalid != NULL ? xmlNextElementSibling(pInvalid) : NULL;

      int iFirst = pFirst != NULL
         ? ValidateElementWithRecovery(pValidation, pDocument, pFirst) : 0;
      int iInvalid = pInvalid != NULL
         ? ValidateElementWithRecovery(pValidation, pDocument, pInvalid) : 1;
      int iLast = pLast != NULL
         ? ValidateElementWithRecovery(pValidation, pDocument, pLast) : 0;
      int iPopRoot = xmlRelaxNGValidatePopElement(
         pValidation, pDocument, pRoot);

      iResult = iFirst == 1 && iInvalid == 0 && iLast == 1 && iPopRoot == 1;
      }

   xmlRelaxNGFreeValidCtxt(pValidation);
   xmlFreeDoc(pDocument);
   xmlRelaxNGFree(pSchema);

   return iResult;
}

int main(void)
{
   char const* szXml = "<invoice><id>42</id></invoice>";
   xmlDocPtr theDocument = xmlReadMemory(
      szXml, (int)strlen(szXml), "invoice.xml", NULL, XML_PARSE_NONET);

   if(theDocument == NULL)
      {
      printf("SMOKE|CHECK|libxml2-parse|FAIL|xmlReadMemory returned no document\n");
      printf("SMOKE|RESULT|FAIL|libxml2 consumer validation failed\n");
      return 2;
      }

   xmlFreeDoc(theDocument);

   printf("SMOKE|CHECK|libxml2-parse|PASS|XML document parsed with network access disabled\n");

   if(!TestRelaxNGRecovery())
      {
      printf("SMOKE|CHECK|relaxng-recovery|FAIL|streaming RelaxNG recovery did not recover after an invalid element\n");
      printf("SMOKE|RESULT|FAIL|libxml2 RelaxNG recovery validation failed\n");
      xmlCleanupParser();
      return 3;
      }

   printf("SMOKE|CHECK|relaxng-recovery|PASS|streaming RelaxNG validation recovered after an invalid element\n");

   xmlCleanupParser();

   printf("SMOKE|RESULT|PASS|libxml2 consumer linked and executed successfully\n");
   return 0;
}
