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
#include "THAPlan.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

#include "mitkPointSet.h"
#include "mitkGizmo.h"

#include <sstream>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

#include "mitkApplyTransformMatrixOperation.h"
#include "mitkInteractionConst.h"

const std::string THAPlan::VIEW_ID = "org.mitk.views.thaplan";

void THAPlan::SetFocus()
{
  m_Controls.pushButton_preop->setFocus();
}

void THAPlan::onPushButton_init_clicked()
{
	//todo init should clean all previous memery
	m_cupPlanMatrix = Eigen::Matrix4d::Identity();
	m_stemPlanMatrix = Eigen::Matrix4d::Identity();

	if (!initPelvis() || !initFemurL() || !initFemurR() || !initCup() || !initStem() || !initLiner() || !initHead())
	{
		return;
	}
	m_Reduction = othopedics::Reduction::New();
	m_Reduction->SetPelvis(m_Pelvis);

	m_Reduction->SetCup(m_Cup);
	m_Reduction->SetStem(m_Stem);

	if (m_OperationSide == othopedics::ESide::left)
	{
		m_Reduction->SetFemur_op(m_Femur_L);
		m_Reduction->SetFemur_contra(m_Femur_R);
	}
	else
	{
		m_Reduction->SetFemur_op(m_Femur_R);
		m_Reduction->SetFemur_contra(m_Femur_L);
	}

	m_Reduction->SetLiner(m_Liner);
	m_Reduction->SetHead(m_Head);

	m_Reduction->PreOperativeReduction_Canal();
	//initial pose of cup and stem
	restoreCupPose();
	restoreStemPose();

	//enable buttons
	m_Controls.pushButton_preop->setDisabled(false);
	m_Controls.pushButton_cupPlan->setDisabled(false);
	m_Controls.pushButton_stemPlan->setDisabled(false);
	m_Controls.pushButton_reduction->setDisabled(false);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	m_state = EState::READY;
}

void THAPlan::HideAllNode(mitk::DataStorage* dataStorage)
{
	if (dataStorage != nullptr)
	{
		auto nodeset = dataStorage->GetAll();
		for (auto it = nodeset->Begin(); it != nodeset->End(); ++it)
		{
			mitk::DataNode* node = it->Value();
			if (nullptr != node)
			{
				node->SetVisibility(false);
			}
		}
	}
}

void THAPlan::listenCupGeoModify()
{
	auto observer = itk::SimpleMemberCommand<THAPlan>::New();
	observer->SetCallbackFunction(this, &THAPlan::onCupGeoModified);

	if (m_CupGeometry.IsNotNull())
	{
		m_CupGeometry->RemoveObserver(m_CupListenTag);
	}
	if (m_dn_cup!=nullptr)
	{
		m_CupGeometry = m_dn_cup->GetData()->GetGeometry();
		m_CupListenTag = m_CupGeometry->AddObserver(itk::ModifiedEvent(), observer);
	}

	//init update
	updateCupPlanMatrix();
}

void THAPlan::onCupGeoModified()
{
	updateCupPlanMatrix();
	updateHipLengthAndOffset();
	updateUI_HipLengthAndOffset();
}

void THAPlan::updateCupPlanMatrix()
{
	if (m_Pelvis != nullptr && m_Cup != nullptr)
	{
		Eigen::Matrix4d Tworld2pelvis = m_Pelvis->m_T_world_local;
		Eigen::Matrix4d Tworld2cup = m_Cup->m_T_world_local;
		Eigen::Matrix4d Tpelvis2cup = Tworld2pelvis.inverse() * Tworld2cup;
		m_cupPlanMatrix = Tpelvis2cup;
	}
}

void THAPlan::listenStemGeoModify()
{
	auto observer = itk::SimpleMemberCommand<THAPlan>::New();
	observer->SetCallbackFunction(this, &THAPlan::onStemGeoModified);

	if (m_StemGeometry.IsNotNull())
	{
		m_StemGeometry->RemoveObserver(m_StemListenTag);
	}
	if (m_dn_stem != nullptr)
	{
		m_StemGeometry = m_dn_stem->GetData()->GetGeometry();
		m_StemListenTag = m_StemGeometry->AddObserver(itk::ModifiedEvent(), observer);
	}
}

void THAPlan::onStemGeoModified()
{
	updateStemPlanMatrix();
	updateHipLengthAndOffset();
	updateUI_HipLengthAndOffset();
}

void THAPlan::updateStemPlanMatrix()
{
	if (m_Femur_L != nullptr && m_Femur_R && m_Stem != nullptr)
	{
		// othopedics::Femur::Pointer femur;
		// if (m_OperationSide == othopedics::ESide::left)
		// {
		// 	femur = m_Femur_L;
		// }
		// else
		// {
		// 	femur = m_Femur_R;
		// }
		// Eigen::Matrix4d Tworld2femurl = femur->m_T_world_local;
		// Eigen::Matrix4d Tworld2steml = m_Stem->m_T_world_local;
		// Eigen::Matrix4d Tfemurl2steml = Tworld2femurl.inverse() * Tworld2steml;
		// m_stemPlanMatrix = Tfemurl2steml;
		m_stemPlanMatrix = m_Stem->m_T_world_local;
	}
}

void THAPlan::Show(Eigen::Vector3d point, std::string name)
{
	auto pointSet = mitk::PointSet::New();
	pointSet->InsertPoint(point.data());

	auto node = mitk::DataNode::New();

	node->SetData(pointSet);
	node->SetName(name);
	node->SetFloatProperty("pointsize", 10);
	double ref[3]{ 0, 0, 0 };
	mitk::Point3D refp{ ref };

	GetDataStorage()->Add(node);
}

void THAPlan::Show(Eigen::Matrix4d transform, std::string name)
{
	mitk::Surface::Pointer mySphere = mitk::Surface::New();

	double axisLength = 5.;

	vtkSmartPointer<vtkSphereSource> vtkSphere = vtkSmartPointer<vtkSphereSource>::New();
	vtkSmartPointer<vtkConeSource> vtkCone = vtkSmartPointer<vtkConeSource>::New();
	vtkSmartPointer<vtkCylinderSource> vtkCylinder = vtkSmartPointer<vtkCylinderSource>::New();
	vtkSmartPointer<vtkPolyData> axis = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkLineSource> vtkLine = vtkSmartPointer<vtkLineSource>::New();
	vtkSmartPointer<vtkLineSource> vtkLine2 = vtkSmartPointer<vtkLineSource>::New();
	vtkSmartPointer<vtkLineSource> vtkLine3 = vtkSmartPointer<vtkLineSource>::New();

	vtkSmartPointer<vtkAppendPolyData> appendPolyData = vtkSmartPointer<vtkAppendPolyData>::New();
	vtkSmartPointer<vtkPolyData> surface = vtkSmartPointer<vtkPolyData>::New();

	//Y-Axis (start with y, cause cylinder is oriented in y by vtk default...)
	vtkCone->SetDirection(0, 1, 0);
	vtkCone->SetHeight(1.0);
	vtkCone->SetRadius(0.4f);
	vtkCone->SetResolution(16);
	vtkCone->SetCenter(0.0, axisLength, 0.0);
	vtkCone->Update();

	vtkCylinder->SetRadius(0.05);
	vtkCylinder->SetHeight(axisLength);
	vtkCylinder->SetCenter(0.0, 0.5 * axisLength, 0.0);
	vtkCylinder->Update();

	appendPolyData->AddInputData(vtkCone->GetOutput());
	appendPolyData->AddInputData(vtkCylinder->GetOutput());
	appendPolyData->Update();
	axis->DeepCopy(appendPolyData->GetOutput());

	//y symbol
	vtkLine->SetPoint1(-0.5, axisLength + 2., 0.0);
	vtkLine->SetPoint2(0.0, axisLength + 1.5, 0.0);
	vtkLine->Update();

	vtkLine2->SetPoint1(0.5, axisLength + 2., 0.0);
	vtkLine2->SetPoint2(-0.5, axisLength + 1., 0.0);
	vtkLine2->Update();

	appendPolyData->AddInputData(vtkLine->GetOutput());
	appendPolyData->AddInputData(vtkLine2->GetOutput());
	appendPolyData->AddInputData(axis);
	appendPolyData->Update();
	surface->DeepCopy(appendPolyData->GetOutput());

	//X-axis
	vtkSmartPointer<vtkTransform> XTransform = vtkSmartPointer<vtkTransform>::New();
	XTransform->RotateZ(-90);
	vtkSmartPointer<vtkTransformPolyDataFilter> TrafoFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	TrafoFilter->SetTransform(XTransform);
	TrafoFilter->SetInputData(axis);
	TrafoFilter->Update();

	//x symbol
	vtkLine->SetPoint1(axisLength + 2., -0.5, 0.0);
	vtkLine->SetPoint2(axisLength + 1., 0.5, 0.0);
	vtkLine->Update();

	vtkLine2->SetPoint1(axisLength + 2., 0.5, 0.0);
	vtkLine2->SetPoint2(axisLength + 1., -0.5, 0.0);
	vtkLine2->Update();

	appendPolyData->AddInputData(vtkLine->GetOutput());
	appendPolyData->AddInputData(vtkLine2->GetOutput());
	appendPolyData->AddInputData(TrafoFilter->GetOutput());
	appendPolyData->AddInputData(surface);
	appendPolyData->Update();
	surface->DeepCopy(appendPolyData->GetOutput());

	//Z-axis
	vtkSmartPointer<vtkTransform> ZTransform = vtkSmartPointer<vtkTransform>::New();
	ZTransform->RotateX(90);
	TrafoFilter->SetTransform(ZTransform);
	TrafoFilter->SetInputData(axis);
	TrafoFilter->Update();

	//z symbol
	vtkLine->SetPoint1(-0.5, 0.0, axisLength + 2.);
	vtkLine->SetPoint2(0.5, 0.0, axisLength + 2.);
	vtkLine->Update();

	vtkLine2->SetPoint1(-0.5, 0.0, axisLength + 2.);
	vtkLine2->SetPoint2(0.5, 0.0, axisLength + 1.);
	vtkLine2->Update();

	vtkLine3->SetPoint1(0.5, 0.0, axisLength + 1.);
	vtkLine3->SetPoint2(-0.5, 0.0, axisLength + 1.);
	vtkLine3->Update();

	appendPolyData->AddInputData(vtkLine->GetOutput());
	appendPolyData->AddInputData(vtkLine2->GetOutput());
	appendPolyData->AddInputData(vtkLine3->GetOutput());
	appendPolyData->AddInputData(TrafoFilter->GetOutput());
	appendPolyData->AddInputData(surface);
	appendPolyData->Update();
	surface->DeepCopy(appendPolyData->GetOutput());

	//Center
	vtkSphere->SetRadius(0.5f);
	vtkSphere->SetCenter(0.0, 0.0, 0.0);
	vtkSphere->Update();

	appendPolyData->AddInputData(vtkSphere->GetOutput());
	appendPolyData->AddInputData(surface);
	appendPolyData->Update();
	surface->DeepCopy(appendPolyData->GetOutput());

	//Scale
	vtkSmartPointer<vtkTransform> ScaleTransform = vtkSmartPointer<vtkTransform>::New();
	ScaleTransform->Scale(20., 20., 20.);

	TrafoFilter->SetTransform(ScaleTransform);
	TrafoFilter->SetInputData(surface);
	TrafoFilter->Update();

	mySphere->SetVtkPolyData(TrafoFilter->GetOutput());

	auto node = mitk::DataNode::New();

	node->SetData(mySphere);
	node->SetName(name);

	double ref[3]{ 0, 0, 0 };
	mitk::Point3D refp{ ref };

	vtkSmartPointer<vtkMatrix4x4> coord = vtkMatrix4x4::New();
	othopedics::EigenToVtkMatrix4x4(transform, coord);

	auto* doOp = new mitk::ApplyTransformMatrixOperation(mitk::OpAPPLYTRANSFORMMATRIX, coord, refp);
	node->GetData()->GetGeometry()->ExecuteOperation(doOp);
	delete doOp;

	GetDataStorage()->Add(node);
	//RequestRenderWindowUpdate();
}

void THAPlan::onPushButton_preop_clicked()
{
	m_state = EState::Preoperative;

	m_Reduction->PreOperativeReduction_Canal();
	m_Reduction->CalPreopOffset();
	m_Reduction->PreOperativeReduction_Mechanical();
	m_Reduction->CalPreopHipLength();

	FlushAllTransform();

	//hide all node,make it clear
	HideAllNode(GetDataStorage());

	//show pelvis and femur
	m_dn_pelvis->SetVisibility(true);
	m_dn_femurL->SetVisibility(true);
	m_dn_femurR->SetVisibility(true);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(GetDataStorage());

	updateUI_HipLengthAndOffset();
	//show result
	// QString opSide;
	// QString state = "Preoperative";
	// if (m_OperationSide == othopedics::ESide::left)
	// {
	// 	opSide = "(Left)";
	// }
	// else
	// {
	// 	opSide = "(Right)";
	// }
	// m_Controls.label_lengthOffsetHeader->setText(state + opSide);
	//
	// double Offset_preop, Offset_contra, OffsetDiff_preop_vs_contra;
	// m_Reduction->GetResult(othopedics::EResult::r_Offset_preop, Offset_preop);
	// m_Reduction->GetResult(othopedics::EResult::r_Offset_contra, Offset_contra);
	// m_Reduction->GetResult(othopedics::EResult::r_OffsetDiff_preop_vs_contra, OffsetDiff_preop_vs_contra);
	//
	// double hipLength_preop, hipLength_contra, hipLengthDiff_preop_vs_contra;
	// m_Reduction->GetResult(othopedics::EResult::r_HipLength_preop, hipLength_preop);
	// m_Reduction->GetResult(othopedics::EResult::r_HipLength_contra, hipLength_contra);
	// m_Reduction->GetResult(othopedics::EResult::r_HipLengthDiff_preop_vs_contra, hipLengthDiff_preop_vs_contra);
	//
	// std::stringstream ss;
	// ss << "Offset_preop: " << Offset_preop <<std::endl;
	// ss << "Offset_contra: " << Offset_contra << std::endl;
	// ss << "OffsetDiff_preop_vs_contra: " << OffsetDiff_preop_vs_contra << std::endl;
	// ss << "r_HipLength_preop: " << hipLength_preop << std::endl;
	// ss << "r_HipLength_contra: " << hipLength_contra << std::endl;
	// ss << "r_HipLengthDiff_preop_vs_contra: " << hipLengthDiff_preop_vs_contra << std::endl;
	//
	// m_Controls.textBrowser->setText(QString::fromStdString(ss.str()));
	//
	// m_Controls.lineEdit_HipLength_vsOpposite->setText(QString::number(hipLengthDiff_preop_vs_contra, 'f', 2));
	// m_Controls.lineEdit_Offset_vsOpposite->setText(QString::number(OffsetDiff_preop_vs_contra, 'f', 2));
	// m_Controls.lineEdit_HipLength_vsPreop->setText("N/A");
	// m_Controls.lineEdit_Offset_vsPreop->setText("N/A");
}

bool THAPlan::restoreCupPose()
{
	Eigen::Matrix4d transform;
	//if not planed, place to init pose
	if (m_cupPlanMatrix.isIdentity())
	{
		//todo move cup init pose to config file
		double Inclination = 40.0;
		double Anteversion = 20.0;

		transform = CalApplyCupAngleMatrix(Eigen::Vector3d(0, 0, 0), Anteversion, Inclination, m_OperationSide);
		Eigen::Vector3d COR;
		if (m_Pelvis == nullptr)
		{
			MITK_WARN << "Cup Plan failed: missing pelvis body";
			return false;
		}
		if (m_OperationSide == othopedics::ESide::left)
		{
			m_Pelvis->GetGlobalLandMark(othopedics::ELandMarks::p_FemurAssemblyPoint_L, COR);
		}
		else
		{
			m_Pelvis->GetGlobalLandMark(othopedics::ELandMarks::p_FemurAssemblyPoint_R, COR);
		}
		
		transform.block<3, 1>(0, 3) += COR;
		m_cupPlanMatrix = transform;
	}

	m_Cup->SetIndexToWorldTransform(m_cupPlanMatrix);
	return true;
}

bool THAPlan::restoreStemPose()
{
	Eigen::Matrix4d transform;
	//if not planed, place to init pose
	othopedics::Femur::Pointer femur;
	if (m_OperationSide == othopedics::ESide::left)
	{
		femur = m_Femur_L;
	}
	else
	{
		femur = m_Femur_R;
	}

	if (m_stemPlanMatrix.isIdentity())
	{
		Eigen::Vector3d femurP, StemP;
		//m_stemPlanMatrix is in Femur coords ,so we don't need to use GetGlobalLandMark
		if (!femur->GetLandMark(othopedics::ELandMarks::f_NeckCanalIntersectPoint, femurP)
			|| !m_Stem->GetLandMark(othopedics::ELandMarks::stem_NeckCanalIntersectPoint, StemP))
		{
			MITK_WARN << "placeStemL failed: missing landmark";
			return false; 
		}
		Eigen::Vector3d t = femurP - StemP;

		transform.setIdentity();
		transform.block<3, 1>(0, 3) = t;
		//m_Stem->SetIndexToWorldTransform(transform);
		m_stemPlanMatrix = transform;
	}

	m_Stem->SetIndexToWorldTransform(femur->m_T_world_local * m_stemPlanMatrix);
	return true;
}

void THAPlan::onPushButton_cupPlan_clicked()
{
	m_state = EState::Plan;
	if (m_Cup == nullptr || m_Pelvis == nullptr)
	{
		MITK_WARN << "Cup Plan failed: missing bodies";
		return;
	}
	//m_Cup->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	//hide all node,make it clear
	HideAllNode(GetDataStorage());

	//show pelvis and cup
	m_dn_pelvis->SetVisibility(true);
	m_dn_cup->SetVisibility(true);

	//todo save and restore cup pose
	restoreCupPose();
	FlushAllTransform();
	//place a gizmo to adjust cup pose
	mitk::Gizmo::AddGizmoToNode(m_dn_cup,GetDataStorage());

	//whenever cup place moved,m_cupPlanMatrix should be updated
	listenCupGeoModify();

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(GetDataStorage());
}

void THAPlan::onPushButton_stemPlan_clicked()
{
	m_state = EState::Plan;
	//m_Reduction->PreOperativeReduction_Canal();
	m_Stem->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	if (m_Cup == nullptr || m_Femur_L == nullptr || m_Femur_R == nullptr)
	{
		MITK_WARN << "Stem Plan failed: missing bodies";
		return;
	}
	//hide all node,make it clear
	HideAllNode(GetDataStorage());

	//show femur and stem
	m_dn_stem->SetVisibility(true);
	
	if (m_OperationSide == othopedics::ESide::left)
	{
		m_dn_femurL->SetVisibility(true);
		m_Femur_L->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	}
	else
	{
		m_dn_femurR->SetVisibility(true);
		m_Femur_R->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	}

	//todo save and restore cup pose
	restoreStemPose();
	FlushAllTransform();
	//place a gizmo to adjust stem pose
	mitk::Gizmo::AddGizmoToNode(m_dn_stem, GetDataStorage());

	//whenever stem moved,m_cupPlanMatrix should be updated
	listenStemGeoModify();

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(GetDataStorage());
}

void THAPlan::updateUI_HipLengthAndOffset()
{
	QString opSide;
	QString state = to_string(m_state);
	if (m_OperationSide == othopedics::ESide::left)
	{
		opSide = "(Left)";
	}
	else
	{
		opSide = "(Right)";
	}
	m_Controls.label_lengthOffsetHeader->setText(state + opSide);

	if (m_state == EState::Plan)
	{
		double Offset_planed, Offset_contra, OffsetDiff_planed_vs_contra, OffsetDiff_planed_vs_preop;
		m_Reduction->GetResult(othopedics::EResult::r_Offset_planed, Offset_planed);
		m_Reduction->GetResult(othopedics::EResult::r_Offset_contra, Offset_contra);
		m_Reduction->GetResult(othopedics::EResult::r_OffsetDiff_planed_vs_contra, OffsetDiff_planed_vs_contra);
		m_Reduction->GetResult(othopedics::EResult::r_OffsetDiff_planed_vs_preop, OffsetDiff_planed_vs_preop);

		double hipLength_planed, hipLength_contra, hipLengthDiff_planed_vs_contra, hipLengthDiff_planed_vs_preop;
		m_Reduction->GetResult(othopedics::EResult::r_HipLength_planed, hipLength_planed);
		m_Reduction->GetResult(othopedics::EResult::r_HipLength_contra, hipLength_contra);
		m_Reduction->GetResult(othopedics::EResult::r_HipLengthDiff_planed_vs_contra, hipLengthDiff_planed_vs_contra);
		m_Reduction->GetResult(othopedics::EResult::r_HipLengthDiff_planed_vs_preop, hipLengthDiff_planed_vs_preop);

		std::stringstream ss;
		ss << "Offset_planed: " << Offset_planed << std::endl;
		ss << "Offset_contra: " << Offset_contra << std::endl;
		ss << "OffsetDiff_planed_vs_contra: " << OffsetDiff_planed_vs_contra << std::endl;
		ss << "OffsetDiff_planed_vs_preop: " << OffsetDiff_planed_vs_preop << std::endl;

		ss << "hipLength_planed: " << hipLength_planed << std::endl;
		ss << "hipLength_contra: " << hipLength_contra << std::endl;
		ss << "hipLengthDiff_planed_vs_contra: " << hipLengthDiff_planed_vs_contra << std::endl;
		ss << "hipLengthDiff_planed_vs_preop: " << hipLengthDiff_planed_vs_preop << std::endl;


		m_Controls.textBrowser->setText(QString::fromStdString(ss.str()));

		m_Controls.lineEdit_HipLength_vsOpposite->setText(QString::number(hipLengthDiff_planed_vs_contra, 'f', 2));
		m_Controls.lineEdit_Offset_vsOpposite->setText(QString::number(OffsetDiff_planed_vs_contra, 'f', 2));
		m_Controls.lineEdit_HipLength_vsPreop->setText(QString::number(hipLengthDiff_planed_vs_preop, 'f', 2));
		m_Controls.lineEdit_Offset_vsPreop->setText(QString::number(OffsetDiff_planed_vs_preop, 'f', 2));
	}
	else if(m_state == EState::Preoperative)
	{
		double Offset_preop, Offset_contra, OffsetDiff_preop_vs_contra;
		m_Reduction->GetResult(othopedics::EResult::r_Offset_preop, Offset_preop);
		m_Reduction->GetResult(othopedics::EResult::r_Offset_contra, Offset_contra);
		m_Reduction->GetResult(othopedics::EResult::r_OffsetDiff_preop_vs_contra, OffsetDiff_preop_vs_contra);

		double hipLength_preop, hipLength_contra, hipLengthDiff_preop_vs_contra;
		m_Reduction->GetResult(othopedics::EResult::r_HipLength_preop, hipLength_preop);
		m_Reduction->GetResult(othopedics::EResult::r_HipLength_contra, hipLength_contra);
		m_Reduction->GetResult(othopedics::EResult::r_HipLengthDiff_preop_vs_contra, hipLengthDiff_preop_vs_contra);

		std::stringstream ss;
		ss << "Offset_preop: " << Offset_preop << std::endl;
		ss << "Offset_contra: " << Offset_contra << std::endl;
		ss << "OffsetDiff_preop_vs_contra: " << OffsetDiff_preop_vs_contra << std::endl;
		ss << "r_HipLength_preop: " << hipLength_preop << std::endl;
		ss << "r_HipLength_contra: " << hipLength_contra << std::endl;
		ss << "r_HipLengthDiff_preop_vs_contra: " << hipLengthDiff_preop_vs_contra << std::endl;

		m_Controls.textBrowser->setText(QString::fromStdString(ss.str()));

		m_Controls.lineEdit_HipLength_vsOpposite->setText(QString::number(hipLengthDiff_preop_vs_contra, 'f', 2));
		m_Controls.lineEdit_Offset_vsOpposite->setText(QString::number(OffsetDiff_preop_vs_contra, 'f', 2));
		m_Controls.lineEdit_HipLength_vsPreop->setText("N/A");
		m_Controls.lineEdit_Offset_vsPreop->setText("N/A");
	}
}

void THAPlan::updateHipLengthAndOffset()
{
	m_Reduction->PlanReduction_Canal(m_cupPlanMatrix, m_stemPlanMatrix);
	m_Reduction->CalPlanedOffset();
	m_Reduction->PlanReduction_Mechanical(m_cupPlanMatrix, m_stemPlanMatrix);
	m_Reduction->CalPlanedHipLength();
}

void THAPlan::FlushAllTransform()
{
	m_Pelvis->FlushTransform();
	m_Cup->FlushTransform();
	m_Stem->FlushTransform();
	m_Femur_L->FlushTransform();
	m_Femur_R->FlushTransform();
	m_Head->FlushTransform();
	m_Liner->FlushTransform();
}

void THAPlan::onPushButton_reduction_clicked()
{
	m_state = EState::Plan;
	//todo if skip cup and stem plan,reduction should place cup and stem initial pose
	//hide all node,make it clear
	HideAllNode(GetDataStorage());

	//show femur, stem, pelvis,cup
	m_dn_stem->SetVisibility(true);
	m_dn_femurL->SetVisibility(true);
	m_dn_femurR->SetVisibility(true);
	m_dn_cup->SetVisibility(true);
	m_dn_pelvis->SetVisibility(true);
	m_dn_head->SetVisibility(true);
	m_dn_liner->SetVisibility(true);


	updateHipLengthAndOffset();
	
	FlushAllTransform();
	Eigen::Vector3d CupCOR, StemAssPoint;
	m_Cup->GetGlobalLandMark(othopedics::ELandMarks::cup_COR, CupCOR);
	m_Stem->GetGlobalLandMark(othopedics::ELandMarks::stem_HeadAssemblyPoint_M, StemAssPoint);
	Show(CupCOR, "cupCOR");
	Show(StemAssPoint, "StemAssPoint");

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(GetDataStorage());

	//show result
	updateUI_HipLengthAndOffset();
}

void THAPlan::OnPushButton_test1_clicked()
{
	Eigen::Matrix4d origin = Eigen::Matrix4d::Identity();
	Show(origin,"Origin");
}

void THAPlan::testOdd45dTransform()
{
	Eigen::Matrix3d rot = Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0, 0, -1).normalized(), Eigen::Vector3d(1, 0, -1).normalized()).matrix();
	Eigen::Matrix4d headTrans;
	headTrans.setIdentity();
	headTrans.block<3, 3>(0, 0) = rot;

	auto cup = GetDataStorage()->GetNamedObject<mitk::Surface>("Cup_54");

	//m_Cup->SetIndexToWorldTransform(headTrans);
	vtkSmartPointer<vtkMatrix4x4> trans = vtkMatrix4x4::New();
	othopedics::EigenToVtkMatrix4x4(headTrans, trans);
	cup->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(trans);
	std::cout << "headTrans" << std::endl;
	std::cout << headTrans <<std::endl;
}

void THAPlan::OnPushButton_test2_clicked()
{
	Eigen::Matrix3d rot = Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0, 0.00001, -1).normalized(), Eigen::Vector3d(1, 0, -1).normalized()).matrix();
	Eigen::Matrix4d headTrans;
	headTrans.setIdentity();
	headTrans.block<3, 3>(0, 0) = rot;

	auto cup = GetDataStorage()->GetNamedObject<mitk::Surface>("Cup_54");

	//m_Cup->SetIndexToWorldTransform(headTrans);
	vtkSmartPointer<vtkMatrix4x4> trans = vtkMatrix4x4::New();
	othopedics::EigenToVtkMatrix4x4(headTrans, trans);
	cup->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(trans);
	std::cout << "headTrans" << std::endl;
	std::cout << headTrans << std::endl;
}

bool THAPlan::initPelvis()
{
	m_Pelvis = othopedics::Pelvis::New();
	//Obtain the marker points attached to the pelvic body 
	mitk::PointSet::PointType ASIS_L, ASIS_R, MidLine, FHC_L, FHC_R;
	if (getPoint("ASIS_L", &ASIS_L) && getPoint("ASIS_R", &ASIS_R) && getPoint("MidLine", &MidLine)
		&& getPoint("FHC_L", &FHC_L) && getPoint("FHC_R", &FHC_R))
	{
		m_Pelvis->SetLandMark(othopedics::ELandMarks::p_ASIS_L, ASIS_L.data());
		m_Pelvis->SetLandMark(othopedics::ELandMarks::p_ASIS_R, ASIS_R.data());
		m_Pelvis->SetLandMark(othopedics::ELandMarks::p_MidLine, MidLine.data());
		m_Pelvis->SetLandMark(othopedics::ELandMarks::p_FemurAssemblyPoint_L, FHC_L.data());
		m_Pelvis->SetLandMark(othopedics::ELandMarks::p_FemurAssemblyPoint_R, FHC_R.data());
	}
	else
	{
		MITK_WARN << "Pelvis init failed: missing landmarks";
		return false;
	}
	m_dn_pelvis = GetDataStorage()->GetNamedNode("pelvis");
	if (m_dn_pelvis == nullptr)
	{
		MITK_WARN << "Pelvis init failed: missing surface";
		return false;
	}
	m_Pelvis->SetSurface(dynamic_cast<mitk::Surface*>(m_dn_pelvis->GetData()));

	return m_Pelvis->Init();
}

bool THAPlan::initFemurR()
{
	m_Femur_R = new othopedics::Femur;
	m_Femur_R->m_Side = othopedics::ESide::right;
	//Set LandMarks
	mitk::PointSet::PointType FHC_R, FNC_R, PFCA_R, DFCA_R, ME_R, LE_R, LT_R;
	if (getPoint("FHC_R", &FHC_R) && getPoint("FNC_R", &FNC_R)
		&& getPoint("PFCA_R", &PFCA_R) && getPoint("DFCA_R", &DFCA_R)
		&& getPoint("ME_R", &ME_R) && getPoint("LE_R", &LE_R)
		&& getPoint("LT_R", &LT_R))
	{

		m_Femur_R->SetLandMark(othopedics::ELandMarks::f_FHC, FHC_R.data());
		m_Femur_R->SetLandMark(othopedics::ELandMarks::f_FNC, FNC_R.data());
		m_Femur_R->SetLandMark(othopedics::ELandMarks::f_PFCA, PFCA_R.data());
		m_Femur_R->SetLandMark(othopedics::ELandMarks::f_DFCA, DFCA_R.data());
		m_Femur_R->SetLandMark(othopedics::ELandMarks::f_ME, ME_R.data());
		m_Femur_R->SetLandMark(othopedics::ELandMarks::f_LE, LE_R.data());
		m_Femur_R->SetLandMark(othopedics::ELandMarks::f_LT, LT_R.data());
	}
	else
	{
		MITK_WARN << "Femur init failed: missing landmarks";
		return false;
	}

	//Set Surface
	m_dn_femurR = GetDataStorage()->GetNamedNode("femur_R");
	if (m_dn_femurR == nullptr)
	{
		MITK_WARN << "Femur init failed: missing surface";
		return false;
	}
	m_Femur_R->SetSurface(dynamic_cast<mitk::Surface*>(m_dn_femurR->GetData()));
	//todo Set Image

	return m_Femur_R->Init();
}

bool THAPlan::initFemurL()
{
	m_Femur_L = new othopedics::Femur;
	m_Femur_L->m_Side = othopedics::ESide::left;
	//Set LandMarks
	mitk::PointSet::PointType FHC_L, FNC_L, PFCA_L, DFCA_L, ME_L, LE_L, LT_L;
	if (getPoint("FHC_L", &FHC_L) && getPoint("FNC_L", &FNC_L)
		&& getPoint("PFCA_L", &PFCA_L) && getPoint("DFCA_L", &DFCA_L)
		&& getPoint("ME_L", &ME_L) && getPoint("LE_L", &LE_L)
		&& getPoint("LT_L", &LT_L))
	{

		m_Femur_L->SetLandMark(othopedics::ELandMarks::f_FHC, FHC_L.data());
		m_Femur_L->SetLandMark(othopedics::ELandMarks::f_FNC, FNC_L.data());
		m_Femur_L->SetLandMark(othopedics::ELandMarks::f_PFCA, PFCA_L.data());
		m_Femur_L->SetLandMark(othopedics::ELandMarks::f_DFCA, DFCA_L.data());
		m_Femur_L->SetLandMark(othopedics::ELandMarks::f_ME, ME_L.data());
		m_Femur_L->SetLandMark(othopedics::ELandMarks::f_LE, LE_L.data());
		m_Femur_L->SetLandMark(othopedics::ELandMarks::f_LT, LT_L.data());
	}
	else
	{
		MITK_WARN << "FemurL init failed: missing landmarks";
		return false;
	}
	//Set Surface
	m_dn_femurL = GetDataStorage()->GetNamedNode("femur_L");
	if (m_dn_femurL == nullptr)
	{
		MITK_WARN << "FemurL init failed: missing surface";
		return false;
	}
	m_Femur_L->SetSurface(dynamic_cast<mitk::Surface*>(m_dn_femurL->GetData()));
	//todo Set Image

	return m_Femur_L->Init();
}

bool THAPlan::initCup(std::string size)
{
	m_Cup = othopedics::Cup::New();
	m_Cup->m_Side = m_OperationSide;
	//hard code cup parameter
	double cor[3]{ 0,0,-1.62 };
	m_Cup->SetLandMark(othopedics::ELandMarks::cup_COR, cor);
	m_dn_cup = GetDataStorage()->GetNamedNode(size);

	if (m_dn_cup == nullptr)
	{
		MITK_WARN << "Cup init failed: missing surface";
		return false;
	}
	m_Cup->SetSurface(dynamic_cast<mitk::Surface*>(m_dn_cup->GetData()));
	return m_Cup->Init();
}

bool THAPlan::initStem(std::string size)
{
	m_Stem = othopedics::Stem::New();
	m_Stem->m_Side = othopedics::ESide::left;

	//hard code stem parameter
	//#4
	// double m[3]{ 0,0,0 };
	// double p1[3]{ 23.26, 0.0, -23.26 };
	// double p2[3]{ 39.55,0.0 ,-39.55 };
	// double p3[3]{ 39.55,0.0 ,-158.75 };
	//#6
	double l[3]{ -2.83 ,0,2.83 };
	double m[3]{ 0,0,0 };
	double s[3]{ 2.83,0,-2.83 };
	m_Stem->SetLandMark(othopedics::ELandMarks::stem_HeadAssemblyPoint_M, m);
	m_Stem->SetLandMark(othopedics::ELandMarks::stem_HeadAssemblyPoint_L, l);
	m_Stem->SetLandMark(othopedics::ELandMarks::stem_HeadAssemblyPoint_S, s);

	double p1[3]{ 40.96, 0.0, -40.96 };
	double p2[3]{ 23.62,0.0 ,-23.62 };
	double p3[3]{ 40.96,0.0 ,-169.1 };
	m_Stem->SetLandMark(othopedics::ELandMarks::stem_CutPoint, p2);
	m_Stem->SetLandMark(othopedics::ELandMarks::stem_NeckCanalIntersectPoint, p1);
	m_Stem->SetLandMark(othopedics::ELandMarks::stem_EndPoint, p3);

	Eigen::Vector3d v_neck = (Eigen::Vector3d(p2) - Eigen::Vector3d(m)).normalized();
	m_Stem->SetAxis(othopedics::EAxes::stem_Neck, m, v_neck.data());

	m_dn_stem = GetDataStorage()->GetNamedNode(size);
	if (m_dn_stem == nullptr)
	{
		MITK_WARN << "Stem init failed: missing surface";
		return false;
	}
	m_Stem->SetSurface(dynamic_cast<mitk::Surface*>(m_dn_stem->GetData()));
	return m_Stem->Init();
}

bool THAPlan::initLiner(std::string size)
{
	m_Liner = othopedics::Body::New();

	m_dn_liner = GetDataStorage()->GetNamedNode(size);
	if (m_dn_liner == nullptr)
	{
		MITK_WARN << "Liner init failed: missing surface";
		return false;
	}
	m_Liner->SetSurface(dynamic_cast<mitk::Surface*>(m_dn_liner->GetData()));
	return m_Liner->Init();
}

bool THAPlan::initHead(std::string size)
{
	m_Head = othopedics::Body::New();

	m_dn_head = GetDataStorage()->GetNamedNode(size);
	if (m_dn_head == nullptr)
	{
		MITK_WARN << "Head init failed: missing surface";
		return false;
	}
	m_Head->SetSurface(dynamic_cast<mitk::Surface*>(m_dn_head->GetData()));
	return m_Head->Init();
}

bool THAPlan::getPoint(std::string name, mitk::PointSet::PointType* point, unsigned index) const
{
	auto dn = GetDataStorage()->GetNamedNode(name);
	if (dn == nullptr)
	{
		MITK_ERROR << "getPoint Error: no DataNode named " << name;
		return false;
	}
	auto pointSet = dynamic_cast<mitk::PointSet*>(dn->GetData());
	if (pointSet == nullptr)
	{
		MITK_ERROR << "getPoint Error: The DataNode is not a PointSet  " << name;
		return false;
	}
	pointSet->GetPointIfExists(index, point);
	return true;
}

void THAPlan::Init(berry::IViewSite::Pointer site, berry::IMemento::Pointer memento)
{
  QmitkAbstractView::Init(site, memento);

  //todo Check data completeness

	MITK_INFO << "THAPlan Init";
}

void THAPlan::CreateQtPartControl(QWidget *parent)
{
  //initialization
  //todo these parameter should from config file or pre steps
  m_OperationSide = othopedics::ESide::left;
  m_cupPlanMatrix = Eigen::Matrix4d::Identity();
  m_stemPlanMatrix = Eigen::Matrix4d::Identity();
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);

	//disable button when not init bodies
	m_Controls.pushButton_preop->setDisabled(true);
	m_Controls.pushButton_cupPlan->setDisabled(true);
	m_Controls.pushButton_stemPlan->setDisabled(true);
	m_Controls.pushButton_reduction->setDisabled(true);

	connect(m_Controls.pushButton_init, &QPushButton::clicked, this, &THAPlan::onPushButton_init_clicked);
  connect(m_Controls.pushButton_preop, &QPushButton::clicked, this, &THAPlan::onPushButton_preop_clicked);
  connect(m_Controls.pushButton_cupPlan, &QPushButton::clicked, this, &THAPlan::onPushButton_cupPlan_clicked);
  connect(m_Controls.pushButton_stemPlan, &QPushButton::clicked, this, &THAPlan::onPushButton_stemPlan_clicked);
  connect(m_Controls.pushButton_reduction, &QPushButton::clicked, this, &THAPlan::onPushButton_reduction_clicked);
	connect(m_Controls.pushButton_test1, &QPushButton::clicked, this, &THAPlan::OnPushButton_test1_clicked);
	connect(m_Controls.pushButton_test2, &QPushButton::clicked, this, &THAPlan::OnPushButton_test2_clicked);
}

