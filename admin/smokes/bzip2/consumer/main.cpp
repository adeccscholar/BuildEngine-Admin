#include <bzlib.h>
#include <array>
#include <cstring>
#include <print>
int main(){ char src[]="BuildEngine bzip2 smoke"; std::array<char,256> packed{},plain{}; unsigned int plen=packed.size(),olen=plain.size(); int c=BZ2_bzBuffToBuffCompress(packed.data(),&plen,src,sizeof(src),1,0,30); int d=c==BZ_OK?BZ2_bzBuffToBuffDecompress(plain.data(),&olen,packed.data(),plen,0,0):c; bool ok=c==BZ_OK&&d==BZ_OK&&olen==sizeof(src)&&std::memcmp(src,plain.data(),sizeof(src))==0; std::println("SMOKE|CHECK|roundtrip|{}|bzip2 compress/decompress",ok?"PASS":"FAIL"); std::println("SMOKE|RESULT|{}|bzip2 consumer usable",ok?"PASS":"FAIL"); return ok?0:1; }
