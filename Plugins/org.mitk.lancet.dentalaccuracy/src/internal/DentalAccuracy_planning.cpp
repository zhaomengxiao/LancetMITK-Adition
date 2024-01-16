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
#include "DentalAccuracy.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <QFileDialog>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkCardinalSpline.h>
#include <vtkCellArray.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkConnectivityFilter.h>
#include <vtkImageAppend.h>
#include <vtkImageCast.h>
#include <vtkImageIterator.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkKdTree.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProbeFilter.h>
#include <vtkRendererCollection.h>
#include <vtkSplineFilter.h>
#include <ep/include/vtk-9.1/vtkTransformFilter.h>

#include "lancetTrackingDeviceSourceConfigurator.h"
#include "lancetVegaTrackingDevice.h"
#include "leastsquaresfit.h"
#include "mitkGizmo.h"
#include "mitkImageToSurfaceFilter.h"
#include "mitkMatrixConvert.h"
#include "mitkNavigationToolStorageDeserializer.h"
#include "mitkPointSet.h"
#include "QmitkDataStorageTreeModel.h"
#include "QmitkRenderWindow.h"
#include "surfaceregistraion.h"


void DentalAccuracy::on_pushButton_implantToCrown_clicked()
{
	// Check the availability of all the data
	if (GetDataStorage()->GetNamedNode("crown_tip_pts") == nullptr)
	{
		m_Controls.textBrowser->append("crown_tip_pts is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("implant_tip_pts") == nullptr)
	{
		m_Controls.textBrowser->append("implant_tip_pts is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("implant_to_move") == nullptr)
	{
		m_Controls.textBrowser->append("implant_to_move is missing");
		return;
	}

	auto implantSurface = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("implant_to_move")->GetData());
	auto implantTipPts = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("implant_tip_pts")->GetData());
	auto crownTipPts = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("crown_tip_pts")->GetData());


	// Assume the 1st point of implantTipPts is the head, the 2nd point is the tail
	// Assume the 1st point of crownTipPts is the crown_baseContactPoint, the 2nd point is the crown_occlusionPoint
	if (implantTipPts->GetSize() != 2)
	{
		m_Controls.textBrowser->append("abutment_tip_pts has wrong size!");
		return;
	}

	if (crownTipPts->GetSize() != 2)
	{
		m_Controls.textBrowser->append("crown_tip_pts has wrong size!");
		return;
	}

	mitk::Point3D implant_head_old = implantTipPts->GetPoint(0);
	mitk::Point3D implant_tail_old = implantTipPts->GetPoint(1);
	mitk::Point3D crown_occlusionPoint = crownTipPts->GetPoint(1);
	mitk::Point3D crown_baseContactPoint = crownTipPts->GetPoint(0);


	// Calculate the rotation matrix
	Eigen::Vector3d controlAxis{
		crown_baseContactPoint[0] - crown_occlusionPoint[0],
		crown_baseContactPoint[1] - crown_occlusionPoint[1],
		crown_baseContactPoint[2] - crown_occlusionPoint[2]
	};
	controlAxis.normalize();


	Eigen::Vector3d implantAxis_old{
		implant_head_old[0] - implant_tail_old[0],
		implant_head_old[1] - implant_tail_old[1],
		implant_head_old[2] - implant_tail_old[2]
	};
	implantAxis_old.normalize();

	Eigen::Vector3d rotAxis = implantAxis_old.cross(controlAxis);
	rotAxis.normalize();

	double rotAngle = acos(implantAxis_old.dot(controlAxis)) * 180 / 3.141593;

	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->RotateWXYZ(rotAngle, rotAxis[0], rotAxis[1], rotAxis[2]);
	tmpTrans->Update();

	auto rotMatrix = tmpTrans->GetMatrix();

	auto implantTipPts_matrix_old = implantTipPts->GetGeometry()->GetVtkMatrix();

	// Calculate the translation offset
	auto tipPts_Trans_tmp = vtkTransform::New();
	tipPts_Trans_tmp->Identity();
	tipPts_Trans_tmp->PostMultiply();
	tipPts_Trans_tmp->SetMatrix(implantTipPts_matrix_old);
	tipPts_Trans_tmp->Concatenate(rotMatrix);
	tipPts_Trans_tmp->Update();

	implantTipPts->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tipPts_Trans_tmp->GetMatrix());
	auto impalnt_tip_head_tmp = implantTipPts->GetPoint(0);
	auto implant_tip_tail_tmp = implantTipPts->GetPoint(1);

	double offset[3]{ 0 };

	offset[0] = crown_baseContactPoint[0] - implant_tip_tail_tmp[0] + controlAxis[0] * 5;
	offset[1] = crown_baseContactPoint[1] - implant_tip_tail_tmp[1] + controlAxis[1] * 5;
	offset[2] = crown_baseContactPoint[2] - implant_tip_tail_tmp[2] + controlAxis[2] * 5;

	rotMatrix->SetElement(0, 3, offset[0]);
	rotMatrix->SetElement(1, 3, offset[1]);
	rotMatrix->SetElement(2, 3, offset[2]);

	auto implant_matrix_old = implantSurface->GetGeometry()->GetVtkMatrix();

	auto implantTrans = vtkTransform::New();
	implantTrans->Identity();
	implantTrans->PostMultiply();
	implantTrans->SetMatrix(implant_matrix_old);
	implantTrans->Concatenate(rotMatrix);
	implantTrans->Update();

	auto tipPts_Trans = vtkTransform::New();
	tipPts_Trans->Identity();
	tipPts_Trans->PostMultiply();
	tipPts_Trans->SetMatrix(tipPts_Trans_tmp->GetMatrix());
	tipPts_Trans->Translate(offset);
	tipPts_Trans->Update();

	implantSurface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantTrans->GetMatrix());


	implantTipPts->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tipPts_Trans->GetMatrix());
	implantTipPts->GetGeometry()->Modified();

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

}

void DentalAccuracy::on_pushButton_abutmentToImplant_clicked()
{
	// Check the availability of all the data
	if (GetDataStorage()->GetNamedNode("abutment_tip_pts") == nullptr)
	{
		m_Controls.textBrowser->append("abutment_tip_pts is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("implant_tip_pts") == nullptr)
	{
		m_Controls.textBrowser->append("implant_tip_pts is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("abutment_to_move") == nullptr)
	{
		m_Controls.textBrowser->append("abutment_to_move is missing");
		return;
	}

	auto abutmentSurface = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("abutment_to_move")->GetData());
	auto abutmentTipPts = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("abutment_tip_pts")->GetData());
	auto implantTipPts = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("implant_tip_pts")->GetData());


	// Assume the 1st point of abutmentTipPts is the implant side, the 2nd point is the crown side
	// Assume the 1st point of implantTipPts is the implant head point, the 2nd point is the implant tail point
	if (abutmentTipPts->GetSize() != 2)
	{
		m_Controls.textBrowser->append("abutment_tip_pts has wrong size!");
		return;
	}

	if (implantTipPts->GetSize() != 2)
	{
		m_Controls.textBrowser->append("implant_tip_pts has wrong size!");
		return;
	}

	mitk::Point3D abutment_implantSide_old = abutmentTipPts->GetPoint(0);
	mitk::Point3D abutment_crownSide_old = abutmentTipPts->GetPoint(1);
	mitk::Point3D implant_head = implantTipPts->GetPoint(0);
	mitk::Point3D implant_tail = implantTipPts->GetPoint(1);


	// Calculate the rotation matrix
	Eigen::Vector3d controlAxis{
		implant_head[0] - implant_tail[0],
		implant_head[1] - implant_tail[1],
		implant_head[2] - implant_tail[2]
	};
	controlAxis.normalize();


	Eigen::Vector3d abutmentAxis_old{
		abutment_implantSide_old[0] - abutment_crownSide_old[0],
		abutment_implantSide_old[1] - abutment_crownSide_old[1],
		abutment_implantSide_old[2] - abutment_crownSide_old[2]
	};
	abutmentAxis_old.normalize();

	Eigen::Vector3d rotAxis = abutmentAxis_old.cross(controlAxis);
	rotAxis.normalize();

	double rotAngle = acos(abutmentAxis_old.dot(controlAxis)) * 180 / 3.141593;

	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->RotateWXYZ(rotAngle, rotAxis[0], rotAxis[1], rotAxis[2]);
	tmpTrans->Update();

	auto rotMatrix = tmpTrans->GetMatrix();

	auto abutmentTipPts_matrix_old = abutmentTipPts->GetGeometry()->GetVtkMatrix();

	// Calculate the translation offset
	auto tipPts_Trans_tmp = vtkTransform::New();
	tipPts_Trans_tmp->Identity();
	tipPts_Trans_tmp->PostMultiply();
	tipPts_Trans_tmp->SetMatrix(abutmentTipPts_matrix_old);
	tipPts_Trans_tmp->Concatenate(rotMatrix);
	tipPts_Trans_tmp->Update();

	abutmentTipPts->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tipPts_Trans_tmp->GetMatrix());
	auto abutmentTip_implantSide_tmp = abutmentTipPts->GetPoint(0);
	auto abutmentTip_crownSide_tmp = abutmentTipPts->GetPoint(1);

	double offset[3]{ 0 };

	offset[0] = implant_tail[0] - abutmentTip_implantSide_tmp[0];
	offset[1] = implant_tail[1] - abutmentTip_implantSide_tmp[1];
	offset[2] = implant_tail[2] - abutmentTip_implantSide_tmp[2];

	rotMatrix->SetElement(0, 3, offset[0]);
	rotMatrix->SetElement(1, 3, offset[1]);
	rotMatrix->SetElement(2, 3, offset[2]);

	auto abutment_matrix_old = abutmentSurface->GetGeometry()->GetVtkMatrix();

	auto abutmentTrans = vtkTransform::New();
	abutmentTrans->Identity();
	abutmentTrans->PostMultiply();
	abutmentTrans->SetMatrix(abutment_matrix_old);
	abutmentTrans->Concatenate(rotMatrix);
	abutmentTrans->Update();

	auto tipPts_Trans = vtkTransform::New();
	tipPts_Trans->Identity();
	tipPts_Trans->PostMultiply();
	tipPts_Trans->SetMatrix(tipPts_Trans_tmp->GetMatrix());
	tipPts_Trans->Translate(offset);
	tipPts_Trans->Update();

	abutmentSurface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(abutmentTrans->GetMatrix());


	abutmentTipPts->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tipPts_Trans->GetMatrix());
	abutmentTipPts->GetGeometry()->Modified();

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}


void DentalAccuracy::on_pushButton_followAbutment_clicked()
{
	// Check the availability of all the data
	if (GetDataStorage()->GetNamedNode("abutment_tip_pts") == nullptr)
	{
		m_Controls.textBrowser->append("abutment_tip_pts is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("implant_tip_pts") == nullptr)
	{
		m_Controls.textBrowser->append("implant_tip_pts is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("implant_to_move") == nullptr)
	{
		m_Controls.textBrowser->append("implant_to_move is missing");
		return;
	}

	auto implantSurface = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("implant_to_move")->GetData());
	auto abutmentTipPts = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("abutment_tip_pts")->GetData());
	auto implantTipPts = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("implant_tip_pts")->GetData());


	// Assume the 1st point of abutmentTipPts is the implant side, the 2nd point is the crown side
	// Assume the 1st point of implantTipPts is the implant head point, the 2nd point is the implant tail point
	if (abutmentTipPts->GetSize() != 2)
	{
		m_Controls.textBrowser->append("abutment_tip_pts has wrong size!");
		return;
	}

	if (implantTipPts->GetSize() != 2)
	{
		m_Controls.textBrowser->append("implant_tip_pts has wrong size!");
		return;
	}

	mitk::Point3D abutment_implantSide = abutmentTipPts->GetPoint(0);
	mitk::Point3D abutment_crownSide = abutmentTipPts->GetPoint(1);
	mitk::Point3D implant_head_old = implantTipPts->GetPoint(0);
	mitk::Point3D implant_tail_old = implantTipPts->GetPoint(1);


	// Calculate the rotation matrix
	Eigen::Vector3d controlAxis{
		abutment_implantSide[0] - abutment_crownSide[0],
		abutment_implantSide[1] - abutment_crownSide[1],
		abutment_implantSide[2] - abutment_crownSide[2]
	};
	controlAxis.normalize();


	Eigen::Vector3d implantAxis_old{
		implant_head_old[0] - implant_tail_old[0],
		implant_head_old[1] - implant_tail_old[1],
		implant_head_old[2] - implant_tail_old[2]
	};
	implantAxis_old.normalize();

	Eigen::Vector3d rotAxis = implantAxis_old.cross(controlAxis);
	rotAxis.normalize();

	double rotAngle = acos(implantAxis_old.dot(controlAxis)) * 180 / 3.141593;

	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->RotateWXYZ(rotAngle, rotAxis[0], rotAxis[1], rotAxis[2]);
	tmpTrans->Update();

	auto rotMatrix = tmpTrans->GetMatrix();

	auto implantTipPts_matrix_old = implantTipPts->GetGeometry()->GetVtkMatrix();

	// Calculate the translation offset
	auto tipPts_Trans_tmp = vtkTransform::New();
	tipPts_Trans_tmp->Identity();
	tipPts_Trans_tmp->PostMultiply();
	tipPts_Trans_tmp->SetMatrix(implantTipPts_matrix_old);
	tipPts_Trans_tmp->Concatenate(rotMatrix);
	tipPts_Trans_tmp->Update();

	implantTipPts->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tipPts_Trans_tmp->GetMatrix());
	auto implantTip_head_tmp = implantTipPts->GetPoint(0);
	auto implantTip_tail_tmp = implantTipPts->GetPoint(1);

	double offset[3]{ 0 };

	offset[0] = abutment_implantSide[0] - implantTip_tail_tmp[0];
	offset[1] = abutment_implantSide[1] - implantTip_tail_tmp[1];
	offset[2] = abutment_implantSide[2] - implantTip_tail_tmp[2];

	rotMatrix->SetElement(0, 3, offset[0]);
	rotMatrix->SetElement(1, 3, offset[1]);
	rotMatrix->SetElement(2, 3, offset[2]);

	auto implant_matrix_old = implantSurface->GetGeometry()->GetVtkMatrix();

	auto implantTrans = vtkTransform::New();
	implantTrans->Identity();
	implantTrans->PostMultiply();
	implantTrans->SetMatrix(implant_matrix_old);
	implantTrans->Concatenate(rotMatrix);
	implantTrans->Update();

	auto tipPts_Trans = vtkTransform::New();
	tipPts_Trans->Identity();
	tipPts_Trans->PostMultiply();
	tipPts_Trans->SetMatrix(tipPts_Trans_tmp->GetMatrix());
	tipPts_Trans->Translate(offset);
	tipPts_Trans->Update();

	implantSurface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantTrans->GetMatrix());


	implantTipPts->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tipPts_Trans->GetMatrix());
	implantTipPts->GetGeometry()->Modified();

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

}

void DentalAccuracy::on_pushButton_followCrown_clicked()
{
	// Check the availability of all the data
	if (GetDataStorage()->GetNamedNode("crown_tip_pts") == nullptr)
	{
		m_Controls.textBrowser->append("crown_tip_pts is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("abutment_tip_pts") == nullptr)
	{
		m_Controls.textBrowser->append("abutment_tip_pts is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("abutment_to_move") == nullptr)
	{
		m_Controls.textBrowser->append("abutment_to_move is missing");
		return;
	}

	auto abutmentSurface = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("abutment_to_move")->GetData());
	auto abutmentTipPts = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("abutment_tip_pts")->GetData());
	auto crownTipPts = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("crown_tip_pts")->GetData());


	// Assume the 1st point of abutmentTipPts is the implant side, the 2nd point is the crown side
	// Assume the 1st point of crownTipPts is the crown_baseContactPoint, the 2nd point is the crown_occlusionPoint
	if (abutmentTipPts->GetSize() != 2)
	{
		m_Controls.textBrowser->append("abutment_tip_pts has wrong size!");
		return;
	}

	if (crownTipPts->GetSize() != 2)
	{
		m_Controls.textBrowser->append("crown_tip_pts has wrong size!");
		return;
	}

	mitk::Point3D abutment_implantSide_old = abutmentTipPts->GetPoint(0);
	mitk::Point3D abutment_crownSide_old = abutmentTipPts->GetPoint(1);
	mitk::Point3D crown_occlusionPoint = crownTipPts->GetPoint(1);
	mitk::Point3D crown_baseContactPoint = crownTipPts->GetPoint(0);


	// Calculate the rotation matrix
	Eigen::Vector3d controlAxis{
		crown_baseContactPoint[0] - crown_occlusionPoint[0],
		crown_baseContactPoint[1] - crown_occlusionPoint[1],
		crown_baseContactPoint[2] - crown_occlusionPoint[2]
	};
	controlAxis.normalize();


	Eigen::Vector3d abutmentAxis_old{
		abutment_implantSide_old[0] - abutment_crownSide_old[0],
		abutment_implantSide_old[1] - abutment_crownSide_old[1],
		abutment_implantSide_old[2] - abutment_crownSide_old[2]
	};
	abutmentAxis_old.normalize();

	Eigen::Vector3d rotAxis = abutmentAxis_old.cross(controlAxis);
	rotAxis.normalize();

	double rotAngle = acos(abutmentAxis_old.dot(controlAxis)) * 180 / 3.141593;

	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->RotateWXYZ(rotAngle, rotAxis[0], rotAxis[1], rotAxis[2]);
	tmpTrans->Update();

	auto rotMatrix = tmpTrans->GetMatrix();

	auto abutmentTipPts_matrix_old = abutmentTipPts->GetGeometry()->GetVtkMatrix();

	// Calculate the translation offset
	auto tipPts_Trans_tmp = vtkTransform::New();
	tipPts_Trans_tmp->Identity();
	tipPts_Trans_tmp->PostMultiply();
	tipPts_Trans_tmp->SetMatrix(abutmentTipPts_matrix_old);
	tipPts_Trans_tmp->Concatenate(rotMatrix);
	tipPts_Trans_tmp->Update();

	abutmentTipPts->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tipPts_Trans_tmp->GetMatrix());
	auto abutment_tip_implantSide_tmp = abutmentTipPts->GetPoint(0);
	auto abutment_tip_crownSide_tmp = abutmentTipPts->GetPoint(1);

	double offset[3]{ 0 };

	offset[0] = crown_baseContactPoint[0] - abutment_tip_crownSide_tmp[0];
	offset[1] = crown_baseContactPoint[1] - abutment_tip_crownSide_tmp[1];
	offset[2] = crown_baseContactPoint[2] - abutment_tip_crownSide_tmp[2];

	rotMatrix->SetElement(0, 3, offset[0]);
	rotMatrix->SetElement(1, 3, offset[1]);
	rotMatrix->SetElement(2, 3, offset[2]);

	auto abutment_matrix_old = abutmentSurface->GetGeometry()->GetVtkMatrix();

	auto abutmentTrans = vtkTransform::New();
	abutmentTrans->Identity();
	abutmentTrans->PostMultiply();
	abutmentTrans->SetMatrix(abutment_matrix_old);
	abutmentTrans->Concatenate(rotMatrix);
	abutmentTrans->Update();

	auto tipPts_Trans = vtkTransform::New();
	tipPts_Trans->Identity();
	tipPts_Trans->PostMultiply();
	tipPts_Trans->SetMatrix(tipPts_Trans_tmp->GetMatrix());
	tipPts_Trans->Translate(offset);
	tipPts_Trans->Update();

	abutmentSurface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(abutmentTrans->GetMatrix());


	abutmentTipPts->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tipPts_Trans->GetMatrix());
	abutmentTipPts->GetGeometry()->Modified();

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();


	on_pushButton_followAbutment_clicked();
}

void DentalAccuracy::on_pushButton_testMoveImplant_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant_to_move") == nullptr)
	{
		m_Controls.textBrowser->append("implant_to_move is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("implant_control_pts") == nullptr)
	{
		m_Controls.textBrowser->append("implant_control_pts is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("implant_tip_pts") == nullptr)
	{
		m_Controls.textBrowser->append("implant_tip_pts is missing");
		return;
	}

	auto implantSurface = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("implant_to_move")->GetData());
	auto controlPts = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("implant_control_pts")->GetData());
	auto tipPts = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("implant_tip_pts")->GetData());


	// Assume the 1st point of controlPts is the exit point, the 2nd point is the entry point
	// Assume the 1st point of tipPts is the implant head point, the 2nd point is the implant tail point
	if (controlPts->GetSize() != 2)
	{
		m_Controls.textBrowser->append("implant_control_pts has wrong size!");
		return;
	}

	if (tipPts->GetSize() != 2)
	{
		m_Controls.textBrowser->append("implant_tip_pts has wrong size!");
		return;
	}

	mitk::Point3D control_exit = controlPts->GetPoint(0);
	mitk::Point3D control_entry = controlPts->GetPoint(1);
	mitk::Point3D tip_head_old = tipPts->GetPoint(0);
	mitk::Point3D tip_tail_old = tipPts->GetPoint(1);


	// Calculate the rotation matrix
	Eigen::Vector3d controlAxis{
		control_exit[0] - control_entry[0],
		control_exit[1] - control_entry[1],
		control_exit[2] - control_entry[2]
	};
	controlAxis.normalize();


	Eigen::Vector3d implantAxis_old{
		tip_head_old[0] - tip_tail_old[0],
		tip_head_old[1] - tip_tail_old[1],
		tip_head_old[2] - tip_tail_old[2]
	};
	implantAxis_old.normalize();

	Eigen::Vector3d rotAxis = implantAxis_old.cross(controlAxis);
	rotAxis.normalize();

	double rotAngle = acos(implantAxis_old.dot(controlAxis)) * 180 / 3.141593;

	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->RotateWXYZ(rotAngle, rotAxis[0], rotAxis[1], rotAxis[2]);
	tmpTrans->Update();

	auto rotMatrix = tmpTrans->GetMatrix();

	auto tipPts_matrix_old = tipPts->GetGeometry()->GetVtkMatrix();

	// rotMatrix->SetElement(0, 3, tipPts_matrix_old->GetElement(0, 3));
	// rotMatrix->SetElement(1, 3, tipPts_matrix_old->GetElement(1, 3));
	// rotMatrix->SetElement(2, 3, tipPts_matrix_old->GetElement(2, 3));

	// Calculate the translation offset
	auto tipPts_Trans_tmp = vtkTransform::New();
	tipPts_Trans_tmp->Identity();
	tipPts_Trans_tmp->PostMultiply();
	tipPts_Trans_tmp->SetMatrix(tipPts_matrix_old);
	tipPts_Trans_tmp->Concatenate(rotMatrix);
	tipPts_Trans_tmp->Update();

	tipPts->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tipPts_Trans_tmp->GetMatrix());
	auto tip_head_tmp = tipPts->GetPoint(0);
	auto tip_tail_tmp = tipPts->GetPoint(1);

	double offset[3]{ 0 };

	// Determine the position mode
	if (m_Controls.radioButton_fixExit->isChecked())
	{
		offset[0] = control_exit[0] - tip_head_tmp[0];
		offset[1] = control_exit[1] - tip_head_tmp[1];
		offset[2] = control_exit[2] - tip_head_tmp[2];
	}

	// Determine the position mode
	if (m_Controls.radioButton_fixEntry->isChecked())
	{
		offset[0] = control_entry[0] - tip_tail_tmp[0];
		offset[1] = control_entry[1] - tip_tail_tmp[1];
		offset[2] = control_entry[2] - tip_tail_tmp[2];
	}

	rotMatrix->SetElement(0, 3, offset[0]);
	rotMatrix->SetElement(1, 3, offset[1]);
	rotMatrix->SetElement(2, 3, offset[2]);

	auto implant_matrix_old = implantSurface->GetGeometry()->GetVtkMatrix();

	auto implantTrans = vtkTransform::New();
	implantTrans->Identity();
	implantTrans->PostMultiply();
	implantTrans->SetMatrix(implant_matrix_old);
	implantTrans->Concatenate(rotMatrix);
	implantTrans->Update();

	auto tipPts_Trans = vtkTransform::New();
	tipPts_Trans->Identity();
	tipPts_Trans->PostMultiply();
	tipPts_Trans->SetMatrix(tipPts_Trans_tmp->GetMatrix());
	tipPts_Trans->Translate(offset);
	tipPts_Trans->Update();

	implantSurface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantTrans->GetMatrix());


	tipPts->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tipPts_Trans->GetMatrix());
	tipPts->GetGeometry()->Modified();

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void DentalAccuracy::ProjectImplantOntoPanorama()
{
	auto panorama_node = GetDataStorage()->GetNamedNode("Panorama");
	auto probeSurface_node = GetDataStorage()->GetNamedNode("Probe surface");
	auto implant_node = GetDataStorage()->GetNamedNode("implant");

	if(panorama_node == nullptr || probeSurface_node == nullptr)
	{
		m_Controls.textBrowser->append("Panorama hasn't been generated!");
		return;
	}

	if(implant_node == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing!");
		return;
	}

	// Get the implant tip point
	auto implantMatrix = implant_node->GetData()->GetGeometry()->GetVtkMatrix();
	auto maxBound_init = implant_node->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implant_node->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	mitk::Point3D implantTail_init;
	implantTail_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	implantTail_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	implantTail_init[2] = minBound_init.GetElement(2);

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->InsertPoint(implantTail_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantTipPoint = tmpPset->GetPoint(0);
	auto implantTailPoint = tmpPset->GetPoint(1);

	double tip[3]{ implantTipPoint[0], implantTipPoint[1] ,implantTipPoint[2] };
	double tail[3]{ implantTailPoint[0], implantTailPoint[1] ,implantTailPoint[2] };

	auto polydata = dynamic_cast<mitk::Surface*>(probeSurface_node->GetData())->GetVtkPolyData();

	// Build a kd-tree
	vtkSmartPointer<vtkKdTree> kdTree = vtkSmartPointer<vtkKdTree>::New();
	// kdTree->SetDataSet(polydata);
	// kdTree->BuildLocator();
	kdTree->BuildLocatorFromPoints(polydata->GetPoints());

	// Find the 1 closest points to tip
	// vtkIdType k = 1;
	vtkNew<vtkIdList> result_tip;
	vtkNew<vtkIdList> result_tail;

	kdTree->FindClosestNPoints(1, tip, result_tip);
	kdTree->FindClosestNPoints(1, tail, result_tail);

	vtkIdType point_ind_tip = result_tip->GetId(0);
	vtkIdType point_ind_tail = result_tail->GetId(0);

	double p_tip[3];
	polydata->GetPoints()->GetPoint(point_ind_tip, p_tip);

	double p_tail[3];
	polydata->GetPoints()->GetPoint(point_ind_tail, p_tail);
	
	auto projectPset = mitk::PointSet::New();
	mitk::Point3D p1;
	mitk::Point3D p2;

	for(int i{0}; i < 3; i++)
	{
		p1[i] = p_tip[i];
		p2[i] = p_tail[i];
	}

	projectPset->InsertPoint(p1);
	projectPset->InsertPoint(p2);

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetData(projectPset);
	tmpNode->SetName("implant_project");

	if(GetDataStorage()->GetNamedNode("implant_project") != nullptr)
	{
		GetDataStorage()->GetNamedNode("implant_project")->SetData(projectPset);
	}else
	{
		GetDataStorage()->Add(tmpNode);
	}

	// Test project the points onto the panorama (with known size: 251x620x60 )
	auto panoramaDims = dynamic_cast<mitk::Image*>(panorama_node->GetData())->GetDimensions();
	auto panoramaSpacings = dynamic_cast<mitk::Image*>(panorama_node->GetData())->GetVtkImageData()->GetSpacing();

	auto projectedPset = mitk::PointSet::New();

	int y_tip = static_cast<double>(point_ind_tip / panoramaDims[0]) * panoramaSpacings[1];
	int x_tip = static_cast<double>(point_ind_tip % panoramaDims[0]) * panoramaSpacings[0];

	mitk::Point3D tip_projection;
	tip_projection[0] = x_tip;
	tip_projection[1] = y_tip;
	tip_projection[2] = panoramaDims[2] * panoramaSpacings[2] / 2;

	int y_tail = static_cast<double>(point_ind_tail / panoramaDims[0])* panoramaSpacings[1];
	int x_tail = static_cast<double>(point_ind_tail % panoramaDims[0])* panoramaSpacings[0];

	mitk::Point3D tail_projection;
	tail_projection[0] = x_tail;
	tail_projection[1] = y_tail;
	tail_projection[2] = tip_projection[2];

	projectedPset->InsertPoint(tip_projection);
	projectedPset->InsertPoint(tail_projection);

	// Apply the -90 rotation along +z-axis because the panorama is rotated after generation
	auto rotateTrans = vtkTransform::New();
	rotateTrans->PostMultiply();
	rotateTrans->Identity();
	rotateTrans->RotateZ(-90);
	rotateTrans->Update();

	projectedPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(rotateTrans->GetMatrix());

	// Align the "implant_panorama" along the newly generated projectedPset
	// Copy the "implant" node data, the vtkPolyData is at the origin
	auto tmpPoly = vtkPolyData::New();
	tmpPoly->DeepCopy(dynamic_cast<mitk::Surface*>(implant_node->GetData())->GetVtkPolyData());
	auto implantProjection_surface = mitk::Surface::New();
	implantProjection_surface->SetVtkPolyData(tmpPoly);

	// Calculate the required rotation
	Eigen::Vector3d srcAxis{0,0,1};
	Eigen::Vector3d dstAxis{ projectedPset->GetPoint(0)[0] - projectedPset->GetPoint(1)[0],
		projectedPset->GetPoint(0)[1] - projectedPset->GetPoint(1)[1],
		projectedPset->GetPoint(0)[2] - projectedPset->GetPoint(1)[2]
	};
	dstAxis.normalize();

	Eigen::Vector3d rotAxis = srcAxis.cross(dstAxis);
	
	auto trans_implant = vtkTransform::New();
	trans_implant->PostMultiply();
	trans_implant->Identity();
	trans_implant->Translate(-boundCenter_init[0], -boundCenter_init[1], -boundCenter_init[2]);
	trans_implant->RotateWXYZ(90, rotAxis[0],rotAxis[1],rotAxis[2]);
	trans_implant->Translate(projectedPset->GetPoint(0)[0], projectedPset->GetPoint(0)[1], projectedPset->GetPoint(0)[2]);
	trans_implant->Update();

	implantProjection_surface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(trans_implant->GetMatrix());


	if (GetDataStorage()->GetNamedNode("implantOnPanorama") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("implantOnPanorama"));
	}

	auto testNode = mitk::DataNode::New();
	testNode->SetName("implantOnPanorama");
	testNode->SetData(implantProjection_surface);
	testNode->SetColor(0, 1, 0);
	
	GetDataStorage()->Add(testNode);


}
