#include <openjpeg.h>
int main(void) {
   opj_dparameters_t params;
   opj_set_default_decoder_parameters(&params);
   return params.decod_format == -1 ? 0 : 0;
}
