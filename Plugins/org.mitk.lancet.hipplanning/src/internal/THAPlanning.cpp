

// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "THAPlanning.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

#include <physioModelFactory.h>
#include <vtkPolyData.h>
#include <ep/include/vtk-9.1/vtkTransformFilter.h>

#include "mitkPointSet.h"
#include "mitkSurfaceToImageFilter.h"

#include "vtkPointData.h"
#include "vtkDataArray.h"

const std::string THAPlanning::VIEW_ID = "org.mitk.views.thaplanning";

void THAPlanning::SetFocus()
{
  // m_Controls.buttonPerformImageProcessing->setFocus();
}

void THAPlanning::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  connect(m_Controls.pushButton_initialConstruct, &QPushButton::clicked, this, &THAPlanning::InitialConstruct);

  // New THA model
  connect(m_Controls.pushButton_femoralVersion, &QPushButton::clicked, this, &THAPlanning::On_pushButton_femoralVersion_clicked);

  // PelvisObject
  connect(m_Controls.pushButton_initializePelvisObject, &QPushButton::clicked, this, &THAPlanning::On_pushButton_initializePelvisObject_clicked);
  connect(m_Controls.pushButton_movePelvisObject, &QPushButton::clicked, this, &THAPlanning::On_pushButton_movePelvisObject_clicked);

	// Right femurObject
  connect(m_Controls.pushButton_initializeRfemurObject, &QPushButton::clicked, this, &THAPlanning::On_pushButton_initializeRfemurObject_clicked);

	// Left femurObject
  connect(m_Controls.pushButton_initializeLfemurObject, &QPushButton::clicked, this, &THAPlanning::On_pushButton_initializeLfemurObject_clicked);

	// noPelvicTilt + canal alignment
  connect(m_Controls.pushButton_noTiltCanalReduction, &QPushButton::clicked, this, &THAPlanning::pushButton_noTiltCanalReduction_clicked);

  // noPelvicTilt + mechanic alignment
  connect(m_Controls.pushButton_noTiltMechanicReduction, &QPushButton::clicked, this, &THAPlanning::pushButton_noTiltMechanicReduction_clicked);

  // SupinePelvicTilt + canal alignment
  connect(m_Controls.pushButton_supineCanalReduction, &QPushButton::clicked, this, &THAPlanning::pushButton_supineCanalReduction_clicked);

  // SupinePelvicTilt + mechanic alignment
  connect(m_Controls.pushButton_supineMechanicReduction, &QPushButton::clicked, this, &THAPlanning::pushButton_supineMechanicReduction_clicked);

  // test stemObject and cupObject
  connect(m_Controls.pushButton_initCupObject, &QPushButton::clicked, this, &THAPlanning::pushButton_initCupObject_clicked);
  connect(m_Controls.pushButton_initStemObject, &QPushButton::clicked, this, &THAPlanning::pushButton_initStemObject_clicked);
  connect(m_Controls.pushButton_moveCupObject, &QPushButton::clicked, this, &THAPlanning::pushButton_moveCupObject_clicked);
  connect(m_Controls.pushButton_moveStemObject, &QPushButton::clicked, this, &THAPlanning::pushButton_moveStemObject_clicked);
  connect(m_Controls.pushButton_changeCupObject, &QPushButton::clicked, this, &THAPlanning::pushButton_changeCupObject_clicked);
  connect(m_Controls.pushButton_changeStemObject, &QPushButton::clicked, this, &THAPlanning::pushButton_changeStemObject_clicked);

  // pelvisCupCouple
  connect(m_Controls.pushButton_initPelvisCupCouple, &QPushButton::clicked, this, &THAPlanning::pushButton_initPelvisCupCouple_clicked);
  connect(m_Controls.pushButton_adjustCup, &QPushButton::clicked, this, &THAPlanning::pushButton_adjustCup_clicked);
  connect(m_Controls.pushButton_adjustCouple, &QPushButton::clicked, this, &THAPlanning::pushButton_adjustCouple_clicked);

  // femurStemCouple
  connect(m_Controls.pushButton_initFemurStemCouple, &QPushButton::clicked, this, &THAPlanning::pushButton_initFemurStemCouple_clicked);
  connect(m_Controls.pushButton_adjustStem, &QPushButton::clicked, this, &THAPlanning::pushButton_adjustStem_clicked);
  connect(m_Controls.pushButton_adjustFemurStemCouple, &QPushButton::clicked, this, &THAPlanning::pushButton_adjustFemurStemCouple_clicked);

  // enhancedReductionObject
  connect(m_Controls.pushButton_initEnhancedReduce, &QPushButton::clicked, this, &THAPlanning::pushButton_initEnhancedReduce_clicked);

  connect(m_Controls.pushButton_noTiltCanal_enhancedReduce, &QPushButton::clicked, this, &THAPlanning::pushButton_noTiltCanal_enhancedReduce_clicked);
  connect(m_Controls.pushButton_noTiltMech_enhancedReduce, &QPushButton::clicked, this, &THAPlanning::pushButton_noTiltMech_enhancedReduce_clicked);
  connect(m_Controls.pushButton_supineCanal_enhancedReduce, &QPushButton::clicked, this, &THAPlanning::pushButton_supineCanal_enhancedReduce_clicked);
  connect(m_Controls.pushButton_supineMech_enhancedReduce, &QPushButton::clicked, this, &THAPlanning::pushButton_supineMech_enhancedReduce_clicked);

  // ------------- DRR test ----------------
  connect(m_Controls.pushButton_testDRR, &QPushButton::clicked, this, &THAPlanning::pushButton_testDRR_clicked);
  connect(m_Controls.pushButton_testStencil, &QPushButton::clicked, this, &THAPlanning::pushButton_testStencil_clicked);



  // Demonstration
  connect(m_Controls.pushButton_demoInit, &QPushButton::clicked, this, &THAPlanning::pushButton_demoInit_clicked);
  connect(m_Controls.pushButton_demoReduce, &QPushButton::clicked, this, &THAPlanning::pushButton_demoReduce_clicked);
  connect(m_Controls.pushButton_demoConfirmImplant, &QPushButton::clicked, this, &THAPlanning::pushButton_demoConfirmImplant_clicked);
  connect(m_Controls.pushButton_demoMoveCup, &QPushButton::clicked, this, &THAPlanning::pushButton_demoMoveCup_clicked);
  connect(m_Controls.pushButton_demoMoveStem, &QPushButton::clicked, this, &THAPlanning::pushButton_demoMoveStem_clicked);
  connect(m_Controls.pushButton_demoDRR, &QPushButton::clicked, this, &THAPlanning::pushButton_demoDRR_clicked);

  connect(m_Controls.pushButton_changehead, &QPushButton::clicked, this, &THAPlanning::pushButton_changeHead_clicked);
  connect(m_Controls.pushButton_changestem, &QPushButton::clicked, this, &THAPlanning::pushButton_changeStem_clicked);
}

void THAPlanning::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
   
}

void THAPlanning::InitialConstruct()
{
	ConstructTHAmodel();
	m_Controls.lineEdit_hipLength_initMinusContra->setText(QString::number(m_hipLength_currentMinusContra));
	m_Controls.lineEdit_hipOffset_initMinusContra->setText(QString::number(m_hipOffset_currentMinusContra));

}


void THAPlanning::ConstructTHAmodel()
{
	// Select operation side
	if (m_Controls.radioButton_right->isChecked())
	{
		THA_MODEL.SetOprationSide(ESide::right);
	}
	else
	{
		THA_MODEL.SetOprationSide(ESide::left);
	}

	// Construct the model
	// -----------Left femur-----------
	// left proximal point of femur canal axis
	auto PFCA_left
		= GetPointWithGeometryMatrix(GetDataStorage()->GetNamedObject<mitk::PointSet>("femurAxis_left"), 0);

	// left distal point of femur canal axis
	auto DFCA_left
		= GetPointWithGeometryMatrix(GetDataStorage()->GetNamedObject<mitk::PointSet>("femurAxis_left"), 1);

	// left femur head center 
	auto FHC_left
		= GetPointWithGeometryMatrix(GetDataStorage()->GetNamedObject<mitk::PointSet>("femurCOR_left"), 0);

	// left lesser trochanter
	auto LT_left
	    = GetPointWithGeometryMatrix(GetDataStorage()->GetNamedObject<mitk::PointSet>("trochanter_left"), 0);

	THA_MODEL.BuildFemur(ESide::left, DFCA_left.GetDataPointer(), PFCA_left.GetDataPointer(), FHC_left.GetDataPointer(), LT_left.GetDataPointer());

	// ----------Right femur----------
	// right proximal point of femur canal axis
	auto PFCA_right
		= GetPointWithGeometryMatrix(GetDataStorage()->GetNamedObject<mitk::PointSet>("femurAxis_right"), 0);

	// right distal point of femur canal axis
	auto DFCA_right
		= GetPointWithGeometryMatrix(GetDataStorage()->GetNamedObject<mitk::PointSet>("femurAxis_right"), 1);

	// right femur head center 
	auto FHC_right
		= GetPointWithGeometryMatrix(GetDataStorage()->GetNamedObject<mitk::PointSet>("femurCOR_right"), 0);

	// right lesser trochanter
	auto LT_right
		= GetPointWithGeometryMatrix(GetDataStorage()->GetNamedObject<mitk::PointSet>("trochanter_right"), 0);

	THA_MODEL.BuildFemur(ESide::right, DFCA_right.GetDataPointer(), PFCA_right.GetDataPointer(), FHC_right.GetDataPointer(), LT_right.GetDataPointer());

	// -------------pelvis-----------------
	// right anterior superior illiac spine point
	auto RASI 
		= GetPointWithGeometryMatrix(GetDataStorage()->GetNamedObject<mitk::PointSet>("ASIS"), 0);

	// left anterior superior illiac spine point
	auto LASI
		= GetPointWithGeometryMatrix(GetDataStorage()->GetNamedObject<mitk::PointSet>("ASIS"), 1);

	// pubic tubercle
	auto PT
		= GetPointWithGeometryMatrix(GetDataStorage()->GetNamedObject<mitk::PointSet>("pelvis_midline"), 0);

	THA_MODEL.BuildPelvis(3, RASI.GetDataPointer(), LASI.GetDataPointer(), PT.GetDataPointer());

	THA_MODEL.CalHipLenAndOffsetBothSides();

	THA_MODEL.Femur_opSide()->GetResult(EResult::f_HipLengthDiff_preOp2Contral, m_hipLength_currentMinusContra);
	m_Controls.textBrowser->append("hipLength_currentMinusContra:" + QString::number(m_hipLength_currentMinusContra));

	THA_MODEL.Femur_opSide()->GetResult(EResult::f_OffsetDiff_preOp2Contral, m_hipOffset_currentMinusContra);
	m_Controls.textBrowser->append("hipOffset_currentMinusContra:" + QString::number(m_hipOffset_currentMinusContra));

	THA_MODEL.Femur_opSide()->GetResult(EResult::f_HipLength, m_hipLength_currentMinusContra);
	m_Controls.textBrowser->append("hipLength:" + QString::number(m_hipLength_currentMinusContra));

	THA_MODEL.Femur_opSide()->GetResult(EResult::f_Offset, m_hipLength_currentMinusContra);
	m_Controls.textBrowser->append("hipOffset:" + QString::number(m_hipLength_currentMinusContra));
	THA_MODEL.Delete();
}

mitk::Point3D THAPlanning::GetPointWithGeometryMatrix(const mitk::PointSet::Pointer inputPointSet, const int pointIndex)
{
	int size = inputPointSet->GetSize();
	mitk::Point3D outputPoint;
	outputPoint[0] = 0;
	outputPoint[1] = 0;
	outputPoint[2] = 0;

	if( (size-1) < pointIndex)
	{
		return outputPoint;
	}

	auto tmpPoint = inputPointSet->GetPoint(pointIndex);

	vtkNew<vtkMatrix4x4> initPointMatrix;
	initPointMatrix->Identity();
	initPointMatrix->SetElement(0, 3, tmpPoint[0]);
	initPointMatrix->SetElement(1, 3, tmpPoint[1]);
	initPointMatrix->SetElement(2, 3, tmpPoint[2]);

	auto geometryMatrix = inputPointSet->GetGeometry()->GetVtkMatrix();

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->Identity();
	tmpTransform->PostMultiply();
	tmpTransform->SetMatrix(initPointMatrix);
	tmpTransform->Concatenate(geometryMatrix);
	tmpTransform->Update();

	auto resultMatrix = tmpTransform->GetMatrix();

	outputPoint[0] = resultMatrix->GetElement(0, 3);
	outputPoint[1] = resultMatrix->GetElement(1, 3);
	outputPoint[2] = resultMatrix->GetElement(2, 3);
	// MITK_INFO << "The current point is: " << outputPoint[0] << "/" << outputPoint[1] << "/" << outputPoint[2];
	return outputPoint;

}

mitk::PointSet::Pointer THAPlanning::GetPointSetWithGeometryMatrix(const mitk::PointSet::Pointer inputPointSet)
{
	auto outputPointSet = mitk::PointSet::New();

	unsigned size = inputPointSet->GetSize();

	for(int i{0}; i < size; i++)
	{
		outputPointSet->InsertPoint(GetPointWithGeometryMatrix(inputPointSet, i));
	}

	return outputPointSet;
}

vtkMatrix4x4* THAPlanning::GenerateMatrix()
{
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();

	double stepSize = m_Controls.lineEdit_moveStepSize->text().toDouble();

	int direction = m_Controls.comboBox_direction->currentIndex();

	if(m_Controls.radioButton_translate->isChecked())
	{
		tmpTransform->Translate(stepSize*(direction == 0), 
			stepSize * (direction == 1),
			stepSize * (direction == 2));
		tmpTransform->Update();
	}

	if(m_Controls.radioButton_rotate->isChecked())
	{
		if(direction == 0)
		{
			tmpTransform->RotateX(stepSize);
		}

		if(direction == 1)
		{
			tmpTransform->RotateY(stepSize);
		}

		if(direction == 2)
		{
			tmpTransform->RotateZ(stepSize);
		}

		tmpTransform->Update();
	}
	
	return tmpTransform->GetMatrix();
}


// ---------------- New THA model ------------------

void THAPlanning::CollectTHAdata()
{
	m_vtkMatrix_femurCorrection_L_hiplength = vtkMatrix4x4::New();
	m_vtkMatrix_femurCorrection_R_hiplength = vtkMatrix4x4::New();
	m_vtkMatrix_femurCorrection_L_offset = vtkMatrix4x4::New();
	m_vtkMatrix_femurCorrection_R_offset = vtkMatrix4x4::New();
	m_vtkMatrix_pelvicCorrection_supine = vtkMatrix4x4::New();

	//------ Left femur --------
	m_pset_lesserTrochanter_L = GetDataStorage()->GetNamedObject<mitk::PointSet>("lesserTrochanter_L");
	m_pset_femurCOR_L = GetDataStorage()->GetNamedObject<mitk::PointSet>("femurCOR_L");
	 m_pset_femurCanal_L = GetDataStorage()->GetNamedObject<mitk::PointSet>("femurCanal_L");
	 m_pset_epicondyles_L = GetDataStorage()->GetNamedObject<mitk::PointSet>("epicondyles_L");
	 m_pset_neckCenter_L = GetDataStorage()->GetNamedObject<mitk::PointSet>("neckCenter_L");

	//------- Right femur --------
	 m_pset_lesserTrochanter_R = GetDataStorage()->GetNamedObject<mitk::PointSet>("lesserTrochanter_R");
	 m_pset_femurCOR_R = GetDataStorage()->GetNamedObject<mitk::PointSet>("femurCOR_R");
	 m_pset_femurCanal_R = GetDataStorage()->GetNamedObject<mitk::PointSet>("femurCanal_R");
	 m_pset_epicondyles_R = GetDataStorage()->GetNamedObject<mitk::PointSet>("epicondyles_R");
	 m_pset_neckCenter_R = GetDataStorage()->GetNamedObject<mitk::PointSet>("neckCenter_R");

	//------- Pelvis -----------
	 m_pset_ASIS = GetDataStorage()->GetNamedObject<mitk::PointSet>("ASIS");;
	 m_pset_midline = GetDataStorage()->GetNamedObject<mitk::PointSet>("midline");

}


double THAPlanning::CalculateNativeFemoralVersion()
{
	// The native femoral version is the angle between the neck axis and epicondylar axis when
	// these 2 axes are projected on a plane perpendicular to the femur canal
	// a positive value (anteversion) is returned when the neck axis angled anteriorly relative to the
	// epicondylar axis

	// Compose neck axis: neck center + femurCOR
	// Compose epicondylar axis, 0 is medial, 1 is lateral:
	// Compose femur canal axis, 0 is proximal, 1 is distal;
	Eigen::Vector3d neckAxis;
	Eigen::Vector3d epicondylarAxis;
	Eigen::Vector3d canalAxis;
	if (m_operationSide == 0)
	{
		neckAxis[0] = GetPointWithGeometryMatrix(m_pset_femurCOR_R, 0)[0] - GetPointWithGeometryMatrix(m_pset_neckCenter_R, 0)[0];
		neckAxis[1] = GetPointWithGeometryMatrix(m_pset_femurCOR_R, 0)[1] - GetPointWithGeometryMatrix(m_pset_neckCenter_R, 0)[1];
		neckAxis[2] = GetPointWithGeometryMatrix(m_pset_femurCOR_R, 0)[2] - GetPointWithGeometryMatrix(m_pset_neckCenter_R, 0)[2];

		epicondylarAxis[0] = GetPointWithGeometryMatrix(m_pset_epicondyles_R, 0)[0] - GetPointWithGeometryMatrix(m_pset_epicondyles_R, 1)[0];
		epicondylarAxis[1] = GetPointWithGeometryMatrix(m_pset_epicondyles_R, 0)[1] - GetPointWithGeometryMatrix(m_pset_epicondyles_R, 1)[1];
		epicondylarAxis[2] = GetPointWithGeometryMatrix(m_pset_epicondyles_R, 0)[2] - GetPointWithGeometryMatrix(m_pset_epicondyles_R, 1)[2];

		canalAxis[0] = GetPointWithGeometryMatrix(m_pset_femurCanal_R, 0)[0] - GetPointWithGeometryMatrix(m_pset_femurCanal_R, 1)[0];
		canalAxis[1] = GetPointWithGeometryMatrix(m_pset_femurCanal_R, 0)[1] - GetPointWithGeometryMatrix(m_pset_femurCanal_R, 1)[1];
		canalAxis[2] = GetPointWithGeometryMatrix(m_pset_femurCanal_R, 0)[2] - GetPointWithGeometryMatrix(m_pset_femurCanal_R, 1)[2];
	}
	else
	{
		neckAxis[0] = GetPointWithGeometryMatrix(m_pset_femurCOR_L, 0)[0] - GetPointWithGeometryMatrix(m_pset_neckCenter_L, 0)[0];
		neckAxis[1] = GetPointWithGeometryMatrix(m_pset_femurCOR_L, 0)[1] - GetPointWithGeometryMatrix(m_pset_neckCenter_L, 0)[1];
		neckAxis[2] = GetPointWithGeometryMatrix(m_pset_femurCOR_L, 0)[2] - GetPointWithGeometryMatrix(m_pset_neckCenter_L, 0)[2];

		epicondylarAxis[0] = GetPointWithGeometryMatrix(m_pset_epicondyles_L, 0)[0] - GetPointWithGeometryMatrix(m_pset_epicondyles_L, 1)[0];
		epicondylarAxis[1] = GetPointWithGeometryMatrix(m_pset_epicondyles_L, 0)[1] - GetPointWithGeometryMatrix(m_pset_epicondyles_L, 1)[1];
		epicondylarAxis[2] = GetPointWithGeometryMatrix(m_pset_epicondyles_L, 0)[2] - GetPointWithGeometryMatrix(m_pset_epicondyles_L, 1)[2];

		canalAxis[0] = GetPointWithGeometryMatrix(m_pset_femurCanal_L, 0)[0] - GetPointWithGeometryMatrix(m_pset_femurCanal_L, 1)[0];
		canalAxis[1] = GetPointWithGeometryMatrix(m_pset_femurCanal_L, 0)[1] - GetPointWithGeometryMatrix(m_pset_femurCanal_L, 1)[1];
		canalAxis[2] = GetPointWithGeometryMatrix(m_pset_femurCanal_L, 0)[2] - GetPointWithGeometryMatrix(m_pset_femurCanal_L, 1)[2];

	}
	canalAxis.normalize();

	// Project the neckAxis onto the canal axis
	Eigen::Vector3d neckAxis_ontoCanalAxis = neckAxis.dot(canalAxis) * canalAxis;

	// neckAxis projection onto the perpendicular plane 
	Eigen::Vector3d neckAxis_ontoPlane = neckAxis - neckAxis_ontoCanalAxis;

	// Project the epicondylarAxis onto the canal axis
	Eigen::Vector3d epicondylarAxis_ontoCanalAxis = epicondylarAxis.dot(canalAxis) * canalAxis;

	// epicondylarAxis projection onto the perpendicular plane
	Eigen::Vector3d epicondylarAxis_ontoPlane = epicondylarAxis - epicondylarAxis_ontoCanalAxis;

	double femoralVersion = (180 / 3.14159) * acos(epicondylarAxis_ontoPlane.dot(neckAxis_ontoPlane)
		/ (epicondylarAxis_ontoPlane.norm() * neckAxis_ontoPlane.norm()));

	// Determine anteversion or retroversion; if ante, assign femoralVersion as (+); if retro, assign as (-)
	double tmpValue = epicondylarAxis_ontoPlane.cross(neckAxis_ontoPlane).dot(canalAxis);
	if (m_operationSide == 1)
	{
		if (tmpValue < 0)
		{
			femoralVersion = -femoralVersion;
		}
	}
	else
	{
		if (tmpValue > 0)
		{
			femoralVersion = -femoralVersion;
		}
	}


	return femoralVersion;
}


void THAPlanning::CalculatePelvicCorrection_supine()
{
	// the rotation axis passes through the midline point
	// the direction of the rotation axis is the cross product of the ASIS axis and the medial-lateral direction

	// 


	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();

}



void THAPlanning::On_pushButton_femoralVersion_clicked()
{
	m_operationSide = 0;
	if(m_Controls.radioButton_left_new->isChecked())
	{
		m_operationSide = 1;
	}

	CollectTHAdata();
	double nativeFemoralVersion = CalculateNativeFemoralVersion();

	m_Controls.textBrowser->append("Native femoral anteversion is: "+ QString::number(nativeFemoralVersion));

}



void THAPlanning::On_pushButton_initializePelvisObject_clicked()
{
	m_pelvisObject = lancet::ThaPelvisObject::New();

	// auto pelvisImage = GetDataStorage()->GetNamedObject<mitk::Image>("pelvisImage");
	// auto pelvisSurface = GetDataStorage()->GetNamedObject<mitk::Surface>("pelvis");
	// auto asisPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("ASIS");
	// auto pelvisCORpset = GetDataStorage()->GetNamedObject<mitk::PointSet>("pelvisCOR");
	// auto midlinePset = GetDataStorage()->GetNamedObject<mitk::PointSet>("midline");
	//
	// m_pelvisObject->Setimage_pelvis(pelvisImage);
	// m_pelvisObject->Setsurface_pelvis(pelvisSurface);
	// m_pelvisObject->Setpset_ASIS(asisPset);
	// m_pelvisObject->Setpset_pelvisCOR(pelvisCORpset);
	// m_pelvisObject->Setpset_midline(midlinePset);

	m_pelvisObject->SetNode_image_pelvis(GetDataStorage()->GetNamedNode("pelvisImage"));
	m_pelvisObject->SetNode_surface_pelvis(GetDataStorage()->GetNamedNode("pelvis"));
	m_pelvisObject->SetNode_pset_ASIS(GetDataStorage()->GetNamedNode("ASIS"));
	m_pelvisObject->SetNode_pset_pelvisCOR(GetDataStorage()->GetNamedNode("pelvisCOR"));
	m_pelvisObject->SetNode_pset_midline(GetDataStorage()->GetNamedNode("midline"));
	m_pelvisObject->SetNode_pset_pubicTubercles(GetDataStorage()->GetNamedNode("pubicTubercles"));

	m_pelvisObject->SetNode_Pset_anteriorLandmark(GetDataStorage()->GetNamedNode("anteriorLandmark"));
	m_pelvisObject->SetNode_Pset_icpPoints(GetDataStorage()->GetNamedNode("icpPoints"));
	m_pelvisObject->SetNode_Pset_posteriorLandmark(GetDataStorage()->GetNamedNode("posteriorLandmark"));
	m_pelvisObject->SetNode_Pset_superiorLandmark(GetDataStorage()->GetNamedNode("superiorLandmark"));
	m_pelvisObject->SetNode_Pset_verificationPoints(GetDataStorage()->GetNamedNode("verificationPoints"));
	m_pelvisObject->SetNode_Surface_clippedPelvis(GetDataStorage()->GetNamedNode("clippedPelvis"));


	m_pelvisObject->AlignPelvicObjectWithWorldFrame();

	auto pelvisFrameSurface = m_pelvisObject->Getsurface_pelvisFrame();

	auto dataNode_pelvisFrame = mitk::DataNode::New();

	dataNode_pelvisFrame->SetData(pelvisFrameSurface);
	dataNode_pelvisFrame->SetName("pelvisFrame");

	GetDataStorage()->Add(dataNode_pelvisFrame, GetDataStorage()->GetNamedNode("pelvis"));

	m_pelvisObject->SetNode_surface_pelvisFrame(dataNode_pelvisFrame);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	m_Controls.textBrowser->append("A pelvicObject has been initialized");
	
	m_Controls.textBrowser->append("Supine pelvic tilt is: " + QString::number(m_pelvisObject->GetpelvicTilt_supine()));

	

}

void THAPlanning::On_pushButton_movePelvisObject_clicked()
{
	vtkNew<vtkMatrix4x4> tmpMatrix;
	tmpMatrix->Identity();

	tmpMatrix->SetElement(0, 3, 60);
	tmpMatrix->SetElement(1, 3, 70);
	tmpMatrix->SetElement(2, 3, 80);

	m_pelvisObject->SetGroupGeometry(tmpMatrix);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::On_pushButton_initializeRfemurObject_clicked()
{
	m_RfemurObject = lancet::ThaFemurObject::New();

	// auto femurImage = GetDataStorage()->GetNamedObject<mitk::Image>("femurImage_right");
	// auto femurSurface = GetDataStorage()->GetNamedObject<mitk::Surface>("femur_R");
	// auto lesserTrochanterPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("lesserTrochanter_R");
	// auto femurCORpSet = GetDataStorage()->GetNamedObject<mitk::PointSet>("femurCOR_R");
	// auto neckCenterPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("neckCenter_R");
	// auto femurCanalPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("femurCanal_R");
	// auto epicondylesPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("epicondyles_R");
	//
	// m_RfemurObject->Setimage_femur(femurImage);
	// m_RfemurObject->Setsurface_femur(femurSurface);
	// m_RfemurObject->Setpset_lesserTrochanter(lesserTrochanterPset);
	// m_RfemurObject->Setpset_femurCOR(femurCORpSet);
	// m_RfemurObject->Setpset_neckCenter(neckCenterPset);
	// m_RfemurObject->Setpset_femurCanal(femurCanalPset);
	// m_RfemurObject->Setpset_epicondyles(epicondylesPset);

	m_RfemurObject->SetNode_image_femur(GetDataStorage()->GetNamedNode("femurImage_right"));
	m_RfemurObject->SetNode_surface_femur(GetDataStorage()->GetNamedNode("femur_R"));
	m_RfemurObject->SetNode_pset_lesserTrochanter(GetDataStorage()->GetNamedNode("lesserTrochanter_R"));
	m_RfemurObject->SetNode_pset_femurCOR(GetDataStorage()->GetNamedNode("femurCOR_R"));
	m_RfemurObject->SetNode_pset_neckCenter(GetDataStorage()->GetNamedNode("neckCenter_R"));
	m_RfemurObject->SetNode_pset_femurCanal(GetDataStorage()->GetNamedNode("femurCanal_R"));
	m_RfemurObject->SetNode_pset_epicondyles(GetDataStorage()->GetNamedNode("epicondyles_R"));

	m_RfemurObject->SetfemurSide(0);
	// m_RfemurObject->SetisOperationSide(1);
	if(m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_RfemurObject->SetisOperationSide(1);
	}else
	{
		m_RfemurObject->SetisOperationSide(0);
	}

	m_RfemurObject->AlignFemurObjectWithWorldFrame();


	auto femurFrameSurface = m_RfemurObject->Getsurface_femurFrame();

	auto dataNode_femurFrame = mitk::DataNode::New();

	dataNode_femurFrame->SetData(femurFrameSurface);
	dataNode_femurFrame->SetName("femurFrame_R");

	GetDataStorage()->Add(dataNode_femurFrame, GetDataStorage()->GetNamedNode("femur_R"));

	m_RfemurObject->SetNode_surface_femurFrame(dataNode_femurFrame);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	m_Controls.textBrowser->append("A right femurObject has been initialized");
}

void THAPlanning::On_pushButton_initializeLfemurObject_clicked()
{
	m_LfemurObject = lancet::ThaFemurObject::New();

	// auto femurImage = GetDataStorage()->GetNamedObject<mitk::Image>("femurImage_left");
	// auto femurSurface = GetDataStorage()->GetNamedObject<mitk::Surface>("femur_L");
	// auto lesserTrochanterPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("lesserTrochanter_L");
	// auto femurCORpSet = GetDataStorage()->GetNamedObject<mitk::PointSet>("femurCOR_L");
	// auto neckCenterPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("neckCenter_L");
	// auto femurCanalPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("femurCanal_L");
	// auto epicondylesPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("epicondyles_L");
	//
	// m_LfemurObject->Setimage_femur(femurImage);
	// m_LfemurObject->Setsurface_femur(femurSurface);
	// m_LfemurObject->Setpset_lesserTrochanter(lesserTrochanterPset);
	// m_LfemurObject->Setpset_femurCOR(femurCORpSet);
	// m_LfemurObject->Setpset_neckCenter(neckCenterPset);
	// m_LfemurObject->Setpset_femurCanal(femurCanalPset);
	// m_LfemurObject->Setpset_epicondyles(epicondylesPset);
	m_LfemurObject->SetNode_image_femur(GetDataStorage()->GetNamedNode("femurImage_left"));
	m_LfemurObject->SetNode_surface_femur(GetDataStorage()->GetNamedNode("femur_L"));
	m_LfemurObject->SetNode_pset_lesserTrochanter(GetDataStorage()->GetNamedNode("lesserTrochanter_L"));
	m_LfemurObject->SetNode_pset_femurCOR(GetDataStorage()->GetNamedNode("femurCOR_L"));
	m_LfemurObject->SetNode_pset_neckCenter(GetDataStorage()->GetNamedNode("neckCenter_L"));
	m_LfemurObject->SetNode_pset_femurCanal(GetDataStorage()->GetNamedNode("femurCanal_L"));
	m_LfemurObject->SetNode_pset_epicondyles(GetDataStorage()->GetNamedNode("epicondyles_L"));
	m_LfemurObject->SetfemurSide(1);
	// m_LfemurObject->SetisOperationSide(0);

	if(m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_LfemurObject->SetisOperationSide(0);
	}else
	{
		m_LfemurObject->SetisOperationSide(1);
	}

	m_LfemurObject->AlignFemurObjectWithWorldFrame();

	auto femurFrameSurface = m_LfemurObject->Getsurface_femurFrame();

	auto dataNode_femurFrame = mitk::DataNode::New();

	dataNode_femurFrame->SetData(femurFrameSurface);
	dataNode_femurFrame->SetName("femurFrame_L");

	GetDataStorage()->Add(dataNode_femurFrame, GetDataStorage()->GetNamedNode("femur_L"));

	m_LfemurObject->SetNode_surface_femurFrame(dataNode_femurFrame);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	m_Controls.textBrowser->append("A left femurObject has been initialized");

}

void THAPlanning::pushButton_noTiltCanalReduction_clicked()
{
	m_ReductionObject = lancet::ThaReductionObject::New();

	m_ReductionObject->SetPelvisObject(m_pelvisObject);

	m_ReductionObject->SetFemurObject_R(m_RfemurObject);

	m_ReductionObject->SetFemurObject_L(m_LfemurObject);

	vtkNew<vtkMatrix4x4> rFemurMatrix;
	vtkNew<vtkMatrix4x4> lFemurMatrix;
	vtkNew<vtkMatrix4x4> pelvisMatrix;

	m_ReductionObject->GetOriginalNoTiltCanalMatrices(pelvisMatrix, rFemurMatrix, lFemurMatrix);

	// m_RfemurObject->SetGroupGeometry(rFemurMatrix);
	// m_LfemurObject->SetGroupGeometry(lFemurMatrix);
	// m_pelvisObject->SetGroupGeometry(pelvisMatrix);

	if (m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_LfemurObject->SetGroupGeometry(lFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(rFemurMatrix);
	}
	else
	{
		m_RfemurObject->SetGroupGeometry(rFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(lFemurMatrix);
	}

	m_PelvisCupCouple->SetCoupleGeometry(pelvisMatrix);
	
	m_Controls.textBrowser->append("right hip length:" + QString::number(m_ReductionObject->GetHipLength_supine_R()));
	m_Controls.textBrowser->append("left hip length:" + QString::number(m_ReductionObject->GetHipLength_supine_L()));
	m_Controls.textBrowser->append("right combined offset:" + QString::number(m_ReductionObject->GetCombinedOffset_supine_R()));
	m_Controls.textBrowser->append("left combined offset:" + QString::number(m_ReductionObject->GetCombinedOffset_supine_L()));

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_noTiltMechanicReduction_clicked()
{
	m_ReductionObject = lancet::ThaReductionObject::New();

	m_ReductionObject->SetPelvisObject(m_pelvisObject);

	m_ReductionObject->SetFemurObject_R(m_RfemurObject);

	m_ReductionObject->SetFemurObject_L(m_LfemurObject);

	vtkNew<vtkMatrix4x4> rFemurMatrix;
	vtkNew<vtkMatrix4x4> lFemurMatrix;
	vtkNew<vtkMatrix4x4> pelvisMatrix;

	m_ReductionObject->GetOriginalNoTiltMechanicMatrices(pelvisMatrix, rFemurMatrix, lFemurMatrix);

	// m_RfemurObject->SetGroupGeometry(rFemurMatrix);
	// m_LfemurObject->SetGroupGeometry(lFemurMatrix);
	// m_pelvisObject->SetGroupGeometry(pelvisMatrix);

	if (m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_LfemurObject->SetGroupGeometry(lFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(rFemurMatrix);
	}
	else
	{
		m_RfemurObject->SetGroupGeometry(rFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(lFemurMatrix);
	}

	m_PelvisCupCouple->SetCoupleGeometry(pelvisMatrix);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_supineCanalReduction_clicked()
{
	m_ReductionObject = lancet::ThaReductionObject::New();

	m_ReductionObject->SetPelvisObject(m_pelvisObject);

	m_ReductionObject->SetFemurObject_R(m_RfemurObject);

	m_ReductionObject->SetFemurObject_L(m_LfemurObject);

	vtkNew<vtkMatrix4x4> rFemurMatrix;
	vtkNew<vtkMatrix4x4> lFemurMatrix;
	vtkNew<vtkMatrix4x4> pelvisMatrix;

	m_ReductionObject->GetOriginalSupineTiltCanalMatrices(pelvisMatrix, rFemurMatrix, lFemurMatrix);

	// m_RfemurObject->SetGroupGeometry(rFemurMatrix);
	// m_LfemurObject->SetGroupGeometry(lFemurMatrix);
	// m_pelvisObject->SetGroupGeometry(pelvisMatrix);

	if (m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_LfemurObject->SetGroupGeometry(lFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(rFemurMatrix);
	}
	else
	{
		m_RfemurObject->SetGroupGeometry(rFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(lFemurMatrix);
	}

	m_PelvisCupCouple->SetCoupleGeometry(pelvisMatrix);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_supineMechanicReduction_clicked()
{
	m_ReductionObject = lancet::ThaReductionObject::New();

	m_ReductionObject->SetPelvisObject(m_pelvisObject);

	m_ReductionObject->SetFemurObject_R(m_RfemurObject);

	m_ReductionObject->SetFemurObject_L(m_LfemurObject);

	vtkNew<vtkMatrix4x4> rFemurMatrix;
	vtkNew<vtkMatrix4x4> lFemurMatrix;
	vtkNew<vtkMatrix4x4> pelvisMatrix;

	m_ReductionObject->GetOriginalSupineTiltMechanicMatrices(pelvisMatrix, rFemurMatrix, lFemurMatrix);

	// m_RfemurObject->SetGroupGeometry(rFemurMatrix);
	// m_LfemurObject->SetGroupGeometry(lFemurMatrix);
	// m_pelvisObject->SetGroupGeometry(pelvisMatrix);

	if (m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_LfemurObject->SetGroupGeometry(lFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(rFemurMatrix);
	}
	else
	{
		m_RfemurObject->SetGroupGeometry(rFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(lFemurMatrix);
	}

	m_PelvisCupCouple->SetCoupleGeometry(pelvisMatrix);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_initCupObject_clicked()
{
	m_CupObject = lancet::ThaCupObject::New();

	// auto cupSurface = GetDataStorage()->GetNamedObject<mitk::Surface>("cup");
	// auto linerSurface = GetDataStorage()->GetNamedObject<mitk::Surface>("liner");
	//
	// m_CupObject->SetCupSurface(cupSurface);
	// m_CupObject->SetLinerSurface(linerSurface);

	m_CupObject->SetNode_Surface_cup(GetDataStorage()->GetNamedNode("cup"));
	m_CupObject->SetNode_Surface_liner(GetDataStorage()->GetNamedNode("liner"));
	//m_CupObject->SetNode_Point_cupline(GetDataStorage()->GetNamedNode("cupLine"));

	if(m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_CupObject->SetOperationSide(0);
	}else
	{
		m_CupObject->SetOperationSide(1);
	}

	m_CupObject->AlignCupObjectWithWorldFrame();

	auto cupFrameSurface = m_CupObject->GetSurface_cupFrame();

	auto dataNode_cupFrame = mitk::DataNode::New();

	dataNode_cupFrame->SetData(cupFrameSurface);
	dataNode_cupFrame->SetName("cupFrame");

	GetDataStorage()->Add(dataNode_cupFrame);

	m_CupObject->SetNode_Surface_cupFrame(dataNode_cupFrame);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	m_Controls.textBrowser->append("A cupObject has been initialized");

}

void THAPlanning::pushButton_initStemObject_clicked()
{
	m_StemObject = lancet::ThaStemObject::New();

	// auto stemSurface = GetDataStorage()->GetNamedObject<mitk::Surface>("stem");
	// auto stemCOR = GetDataStorage()->GetNamedObject<mitk::PointSet>("stemCOR");
	// auto headSurface = GetDataStorage()->GetNamedObject<mitk::Surface>("head_28");
	//
	// m_StemObject->SetStemSurface(stemSurface);
	// m_StemObject->SetHeadSurface(headSurface);
	// m_StemObject->SetHeadCenter(stemCOR);

	m_StemObject->SetNode_Surface_stem(GetDataStorage()->GetNamedNode("stem"));
	m_StemObject->SetNode_Pset_headCenter(GetDataStorage()->GetNamedNode("headCOR_28"));
	m_StemObject->SetNode_Surface_head(GetDataStorage()->GetNamedNode("head_28"));
	m_StemObject->SetNode_Pset_StemCutPlane(GetDataStorage()->GetNamedNode("stemCutPlane"));
	//m_StemObject->SetNode_Pset_headAxis(GetDataStorage()->GetNamedNode("headAxis"));
	//m_StemObject->SetNode_Pset_stemLine(GetDataStorage()->GetNamedNode("stemLine"));
	//m_StemObject->SetNode_Pset_stemNormal(GetDataStorage()->GetNamedNode("stemNormal"));

	if(m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_StemObject->SetOperationSide(0);
	}else
	{
		m_StemObject->SetOperationSide(1);
	}
	
	m_StemObject->AlignStemObjectWithWorldFrame();

	//m_StemObject->GenerateStemCutPlaneSurface();

	auto NodeSurface = m_StemObject->GetNode_Surface_StemCutPlane();
	NodeSurface->SetName("stemCutPlaneSurface");
	NodeSurface->SetColor(0, 1, 0);
	GetDataStorage()->Add(NodeSurface);

	auto stemFrameSurface = m_StemObject->GetSurface_stemFrame();

	auto dataNode_stemFrame = mitk::DataNode::New();

	dataNode_stemFrame->SetData(stemFrameSurface);
	dataNode_stemFrame->SetName("stemFrame");

	GetDataStorage()->Add(dataNode_stemFrame);

	m_StemObject->SetNode_Surface_stemFrame(dataNode_stemFrame);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	m_Controls.textBrowser->append("A stemObject has been initialized");
}

void THAPlanning::pushButton_moveCupObject_clicked()
{
	auto tmpMatrix = m_CupObject->GetvtkMatrix_groupGeometry();

	GenerateMatrix();

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(tmpMatrix);
	tmpTransform->Concatenate(GenerateMatrix());
	tmpTransform->Update();

	m_CupObject->SetGroupGeometry(tmpTransform->GetMatrix());

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_moveStemObject_clicked()
{
	auto tmpMatrix = m_StemObject->GetvtkMatrix_groupGeometry();

	GenerateMatrix();

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(tmpMatrix);
	tmpTransform->Concatenate(GenerateMatrix());
	tmpTransform->Update();

	m_StemObject->SetGroupGeometry(tmpTransform->GetMatrix());

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_changeCupObject_clicked()
{
	auto newCupSurface = GetDataStorage()->GetNamedObject<mitk::Surface>("cup_50");
	m_CupObject->SetCupSurface(newCupSurface);
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_changeStemObject_clicked()
{
	auto newStemSurface = GetDataStorage()->GetNamedObject<mitk::Surface>("stem_5");
	m_StemObject->SetStemSurface(newStemSurface);
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_initPelvisCupCouple_clicked()
{
	m_PelvisCupCouple = lancet::ThaPelvisCupCouple::New();

	m_PelvisCupCouple->SetPelvisObject(m_pelvisObject);
	m_PelvisCupCouple->SetCupObject(m_CupObject);

	m_PelvisCupCouple->InitializePelvisFrameToCupFrameMatrix();

	m_Controls.lineEdit_cupVersion->setText(QString::number(m_PelvisCupCouple->GetCupVersion_supine()));
	m_Controls.lineEdit_cupInclination->setText(QString::number(m_PelvisCupCouple->GetCupInclination_supine()));

	m_Controls.lineEdit_cupCOR_si->setText(QString::number(m_PelvisCupCouple->GetCupCOR_SI_supine()));
	m_Controls.lineEdit_cupCOR_ml->setText(QString::number(m_PelvisCupCouple->GetCupCOR_ML_supine()));
	m_Controls.lineEdit_cupCOR_ap->setText(QString::number(m_PelvisCupCouple->GetCupCOR_AP_supine()));

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_adjustCup_clicked()
{
	vtkNew<vtkMatrix4x4> newMatrix;
	newMatrix->DeepCopy(GenerateMatrix());

	vtkNew<vtkTransform> pelvisFrameToCupFrameTransform_new;
	pelvisFrameToCupFrameTransform_new->PreMultiply();
	pelvisFrameToCupFrameTransform_new->SetMatrix(m_PelvisCupCouple->GetvtkMatrix_pelvisFrameToCupFrame());
	pelvisFrameToCupFrameTransform_new->Concatenate(newMatrix);

	auto pelvisFrameToCupFrameMatrix_new = pelvisFrameToCupFrameTransform_new->GetMatrix();

	m_PelvisCupCouple->SetPelvisFrameToCupFrameMatrix(pelvisFrameToCupFrameMatrix_new);

	m_Controls.lineEdit_cupVersion->setText(QString::number(m_PelvisCupCouple->GetCupVersion_supine()));
	m_Controls.lineEdit_cupInclination->setText(QString::number(m_PelvisCupCouple->GetCupInclination_supine()));

	m_Controls.lineEdit_cupCOR_si->setText(QString::number(m_PelvisCupCouple->GetCupCOR_SI_supine()));
	m_Controls.lineEdit_cupCOR_ml->setText(QString::number(m_PelvisCupCouple->GetCupCOR_ML_supine()));
	m_Controls.lineEdit_cupCOR_ap->setText(QString::number(m_PelvisCupCouple->GetCupCOR_AP_supine()));

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();


	m_EnhancedReductionObject->CalReductionMatrices();
	m_Controls.lineEdit_enhancedHiplen_R->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_R()));
	m_Controls.lineEdit_enhancedHiplen_L->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_L()));
	m_Controls.lineEdit_enhancedComOffset_R->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_R()));
	m_Controls.lineEdit_enhancedComOffset_L->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_L()));


}

void THAPlanning::pushButton_adjustCouple_clicked()
{
	vtkNew<vtkMatrix4x4> newMatrix;
	newMatrix->DeepCopy(GenerateMatrix());

	vtkNew<vtkTransform> worldToPelvisTransform_new;
	worldToPelvisTransform_new->PreMultiply();
	worldToPelvisTransform_new->SetMatrix(m_PelvisCupCouple->GetvtkMatrix_coupleGeometry());
	worldToPelvisTransform_new->Concatenate(newMatrix);
	worldToPelvisTransform_new->Update();

	m_PelvisCupCouple->SetCoupleGeometry(worldToPelvisTransform_new->GetMatrix());

	m_Controls.lineEdit_cupVersion->setText(QString::number(m_PelvisCupCouple->GetCupVersion_supine()));
	m_Controls.lineEdit_cupInclination->setText(QString::number(m_PelvisCupCouple->GetCupInclination_supine()));

	m_Controls.lineEdit_cupCOR_si->setText(QString::number(m_PelvisCupCouple->GetCupCOR_SI_supine()));
	m_Controls.lineEdit_cupCOR_ml->setText(QString::number(m_PelvisCupCouple->GetCupCOR_ML_supine()));
	m_Controls.lineEdit_cupCOR_ap->setText(QString::number(m_PelvisCupCouple->GetCupCOR_AP_supine()));

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

}


void THAPlanning::pushButton_initFemurStemCouple_clicked()
{
	m_FemurStemCouple = lancet::ThaFemurStemCouple::New();

	if(m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_FemurStemCouple->SetFemurObject(m_RfemurObject);
	}else
	{
		m_FemurStemCouple->SetFemurObject(m_LfemurObject);
	}

	m_FemurStemCouple->SetStemObject(m_StemObject);

	m_FemurStemCouple->InitializeFemurFrameToStemFrameMatrix();

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_adjustStem_clicked()
{
	vtkNew<vtkMatrix4x4> newMatrix;
	newMatrix->DeepCopy(GenerateMatrix());

	vtkNew<vtkTransform> tmpTransform; // femurFrameToStemFrame matrix
	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(newMatrix);
	tmpTransform->Concatenate(m_FemurStemCouple->GetvtkMatrix_femurFrameToStemFrame());

	m_FemurStemCouple->SetFemurFrameToStemFrameMatrix(tmpTransform->GetMatrix());
	
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	m_EnhancedReductionObject->CalReductionMatrices();
	m_Controls.lineEdit_enhancedHiplen_R->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_R()));
	m_Controls.lineEdit_enhancedHiplen_L->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_L()));
	m_Controls.lineEdit_enhancedComOffset_R->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_R()));
	m_Controls.lineEdit_enhancedComOffset_L->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_L()));


}

void THAPlanning::pushButton_adjustFemurStemCouple_clicked()
{
	vtkNew<vtkMatrix4x4> newMatrix;
	newMatrix->DeepCopy(GenerateMatrix());

	vtkNew<vtkTransform> tmpTransform; // worldFrameToFemurFrame Transform

	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(newMatrix);
	tmpTransform->Concatenate(m_FemurStemCouple->GetvtkMatrix_coupleGeometry());
	tmpTransform->Update();

	m_FemurStemCouple->SetCoupleGeometry(tmpTransform->GetMatrix());

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}


void THAPlanning::pushButton_initEnhancedReduce_clicked()
{
	m_EnhancedReductionObject = lancet::ThaEnhancedReductionObject::New();

	m_EnhancedReductionObject->SetPelvisCupCouple(m_PelvisCupCouple);

	m_EnhancedReductionObject->SetFemurStemCouple(m_FemurStemCouple);

	if (m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_EnhancedReductionObject->SetFemurObject(m_LfemurObject);
	}else
	{
		m_EnhancedReductionObject->SetFemurObject(m_RfemurObject);
	}

}



void THAPlanning::pushButton_noTiltCanal_enhancedReduce_clicked()
{
	vtkNew<vtkMatrix4x4> pelvisMatrix;
	vtkNew<vtkMatrix4x4> rightFemurMatrix;
	vtkNew<vtkMatrix4x4> leftFemurMatrix;

	m_EnhancedReductionObject->GetNoTiltCanalMatrices(pelvisMatrix, rightFemurMatrix, leftFemurMatrix);

	if (m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_LfemurObject->SetGroupGeometry(leftFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(rightFemurMatrix);
	}
	else
	{
		m_RfemurObject->SetGroupGeometry(rightFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(leftFemurMatrix);
	}

	m_PelvisCupCouple->SetCoupleGeometry(pelvisMatrix);

	// m_EnhancedReductionObject->CalReductionMatrices();
	m_Controls.lineEdit_enhancedHiplen_R->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_R()));
	m_Controls.lineEdit_enhancedHiplen_L->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_L()));
	m_Controls.lineEdit_enhancedComOffset_R->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_R()));
	m_Controls.lineEdit_enhancedComOffset_L->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_L()));


	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_noTiltMech_enhancedReduce_clicked()
{
	vtkNew<vtkMatrix4x4> pelvisMatrix;
	vtkNew<vtkMatrix4x4> rightFemurMatrix;
	vtkNew<vtkMatrix4x4> leftFemurMatrix;

	m_EnhancedReductionObject->GetNoTiltMechanicMatrices(pelvisMatrix, rightFemurMatrix, leftFemurMatrix);

	if (m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_LfemurObject->SetGroupGeometry(leftFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(rightFemurMatrix);
	}
	else
	{
		m_RfemurObject->SetGroupGeometry(rightFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(leftFemurMatrix);
	}

	m_PelvisCupCouple->SetCoupleGeometry(pelvisMatrix);

	// m_EnhancedReductionObject->CalReductionMatrices();
	m_Controls.lineEdit_enhancedHiplen_R->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_R()));
	m_Controls.lineEdit_enhancedHiplen_L->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_L()));
	m_Controls.lineEdit_enhancedComOffset_R->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_R()));
	m_Controls.lineEdit_enhancedComOffset_L->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_L()));


	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_supineCanal_enhancedReduce_clicked()
{
	vtkNew<vtkMatrix4x4> pelvisMatrix;
	vtkNew<vtkMatrix4x4> rightFemurMatrix;
	vtkNew<vtkMatrix4x4> leftFemurMatrix;

	m_EnhancedReductionObject->GetSupineTiltCanalMatrices(pelvisMatrix, rightFemurMatrix, leftFemurMatrix);

	if (m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_LfemurObject->SetGroupGeometry(leftFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(rightFemurMatrix);
	}
	else
	{
		m_RfemurObject->SetGroupGeometry(rightFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(leftFemurMatrix);
	}

	m_PelvisCupCouple->SetCoupleGeometry(pelvisMatrix);

	// m_EnhancedReductionObject->CalReductionMatrices();
	m_Controls.lineEdit_enhancedHiplen_R->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_R()));
	m_Controls.lineEdit_enhancedHiplen_L->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_L()));
	m_Controls.lineEdit_enhancedComOffset_R->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_R()));
	m_Controls.lineEdit_enhancedComOffset_L->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_L()));
	
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_supineMech_enhancedReduce_clicked()
{
	vtkNew<vtkMatrix4x4> pelvisMatrix;
	vtkNew<vtkMatrix4x4> rightFemurMatrix;
	vtkNew<vtkMatrix4x4> leftFemurMatrix;

	m_EnhancedReductionObject->GetSupineTiltMechanicMatrices(pelvisMatrix, rightFemurMatrix, leftFemurMatrix);

	if (m_Controls.radioButton_implantObject_R->isChecked())
	{
		m_LfemurObject->SetGroupGeometry(leftFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(rightFemurMatrix);
	}
	else
	{
		m_RfemurObject->SetGroupGeometry(rightFemurMatrix);
		m_FemurStemCouple->SetCoupleGeometry(leftFemurMatrix);
	}

	m_PelvisCupCouple->SetCoupleGeometry(pelvisMatrix);

	// m_EnhancedReductionObject->CalReductionMatrices();
	m_Controls.lineEdit_enhancedHiplen_R->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_R()));
	m_Controls.lineEdit_enhancedHiplen_L->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_L()));
	m_Controls.lineEdit_enhancedComOffset_R->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_R()));
	m_Controls.lineEdit_enhancedComOffset_L->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_L()));

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}


void THAPlanning::pushButton_demoInit_clicked()
{
	if (m_Controls.radioButton_demoRside->isChecked())
	{
		m_Controls.radioButton_implantObject_R->setChecked(true);
		m_Controls.radioButton_implantObject_L->setChecked(false);
	}
	else
	{
		m_Controls.radioButton_implantObject_R->setChecked(false);
		m_Controls.radioButton_implantObject_L->setChecked(true);
	}

	// Initialize m_pelvisObject
	On_pushButton_initializePelvisObject_clicked();
	m_Controls.lineEdit_demoPelvicTilt_supine->setText(QString::number(m_pelvisObject->GetpelvicTilt_supine()));

	// Initialize m_RfemurObject
	On_pushButton_initializeRfemurObject_clicked();

	// Show native femoralversion_R
	m_Controls.lineEdit_native_femoralVersion_R->setText(QString::number(m_RfemurObject->GetfemurVersion()));

	// Initialize m_LfemurObject
	On_pushButton_initializeLfemurObject_clicked();

	// Show native femoralversion_R
	m_Controls.lineEdit_native_femoralVersion_L->setText(QString::number(m_LfemurObject->GetfemurVersion()));

	// Initialize m_CupObject and m_StemObject
	pushButton_initCupObject_clicked();

	pushButton_initStemObject_clicked();
	
	// Initialize m_PelvisCupCouple
	pushButton_initPelvisCupCouple_clicked();

	// Initialize m_FemurStemCouple
	pushButton_initFemurStemCouple_clicked();

	
	// Initialize m_EnhancedReductionObject
	pushButton_initEnhancedReduce_clicked();
	pushButton_noTiltCanal_enhancedReduce_clicked();



	// Utilize m_ReductionObject
	pushButton_noTiltCanalReduction_clicked();



	// Update UI parameters
	m_Controls.lineEdit_demoHiplen_R->setText(QString::number(m_ReductionObject->GetHipLength_supine_R()));
	m_Controls.lineEdit_demoHiplen_L->setText(QString::number(m_ReductionObject->GetHipLength_supine_L()));
	m_Controls.lineEdit_demoOffset_R->setText(QString::number(m_ReductionObject->GetCombinedOffset_supine_R()));
	m_Controls.lineEdit_demoOffset_L->setText(QString::number(m_ReductionObject->GetCombinedOffset_supine_L()));

	// m_Controls.lineEdit_demoIntraHiplen_R->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_R()));
	// m_Controls.lineEdit_demoIntraHiplen_L->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_L()));
	// m_Controls.lineEdit_demoIntraOffset_R->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_R()));
	// m_Controls.lineEdit_demoIntraOffset_L->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_L()));

	// m_Controls.lineEdit_demoSupineVersion->setText(QString::number(m_PelvisCupCouple->GetCupVersion_supine()));
	// m_Controls.lineEdit_demoSupineInclin->setText(QString::number(m_PelvisCupCouple->GetCupInclination_supine()));
	//
	// m_Controls.lineEdit_demoNoTiltVersion->setText(QString::number(m_PelvisCupCouple->GetCupVersion_noTilt()));
	// m_Controls.lineEdit_demoNoTiltInclin->setText(QString::number(m_PelvisCupCouple->GetCupInclination_noTilt()));

	// m_Controls.lineEdit_demoCupSI_supine->setText(QString::number(m_PelvisCupCouple->GetCupCOR_SI_supine()));
	// m_Controls.lineEdit_demoCupML_supine->setText(QString::number(m_PelvisCupCouple->GetCupCOR_ML_supine()));
	// m_Controls.lineEdit_demoCupAP_supine->setText(QString::number(m_PelvisCupCouple->GetCupCOR_AP_supine()));

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	GetDataStorage()->GetNamedNode("pelvis")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("femur_R")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("femur_L")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("stemFrame")->SetVisibility(false);
	GetDataStorage()->GetNamedNode("cupFrame")->SetVisibility(false);

	GetDataStorage()->GetNamedNode("pelvisFrame")->SetVisibility(false);
	GetDataStorage()->GetNamedNode("femurFrame_R")->SetVisibility(false);
	GetDataStorage()->GetNamedNode("femurFrame_L")->SetVisibility(false);
	
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

}


void THAPlanning::pushButton_demoReduce_clicked()
{
	if(m_Controls.radioButton_demoPreop->isChecked() && 
		m_Controls.radioButton_demoCanalAlign->isChecked() &&
		m_Controls.radioButton_demoNoTilt->isChecked())
	{
		pushButton_noTiltCanalReduction_clicked();
	}

	if (m_Controls.radioButton_demoPreop->isChecked() &&
		m_Controls.radioButton_demoCanalAlign->isChecked() &&
		(m_Controls.radioButton_demoNoTilt->isChecked() == 0))
	{
		pushButton_supineCanalReduction_clicked();
	}

	if (m_Controls.radioButton_demoPreop->isChecked() &&
		(m_Controls.radioButton_demoCanalAlign->isChecked() == 0) &&
		m_Controls.radioButton_demoNoTilt->isChecked())
	{
		pushButton_noTiltMechanicReduction_clicked();
	}

	if (m_Controls.radioButton_demoPreop->isChecked() &&
		(m_Controls.radioButton_demoCanalAlign->isChecked() == 0) &&
		(m_Controls.radioButton_demoNoTilt->isChecked() == 0))
	{
		pushButton_supineMechanicReduction_clicked();
	}

	// ----------------------

	if ((m_Controls.radioButton_demoPreop->isChecked()== 0) &&
		m_Controls.radioButton_demoCanalAlign->isChecked() &&
		m_Controls.radioButton_demoNoTilt->isChecked())
	{
		pushButton_noTiltCanal_enhancedReduce_clicked();
	}

	if ((m_Controls.radioButton_demoPreop->isChecked() == 0) &&
		m_Controls.radioButton_demoCanalAlign->isChecked() &&
		(m_Controls.radioButton_demoNoTilt->isChecked() == 0))
	{
		pushButton_supineCanal_enhancedReduce_clicked();
	}

	if ((m_Controls.radioButton_demoPreop->isChecked() == 0) &&
		(m_Controls.radioButton_demoCanalAlign->isChecked() == 0) &&
		m_Controls.radioButton_demoNoTilt->isChecked())
	{
		pushButton_noTiltMech_enhancedReduce_clicked();
	}

	if ((m_Controls.radioButton_demoPreop->isChecked() == 0) &&
		(m_Controls.radioButton_demoCanalAlign->isChecked() == 0) &&
		(m_Controls.radioButton_demoNoTilt->isChecked() == 0))
	{
		pushButton_supineMech_enhancedReduce_clicked();
	}

	if(m_Controls.radioButton_demoPreop->isChecked())
	{
		ShowImplants(true);
		m_Controls.lineEdit_demoIntraHiplen_R->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_R()));
		m_Controls.lineEdit_demoIntraHiplen_L->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_L()));
		m_Controls.lineEdit_demoIntraOffset_R->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_R()));
		m_Controls.lineEdit_demoIntraOffset_L->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_L()));

		m_Controls.lineEdit_demoSupineVersion->setText(QString::number(m_PelvisCupCouple->GetCupVersion_supine()));
		m_Controls.lineEdit_demoSupineInclin->setText(QString::number(m_PelvisCupCouple->GetCupInclination_supine()));

		m_Controls.lineEdit_demoNoTiltVersion->setText(QString::number(m_PelvisCupCouple->GetCupVersion_noTilt()));
		m_Controls.lineEdit_demoNoTiltInclin->setText(QString::number(m_PelvisCupCouple->GetCupInclination_noTilt()));

		m_Controls.lineEdit_demoCupSI_supine->setText(QString::number(m_PelvisCupCouple->GetCupCOR_SI_supine()));
		m_Controls.lineEdit_demoCupML_supine->setText(QString::number(m_PelvisCupCouple->GetCupCOR_ML_supine()));
		m_Controls.lineEdit_demoCupAP_supine->setText(QString::number(m_PelvisCupCouple->GetCupCOR_AP_supine()));
	}else
	{
		ShowImplants(true);
	}

	// Update UI parameters
	m_Controls.lineEdit_demoHiplen_R->setText(QString::number(m_ReductionObject->GetHipLength_supine_R()));
	m_Controls.lineEdit_demoHiplen_L->setText(QString::number(m_ReductionObject->GetHipLength_supine_L()));
	m_Controls.lineEdit_demoOffset_R->setText(QString::number(m_ReductionObject->GetCombinedOffset_supine_R()));
	m_Controls.lineEdit_demoOffset_L->setText(QString::number(m_ReductionObject->GetCombinedOffset_supine_L()));

	m_Controls.lineEdit_demoIntraHiplen_R->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_R()));
	m_Controls.lineEdit_demoIntraHiplen_L->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_L()));
	m_Controls.lineEdit_demoIntraOffset_R->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_R()));
	m_Controls.lineEdit_demoIntraOffset_L->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_L()));

	m_Controls.lineEdit_demoSupineVersion->setText(QString::number(m_PelvisCupCouple->GetCupVersion_supine()));
	m_Controls.lineEdit_demoSupineInclin->setText(QString::number(m_PelvisCupCouple->GetCupInclination_supine()));
	
	m_Controls.lineEdit_demoNoTiltVersion->setText(QString::number(m_PelvisCupCouple->GetCupVersion_noTilt()));
	m_Controls.lineEdit_demoNoTiltInclin->setText(QString::number(m_PelvisCupCouple->GetCupInclination_noTilt()));

	m_Controls.lineEdit_demoCupSI_supine->setText(QString::number(m_PelvisCupCouple->GetCupCOR_SI_supine()));
	m_Controls.lineEdit_demoCupML_supine->setText(QString::number(m_PelvisCupCouple->GetCupCOR_ML_supine()));
	m_Controls.lineEdit_demoCupAP_supine->setText(QString::number(m_PelvisCupCouple->GetCupCOR_AP_supine()));
	
}

void THAPlanning::ShowImplants(bool showOrHide)
{
	if(showOrHide == true) // show the correct implant surfaces
	{
		// GetDataStorage()->GetNamedNode("stemFrame")->SetVisibility(true);
		// GetDataStorage()->GetNamedNode("cupFrame")->SetVisibility(true);
		// if(m_Controls.comboBox_demoCup->currentIndex() == 0)
		// {
		// 	GetDataStorage()->GetNamedNode("cup")->SetVisibility(true);
		// }
		//
		// if (m_Controls.comboBox_demoCup->currentIndex() == 1)
		// {
		// 	GetDataStorage()->GetNamedNode("cup_50")->SetVisibility(true);
		// }
		//
		// if (m_Controls.comboBox_demoLiner->currentIndex() == 0)
		// {
		// 	GetDataStorage()->GetNamedNode("liner")->SetVisibility(true);
		// }
		//
		// if (m_Controls.comboBox_demoLiner->currentIndex() == 1)
		// {
		// 	GetDataStorage()->GetNamedNode("liner_50")->SetVisibility(true);
		// }
		//
		// if (m_Controls.comboBox_demoStem->currentIndex() == 0)
		// {
		// 	GetDataStorage()->GetNamedNode("stem")->SetVisibility(true);
		// }
		//
		// if (m_Controls.comboBox_demoStem->currentIndex() == 1)
		// {
		// 	GetDataStorage()->GetNamedNode("stem_5")->SetVisibility(true);
		// }
		//
		// if (m_Controls.comboBox_demoHead->currentIndex() == 0)
		// {
		// 	GetDataStorage()->GetNamedNode("head_28")->SetVisibility(true);
		// }
		//
		// if (m_Controls.comboBox_demoHead->currentIndex() == 1)
		// {
		// 	GetDataStorage()->GetNamedNode("head_32")->SetVisibility(true);
		// }

		m_StemObject->GetNode_Pset_headCenter()->SetVisibility(1);
		m_StemObject->GetNode_Surface_head()->SetVisibility(1);
		m_StemObject->GetNode_Surface_stem()->SetVisibility(1);
		m_StemObject->GetNode_Surface_stemFrame()->SetVisibility(1);

		m_CupObject->GetNode_Surface_cup()->SetVisibility(1);
		m_CupObject->GetNode_Surface_cupFrame()->SetVisibility(1);
		m_CupObject->GetNode_Surface_liner()->SetVisibility(1);

	}else // hide the correct implant surfaces
	{
		// GetDataStorage()->GetNamedNode("stemFrame")->SetVisibility(false);
		// GetDataStorage()->GetNamedNode("cupFrame")->SetVisibility(false);
		// GetDataStorage()->GetNamedNode("cup")->SetVisibility(false);
		// GetDataStorage()->GetNamedNode("cup_50")->SetVisibility(false);
		// GetDataStorage()->GetNamedNode("liner")->SetVisibility(false);
		// GetDataStorage()->GetNamedNode("liner_50")->SetVisibility(false);
		// GetDataStorage()->GetNamedNode("stem")->SetVisibility(false);
		// GetDataStorage()->GetNamedNode("stem_5")->SetVisibility(false);
		// GetDataStorage()->GetNamedNode("head_28")->SetVisibility(false);
		// GetDataStorage()->GetNamedNode("head_32")->SetVisibility(false);

		m_StemObject->GetNode_Pset_headCenter()->SetVisibility(0);
		m_StemObject->GetNode_Surface_head()->SetVisibility(0);
		m_StemObject->GetNode_Surface_stem()->SetVisibility(0);
		m_StemObject->GetNode_Surface_stemFrame()->SetVisibility(0);

		m_CupObject->GetNode_Surface_cup()->SetVisibility(0);
		m_CupObject->GetNode_Surface_cupFrame()->SetVisibility(0);
		m_CupObject->GetNode_Surface_liner()->SetVisibility(0);
	}
}

void THAPlanning::pushButton_changeStem_clicked()
{
	mitk::Surface* newStem;
	mitk::PointSet* newStemCutPlane;
	mitk::DataNode* oldStemNode;
	mitk::DataNode* oldStemCutPlaneNode;
	mitk::DataNode* newStemNode;
	mitk::DataNode* newStemCutPlaneNode;
	if (isStem5)
	{
		newStem = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("stem")->GetData());
		newStemCutPlane = GetDataStorage()->GetNamedObject<mitk::PointSet>("stemCutPlane");
		newStemNode = GetDataStorage()->GetNamedNode("stem");
		newStemCutPlaneNode = GetDataStorage()->GetNamedNode("stemCutPlane");
		oldStemNode = GetDataStorage()->GetNamedNode("stem_5");
		oldStemCutPlaneNode = GetDataStorage()->GetNamedNode("stemCutPlane_5");
		isStem5 = false;
	}
	else
	{
		newStem = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("stem_5")->GetData());
		newStemCutPlane = GetDataStorage()->GetNamedObject<mitk::PointSet>("stemCutPlane_5");
		newStemNode = GetDataStorage()->GetNamedNode("stem_5");
		newStemCutPlaneNode = GetDataStorage()->GetNamedNode("stemCutPlane_5");
		oldStemNode = GetDataStorage()->GetNamedNode("stem");
		oldStemCutPlaneNode = GetDataStorage()->GetNamedNode("stemCutPlane");
		isStem5 = true;
	}
	
	//auto newStemCutPlane = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("stemCutPlane_5"));
	
	m_FemurStemCouple->ChangeStem(newStem,newStemCutPlane);


	//auto oldStemNode = GetDataStorage()->GetNamedNode("stem");
	oldStemNode->SetVisibility(false);
	oldStemCutPlaneNode->SetVisibility(false);
	//auto newStemNode = GetDataStorage()->GetNamedNode("stem_5");
	newStemNode->SetVisibility(true);
	newStemCutPlaneNode->SetVisibility(true);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	
}

void THAPlanning::pushButton_changeHead_clicked()
{
	//mitk::Surface* newHead;
	//mitk::PointSet* newHeadCenter;
	//mitk::DataNode* 
	auto newHead = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("head_32")->GetData());
	auto newHeadCenter = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("headCOR_32")->GetData());

	m_FemurStemCouple->ChangeHead(newHead, newHeadCenter);

	auto oldHeadNode = GetDataStorage()->GetNamedNode("head_28");
	oldHeadNode->SetVisibility(false);
	auto newHeadNode = GetDataStorage()->GetNamedNode("head_32");
	newHeadNode->SetVisibility(true);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void THAPlanning::pushButton_demoConfirmImplant_clicked()
{
	auto node_cup_0 = GetDataStorage()->GetNamedNode("cup");
	auto node_cup_1 = GetDataStorage()->GetNamedNode("cup_50");
	auto node_liner_0 = GetDataStorage()->GetNamedNode("liner");
	auto node_liner_1 = GetDataStorage()->GetNamedNode("liner_50");

	auto node_stem_0 = GetDataStorage()->GetNamedNode("stem");
	auto node_stem_1 = GetDataStorage()->GetNamedNode("stem_5");
	auto node_head_0 = GetDataStorage()->GetNamedNode("head_28");
	auto node_head_1 = GetDataStorage()->GetNamedNode("head_32");

	if (m_Controls.comboBox_demoCup->currentIndex() == 0)
	{
		// m_CupObject->SetCupSurface(GetDataStorage()->GetNamedObject<mitk::Surface>("cup"));
		m_CupObject->SetNode_Surface_cup(node_cup_0);
	}

	if (m_Controls.comboBox_demoCup->currentIndex() == 1)
	{
		//m_CupObject->SetCupSurface(GetDataStorage()->GetNamedObject<mitk::Surface>("cup_50"));
		m_CupObject->SetNode_Surface_cup(node_cup_1);
	}

	if (m_Controls.comboBox_demoLiner->currentIndex() == 0)
	{
		//m_CupObject->SetLinerSurface(GetDataStorage()->GetNamedObject<mitk::Surface>("liner"));
		m_CupObject->SetNode_Surface_liner(node_liner_0);
	}

	if (m_Controls.comboBox_demoLiner->currentIndex() == 1)
	{
		// m_CupObject->SetLinerSurface(GetDataStorage()->GetNamedObject<mitk::Surface>("liner_50"));
		m_CupObject->SetNode_Surface_liner(node_liner_1);
	}

	if (m_Controls.comboBox_demoStem->currentIndex() == 0)
	{
		// m_StemObject->SetStemSurface(GetDataStorage()->GetNamedObject<mitk::Surface>("stem"));
		m_StemObject->SetNode_Surface_stem(node_stem_0);
	}

	if (m_Controls.comboBox_demoStem->currentIndex() == 1)
	{
		// m_StemObject->SetStemSurface(GetDataStorage()->GetNamedObject<mitk::Surface>("stem_5"));
		m_StemObject->SetNode_Surface_stem(node_stem_1);
	}

	if (m_Controls.comboBox_demoHead->currentIndex() == 0)
	{
		//m_StemObject->SetHeadSurface(GetDataStorage()->GetNamedObject<mitk::Surface>("head_28"));
		m_StemObject->SetNode_Surface_head(node_head_0);
	}

	if (m_Controls.comboBox_demoHead->currentIndex() == 1)
	{
		//m_StemObject->SetHeadSurface(GetDataStorage()->GetNamedObject<mitk::Surface>("head_32"));
		m_StemObject->SetNode_Surface_head(node_head_1);
	}

	ShowImplants(true);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}


void THAPlanning::pushButton_demoMoveCup_clicked()
{
	if(m_Controls.radioButton_demoTranslate->isChecked())
	{
		m_Controls.radioButton_translate->setChecked(true);
	}else
	{
		m_Controls.radioButton_rotate->setChecked(true);
	}

	m_Controls.comboBox_direction->setCurrentIndex(m_Controls.comboBox_demoDirection->currentIndex());

	m_Controls.lineEdit_moveStepSize->setText(m_Controls.lineEdit_demoStepSize->text());

	pushButton_adjustCup_clicked();

	m_Controls.lineEdit_demoIntraHiplen_R->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_R()));
	m_Controls.lineEdit_demoIntraHiplen_L->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_L()));
	m_Controls.lineEdit_demoIntraOffset_R->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_R()));
	m_Controls.lineEdit_demoIntraOffset_L->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_L()));

	m_Controls.lineEdit_demoSupineVersion->setText(QString::number(m_PelvisCupCouple->GetCupVersion_supine()));
	m_Controls.lineEdit_demoSupineInclin->setText(QString::number(m_PelvisCupCouple->GetCupInclination_supine()));
	
	m_Controls.lineEdit_demoNoTiltVersion->setText(QString::number(m_PelvisCupCouple->GetCupVersion_noTilt()));
	m_Controls.lineEdit_demoNoTiltInclin->setText(QString::number(m_PelvisCupCouple->GetCupInclination_noTilt()));

	m_Controls.lineEdit_demoCupSI_supine->setText(QString::number(m_PelvisCupCouple->GetCupCOR_SI_supine()));
	m_Controls.lineEdit_demoCupML_supine->setText(QString::number(m_PelvisCupCouple->GetCupCOR_ML_supine()));
	m_Controls.lineEdit_demoCupAP_supine->setText(QString::number(m_PelvisCupCouple->GetCupCOR_AP_supine()));


}

void THAPlanning::pushButton_demoMoveStem_clicked()
{
	if (m_Controls.radioButton_demoTranslate->isChecked())
	{
		m_Controls.radioButton_translate->setChecked(true);
	}
	else
	{
		m_Controls.radioButton_rotate->setChecked(true);
	}

	m_Controls.comboBox_direction->setCurrentIndex(m_Controls.comboBox_demoDirection->currentIndex());

	m_Controls.lineEdit_moveStepSize->setText(m_Controls.lineEdit_demoStepSize->text());

	pushButton_adjustStem_clicked();

	m_Controls.lineEdit_demoIntraHiplen_R->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_R()));
	m_Controls.lineEdit_demoIntraHiplen_L->setText(QString::number(m_EnhancedReductionObject->GetHipLength_supine_L()));
	m_Controls.lineEdit_demoIntraOffset_R->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_R()));
	m_Controls.lineEdit_demoIntraOffset_L->setText(QString::number(m_EnhancedReductionObject->GetCombinedOffset_supine_L()));

}

void THAPlanning::pushButton_testDRR_clicked()
{
	auto inputImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("cup CT")->GetData());

	itk::SmartPointer<DrrFilter> drrFilter = DrrFilter::New();

	drrFilter->Setrx(270);
	drrFilter->Setty(-200);
	drrFilter->SetInput(inputImage);
	drrFilter->Update();

	auto newNode = mitk::DataNode::New();
	newNode->SetName("DRR");
	newNode->SetData(drrFilter->GetOutput());

	GetDataStorage()->Add(newNode);
}


void THAPlanning::pushButton_testStencil_clicked()
{
	// Generate a white image to apply the polydata stencil
	auto nodeSurface = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("cup")->GetData());

	vtkNew<vtkPolyData> surfacePolyData;
	surfacePolyData->DeepCopy(nodeSurface->GetVtkPolyData());

	vtkNew<vtkTransformFilter> tmpTransFilter;
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->SetMatrix(nodeSurface->GetGeometry()->GetVtkMatrix());
	tmpTransFilter->SetTransform(tmpTransform);
	tmpTransFilter->SetInputData(surfacePolyData);
	tmpTransFilter->Update();

	auto objectSurface = mitk::Surface::New();
	objectSurface->SetVtkPolyData(tmpTransFilter->GetPolyDataOutput());

	vtkNew<vtkImageData> whiteImage;
	double imageBounds[6]{ 0 };
	double imageSpacing[3]{ 0.2, 0.2, 0.2 };
	whiteImage->SetSpacing(imageSpacing);

	auto geometry = objectSurface->GetGeometry();
	auto surfaceBounds = geometry->GetBounds();
	for (int n = 0; n < 6; n++)
	{
		imageBounds[n] = surfaceBounds[n];
	}

	int dim[3];
	for (int i = 0; i < 3; i++)
	{
		dim[i] = static_cast<int>(ceil((imageBounds[i * 2 + 1] - imageBounds[i * 2]) / imageSpacing[i]));
	}
	whiteImage->SetDimensions(dim);
	whiteImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

	double origin[3];
	origin[0] = imageBounds[0] + imageSpacing[0] / 2;
	origin[1] = imageBounds[2] + imageSpacing[1] / 2;
	origin[2] = imageBounds[4] + imageSpacing[2] / 2;
	whiteImage->SetOrigin(origin);
	whiteImage->AllocateScalars(VTK_SHORT, 1);

	// fill the image with foreground voxels:
	short insideValue = 1024;
	// short outsideValue = 0;
	vtkIdType count = whiteImage->GetNumberOfPoints();
	for (vtkIdType i = 0; i < count; ++i)
	{
		whiteImage->GetPointData()->GetScalars()->SetTuple1(i, insideValue);
	}

	auto imageToCrop = mitk::Image::New();
	imageToCrop->Initialize(whiteImage);
	imageToCrop->SetVolume(whiteImage->GetScalarPointer());


	// Apply the stencil
	mitk::SurfaceToImageFilter::Pointer surfaceToImageFilter = mitk::SurfaceToImageFilter::New();
	surfaceToImageFilter->SetImage(imageToCrop);
	surfaceToImageFilter->SetInput(objectSurface);
	surfaceToImageFilter->SetReverseStencil(false);

	mitk::Image::Pointer convertedImage = mitk::Image::New();
	surfaceToImageFilter->Update();
	convertedImage = surfaceToImageFilter->GetOutput();

	auto newNode = mitk::DataNode::New();

	newNode->SetName("cup CT");

	// add new node
	newNode->SetData(convertedImage);
	GetDataStorage()->Add(newNode);

}


void THAPlanning::pushButton_demoDRR_clicked()
{
	// auto imageCombiner = lancet::Tha3DImageGenerator::New();
	//
	// imageCombiner->SetPelvisImage(GetDataStorage()->GetNamedObject<mitk::Image>("pelvisImage"));
	// imageCombiner->SetFemurImage_R(GetDataStorage()->GetNamedObject<mitk::Image>("femurImage_right"));
	// imageCombiner->SetFemurImage_L(GetDataStorage()->GetNamedObject<mitk::Image>("femurImage_left"));
	//
	// imageCombiner->SetCupSurface(GetDataStorage()->GetNamedObject<mitk::Surface>("cup"));
	// imageCombiner->SetLinerSurface(GetDataStorage()->GetNamedObject<mitk::Surface>("liner"));
	// imageCombiner->SetStemSurface(GetDataStorage()->GetNamedObject<mitk::Surface>("stem"));
	// imageCombiner->SetBallHeadSurface(GetDataStorage()->GetNamedObject<mitk::Surface>("head_28"));
	//
	// auto image = imageCombiner->Generate3Dimage();
	//
	// auto newNode = mitk::DataNode::New();
	// newNode->SetName("combined CT");
	// newNode->SetData(image);
	// GetDataStorage()->Add(newNode);

	if (GetDataStorage()->GetNamedNode("synthesized CT") != nullptr
		&& GetDataStorage()->GetNamedNode("DRR") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("synthesized CT"));
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("DRR"));
	};

	auto imageCombiner = lancet::Tha3DImageGenerator::New();
	imageCombiner->SetPelvisImage(m_pelvisObject->Getimage_pelvis());
	imageCombiner->SetFemurImage_R(m_RfemurObject->Getimage_femur());
	imageCombiner->SetFemurImage_L(m_LfemurObject->Getimage_femur());

	if(m_Controls.radioButton_demoIntrop->isChecked()) // need to include implants
	{
		imageCombiner->SetCupSurface(m_CupObject->GetSurface_cup());
		imageCombiner->SetLinerSurface(m_CupObject->GetSurface_liner());
		imageCombiner->SetStemSurface(m_StemObject->GetSurface_stem());
		imageCombiner->SetBallHeadSurface(m_StemObject->GetSurface_head());
	}

	auto image = imageCombiner->Generate3Dimage();
	
	auto newNode = mitk::DataNode::New();
	newNode->SetName("synthesized CT");
	newNode->SetData(image);
	GetDataStorage()->Add(newNode);
	GetDataStorage()->GetNamedNode("synthesized CT")->SetVisibility(false);

	// Generate DRR
	itk::SmartPointer<DrrFilter> drrFilter = DrrFilter::New();

	drrFilter->Setsid(1500);
	drrFilter->Setrx(270);
	drrFilter->Setty(-800);
	drrFilter->SetInput(image);
	drrFilter->Update();

	auto newNode_1 = mitk::DataNode::New();
	newNode_1->SetName("DRR");
	newNode_1->SetData(drrFilter->GetOutput());

	GetDataStorage()->Add(newNode_1);

}
