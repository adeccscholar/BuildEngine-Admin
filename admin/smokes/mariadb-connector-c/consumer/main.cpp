#include <mysql.h>
#include <cstdio>
#include <memory>
int main() {
   unsigned long const version=mysql_get_client_version();
   bool const versionOk=version>=30400UL;
   std::printf("SMOKE|CHECK|client-version|%s|%lu\n",versionOk?"PASS":"FAIL",version);
   std::unique_ptr<MYSQL,decltype(&mysql_close)> handle{mysql_init(nullptr),&mysql_close};
   bool const initOk=static_cast<bool>(handle);
   std::printf("SMOKE|CHECK|client-init|%s|mysql_init/mysql_close\n",initOk?"PASS":"FAIL");
   std::printf("SMOKE|RESULT|%s|MariaDB Connector/C DLL consumer\n",(versionOk&&initOk)?"PASS":"FAIL");
   return versionOk&&initOk?0:1;
}
