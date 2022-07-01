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
#include <vtkPlaneSource.h>
#include <vtkSphereSource.h>
#include <vtkTexture.h>
#include <opencv2/opencv.hpp>

#include "basic.h"
#include "mitkLookupTable.h"
#include "mitkLookupTableProperty.h"
#include "mitkSmartPointerProperty.h"
#include "surfaceregistraion.h"


bool SpineCArmRegistration::GetMatrixImageCalibratorMarkerToRenderWindow_pnp()
{
	if (m_Controls.mitkNodeSelectWidget_designLballs_pnp->GetSelectedNode() != nullptr && m_Controls.mitkNodeSelectWidget_extractedLballs_pnp->GetSelectedNode() != nullptr)
	{

		auto pointsetLballsByDesign = mitk::PointSet::New();
		auto pointsetLballs = mitk::PointSet::New();

		pointsetLballsByDesign = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_designLballs_pnp->GetSelectedNode()->GetData());
		pointsetLballs = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_extractedLballs_pnp->GetSelectedNode()->GetData());


		auto landmarkRegistrator = mitk::SurfaceRegistration::New();

		landmarkRegistrator->SetLandmarksTarget(pointsetLballsByDesign);
		landmarkRegistrator->SetLandmarksSrc(pointsetLballs);
		landmarkRegistrator->ComputeLandMarkResult();

		auto tmpMatrix = landmarkRegistrator->GetResult();

		vtkMatrix4x4::DeepCopy(m_ArrayImageCalibratorMarkerToRenderWindow_pnp, tmpMatrix);

		m_Controls.textBrowser->append("Max landmark error: " + QString::number(landmarkRegistrator->GetmaxLandmarkError()));
		m_Controls.textBrowser->append("Average landmark error: " + QString::number(landmarkRegistrator->GetavgLandmarkError()));

		return true;

	}


	m_Controls.textBrowser->append("GetMatrixImageCalibratorMarkerToRenderWindow_pnp(): 'L balls by design' or 'L balls (extracted)' haven't been assigned yet");


	return false;
}

bool SpineCArmRegistration::GetSourceInImage_pnp()
{
	if (m_Controls.mitkNodeSelectWidget_designMballs_pnp->GetSelectedNode() != nullptr &&
		m_Controls.mitkNodeSelectWidget_extractedMballs_pnp->GetSelectedNode() != nullptr &&
		m_Controls.mitkNodeSelectWidget_image_pnp->GetSelectedNode() != nullptr)
	{
		if (dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_designMballs_pnp->GetSelectedNode()->GetData())->GetSize() ==
			dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_extractedMballs_pnp->GetSelectedNode()->GetData())->GetSize()
			)
		{
			auto inputImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_image_pnp->GetSelectedNode()->GetData());
			auto imageSpacing = inputImage->GetGeometry()->GetSpacing();
			auto tmpVtkMatrix = vtkMatrix4x4::New();
			tmpVtkMatrix->Zero();
			tmpVtkMatrix->SetElement(0, 0, 1 / imageSpacing[0]);
			tmpVtkMatrix->SetElement(1, 1, 1 / imageSpacing[1]);
			tmpVtkMatrix->SetElement(2, 2, 1 / imageSpacing[2]);
			tmpVtkMatrix->SetElement(3, 3, 1);

			auto vtkTransformRenderWindowToImage = vtkTransform::New();
			vtkTransformRenderWindowToImage->Identity();
			vtkTransformRenderWindowToImage->PreMultiply();
			vtkTransformRenderWindowToImage->SetMatrix(inputImage->GetGeometry()->GetVtkMatrix());
			vtkTransformRenderWindowToImage->Concatenate(tmpVtkMatrix);

			vtkMatrix4x4::DeepCopy(m_ArrayRenderWindowToImage_pnp, vtkTransformRenderWindowToImage->GetMatrix());

			auto designPointset = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_designMballs_pnp->GetSelectedNode()->GetData());
			// auto extracedPointset = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_APextractedMballs->GetSelectedNode()->GetData());

			// Line end points under renderWindow coordinate
			m_linesEndsNode = m_Controls.mitkNodeSelectWidget_extractedMballs_pnp->GetSelectedNode();

			// Procure the line start points under renderWindow coordinate
			auto startsPointset = mitk::PointSet::New();
			auto matrixRenderWindowToImageCalibratorMarker = vtkMatrix4x4::New(); // Transform matrix from renderWindow to ApImageCalibratorMarker
			matrixRenderWindowToImageCalibratorMarker->DeepCopy(m_ArrayImageCalibratorMarkerToRenderWindow_pnp);
			matrixRenderWindowToImageCalibratorMarker->Invert();

			for (int i = 0; i < designPointset->GetSize(); i++)
			{
				// Point matrix of each design pointset element
				auto tmpPointMatrix = vtkMatrix4x4::New();
				tmpPointMatrix->Identity();
				tmpPointMatrix->SetElement(0, 3, (designPointset->GetPoint(i))[0]);
				tmpPointMatrix->SetElement(1, 3, (designPointset->GetPoint(i))[1]);
				tmpPointMatrix->SetElement(2, 3, (designPointset->GetPoint(i))[2]);

				auto tmpTransform = vtkTransform::New();
				tmpTransform->Identity();
				tmpTransform->PostMultiply();
				tmpTransform->SetMatrix(tmpPointMatrix);
				tmpTransform->Concatenate(matrixRenderWindowToImageCalibratorMarker);

				mitk::Point3D tmpPoint;
				tmpPoint[0] = tmpTransform->GetMatrix()->GetElement(0, 3);
				tmpPoint[1] = tmpTransform->GetMatrix()->GetElement(1, 3);
				tmpPoint[2] = tmpTransform->GetMatrix()->GetElement(2, 3);

				startsPointset->InsertPoint(tmpPoint);
			}
			auto tmpNode = mitk::DataNode::New();
			tmpNode->SetName("tmp Node");
			tmpNode->SetData(startsPointset);
			GetDataStorage()->Add(tmpNode);
			m_linesStartsNode = tmpNode;

			// Get the intersection i.e., the ray source under renderWindow coordinate
			double raySourceUnderRenderWindow[3];
			GetLinesIntersection(raySourceUnderRenderWindow);

			GetDataStorage()->Remove(tmpNode);

			// Describe the ray source under image coordinate
			auto tmpRaySourcePointMatrix = vtkMatrix4x4::New();
			tmpRaySourcePointMatrix->Identity();
			tmpRaySourcePointMatrix->SetElement(0, 3, raySourceUnderRenderWindow[0]);
			tmpRaySourcePointMatrix->SetElement(1, 3, raySourceUnderRenderWindow[1]);
			tmpRaySourcePointMatrix->SetElement(2, 3, raySourceUnderRenderWindow[2]);

			auto matrixImageToRenderWindow = vtkMatrix4x4::New();
			matrixImageToRenderWindow->DeepCopy(m_ArrayRenderWindowToImage_pnp);
			matrixImageToRenderWindow->Invert();
			auto tempTransform = vtkTransform::New();
			tempTransform->Identity();
			tempTransform->PostMultiply();
			tempTransform->SetMatrix(tmpRaySourcePointMatrix);
			tempTransform->Concatenate(matrixImageToRenderWindow);

			m_SourceInImage_pnp[0] = tempTransform->GetMatrix()->GetElement(0, 3);
			m_SourceInImage_pnp[1] = tempTransform->GetMatrix()->GetElement(1, 3);
			m_SourceInImage_pnp[2] = tempTransform->GetMatrix()->GetElement(2, 3);

			m_Controls.textBrowser->append("RaySource in image coordinate:"
				+ QString::number(m_SourceInImage_pnp[0]) + " /" +
				QString::number(m_SourceInImage_pnp[1]) + " /" +
				QString::number(m_SourceInImage_pnp[2]));

			return true;

		}

		m_Controls.textBrowser->append("The start and end pointset do not have the same size");


	}
	else
	{
		m_Controls.textBrowser->append("GetSourceInImage_pnp(): The Medium steelball pointsets or input image are not ready");
	}

	return false;
}

bool SpineCArmRegistration::GetMatrixNdiToImage_pnp()
{
	if(m_Controls.mitkNodeSelectWidget_image_pnp->GetSelectedNode() != nullptr &&
		m_Controls.mitkNodeSelectWidget_designPnPballs->GetSelectedNode() != nullptr &&
		m_Controls.mitkNodeSelectWidget_imagePnPballs->GetSelectedNode() != nullptr &&
		m_Controls.mitkNodeSelectWidget_ndiPnP->GetSelectedNode() != nullptr &&
		GetSourceInImage_pnp())
	{
		// Apply PnP to get the transform from the NDI world coordinate to the C-arm camera coordinate
	// Code adapted from: https://learnopencv.com/head-pose-estimation-using-opencv-and-dlib/


	// Camera internals
		auto inputImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_image_pnp->GetSelectedNode()->GetData());
		auto imageSpacing = inputImage->GetGeometry()->GetSpacing(); // dx, dy, dz spacing of the input image 
		double focal_length =  m_SourceInImage_pnp[2];
		double center_x = m_SourceInImage_pnp[1] / imageSpacing[1];
		double center_y = m_SourceInImage_pnp[0] / imageSpacing[0];

		cv::Mat camera_matrix = (cv::Mat_<double>(3, 3)
			<< focal_length / imageSpacing[1], 0, center_x,
			0, focal_length / imageSpacing[0], center_y,
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
				tmpTransform->GetMatrix()->GetElement(1, 3),
				tmpTransform->GetMatrix()->GetElement(0, 3)
			));
		}


		// Output rotation and translation
		cv::Mat rotation_vector; // Rotation in axis-angle form
		cv::Mat translation_vector;

		// Solve for pose
		cv::solvePnP(steelballsInNDI, steelballsInImagePixel, camera_matrix, dist_coeffs, rotation_vector, translation_vector);

		cv::Mat cvMatRotation;
		cv::Rodrigues(rotation_vector, cvMatRotation);


		// Array: camera to NDI
		double arrayCameraToNdi[16]
		{
			cvMatRotation.at<double>(0, 0), cvMatRotation.at<double>(0, 1), cvMatRotation.at<double>(0, 2), translation_vector.at<double>(0),
			cvMatRotation.at<double>(1, 0), cvMatRotation.at<double>(1, 1), cvMatRotation.at<double>(1, 2), translation_vector.at<double>(1),
			cvMatRotation.at<double>(2, 0), cvMatRotation.at<double>(2, 1), cvMatRotation.at<double>(2, 2), translation_vector.at<double>(2),
			0, 0, 0, 1
		};

		std::cout << "Test focal length effect";
		std::cout << "translation" << translation_vector;
		std::cout << "rotation" << cvMatRotation;


		// vtkMatrix: NDI to camera
		auto vtkMatrixNdiToCamera = vtkMatrix4x4::New();
		vtkMatrixNdiToCamera->DeepCopy(arrayCameraToNdi);
		vtkMatrixNdiToCamera->Invert();

		// vtkMatrix: Camera to image
		auto vtkMatrixCameraToImage = vtkMatrix4x4::New();
		vtkMatrixCameraToImage->Zero();
		vtkMatrixCameraToImage->SetElement(0, 3, - m_SourceInImage_pnp[1]);
		vtkMatrixCameraToImage->SetElement(1, 3, - m_SourceInImage_pnp[0]);
		vtkMatrixCameraToImage->SetElement(2, 3, m_SourceInImage_pnp[2]);
		vtkMatrixCameraToImage->SetElement(0, 1, 1);
		vtkMatrixCameraToImage->SetElement(1, 0, 1);
		vtkMatrixCameraToImage->SetElement(2, 2, -1);
		vtkMatrixCameraToImage->SetElement(3, 3, 1);

		auto tmpTransform = vtkTransform::New();
		tmpTransform->Identity();
		tmpTransform->PostMultiply();
		tmpTransform->SetMatrix(vtkMatrixCameraToImage);
		tmpTransform->Concatenate(vtkMatrixNdiToCamera);

		vtkMatrix4x4::DeepCopy(m_ArrayNdiToImage_pnp, tmpTransform->GetMatrix());

		return true;
	}


	m_Controls.textBrowser->append("GetMatrixNdiToImage_pnp(): 1. C-arm images or 2. design/extracted PnP balls or 3. NDI data carrier or 4. RaySource location are not ready");
	return false;

}



void SpineCArmRegistration::InitScene_pnp()
{
	// World coordinate
	double color[3]{ 0,0,0 };
	PlotCoordinate(GetDataStorage(), "NDI World coordinate", color);
	
	auto inputImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_image_pnp->GetSelectedNode()->GetData());
	//auto ltImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_LtImage->GetSelectedNode()->GetData());
	
	
	double resolutionPlane0[2]{ inputImage->GetGeometry()->GetSpacing()[0] ,inputImage->GetGeometry()->GetSpacing()[1] };
	// double resolutionPlane1[2]{ ltImage->GetGeometry()->GetSpacing()[0] ,ltImage->GetGeometry()->GetSpacing()[1] };
	MITK_INFO << "-------------------" << inputImage->GetGeometry()->GetSpacing()[0] << "---" << inputImage->GetGeometry()->GetSpacing()[1] << "---" << inputImage->GetGeometry()->GetSpacing()[2];
	
	
	double pixelNum0[2]{ double((inputImage->GetDimensions())[0]), double((inputImage->GetDimensions())[1]) };
	// double pixelNum1[2]{ double((ltImage->GetDimensions())[0]), double((ltImage->GetDimensions())[1]) };
	MITK_INFO << "-------------------" << pixelNum0[0] << "---" << pixelNum0[1];
	
	double dimensionPlane0[2]{ pixelNum0[0] * resolutionPlane0[0], pixelNum0[1] * resolutionPlane0[1] };
	// double dimensionPlane1[2]{ pixelNum1[0] * resolutionPlane1[0], pixelNum1[1] * resolutionPlane1[1] };
	
	MITK_INFO << "-------------------" << dimensionPlane0[0] << "---" << dimensionPlane0[1];
	
	auto imagerPlaneSource0 = vtkSmartPointer<vtkPlaneSource>::New();
	auto raySource0 = vtkSmartPointer<vtkSphereSource>::New();
	
	// Create a plane and source for C-arm posture 0
	double color0[3]{ 1,1,1 };
	if(m_Controls.radioButton_ap->isChecked())
	{
		PlotCoordinate(GetDataStorage(), "AP coordinate", color0);
	}else
	{
		PlotCoordinate(GetDataStorage(), "LT coordinate", color0);
	}
	
	imagerPlaneSource0->SetOrigin(0, 0, 0);
	imagerPlaneSource0->SetPoint1(dimensionPlane0[0], 0, 0);
	imagerPlaneSource0->SetPoint2(0, dimensionPlane0[1], 0);
	imagerPlaneSource0->Update();
	
	auto imagerPlaneNode0 = mitk::DataNode::New();
	auto imagerPlaneSurface0 = mitk::Surface::New();
	imagerPlaneSurface0->SetVtkPolyData(imagerPlaneSource0->GetOutput());
	imagerPlaneNode0->SetData(imagerPlaneSurface0);
	
	if (m_Controls.radioButton_ap->isChecked())
	{
		imagerPlaneNode0->SetName("AP imager helper");
		imagerPlaneNode0->SetColor(0.4, 0, 0);
	}else
	{
		imagerPlaneNode0->SetName("LT imager helper");
		imagerPlaneNode0->SetColor(0.0, 0.0, 0.4);
	}
	
	imagerPlaneNode0->SetVisibility(true);
	imagerPlaneNode0->SetOpacity(0.5);
	GetDataStorage()->Add(imagerPlaneNode0);
	
	raySource0->SetCenter(m_SourceInImage_pnp);
	raySource0->SetRadius(17);
	raySource0->Update();
	
	auto raySourceNode0 = mitk::DataNode::New();
	auto raySourceSurface0 = mitk::Surface::New();
	raySourceSurface0->SetVtkPolyData(raySource0->GetOutput());
	raySourceNode0->SetData(raySourceSurface0);
	
	if (m_Controls.radioButton_ap->isChecked())
	{
		raySourceNode0->SetName("AP raySource");
		raySourceNode0->SetColor(1.0, 0.0, 0.0);
	}else
	{
		raySourceNode0->SetName("LT raySource");
		raySourceNode0->SetColor(0.0, 0.0, 1.0);
	}
	
	
	raySourceNode0->SetVisibility(true);
	raySourceNode0->SetOpacity(1.0);
	GetDataStorage()->Add(raySourceNode0);



}

void SpineCArmRegistration::ApplyRegistration_pnp()
{
	auto matrixNdiToImage = vtkMatrix4x4::New();
	matrixNdiToImage->DeepCopy(m_ArrayNdiToImage_pnp);

	if(m_Controls.radioButton_ap->isChecked())
	{
		GetDataStorage()->GetNamedNode("AP raySource")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixNdiToImage);
		GetDataStorage()->GetNamedNode("AP imager helper")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixNdiToImage);
		GetDataStorage()->GetNamedNode("AP coordinate")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixNdiToImage);
	}else
	{
		GetDataStorage()->GetNamedNode("LT raySource")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixNdiToImage);
		GetDataStorage()->GetNamedNode("LT imager helper")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixNdiToImage);
		GetDataStorage()->GetNamedNode("LT coordinate")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixNdiToImage);
	}

	auto inputImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_image_pnp->GetSelectedNode()->GetData());
	
	// Display the Ap image according to the NDI coordinate
	auto tmpApMatrix = vtkMatrix4x4::New();
	tmpApMatrix->Identity();
	tmpApMatrix->SetElement(0, 0, inputImage->GetGeometry()->GetSpacing()[0]);
	tmpApMatrix->SetElement(1, 1, inputImage->GetGeometry()->GetSpacing()[1]);

	auto tmpApTrans = vtkTransform::New();
	tmpApTrans->PostMultiply();
	tmpApTrans->Identity();
	tmpApTrans->SetMatrix(tmpApMatrix);
	tmpApTrans->Concatenate(matrixNdiToImage);
	m_Controls.mitkNodeSelectWidget_image_pnp->GetSelectedNode()->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpApTrans->GetMatrix());


	auto image = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_image_pnp->GetSelectedNode()->GetData());


	// Apply texture
	auto node = GetDataStorage()->GetNamedNode("AP imager helper");
	if (m_Controls.radioButton_ap->isChecked())
	{
		mitk::SmartPointerProperty::Pointer prop = mitk::SmartPointerProperty::New(image);
		GetDataStorage()->GetNamedNode("AP imager helper")->SetDoubleProperty("Texture minimum", 0.0);
		GetDataStorage()->GetNamedNode("AP imager helper")->SetDoubleProperty("Texture maximum", 1600.0);
		GetDataStorage()->GetNamedNode("AP imager helper")->SetProperty("Surface.Texture", prop);
	}else
	{
		node = GetDataStorage()->GetNamedNode("LT imager helper");
		mitk::SmartPointerProperty::Pointer prop = mitk::SmartPointerProperty::New(image);
		GetDataStorage()->GetNamedNode("LT imager helper")->SetDoubleProperty("Texture minimum", 0.0);
		GetDataStorage()->GetNamedNode("LT imager helper")->SetDoubleProperty("Texture maximum", 1600.0);
		GetDataStorage()->GetNamedNode("LT imager helper")->SetProperty("Surface.Texture", prop);
	}
	// Remove shading
	node->RemoveProperty("material.wireframeLineWidth");
	node->RemoveProperty("material.pointSize");

	node->RemoveProperty("material.ambientCoefficient");
	node->RemoveProperty("material.diffuseCoefficient");
	node->RemoveProperty("material.specularCoefficient");
	node->RemoveProperty("material.specularPower");

	node->RemoveProperty("material.representation");
	node->RemoveProperty("material.interpolation");

	
	  

}


void SpineCArmRegistration::RegisterProjectionModel_pnp()
{
	if(GetMatrixImageCalibratorMarkerToRenderWindow_pnp() &&
		GetSourceInImage_pnp() &&
		GetMatrixNdiToImage_pnp())
	{
		InitScene_pnp();
		ApplyRegistration_pnp();
	}
}


void SpineCArmRegistration::ConfirmApPoint_pnp()
{
	if (GetDataStorage()->GetNamedNode("AP imager helper") != nullptr &&
		m_Controls.mitkNodeSelectWidget_ApPoint_pnp->GetSelectedNode() != nullptr)
	{
		// Draw the AP ray
		mitk::Point3D apPoint = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_ApPoint_pnp->GetSelectedNode()->GetData())->GetPoint(0);
		mitk::Point3D apSource = GetDataStorage()->GetNamedNode("AP raySource")->GetData()->GetGeometry()->GetCenter();
		double apPointArray[3]{ apPoint[0],apPoint[1],apPoint[2] };
		double apSourceArray[3]{ apSource[0],apSource[1],apSource[2] };
		double colorApRay[3]{ 1,0,0 };
		DrawLine(apPointArray, apSourceArray, colorApRay, 1, "AP ray");

		// Draw the auxiliary lines
		// Get the normal of LT imager
		auto tmpTransMatrix = GetDataStorage()->GetNamedNode("LT imager helper")->GetData()->GetGeometry()->GetVtkMatrix();
		double ltPlaneNormalArray[3]
		{
			tmpTransMatrix->GetElement(0,2) ,
			tmpTransMatrix->GetElement(1,2) ,
			tmpTransMatrix->GetElement(2,2)
		};


		double ltPlanePointArray[3]
		{
			tmpTransMatrix->GetElement(0,3),
			tmpTransMatrix->GetElement(1,3),
			tmpTransMatrix->GetElement(2,3)
		};

		mitk::Point3D ltSource = GetDataStorage()->GetNamedNode("LT raySource")->GetData()->GetGeometry()->GetCenter();
		double ltSourceArray[3]{ ltSource[0],ltSource[1],ltSource[2] };

		double auxiliaryPoint0[3];
		double auxiliaryPoint1[3];
		lancetAlgorithm::GetLinePlaneIntersection(auxiliaryPoint0, ltSourceArray, apSourceArray, ltPlanePointArray, ltPlaneNormalArray);
		lancetAlgorithm::GetLinePlaneIntersection(auxiliaryPoint1, ltSourceArray, apPointArray, ltPlanePointArray, ltPlaneNormalArray);

		double colorProjectionLine[3]{ 0.7,0.7,0.7 };
		DrawLine(auxiliaryPoint0, auxiliaryPoint1, colorProjectionLine, 1, "Projection line");

		double tmpColor[3]{ 0,0,1 };
		//DrawLine(auxiliaryPoint0, ltSourceArray, colorApRay, 0.2, "Auxiliary line 0");
		//DrawLine(auxiliaryPoint1, ltSourceArray, colorApRay, 0.2, "Auxiliary line 1");



	}
	else
	{
		m_Controls.textBrowser->append("ConfirmApPoint_pnp(): data not ready, draw an auxiliary line ");
	}
}

void SpineCArmRegistration::ConfirmLtPoint_pnp()
{
	if (GetDataStorage()->GetNamedNode("LT imager helper") != nullptr &&
		m_Controls.mitkNodeSelectWidget_3Dpointset_pnp->GetSelectedNode() != nullptr &&
		m_Controls.mitkNodeSelectWidget_LtPoint_pnp->GetSelectedNode() != nullptr)
	{
		mitk::Point3D ltLinePoint = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_LtPoint_pnp->GetSelectedNode()->GetData())->GetPoint(0);
		double ltLinePointArray[3]{ ltLinePoint[0], ltLinePoint[1] ,ltLinePoint[2] };
		mitk::Point3D ltSource = GetDataStorage()->GetNamedNode("LT raySource")->GetData()->GetGeometry()->GetCenter();
		double ltSourceArray[3]{ ltSource[0],ltSource[1],ltSource[2] };
		double tmpColor[3]{ 0,0,0.5 };
		DrawLine(ltLinePointArray, ltSourceArray, tmpColor, 0.2, "LT ray");


		mitk::Point3D apPoint = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_ApPoint_pnp->GetSelectedNode()->GetData())->GetPoint(0);
		mitk::Point3D apSource = GetDataStorage()->GetNamedNode("AP raySource")->GetData()->GetGeometry()->GetCenter();
		double apPointArray[3]{ apPoint[0],apPoint[1],apPoint[2] };
		double apSourceArray[3]{ apSource[0],apSource[1],apSource[2] };
		// double colorApRay[3]{ 1,0,0 };
		// DrawLine(apPointArray, apSourceArray, colorApRay, 0.2, "AP ray");

		double targetPointArray[3];
		auto tmpPointset = mitk::PointSet::New();
		tmpPointset->InsertPoint(apSource);
		tmpPointset->InsertPoint(ltSource);
		auto tmpNode = mitk::DataNode::New();
		tmpNode->SetName("Line starts");
		tmpNode->SetData(tmpPointset);
		GetDataStorage()->Add(tmpNode);
		m_linesStartsNode = tmpNode;

		auto tmpPointset1 = mitk::PointSet::New();
		tmpPointset1->InsertPoint(apPoint);
		tmpPointset1->InsertPoint(ltLinePoint);
		auto tmpNode1 = mitk::DataNode::New();
		tmpNode1->SetName("Line ends");
		tmpNode1->SetData(tmpPointset1);
		GetDataStorage()->Add(tmpNode1);
		m_linesEndsNode = tmpNode1;

		GetLinesIntersection(targetPointArray);

		GetDataStorage()->Remove(tmpNode);
		GetDataStorage()->Remove(tmpNode1);


		mitk::Point3D obtained3DPoint;
		obtained3DPoint[0] = targetPointArray[0];
		obtained3DPoint[1] = targetPointArray[1];
		obtained3DPoint[2] = targetPointArray[2];

		dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_3Dpointset_pnp->GetSelectedNode()->GetData())->InsertPoint(obtained3DPoint);
	}
	else
	{
		m_Controls.textBrowser->append("ConfirmLtPoint_pnp(): data not ready, cannot add 3D point ");
	}
}








