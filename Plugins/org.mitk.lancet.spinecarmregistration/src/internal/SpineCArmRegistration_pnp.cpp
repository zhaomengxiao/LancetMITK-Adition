/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// Blueberry
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "SpineCArmRegistration.h"


// Qt

// mitk image
#include <mitkImage.h>
#include <vtkLineSource.h>
#include <opencv2/calib3d/calib3d.hpp>
#include "mitkNodePredicateDataType.h"
#include "mitkPointSet.h"
// #include <Eigen/Dense>
// #include <opencv2/core/eigen.hpp>
// #include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

bool SpineCArmRegistration::GetMatrixNdiToImage_pnp()
{
	// Apply PnP to get the transform from the NDI world coordinate to the C-arm camera coordinate
	// Code adapted from: https://learnopencv.com/head-pose-estimation-using-opencv-and-dlib/


	// Camera internals
	auto inputImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_image_pnp->GetSelectedNode()->GetData());
	auto imageSpacing = inputImage->GetGeometry()->GetSpacing(); // dx, dy, dz spacing of the input image 
	double focal_length = m_SourceInImage_pnp[2];
	double center_x = m_SourceInImage_pnp[0] / imageSpacing[0];
	double center_y = m_SourceInImage_pnp[1] / imageSpacing[1];

	cv::Mat camera_matrix = (cv::Mat_<double>(3, 3)
		<< focal_length / imageSpacing[0], 0, center_x,
		0, focal_length / imageSpacing[1], center_y,
		0, 0, 1); // camera intrinsic matrix

	cv::Mat dist_coeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type); // Assuming no lens distortion


	// PnP steelballs in NDI coordinate
	std::vector<cv::Point3d> steelballsInNDI;

	auto designBallPointset = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_designPnPballs->GetSelectedNode()->GetData());
	int ballNum = designBallPointset->GetSize();
	auto matrixNdiToPnpDrfMarker = m_Controls.mitkNodeSelectWidget_ndiPnP->GetSelectedNode()->GetData()->GetGeometry()->GetVtkMatrix();
	vtkMatrix4x4::DeepCopy(m_ArrayNdiToPnpDrfMarker_pnp, matrixNdiToPnpDrfMarker);

	for (int i = 0; i < ballNum; i++)
	{
		// Point matrix of each design pointset element
		auto tmpPointMatrix = vtkMatrix4x4::New();
		tmpPointMatrix->Identity();
		tmpPointMatrix->SetElement(0, 3, (designBallPointset->GetPoint(i))[0]);
		tmpPointMatrix->SetElement(1, 3, (designBallPointset->GetPoint(i))[1]);
		tmpPointMatrix->SetElement(2, 3, (designBallPointset->GetPoint(i))[2]);

		auto tmpTransform = vtkTransform::New();
		tmpTransform->Identity();
		tmpTransform->PostMultiply();
		tmpTransform->SetMatrix(tmpPointMatrix);
		tmpTransform->Concatenate(matrixNdiToPnpDrfMarker);

		steelballsInNDI.push_back(cv::Point3d(
			tmpTransform->GetMatrix()->GetElement(0, 3),
			tmpTransform->GetMatrix()->GetElement(1, 3),
			tmpTransform->GetMatrix()->GetElement(2, 3)
		));
	}

	// PnP steelballs in imagePixel coordinate
	std::vector<cv::Point2d> steelballsInImagePixel;

	auto matrixImagePixelToRenderWindow = inputImage->GetGeometry()->GetVtkMatrix();
	matrixImagePixelToRenderWindow->Invert();
	auto extractedBallPointset = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_imagePnPballs->GetSelectedNode()->GetData());

	for (int i = 0; i < ballNum; i++)
	{
		// Point matrix of each design pointset element
		auto tmpPointMatrix = vtkMatrix4x4::New();
		tmpPointMatrix->Identity();
		tmpPointMatrix->SetElement(0, 3, (extractedBallPointset->GetPoint(i))[0]);
		tmpPointMatrix->SetElement(1, 3, (extractedBallPointset->GetPoint(i))[1]);
		tmpPointMatrix->SetElement(2, 3, (extractedBallPointset->GetPoint(i))[2]);

		auto tmpTransform = vtkTransform::New();
		tmpTransform->Identity();
		tmpTransform->PostMultiply();
		tmpTransform->SetMatrix(tmpPointMatrix);
		tmpTransform->Concatenate(matrixImagePixelToRenderWindow);

		steelballsInImagePixel.push_back(cv::Point2d(
			tmpTransform->GetMatrix()->GetElement(0, 3),
			tmpTransform->GetMatrix()->GetElement(1, 3)
		));
	}


	// Output rotation and translation
	cv::Mat rotation_vector; // Rotation in axis-angle form
	cv::Mat translation_vector;

	// Solve for pose
	cv::solvePnP(steelballsInNDI, steelballsInImagePixel, camera_matrix, dist_coeffs, rotation_vector, translation_vector);

	cv::Mat cvMatRotation;
	cv::Rodrigues(rotation_vector,cvMatRotation);


	// Array: camera to NDI
	double arrayCameraToNdi[16]
	{
		cvMatRotation.at<double>(0, 0), cvMatRotation.at<double>(0, 1), cvMatRotation.at<double>(0, 2), translation_vector.at<double>(0),
		cvMatRotation.at<double>(1, 0), cvMatRotation.at<double>(1, 1), cvMatRotation.at<double>(1, 2), translation_vector.at<double>(1),
		cvMatRotation.at<double>(2, 0), cvMatRotation.at<double>(2, 1), cvMatRotation.at<double>(2, 2), translation_vector.at<double>(2),
		0, 0, 0, 1
	};

	// vtkMatrix: NDI to camera
	auto vtkMatrixNdiToCamera = vtkMatrix4x4::New();
	vtkMatrixNdiToCamera->DeepCopy(arrayCameraToNdi);
	vtkMatrixNdiToCamera->Invert();

	// vtkMatrix: Camera to image
	auto vtkMatrixCameraToImage = vtkMatrix4x4::New();
	vtkMatrixCameraToImage->Identity();
	vtkMatrixCameraToImage->SetElement(0, 3, m_SourceInImage_pnp[0]);
	vtkMatrixCameraToImage->SetElement(1, 3, m_SourceInImage_pnp[1]);

	auto tmpTransform = vtkTransform::New();
	tmpTransform->Identity();
	tmpTransform->PostMultiply();
	tmpTransform->SetMatrix(vtkMatrixCameraToImage);
	tmpTransform->Concatenate(vtkMatrixNdiToCamera);

	vtkMatrix4x4::DeepCopy(m_ArrayNdiToImage_pnp, tmpTransform->GetMatrix());

	return true;


}






