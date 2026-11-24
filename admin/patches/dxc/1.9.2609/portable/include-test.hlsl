// DXC native behavior fixture: resolve a local include.
#include "include-test.hlsli"
float4 main() : SV_Target {
   return TestColor();
}
