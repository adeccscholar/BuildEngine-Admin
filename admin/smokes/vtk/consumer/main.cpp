#include <vtkElevationFilter.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkVersion.h>

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

   bool const bRuntime = theFilter->GetOutput() != nullptr &&
                         theFilter->GetOutput()->GetNumberOfPoints() == 2;
   std::println("SMOKE|CHECK|runtime|{}|VTK {} CommonCore/CommonDataModel/FiltersCore pipeline",
                bRuntime ? "PASS" : "FAIL", vtkVersion::GetVTKVersion());
   std::println("SMOKE|RESULT|{}|VTK consumer usable", bRuntime ? "PASS" : "FAIL");
   return bRuntime ? 0 : 1;
}
