// DXC native behavior fixture: valid compute shader, no GPU required.
RWStructuredBuffer<uint> outputValues : register(u0);
[numthreads(8, 1, 1)]
void main(uint3 threadId : SV_DispatchThreadID) {
   outputValues[threadId.x] = threadId.x * 3u + 7u;
}
