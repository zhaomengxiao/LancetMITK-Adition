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

void SpineCArmRegistration::GetMatrixApImageCalibratorMarkerToRenderWindow()
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

	}else
	{
		m_Controls.textBrowser->append("'L balls by design' or 'L balls (AP,extracted)' haven't been assigned yet");
	}

}

void SpineCArmRegistration::GetMatrixLtImageCalibratorMarkerToRenderWindow()
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

	}
	else
	{
		m_Controls.textBrowser->append("'L balls by design' or 'L balls (LT,extracted)' haven't been assigned yet");
	}
}

void SpineCArmRegistration::GetMatrixNdiToApImage()
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
		vtkMatrix4x4::DeepCopy(m_ArrayRenderWindowToApImage, apImage->GetGeometry()->GetVtkMatrix());


		auto tmpTransform = vtkTransform::New();
		tmpTransform->PostMultiply();
		tmpTransform->Identity();
		tmpTransform->SetMatrix(apImage->GetGeometry()->GetVtkMatrix());
		tmpTransform->Concatenate(vtkMatrixApImageCalibratorMarkerToRenderWindow);
		tmpTransform->Concatenate(ndiDataCarrier->GetGeometry()->GetVtkMatrix());
		
		vtkMatrix4x4::DeepCopy(m_ArrayNdiToApImage, tmpTransform->GetMatrix());

	}else
	{
		m_Controls.textBrowser->append("Data missing, please make sure all the inputs are valid");
	}
}

void SpineCArmRegistration::GetMatrixNdiToLtImage()
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
		vtkMatrix4x4::DeepCopy(m_ArrayRenderWindowToLtImage, ltImage->GetGeometry()->GetVtkMatrix());


		auto tmpTransform = vtkTransform::New();
		tmpTransform->PostMultiply();
		tmpTransform->Identity();
		tmpTransform->SetMatrix(ltImage->GetGeometry()->GetVtkMatrix());
		tmpTransform->Concatenate(vtkMatrixLtImageCalibratorMarkerToRenderWindow);
		tmpTransform->Concatenate(ndiDataCarrier->GetGeometry()->GetVtkMatrix());

		vtkMatrix4x4::DeepCopy(m_ArrayNdiToLtImage, tmpTransform->GetMatrix());

	}
	else
	{
		m_Controls.textBrowser->append("Data missing, please make sure all the inputs are valid");
	}
}


