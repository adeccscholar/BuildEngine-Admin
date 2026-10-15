#include <openjpeg.h>
#include <string.h>
int main(void) {
   opj_dparameters_t params;
   opj_set_default_decoder_parameters(&params);
   const char* version = opj_version();
   return version && strlen(version) > 0 ? 0 : 1;
}
