#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <print>
#include <string_view>

int main() {
   std::string_view const svObj {
      "v 0 0 0\n"
      "v 1 0 0\n"
      "v 0 1 0\n"
      "f 1 2 3\n"
   };

   Assimp::Importer theImporter;
   aiScene const* pScene = theImporter.ReadFileFromMemory(
      svObj.data(), svObj.size(), aiProcess_Triangulate, "obj");

   bool const bScene = pScene != nullptr;
   std::println("SMOKE|CHECK|obj-scene|{}|Assimp imports a real OBJ scene from memory",
      bScene ? "PASS" : "FAIL");

   bool const bMesh = bScene && pScene->mNumMeshes == 1 &&
      pScene->mMeshes != nullptr && pScene->mMeshes[0] != nullptr;
   std::println("SMOKE|CHECK|obj-mesh|{}|Assimp imports exactly one mesh",
      bMesh ? "PASS" : "FAIL");

   bool const bFace = bMesh && pScene->mMeshes[0]->mNumFaces == 1;
   std::println("SMOKE|CHECK|obj-triangle|{}|Assimp imports the OBJ triangle",
      bFace ? "PASS" : "FAIL");

   bool const bPassed = bScene && bMesh && bFace;
   std::println("SMOKE|RESULT|{}|Assimp real OBJ import", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
