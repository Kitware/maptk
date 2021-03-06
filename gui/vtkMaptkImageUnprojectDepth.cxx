// This file is part of TeleSculptor, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/TeleSculptor/blob/master/LICENSE for details.

#include "vtkMaptkImageUnprojectDepth.h"

#include "arrows/vtk/vtkKwiverCamera.h"

#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>

vtkStandardNewMacro(vtkMaptkImageUnprojectDepth);

vtkCxxSetObjectMacro(vtkMaptkImageUnprojectDepth,
                     Camera, kwiver::arrows::vtk::vtkKwiverCamera);

//-----------------------------------------------------------------------------
vtkMaptkImageUnprojectDepth::vtkMaptkImageUnprojectDepth()
{
  this->Camera = nullptr;

  this->DepthArrayName = nullptr;
  this->SetDepthArrayName("Depths");

  this->UnprojectedPointArrayName = nullptr;
  this->SetUnprojectedPointArrayName("Points");
}

//-----------------------------------------------------------------------------
vtkMaptkImageUnprojectDepth::~vtkMaptkImageUnprojectDepth()
{
  this->SetCamera(nullptr);
  this->SetDepthArrayName(nullptr);
  this->SetUnprojectedPointArrayName(nullptr);
}

//-----------------------------------------------------------------------------
void vtkMaptkImageUnprojectDepth::SimpleExecute(vtkImageData* input,
                                                vtkImageData* output)
{
  // All we're doing is adding an array to the point data
  output->ShallowCopy(input);

  if (!this->Camera)
  {
    vtkErrorMacro(<< "Maptk Camera must be specified!");
    return;
  }

  vtkDoubleArray* depths = vtkDoubleArray::SafeDownCast(
    output->GetPointData()->GetArray(this->DepthArrayName));
  if (!depths || depths->GetNumberOfComponents() != 1)
  {
    vtkErrorMacro(<< "Specified input depth array is not present or is not "
                  << "a scalar DoubleArray as expected!");
    return;
  }

  int dims[3], extents[6];
  double origin[3], spacing[3];
  output->GetDimensions(dims);
  output->GetExtent(extents);
  output->GetOrigin(origin);
  output->GetSpacing(spacing);

  if (dims[0] < 2 || dims[1] < 2 || dims[2] != 1)
  {
    vtkErrorMacro(<< "Expecting XY plane but input is " << dims[0] << " by " << dims[1] << " by " << dims[2]);
    return;
  }

  vtkIntArray* crop = static_cast<vtkIntArray*>(output->GetFieldData()->GetArray("Crop"));
  vtkSmartPointer<kwiver::arrows::vtk::vtkKwiverCamera> cropCam;
  auto cam = this->Camera;
  if (crop)
  {
    cropCam = cam->CropCamera(crop->GetValue(0), crop->GetValue(1), crop->GetValue(2), crop->GetValue(3));
    cam = cropCam.GetPointer();
  }

  // Get the scaled camera for doing the unproject
  auto const imageRatio =
    static_cast<double>(input->GetDimensions()[0]) /
    static_cast<double>(cam->GetImageDimensions()[0]);
  auto const scaledCamera = cam->ScaledK(imageRatio);

  vtkDataArray* inputPoints = output->GetPointData()->GetArray(
    this->UnprojectedPointArrayName);
  if (inputPoints)
  {
    vtkDebugMacro(<< "Output array exists - replacing existing array!");
  }

  // We need the image height to y-flip for the Unproject
  int height = dims[1];

  vtkIdType numberOfPoints = output->GetNumberOfPoints();

  // Setup our output array
  vtkFloatArray* points = vtkFloatArray::New();
  points->SetName(this->UnprojectedPointArrayName);
  points->SetNumberOfComponents(3);
  points->SetNumberOfTuples(numberOfPoints);

  output->GetPointData()->AddArray(points);
  points->FastDelete();

  // loop over each point / depth in the image, computing the unprojected point
  double* depthPtr = depths->GetPointer(0);
  float* pointsPtr = points->GetPointer(0);
  double pixel[2];
  for (int row = extents[2]; row <= extents[3]; ++row)
  {
    pixel[1] = height - 1 - (origin[1] + row * spacing[1]);
    for (int column = extents[0]; column <= extents[1]; ++column, ++depthPtr)
    {
      pixel[0] = origin[0] + column * spacing[0];

      auto const p = scaledCamera->UnprojectPoint(pixel, *depthPtr);
      *pointsPtr = p[0];
      *(++pointsPtr) = p[1];
      *(++pointsPtr) = p[2];
      ++pointsPtr;
    }
  }
}
