// DXC native behavior fixture: invalid HLSL must fail with diagnostics.
float4 main() : SV_Target {
   return missingFunction(;
}
