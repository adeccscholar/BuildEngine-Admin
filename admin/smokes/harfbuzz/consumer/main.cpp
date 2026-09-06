#include <hb.h>
#include <hb-subset.h>

#include <cstdio>

int main()
{
   hb_blob_t* pBlob = hb_blob_get_empty();
   hb_face_t* pFace = hb_face_create(pBlob, 0);

   if (pFace == nullptr)
   {
      std::printf("SMOKE|CHECK|harfbuzz-face|FAIL|hb_face_create returned null\n");
      std::printf("SMOKE|RESULT|FAIL|harfbuzz consumer validation failed\n");
      return 2;
   }

   hb_subset_input_t* pInput = hb_subset_input_create_or_fail();
   if (pInput == nullptr)
   {
      hb_face_destroy(pFace);
      std::printf("SMOKE|CHECK|harfbuzz-subset|FAIL|hb_subset_input_create_or_fail returned null\n");
      std::printf("SMOKE|RESULT|FAIL|harfbuzz consumer validation failed\n");
      return 3;
   }

   hb_subset_input_destroy(pInput);
   hb_face_destroy(pFace);

   std::printf("SMOKE|CHECK|harfbuzz-face|PASS|core API linked and executed\n");
   std::printf("SMOKE|CHECK|harfbuzz-subset|PASS|subset API linked and executed\n");
   std::printf("SMOKE|RESULT|PASS|harfbuzz consumer linked and executed successfully\n");
   return 0;
}
