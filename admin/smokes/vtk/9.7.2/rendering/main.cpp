// SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: LicenseRef-PolyForm-Noncommercial-1.0.0
#include <vtkAxesActor.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkOpenGLRenderer.h>
#include <vtkPNGWriter.h>
#include <vtkTextActor.h>
#include <vtkVersion.h>
#include <print>
#include <string_view>

// Validate actual linked rendering implementations without opening a desktop GL context.
int main() {
   vtkNew<vtkOpenGLRenderer> theRenderer;
   vtkNew<vtkGPUVolumeRayCastMapper> theRayCaster;
   vtkNew<vtkInteractorStyleTrackballCamera> theInteraction;
   vtkNew<vtkAxesActor> theAxes;
   vtkNew<vtkTextActor> theText;
   vtkNew<vtkPNGWriter> thePng;
   bool const bVersion = std::string_view{vtkVersion::GetVTKVersion()} == "9.7.2";
   bool const bRenderer = std::string_view{theRenderer->GetClassName()} == "vtkOpenGLRenderer";
   bool const bRayCaster = theRayCaster->IsA("vtkOpenGLGPUVolumeRayCastMapper") != 0;
   bool const bUI = theInteraction && theAxes && theText && thePng;
   std::println("SMOKE|CHECK|version|{}|VTK 9.7.2", bVersion ? "PASS" : "FAIL");
   std::println("SMOKE|CHECK|opengl2|{}|actual OpenGL2 renderer", bRenderer ? "PASS" : "FAIL");
   std::println("SMOKE|CHECK|volume|{}|OpenGL GPU ray cast mapper ({})",
                bRayCaster ? "PASS" : "FAIL", theRayCaster->GetClassName());
   std::println("SMOKE|CHECK|ui-image|{}|interaction, axes, text, PNG", bUI ? "PASS" : "FAIL");
   bool const bOk = bVersion && bRenderer && bRayCaster && bUI;
   std::println("SMOKE|RESULT|{}|VTK rendering libraries linked", bOk ? "PASS" : "FAIL");
   return bOk ? 0 : 1;
}
