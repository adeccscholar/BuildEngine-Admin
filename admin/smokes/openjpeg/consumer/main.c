#include <openjpeg.h>
#include <stdio.h>
int main(void) {
   opj_dparameters_t params;
   opj_set_default_decoder_parameters(&params);
   const char* version = opj_version();
   int const ok = version != NULL && version[0] != '\0';
   printf("SMOKE|CHECK|decoder-init|%s|decoder defaults and version\n", ok ? "PASS" : "FAIL");
   printf("SMOKE|RESULT|%s|OpenJPEG consumer\n", ok ? "PASS" : "FAIL");
   return ok ? 0 : 1;
}
