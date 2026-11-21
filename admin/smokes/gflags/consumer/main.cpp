#include <gflags/gflags.h>
DEFINE_int32(buildengine_test_value, 17, "Native gflags consumer smoke");
int main(int iArgc, char** pszArgv) {
   gflags::ParseCommandLineFlags(&iArgc, &pszArgv, true);
   return FLAGS_buildengine_test_value == 17 ? 0 : 1;
}
