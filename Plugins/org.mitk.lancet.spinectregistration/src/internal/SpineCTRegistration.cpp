/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "SpineCTRegistration.h"

// Qt
#include "leastsquaresfit.h"
#include <QMessageBox>
#include <QPushButton>

#include "mitkImageToSurfaceFilter.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include "mitkPointSet.h"
#include "mitkSurface.h"
#include "surfaceregistraion.h"
#include "vtkConnectivityFilter.h"
#include <QPushButton>
#include <mitkImage.h>
#include <vtkSphereSource.h>
#include <ep\include\vtk-9.1\vtkTransformFilter.h>

// Reset the origin of an mitk::Image to (0, 0, 0), realign the image's axes to the standard xyz axes
void SpineCTRegistration::ResetImage()
{
  // Image origin to (0, 0, 0)
  auto inputImage = dynamic_cast<mitk::Image *>(m_CtImageDataNode->GetData());

  mitk::Point3D imageOrigin;
  imageOrigin[0] = 0.0;
  imageOrigin[1] = 0.0;
  imageOrigin[2] = 0.0;

  inputImage->SetOrigin(imageOrigin);

  // Align the image's axes to the standard xyz axes
  auto tmpVtkTransform = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkMatrix4x4> tmpVtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  tmpVtkTransform->Identity();
  tmpVtkTransform->GetMatrix(tmpVtkMatrix);

  inputImage->GetGeometry(0)->SetIndexToWorldTransformByVtkMatrixWithoutChangingSpacing(tmpVtkMatrix);
  // SetIndexToWorldTransformByVtkMatrix(tmpVtkMatrix) will set the spacing as (1, 1, 1),
  // because the spacing is determined by the matrix diagonal
}

void SpineCTRegistration::ReconstructSpineSurface()
{
  auto inputCtImage = dynamic_cast<mitk::Image *>(m_CtImageDataNode->GetData());

  // The isosurface of all steelballs as into a single polydata
  double threshold = m_Controls.lineEdit_BoneSurfaceThreshold->text().toDouble();
  auto mitkSteelBallSurfaces = mitk::Surface::New();
  mitk::ImageToSurfaceFilter::Pointer imageToSurfaceFilter = mitk::ImageToSurfaceFilter::New();

  imageToSurfaceFilter->SetInput(inputCtImage);
  imageToSurfaceFilter->SetThreshold(threshold);
  mitkSteelBallSurfaces = imageToSurfaceFilter->GetOutput();

  // draw extracted  steel ball surfaces
  auto nodeSteelballSurfaces = mitk::DataNode::New();
  nodeSteelballSurfaces->SetName("Bone surface");
  // add new node
  nodeSteelballSurfaces->SetData(mitkSteelBallSurfaces);
  GetDataStorage()->Add(nodeSteelballSurfaces);
}

void SpineCTRegistration::DEMOregistration()
{
	auto inputCtImage = dynamic_cast<mitk::Image*>(m_CtImageDataNode->GetData());

	auto pointset_extractedballs = dynamic_cast<mitk::PointSet*>(m_DEMO_Pointset_extractedballs->GetData());

	m_G_renderWindow_patientCT_matrix = inputCtImage->GetGeometry()->GetVtkMatrix()->GetData();


	auto landmarkRegistrator = mitk::SurfaceRegistration::New();

	MITK_INFO << "Proceedinng Landmark registration";
	if (m_LandmarkSrcPointsetDataNode != nullptr && m_LandmarkTargetPointsetDataNode != nullptr)
	{

		auto targetPointset = dynamic_cast<mitk::PointSet*>(m_DEMO_Pointset_extractedballs->GetData()); // source pointSet needs an extra offset: G_renderWindow_patientCT_matrix
		
		auto sourcePointset = dynamic_cast<mitk::PointSet*>(m_DEMO_Pointset_stlballs->GetData());
		landmarkRegistrator->SetLandmarksSrc(sourcePointset);
		landmarkRegistrator->SetLandmarksTarget(targetPointset);
		landmarkRegistrator->ComputeLandMarkResult();


	}

	std::ostringstream os;
	landmarkRegistrator->GetResult()->Print(os);

	m_Controls.textBrowser_DEMOigt->append("-------------Start landmark registration----------");
	m_Controls.textBrowser_DEMOigt->append(QString::fromStdString(os.str()));
	m_Controls.textBrowser_DEMOigt->append("-------------End landmark registration----------");

// m_G_patientCT_CTdrf_matrix = landmarkRegistrator->GetResult()->GetData();

	vtkSmartPointer<vtkMatrix4x4> patientCT_renderWindow_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix4x4::Invert(inputCtImage->GetGeometry()->GetVtkMatrix(), patientCT_renderWindow_matrix);
	vtkTransform* tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(landmarkRegistrator->GetResult());
	tmpTrans->Concatenate(patientCT_renderWindow_matrix);

	m_G_patientCT_CTdrf_matrix = tmpTrans->GetMatrix()->GetData();

	vtkSmartPointer<vtkMatrix4x4> CTdrf_patientCT_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix4x4::Invert(tmpTrans->GetMatrix(), CTdrf_patientCT_matrix);
	inputCtImage->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(CTdrf_patientCT_matrix);



}


