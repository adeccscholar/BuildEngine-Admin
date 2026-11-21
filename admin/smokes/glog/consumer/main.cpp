#include <glog/logging.h>
int main(int iArgc, char** pszArgv) {
   if(iArgc < 1 || pszArgv == nullptr)
      return 1;
   google::InitGoogleLogging(pszArgv[0]);
   LOG(INFO) << "BuildEngine native glog smoke";
   google::ShutdownGoogleLogging();
   return 0;
}
