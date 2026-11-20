#include <Recast.h>
#include <DetourNavMesh.h>
#include <print>

int main() {
   rcHeightfield* const pField = rcAllocHeightfield();
   dtNavMesh* const pMesh = dtAllocNavMesh();
   bool const bPassed = pField != nullptr && pMesh != nullptr;
   if(pMesh != nullptr) dtFreeNavMesh(pMesh);
   if(pField != nullptr) rcFreeHeightField(pField);
   std::println("SMOKE|CHECK|allocators|{}|Recast and Detour allocators are available through DLLs", bPassed ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|Recast Navigation consumer", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
