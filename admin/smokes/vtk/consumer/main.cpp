#include <vtkElevationFilter.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkVersion.h>
#include <vtkSMP.h>

#include <print>

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

   bool const bTbb = VTK_SMP_ENABLE_TBB != 0;
   bool const bOpenMP = VTK_SMP_ENABLE_OPENMP != 0;
   std::println("SMOKE|CHECK|tbb-backend|{}|VTK native TBB backend", bTbb ? "PASS" : "FAIL");
   std::println("SMOKE|CHECK|openmp-backend|{}|VTK native OpenMP backend", bOpenMP ? "PASS" : "FAIL");
   bool const bRuntime = theFilter->GetOutput() != nullptr &&
                         theFilter->GetOutput()->GetNumberOfPoints() == 2;
   std::println("SMOKE|CHECK|runtime|{}|VTK {} CommonCore/CommonDataModel/FiltersCore pipeline",
                bRuntime ? "PASS" : "FAIL", vtkVersion::GetVTKVersion());
   std::println("SMOKE|RESULT|{}|VTK consumer usable", (bRuntime && bTbb && bOpenMP) ? "PASS" : "FAIL");
   return (bRuntime && bTbb && bOpenMP) ? 0 : 1;
}
