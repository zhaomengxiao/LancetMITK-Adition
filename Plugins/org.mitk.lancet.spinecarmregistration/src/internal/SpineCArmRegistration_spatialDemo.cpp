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
#include "SpineCArmRegistration.h"


// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkPlaneSource.h>
#include <vtkPolyLine.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

#include "mitkNodePredicateAnd.h"
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include "mitkPointSet.h"
#include "mitkSurface.h"
#include "QmitkDataStorageTreeModel.h"
#include "basic.h"
#include "surfaceregistraion.h"

bool SpineCArmRegistration::GetMatrixApImageCalibratorMarkerToRenderWindow()
{
	if (m_Controls.mitkNodeSelectWidget_designLballs->GetSelectedNode() != nullptr && m_Controls.mitkNodeSelectWidget_APextractedLballs->GetSelectedNode() != nullptr)
	{

		auto pointsetLballsByDesign = mitk::PointSet::New();
		auto pointsetLballsAP = mitk::PointSet::New();

		pointsetLballsByDesign = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_designLballs->GetSelectedNode()->GetData());
		pointsetLballsAP = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_APextractedLballs->GetSelectedNode()->GetData());


		auto landmarkRegistrator = mitk::SurfaceRegistration::New();

		landmarkRegistrator->SetLandmarksTarget(pointsetLballsByDesign);
		landmarkRegistrator->SetLandmarksSrc(pointsetLballsAP);
		landmarkRegistrator->ComputeLandMarkResult();

		auto tmpMatrix = landmarkRegistrator->GetResult();

		vtkMatrix4x4::DeepCopy(m_ArrayApImageCalibratorMarkerToRenderWindow,tmpMatrix);

		return true;

	}


	m_Controls.textBrowser->append("GetMatrixApImageCalibratorMarkerToRenderWindow(): 'L balls by design' or 'L balls (AP,extracted)' haven't been assigned yet");
	

	return false;

}

bool SpineCArmRegistration::GetMatrixLtImageCalibratorMarkerToRenderWindow()
{
	if (m_Controls.mitkNodeSelectWidget_designLballs->GetSelectedNode() != nullptr && m_Controls.mitkNodeSelectWidget_LTextractedLballs->GetSelectedNode() != nullptr)
	{

		auto pointsetLballsByDesign = mitk::PointSet::New();
		auto pointsetLballsLT = mitk::PointSet::New();

		pointsetLballsByDesign = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_designLballs->GetSelectedNode()->GetData());
		pointsetLballsLT = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_LTextractedLballs->GetSelectedNode()->GetData());


		auto landmarkRegistrator = mitk::SurfaceRegistration::New();

		landmarkRegistrator->SetLandmarksTarget(pointsetLballsByDesign);
		landmarkRegistrator->SetLandmarksSrc(pointsetLballsLT);
		landmarkRegistrator->ComputeLandMarkResult();

		auto tmpMatrix = landmarkRegistrator->GetResult();

		vtkMatrix4x4::DeepCopy(m_ArrayLtImageCalibratorMarkerToRenderWindow, tmpMatrix);

		return true;
	}
	
	m_Controls.textBrowser->append("GetMatrixLtImageCalibratorMarkerToRenderWindow(): 'L balls by design' or 'L balls (LT,extracted)' haven't been assigned yet");

	return false;
}

bool SpineCArmRegistration::GetMatrixNdiToApImage()
{
	if(m_Controls.mitkNodeSelectWidget_ApImage->GetSelectedNode() != nullptr &&
		m_Controls.mitkNodeSelectWidget_ndiAp->GetSelectedNode() != nullptr && 
		m_Controls.mitkNodeSelectWidget_designLballs->GetSelectedNode() != nullptr && 
		m_Controls.mitkNodeSelectWidget_APextractedLballs->GetSelectedNode() != nullptr)
	{
		auto apImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_ApImage->GetSelectedNode()->GetData());
		auto ndiDataCarrier = m_Controls.mitkNodeSelectWidget_ndiAp->GetSelectedNode()->GetData();

		auto vtkMatrixApImageCalibratorMarkerToRenderWindow = vtkMatrix4x4::New();
		vtkMatrixApImageCalibratorMarkerToRenderWindow->DeepCopy(m_ArrayApImageCalibratorMarkerToRenderWindow);

		vtkMatrix4x4::DeepCopy(m_ArrayNdiToApImageCalibratorMarker, ndiDataCarrier->GetGeometry()->GetVtkMatrix());

		auto imageSpacing = apImage->GetGeometry()->GetSpacing();
		auto tmpVtkMatrix = vtkMatrix4x4::New();
		tmpVtkMatrix->Zero();
		tmpVtkMatrix->SetElement(0, 0, 1 / imageSpacing[0]);
		tmpVtkMatrix->SetElement(1, 1, 1 / imageSpacing[1]);
		tmpVtkMatrix->SetElement(2, 2, 1 / imageSpacing[2]);
		tmpVtkMatrix->SetElement(3, 3, 1 );

		auto vtkTransformRenderWindowToApImage = vtkTransform::New();
		vtkTransformRenderWindowToApImage->Identity();
		vtkTransformRenderWindowToApImage->PreMultiply();
		vtkTransformRenderWindowToApImage->SetMatrix(apImage->GetGeometry()->GetVtkMatrix());
		vtkTransformRenderWindowToApImage->Concatenate(tmpVtkMatrix);

		vtkMatrix4x4::DeepCopy(m_ArrayRenderWindowToApImage, vtkTransformRenderWindowToApImage->GetMatrix());

		auto tmpTransform = vtkTransform::New();
		tmpTransform->PostMultiply();
		tmpTransform->Identity();
		tmpTransform->SetMatrix(vtkTransformRenderWindowToApImage->GetMatrix());
		tmpTransform->Concatenate(vtkMatrixApImageCalibratorMarkerToRenderWindow);
		tmpTransform->Concatenate(ndiDataCarrier->GetGeometry()->GetVtkMatrix());
		
		vtkMatrix4x4::DeepCopy(m_ArrayNdiToApImage, tmpTransform->GetMatrix());

		return true;

	}

	m_Controls.textBrowser->append("GetMatrixNdiToApImage(): C-arm images or Large steelballs or NDI data carrier are not ready");

	return false;
}

bool SpineCArmRegistration::GetMatrixNdiToLtImage()
{
	if (m_Controls.mitkNodeSelectWidget_LtImage->GetSelectedNode() != nullptr &&
		m_Controls.mitkNodeSelectWidget_ndiLt->GetSelectedNode() != nullptr &&
		m_Controls.mitkNodeSelectWidget_designLballs->GetSelectedNode() != nullptr &&
		m_Controls.mitkNodeSelectWidget_LTextractedLballs->GetSelectedNode() != nullptr)
	{
		auto ltImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_LtImage->GetSelectedNode()->GetData());
		auto ndiDataCarrier = m_Controls.mitkNodeSelectWidget_ndiLt->GetSelectedNode()->GetData();

		auto vtkMatrixLtImageCalibratorMarkerToRenderWindow = vtkMatrix4x4::New();
		vtkMatrixLtImageCalibratorMarkerToRenderWindow->DeepCopy(m_ArrayLtImageCalibratorMarkerToRenderWindow);

		vtkMatrix4x4::DeepCopy(m_ArrayNdiToLtImageCalibratorMarker, ndiDataCarrier->GetGeometry()->GetVtkMatrix());


		auto imageSpacing = ltImage->GetGeometry()->GetSpacing();
		auto tmpVtkMatrix = vtkMatrix4x4::New();
		tmpVtkMatrix->Zero();
		tmpVtkMatrix->SetElement(0, 0, 1 / imageSpacing[0]);
		tmpVtkMatrix->SetElement(1, 1, 1 / imageSpacing[1]);
		tmpVtkMatrix->SetElement(2, 2, 1 / imageSpacing[2]);
		tmpVtkMatrix->SetElement(3, 3, 1);

		auto vtkTransformRenderWindowToLtImage = vtkTransform::New();
		vtkTransformRenderWindowToLtImage->Identity();
		vtkTransformRenderWindowToLtImage->PreMultiply();
		vtkTransformRenderWindowToLtImage->SetMatrix(ltImage->GetGeometry()->GetVtkMatrix());
		vtkTransformRenderWindowToLtImage->Concatenate(tmpVtkMatrix);

		vtkMatrix4x4::DeepCopy(m_ArrayRenderWindowToLtImage, vtkTransformRenderWindowToLtImage->GetMatrix());


		auto tmpTransform = vtkTransform::New();
		tmpTransform->PostMultiply();
		tmpTransform->Identity();
		tmpTransform->SetMatrix(vtkTransformRenderWindowToLtImage->GetMatrix());
		tmpTransform->Concatenate(vtkMatrixLtImageCalibratorMarkerToRenderWindow);
		tmpTransform->Concatenate(ndiDataCarrier->GetGeometry()->GetVtkMatrix());

		vtkMatrix4x4::DeepCopy(m_ArrayNdiToLtImage, tmpTransform->GetMatrix());

		return true;
	}
	
	m_Controls.textBrowser->append("GetMatrixNdiToLtImage(): C-arm images or Large steelballs or NDI data carrier are not ready");

	return false;
}

bool SpineCArmRegistration::GetApSourceInApImage()
{
	if( m_Controls.mitkNodeSelectWidget_designMballs->GetSelectedNode() != nullptr &&
		m_Controls.mitkNodeSelectWidget_APextractedMballs->GetSelectedNode() != nullptr)
	{
		if(dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_designMballs->GetSelectedNode()->GetData())->GetSize() ==
			dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_APextractedMballs->GetSelectedNode()->GetData())->GetSize()
			)
		{
			auto designPointset = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_designMballs->GetSelectedNode()->GetData());
			// auto extracedPointset = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_APextractedMballs->GetSelectedNode()->GetData());

			// Line end points under renderWindow coordinate
			m_linesEndsNode = m_Controls.mitkNodeSelectWidget_APextractedMballs->GetSelectedNode(); 

			// Procure the line start points under renderWindow coordinate
			auto startsPointset = mitk::PointSet::New();
			auto matrixRenderWindowToApImageCalibratorMarker = vtkMatrix4x4::New(); // Transform matrix from renderWindow to ApImageCalibratorMarker
			matrixRenderWindowToApImageCalibratorMarker->DeepCopy(m_ArrayApImageCalibratorMarkerToRenderWindow);
			matrixRenderWindowToApImageCalibratorMarker->Invert();

			for( int i = 0 ; i < designPointset->GetSize() ; i++)
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
				tmpTransform->Concatenate(matrixRenderWindowToApImageCalibratorMarker);

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

			// Describe the ray source under imager coordinate
			auto tmpRaySourcePointMatrix = vtkMatrix4x4::New();
			tmpRaySourcePointMatrix->Identity();
			tmpRaySourcePointMatrix->SetElement(0, 3, raySourceUnderRenderWindow[0]);
			tmpRaySourcePointMatrix->SetElement(1, 3, raySourceUnderRenderWindow[1]);
			tmpRaySourcePointMatrix->SetElement(2, 3, raySourceUnderRenderWindow[2]);


			auto matrixImageToRenderWindow = vtkMatrix4x4::New();
			matrixImageToRenderWindow->DeepCopy(m_ArrayRenderWindowToApImage);
			matrixImageToRenderWindow->Invert();
			auto tempTransform = vtkTransform::New();
			tempTransform->Identity();
			tempTransform->PostMultiply();
			tempTransform->SetMatrix(tmpRaySourcePointMatrix);
			tempTransform->Concatenate(matrixImageToRenderWindow);

			m_ApSourceInApImage[0] = tempTransform->GetMatrix()->GetElement(0, 3);
			m_ApSourceInApImage[1] = tempTransform->GetMatrix()->GetElement(1, 3);
			m_ApSourceInApImage[2] = tempTransform->GetMatrix()->GetElement(2, 3);

			m_Controls.textBrowser->append("AP raySource in imager coordinate:"
				+ QString::number(m_ApSourceInApImage[0]) + " /" +
				QString::number(m_ApSourceInApImage[1]) + " /" +
				QString::number(m_ApSourceInApImage[2]));

			return true;

		}

		m_Controls.textBrowser->append("The start and end pointset do not have the same size");
		
		
	}else
	{
		m_Controls.textBrowser->append("GetApSourceInApImage(): The Medium steelball pointsets are not ready");
	}

	return false;
}

bool SpineCArmRegistration::GetLtSourceInLtImage()
{
	if (m_Controls.mitkNodeSelectWidget_designMballs->GetSelectedNode() != nullptr &&
		m_Controls.mitkNodeSelectWidget_LTextractedMballs->GetSelectedNode() != nullptr)
	{
		if (dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_designMballs->GetSelectedNode()->GetData())->GetSize() ==
			dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_LTextractedMballs->GetSelectedNode()->GetData())->GetSize()
			)
		{
			auto designPointset = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_designMballs->GetSelectedNode()->GetData());
			// auto extracedPointset = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_APextractedMballs->GetSelectedNode()->GetData());

			// Line end points under renderWindow coordinate
			m_linesEndsNode = m_Controls.mitkNodeSelectWidget_LTextractedMballs->GetSelectedNode();

			// Procure the line start points under renderWindow coordinate
			auto startsPointset = mitk::PointSet::New();
			auto matrixRenderWindowToLtImageCalibratorMarker = vtkMatrix4x4::New(); // Transform matrix from renderWindow to ApImageCalibratorMarker
			matrixRenderWindowToLtImageCalibratorMarker->DeepCopy(m_ArrayLtImageCalibratorMarkerToRenderWindow);
			matrixRenderWindowToLtImageCalibratorMarker->Invert();

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
				tmpTransform->Concatenate(matrixRenderWindowToLtImageCalibratorMarker);

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

			// Describe the ray source under imager coordinate
			auto tmpRaySourcePointMatrix = vtkMatrix4x4::New();
			tmpRaySourcePointMatrix->Identity();
			tmpRaySourcePointMatrix->SetElement(0, 3, raySourceUnderRenderWindow[0]);
			tmpRaySourcePointMatrix->SetElement(1, 3, raySourceUnderRenderWindow[1]);
			tmpRaySourcePointMatrix->SetElement(2, 3, raySourceUnderRenderWindow[2]);


			auto matrixImageToRenderWindow = vtkMatrix4x4::New();
			matrixImageToRenderWindow->DeepCopy(m_ArrayRenderWindowToApImage);
			matrixImageToRenderWindow->Invert();
			auto tempTransform = vtkTransform::New();
			tempTransform->Identity();
			tempTransform->PostMultiply();
			tempTransform->SetMatrix(tmpRaySourcePointMatrix);
			tempTransform->Concatenate(matrixImageToRenderWindow);

			m_LtSourceInLtImage[0] = tempTransform->GetMatrix()->GetElement(0, 3);
			m_LtSourceInLtImage[1] = tempTransform->GetMatrix()->GetElement(1, 3);
			m_LtSourceInLtImage[2] = tempTransform->GetMatrix()->GetElement(2, 3);

			m_Controls.textBrowser->append("LT raySource in imager coordinate:"
				+ QString::number(m_LtSourceInLtImage[0]) + " /" +
				QString::number(m_LtSourceInLtImage[1]) + " /" +
				QString::number(m_LtSourceInLtImage[2]) );

			return true;
		}

		m_Controls.textBrowser->append("The start and end pointset do not have the same size");
		

	}
	else
	{
		m_Controls.textBrowser->append("GetLtSourceInLtImage(): The Medium steelball pointsets are not ready");
	}

	return false;
}

void SpineCArmRegistration::RegistertheProjectionModel()
{
	if(GetMatrixApImageCalibratorMarkerToRenderWindow() && 
		GetMatrixLtImageCalibratorMarkerToRenderWindow() &&
		GetMatrixNdiToApImage() &&
		GetMatrixNdiToLtImage() &&
		GetApSourceInApImage() &&
		GetLtSourceInLtImage())
	{
		InitDemoScene();
		ApplyRegistration();
	}else
	{
		m_Controls.textBrowser->append("RegistertheProjectionModel(): data not ready, cannot register ");
	}

	// InitDemoScene();
	// ApplyRegistration();
	   
}

void SpineCArmRegistration::ConfirmDemoApPoint()
{
	if(GetDataStorage()->GetNamedNode("AP imager helper") != nullptr &&
		m_Controls.mitkNodeSelectWidget_demoApPoint->GetSelectedNode() != nullptr)
	{
		// Draw the AP ray
		mitk::Point3D apPoint = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_demoApPoint->GetSelectedNode()->GetData())->GetPoint(0);
		mitk::Point3D apSource = GetDataStorage()->GetNamedNode("AP raySource")->GetData()->GetGeometry()->GetCenter();
		double apPointArray[3]{ apPoint[0],apPoint[1],apPoint[2] };
		double apSourceArray[3]{ apSource[0],apSource[1],apSource[2] };
		double colorApRay[3]{ 1,0,0 };
		DrawLine(apPointArray, apSourceArray, colorApRay,1, "AP ray");

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
		m_Controls.textBrowser->append("ConfirmDemoApPoint(): data not ready, draw an auxiliary line ");
	}
}

void SpineCArmRegistration::ConfirmDemoLtPoint()
{
	if (GetDataStorage()->GetNamedNode("LT imager helper") != nullptr &&
		m_Controls.mitkNodeSelectWidget_demo3Dpointset->GetSelectedNode() != nullptr &&
		m_Controls.mitkNodeSelectWidget_demoLtPoint->GetSelectedNode() != nullptr)
	{
		mitk::Point3D ltLinePoint = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_demoLtPoint->GetSelectedNode()->GetData())->GetPoint(0);
		double ltLinePointArray[3]{ ltLinePoint[0], ltLinePoint[1] ,ltLinePoint[2] };
		mitk::Point3D ltSource = GetDataStorage()->GetNamedNode("LT raySource")->GetData()->GetGeometry()->GetCenter();
		double ltSourceArray[3]{ ltSource[0],ltSource[1],ltSource[2] };
		double tmpColor[3]{ 0,0,0.5 };
		DrawLine(ltLinePointArray, ltSourceArray, tmpColor, 0.2, "LT ray");


		mitk::Point3D apPoint = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_demoApPoint->GetSelectedNode()->GetData())->GetPoint(0);
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

		dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_demo3Dpointset->GetSelectedNode()->GetData())->InsertPoint(obtained3DPoint);
	}
	else
	{
		m_Controls.textBrowser->append("ConfirmDemoLtPoint(): data not ready, cannot add 3D point ");
	}
}


void SpineCArmRegistration::ApplyRegistration() // should be no problem
{
	auto matrixNdiToApImage = vtkMatrix4x4::New();
	matrixNdiToApImage->DeepCopy(m_ArrayNdiToApImage);
	auto matrixNdiToLtImage = vtkMatrix4x4::New();
	matrixNdiToLtImage->DeepCopy(m_ArrayNdiToLtImage);

	GetDataStorage()->GetNamedNode("AP raySource")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixNdiToApImage);
	GetDataStorage()->GetNamedNode("AP imager helper")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixNdiToApImage);
	GetDataStorage()->GetNamedNode("AP coordinate")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixNdiToApImage);

	GetDataStorage()->GetNamedNode("LT raySource")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixNdiToLtImage);
	GetDataStorage()->GetNamedNode("LT imager helper")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixNdiToLtImage);
	GetDataStorage()->GetNamedNode("LT coordinate")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixNdiToLtImage);


	auto apImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_ApImage->GetSelectedNode()->GetData());
	auto ltImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_LtImage->GetSelectedNode()->GetData());

	// Display the Ap image according to the NDI coordinate
	auto tmpApMatrix = vtkMatrix4x4::New();
	tmpApMatrix->Identity();
	tmpApMatrix->SetElement(0, 0, apImage->GetGeometry()->GetSpacing()[0]);
	tmpApMatrix->SetElement(1, 1, apImage->GetGeometry()->GetSpacing()[1]);

	auto tmpApTrans = vtkTransform::New();
	tmpApTrans->PostMultiply();
	tmpApTrans->Identity();
	tmpApTrans->SetMatrix(tmpApMatrix);
	tmpApTrans->Concatenate(matrixNdiToApImage);
	m_Controls.mitkNodeSelectWidget_ApImage->GetSelectedNode()->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpApTrans->GetMatrix());


	// Display the Lt image according to the NDI coordinate
	auto tmpLtMatrix = vtkMatrix4x4::New();
	tmpLtMatrix->Identity();
	tmpLtMatrix->SetElement(0, 0, ltImage->GetGeometry()->GetSpacing()[0]);
	tmpLtMatrix->SetElement(1, 1, ltImage->GetGeometry()->GetSpacing()[1]);

	auto tmpLtTrans = vtkTransform::New();
	tmpLtTrans->PostMultiply();
	tmpLtTrans->Identity();
	tmpLtTrans->SetMatrix(tmpLtMatrix);
	tmpLtTrans->Concatenate(matrixNdiToLtImage);
	m_Controls.mitkNodeSelectWidget_LtImage->GetSelectedNode()->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpLtTrans->GetMatrix());


}


void SpineCArmRegistration::InitDemoScene() // should be no problem
{
	// World coordinate
	double color[3]{ 0,0,0 };
	PlotCoordinate(GetDataStorage(), "NDI World coordinate", color);

	auto apImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_ApImage->GetSelectedNode()->GetData());
	auto ltImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_LtImage->GetSelectedNode()->GetData());


	double resolutionPlane0[2]{ apImage->GetGeometry()->GetSpacing()[0] ,apImage->GetGeometry()->GetSpacing()[1] };
	double resolutionPlane1[2]{ ltImage->GetGeometry()->GetSpacing()[0] ,ltImage->GetGeometry()->GetSpacing()[1] };
	MITK_INFO << "-------------------" << apImage->GetGeometry()->GetSpacing()[0] << "---" << apImage->GetGeometry()->GetSpacing()[1]<< "---"<< apImage->GetGeometry()->GetSpacing()[2];
	

	double pixelNum0[2]{ double((apImage->GetDimensions())[0]), double((apImage->GetDimensions())[1] )};
	double pixelNum1[2]{ double((ltImage->GetDimensions())[0]), double((ltImage->GetDimensions())[1])};
	MITK_INFO << "-------------------" << pixelNum0[0] << "---" << pixelNum0[1];

	


	double dimensionPlane0[2]{ pixelNum0[0] * resolutionPlane0[0], pixelNum0[1] * resolutionPlane0[1] };
	double dimensionPlane1[2]{ pixelNum1[0] * resolutionPlane1[0], pixelNum1[1] * resolutionPlane1[1] };

	MITK_INFO << "-------------------" << dimensionPlane0[0] << "---" << dimensionPlane0[1];

	auto imagerPlaneSource0 = vtkSmartPointer<vtkPlaneSource>::New();
	auto raySource0 = vtkSmartPointer<vtkSphereSource>::New();
	auto imagerPlaneSource1 = vtkSmartPointer<vtkPlaneSource>::New();
	auto raySource1 = vtkSmartPointer<vtkSphereSource>::New();


	// Create a plane and source for C-arm posture 0
	double color0[3]{ 1,1,1 };
	PlotCoordinate(GetDataStorage(), "AP coordinate", color0);



	imagerPlaneSource0->SetOrigin(0, 0, 0);
	imagerPlaneSource0->SetPoint1(dimensionPlane0[0], 0, 0);
	imagerPlaneSource0->SetPoint2(0, dimensionPlane0[1], 0);
	imagerPlaneSource0->Update();

	auto imagerPlaneNode0 = mitk::DataNode::New();
	auto imagerPlaneSurface0 = mitk::Surface::New();
	imagerPlaneSurface0->SetVtkPolyData(imagerPlaneSource0->GetOutput());
	imagerPlaneNode0->SetData(imagerPlaneSurface0);
	imagerPlaneNode0->SetName("AP imager helper");
	imagerPlaneNode0->SetColor(0.4, 0.0, 0.0);
	imagerPlaneNode0->SetVisibility(true);
	imagerPlaneNode0->SetOpacity(0.5);
	GetDataStorage()->Add(imagerPlaneNode0);

	raySource0->SetCenter(m_ApSourceInApImage);
	raySource0->SetRadius(17);
	raySource0->Update();

	auto raySourceNode0 = mitk::DataNode::New();
	auto raySourceSurface0 = mitk::Surface::New();
	raySourceSurface0->SetVtkPolyData(raySource0->GetOutput());
	raySourceNode0->SetData(raySourceSurface0);
	raySourceNode0->SetName("AP raySource");
	raySourceNode0->SetColor(1.0, 0.0, 0.0);
	raySourceNode0->SetVisibility(true);
	raySourceNode0->SetOpacity(1.0);
	GetDataStorage()->Add(raySourceNode0);


	// Create a plane and source for C-arm posture 1

	double color1[3]{ 1,1,1 };
	PlotCoordinate(GetDataStorage(), "LT coordinate", color1);

	imagerPlaneSource1->SetOrigin(0, 0, 0);
	imagerPlaneSource1->SetPoint1(dimensionPlane1[0], 0, 0);
	imagerPlaneSource1->SetPoint2(0, dimensionPlane1[1], 0);
	imagerPlaneSource1->Update();

	auto imagerPlaneNode1 = mitk::DataNode::New();
	auto imagerPlaneSurface1 = mitk::Surface::New();
	imagerPlaneSurface1->SetVtkPolyData(imagerPlaneSource1->GetOutput());
	imagerPlaneNode1->SetData(imagerPlaneSurface1);
	imagerPlaneNode1->SetName("LT imager helper");
	imagerPlaneNode1->SetColor(0.0, 0.0, 0.4);
	imagerPlaneNode1->SetVisibility(true);
	imagerPlaneNode1->SetOpacity(0.5);
	GetDataStorage()->Add(imagerPlaneNode1);

	raySource1->SetCenter(m_LtSourceInLtImage);
	raySource1->SetRadius(17);
	raySource1->Update();

	auto raySourceNode1 = mitk::DataNode::New();
	auto raySourceSurface1 = mitk::Surface::New();
	raySourceSurface1->SetVtkPolyData(raySource1->GetOutput());
	raySourceNode1->SetData(raySourceSurface1);
	raySourceNode1->SetName("LT raySource");
	raySourceNode1->SetColor(0.0, 0.0, 1.0);
	raySourceNode1->SetVisibility(true);
	raySourceNode1->SetOpacity(1.0);
	GetDataStorage()->Add(raySourceNode1);
}



