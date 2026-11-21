#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <print>
#include <string_view>
int main() {
   std::string_view const svObj="v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n";
   Assimp::Importer theImporter;
   aiScene const* pScene=theImporter.ReadFileFromMemory(svObj.data(),svObj.size(),aiProcess_Triangulate,"obj");
   bool const bPassed=pScene && pScene->mNumMeshes==1 && pScene->mMeshes[0]->mNumFaces==1;
   std::println("SMOKE|RESULT|{}|Assimp real OBJ import",bPassed?"PASS":"FAIL");
   return bPassed?0:1;
}
