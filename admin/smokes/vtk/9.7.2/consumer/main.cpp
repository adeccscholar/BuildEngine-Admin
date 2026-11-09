#include <vtkDataObjectMeshCache.h>
#include <vtkElevationFilter.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSMP.h>
#include <vtkVersion.h>

#include <print>
#include <string_view>

int main() {
   vtkNew<vtkPoints> thePoints;
   thePoints->InsertNextPoint(0.0, 0.0, 0.0);
   thePoints->InsertNextPoint(0.0, 0.0, 1.0);

   vtkNew<vtkPolyData> theData;
   theData->SetPoints(thePoints);

   vtkNew<vtkElevationFilter> theFilter;
   theFilter->SetLowPoint(0.0, 0.0, 0.0);
   theFilter->SetHighPoint(0.0, 0.0, 1.0);
   theFilter->SetInputData(theData);
   theFilter->Update();

   vtkNew<vtkDataObjectMeshCache> theCache;

   bool const bVersion = std::string_view { vtkVersion::GetVTKVersion() } == "9.7.2";
   bool const bTbb = VTK_SMP_ENABLE_TBB != 0;
   bool const bOpenMP = VTK_SMP_ENABLE_OPENMP != 0;
   bool const bCache = std::string_view { theCache->GetClassName() } == "vtkDataObjectMeshCache";
   bool const bRuntime = theFilter->GetOutput() != nullptr &&
                         theFilter->GetOutput()->GetNumberOfPoints() == 2;

   std::println("SMOKE|CHECK|version|{}|VTK {}", bVersion ? "PASS" : "FAIL",
                vtkVersion::GetVTKVersion());
   std::println("SMOKE|CHECK|tbb-backend|{}|VTK native TBB backend enabled",
                bTbb ? "PASS" : "FAIL");
   std::println("SMOKE|CHECK|openmp-backend|{}|VTK native OpenMP backend enabled",
                bOpenMP ? "PASS" : "FAIL");
   std::println("SMOKE|CHECK|common-cache|{}|VTK::CommonCache DLL consumer",
                bCache ? "PASS" : "FAIL");
   std::println("SMOKE|CHECK|runtime|{}|CommonCore/CommonDataModel/FiltersCore pipeline",
                bRuntime ? "PASS" : "FAIL");

   bool const bSuccess = bVersion && bTbb && bOpenMP && bCache && bRuntime;
   std::println("SMOKE|RESULT|{}|VTK 9.7.2 consumer usable",
                bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
