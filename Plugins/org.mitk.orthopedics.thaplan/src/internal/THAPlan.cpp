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

const std::string THAPlan::VIEW_ID = "org.mitk.views.thaplan";

void THAPlan::SetFocus()
{
  m_Controls.pushButton_preop->setFocus();
}

void THAPlan::onPushButton_init_clicked()
{
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

	//disable button when not init bodies
	m_Controls.pushButton_preop->setDisabled(false);
	m_Controls.pushButton_cupPlan->setDisabled(false);
	m_Controls.pushButton_stemPlan->setDisabled(false);
	m_Controls.pushButton_reduction->setDisabled(false);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
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
	observer->SetCallbackFunction(this, &THAPlan::updateCupPlanMatrix);

	if (m_CupGeometry.IsNotNull())
	{
		m_CupGeometry->RemoveObserver(m_CupListenTag);
	}
	if (m_dn_cup!=nullptr)
	{
		m_CupGeometry = m_dn_cup->GetData()->GetGeometry();
		m_CupListenTag = m_CupGeometry->AddObserver(itk::ModifiedEvent(), observer);
	}
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
	observer->SetCallbackFunction(this, &THAPlan::updateStemPlanMatrix);

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

void THAPlan::updateStemPlanMatrix()
{
	if (m_Femur_L != nullptr && m_Femur_R && m_Stem != nullptr)
	{
		othopedics::Femur::Pointer femur;
		if (m_OperationSide == othopedics::ESide::left)
		{
			femur = m_Femur_L;
		}
		else
		{
			femur = m_Femur_R;
		}
		Eigen::Matrix4d Tworld2femurl = femur->m_T_world_local;
		Eigen::Matrix4d Tworld2steml = m_Stem->m_T_world_local;
		Eigen::Matrix4d Tfemurl2steml = Tworld2femurl.inverse() * Tworld2steml;
		m_stemPlanMatrix = Tfemurl2steml;
		//MITK_WARN << "updated m_stemPlanMatrix";
		//MITK_WARN << m_stemPlanMatrix;
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

void THAPlan::onPushButton_preop_clicked()
{
	m_Reduction->PreOperativeReduction_Canal();
	m_Reduction->CalPreopOffset();
	m_Reduction->PreOperativeReduction_Mechanical();
	m_Reduction->CalPreopHipLength();

	//hide all node,make it clear
	HideAllNode(GetDataStorage());

	//show pelvis and femur
	GetDataStorage()->GetNamedNode("pelvis")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("femur_R")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("femur_L")->SetVisibility(true);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(GetDataStorage());
	//show result
	QString opSide;
	QString state = "Preoperative";
	if (m_OperationSide == othopedics::ESide::left)
	{
		opSide = "(Left)";
	}
	else
	{
		opSide = "(Right)";
	}
	m_Controls.label_lengthOffsetHeader->setText(state + opSide);

	double Offset_preop, Offset_contra, OffsetDiff_preop_vs_contra;
	m_Reduction->GetResult(othopedics::EResult::r_Offset_preop, Offset_preop);
	m_Reduction->GetResult(othopedics::EResult::r_Offset_contra, Offset_contra);
	m_Reduction->GetResult(othopedics::EResult::r_OffsetDiff_preop_vs_contra, OffsetDiff_preop_vs_contra);

	double hipLength_preop, hipLength_contra, hipLengthDiff_preop_vs_contra;
	m_Reduction->GetResult(othopedics::EResult::r_HipLength_preop, hipLength_preop);
	m_Reduction->GetResult(othopedics::EResult::r_HipLength_contra, hipLength_contra);
	m_Reduction->GetResult(othopedics::EResult::r_HipLengthDiff_preop_vs_contra, hipLengthDiff_preop_vs_contra);

	std::stringstream ss;
	ss << "Offset_preop: " << Offset_preop <<std::endl;
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

void THAPlan::onPushButton_cupPlan_clicked()
{
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
			return;
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
	else
	{
		transform = m_cupPlanMatrix;
	}


	m_Cup->SetIndexToWorldTransform(transform);

	//place a gizmo to adjust cup pose
	mitk::Gizmo::AddGizmoToNode(m_dn_cup,GetDataStorage());

	//whenever cup place moved,m_cupPlanMatrix should be updated
	listenCupGeoModify();

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(GetDataStorage());
}

void THAPlan::onPushButton_stemPlan_clicked()
{
	m_Reduction->PreOperativeReduction_Canal();
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
	}
	else
	{
		m_dn_femurR->SetVisibility(true);
	}

	//todo save and restore cup pose
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
		if (!femur->GetGlobalLandMark(othopedics::ELandMarks::f_NeckCanalIntersectPoint, femurP)
			|| !m_Stem->GetGlobalLandMark(othopedics::ELandMarks::stem_NeckCanalIntersectPoint, StemP))
		{
			MITK_WARN << "placeStemL failed: missing landmark";
			return;
		}

		Eigen::Vector3d t = femurP - StemP;

		transform.setIdentity();
		transform.block<3, 1>(0, 3) = t;
		m_Stem->SetIndexToWorldTransform(transform);
		updateStemPlanMatrix();
	}
	else
	{
		transform =  m_stemPlanMatrix;
		m_Stem->SetIndexToWorldTransform(femur->m_T_world_local * transform);
	}

	//place a gizmo to adjust stem pose
	mitk::Gizmo::AddGizmoToNode(m_dn_stem, GetDataStorage());


	//whenever stem moved,m_cupPlanMatrix should be updated
	listenStemGeoModify();

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(GetDataStorage());
}

void THAPlan::onPushButton_reduction_clicked()
{
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


	m_Reduction->PlanReduction_Canal(m_cupPlanMatrix, m_stemPlanMatrix);
	m_Reduction->CalPlanedOffset();
	m_Reduction->PlanReduction_Mechanical(m_cupPlanMatrix, m_stemPlanMatrix);
	m_Reduction->CalPlanedHipLength();
	

	Eigen::Vector3d CupCOR, StemAssPoint;
	m_Cup->GetGlobalLandMark(othopedics::ELandMarks::cup_COR, CupCOR);
	m_Stem->GetGlobalLandMark(othopedics::ELandMarks::stem_HeadAssemblyPoint_M, StemAssPoint);
	Show(CupCOR, "cupCOR");
	Show(StemAssPoint, "StemAssPoint");

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(GetDataStorage());

	//show result
	QString opSide;
	QString state = "Planed";
	if (m_OperationSide == othopedics::ESide::left)
	{
		opSide = "(Left)";
	}
	else
	{
		opSide = "(Right)";
	}
	m_Controls.label_lengthOffsetHeader->setText(state + opSide);
	
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

void THAPlan::OnPushButton_test1_clicked()
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

// void THAPlan::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
//                                                 const QList<mitk::DataNode::Pointer> &nodes)
// {
//   // iterate all selected objects, adjust warning visibility
//   foreach (mitk::DataNode::Pointer node, nodes)
//   {
//     if (node.IsNotNull() && dynamic_cast<mitk::Image *>(node->GetData()))
//     {
//       m_Controls.labelWarning->setVisible(false);
//       m_Controls.buttonPerformImageProcessing->setEnabled(true);
//       return;
//     }
//   }
//
//   m_Controls.labelWarning->setVisible(true);
//   m_Controls.buttonPerformImageProcessing->setEnabled(false);
// }

