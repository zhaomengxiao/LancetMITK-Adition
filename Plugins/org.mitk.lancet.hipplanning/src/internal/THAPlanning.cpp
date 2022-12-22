

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

#include "mitkPointSet.h"


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
	 m_pset_midline = GetDataStorage()->GetNamedObject<mitk::PointSet>("midline");;
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



