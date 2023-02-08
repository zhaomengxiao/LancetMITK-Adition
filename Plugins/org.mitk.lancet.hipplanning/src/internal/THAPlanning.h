/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef THAPlanning_h
#define THAPlanning_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_THAPlanningControls.h"

#include "mitkPointSet.h"

#include "lancetThaFemurObject.h"
#include "lancetThaPelvisObject.h"
#include "lancetThaReductionObject.h"
#include "lancetThaStemObject.h"
#include "lancetThaCupObject.h"
#include "lancetThaPelvisCupCouple.h"


/**
  \brief THAPlanning

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class THAPlanning : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;

  /// \brief Called when the user clicks the GUI button

  Ui::THAPlanningControls m_Controls;

	// Test the old THA_Model
  double m_hipLength_initMinusContra{ 0 };
  double m_hipOffset_initMinusContra{ 0 };

  double m_hipLength_currentMinusContra{ 0 };
  double m_hipOffset_currentMinusContra{ 0 };

  double m_hipLength_currentMinusInit{ 0 }; // hipLength_currentMinusContra - hipLength_initMinusContra
  double m_hipOffset_currentMinusInit{ 0 }; // hipOffset_currentMinusContra - hipOffset_initMinusContra

  void InitialConstruct();
  void CurrentConstruct();

  void ConstructTHAmodel(); // update m_hipLength_currentMinusContra & m_hipOffset_currentMinusContra


	//----------- New THA model with specification on --------------
	// https://gn1phhht53.feishu.cn/wiki/wikcnmt3TwoT0lS5r3NHFNH5U4f?table=tblk7ke86l8RyAoN&view=vew1Kd7wVv

	//------- Physiological landmarks --------
    //------ Left femur --------
  mitk::PointSet::Pointer m_pset_lesserTrochanter_L;
  mitk::PointSet::Pointer m_pset_femurCOR_L;
  mitk::PointSet::Pointer m_pset_femurCanal_L; // left femur anatomical axis
  mitk::PointSet::Pointer m_pset_epicondyles_L;
  mitk::PointSet::Pointer m_pset_neckCenter_L;

	//------- Right femur --------
  mitk::PointSet::Pointer m_pset_lesserTrochanter_R;
  mitk::PointSet::Pointer m_pset_femurCOR_R;
  mitk::PointSet::Pointer m_pset_femurCanal_R; // right femur anatomical axis 
  mitk::PointSet::Pointer m_pset_epicondyles_R;
  mitk::PointSet::Pointer m_pset_neckCenter_R;

	//------- Pelvis -----------
  mitk::PointSet::Pointer m_pset_ASIS;
  mitk::PointSet::Pointer m_pset_midline;

	// ------- Physiological value calculation --------
 
    // Operation side
  int m_operationSide{ 0 }; // right femur: 0   left femur: 1

	//coarse Data collection and initialization
  void CollectTHAdata(); 

    // Femoral version
  double CalculateNativeFemoralVersion();
  void On_pushButton_femoralVersion_clicked();

    // ---------- Virtual correction ---------------

  vtkSmartPointer<vtkMatrix4x4> m_vtkMatrix_pelvicCorrection_supine; // conversion from initial identity matrix to the corrected pose 
  vtkSmartPointer<vtkMatrix4x4> m_vtkMatrix_femurCorrection_R_hiplength; // conversion from initial identity matrix to the corrected pose 
  vtkSmartPointer<vtkMatrix4x4> m_vtkMatrix_femurCorrection_L_hiplength; // conversion from initial identity matrix to the corrected pose 
  vtkSmartPointer<vtkMatrix4x4> m_vtkMatrix_femurCorrection_R_offset; // conversion from initial identity matrix to the corrected pose 
  vtkSmartPointer<vtkMatrix4x4> m_vtkMatrix_femurCorrection_L_offset; // conversion from initial identity matrix to the corrected pose 

  void CalculatePelvicCorrection_supine(); // update m_vtkMatrix_pelvicCorrection_supine
  void CalculateRightFemurCorrection_hipLength(); // update m_vtkMatrix_femurCorrection_R_hiplength, call CalculatePelvicCorrection_supine() internally
  void CalculateLeftFemurCorrection_hipLength(); // update m_vtkMatrix_femurCorrection_L_hiplength, call CalculatePelvicCorrection_supine() internally
  void CalculateRightFemurCorrection_offset(); // update m_vtkMatrix_femurCorrection_R_offset, call CalculatePelvicCorrection_supine() internally
  void CalculateLeftFemurCorrection_offset(); // update m_vtkMatrix_femurCorrection_L_offset, call CalculatePelvicCorrection_supine() internally

  // ---------------- Test pelvisObject -----------------
  void On_pushButton_initializePelvisObject_clicked();
  void On_pushButton_movePelvisObject_clicked();
  lancet::ThaPelvisObject::Pointer m_pelvisObject;

  // ---------------- Test femurObject -----------------
  void On_pushButton_initializeLfemurObject_clicked();
  void On_pushButton_initializeRfemurObject_clicked();
  lancet::ThaFemurObject::Pointer m_LfemurObject;
  lancet::ThaFemurObject::Pointer m_RfemurObject;

	// --------------- Test reductionObject ------------
  void pushButton_noTiltCanalReduction_clicked();
  void pushButton_noTiltMechanicReduction_clicked();
  void pushButton_supineCanalReduction_clicked();
  void pushButton_supineMechanicReduction_clicked();
  lancet::ThaReductionObject::Pointer m_ReductionObject;

  // --------- Test cupObject and stemObject ------------
  lancet::ThaCupObject::Pointer m_CupObject;
  lancet::ThaStemObject::Pointer m_StemObject;
  void pushButton_initCupObject_clicked();
  void pushButton_initStemObject_clicked();
  void pushButton_moveCupObject_clicked();
  void pushButton_moveStemObject_clicked();
  void pushButton_changeCupObject_clicked();
  void pushButton_changeStemObject_clicked();

  // --------- Test pelvisCupCouple ---------
  void pushButton_initPelvisCupCouple_clicked();
  void pushButton_adjustCup_clicked();
  void pushButton_adjustCouple_clicked();
  lancet::ThaPelvisCupCouple::Pointer m_PelvisCupCouple;

	//---------- Tool functions --------------

	// retrieve a 4x4 vtkMatrix from the UI
  vtkMatrix4x4* GenerateMatrix();
	
	// Append the geometry matrix to the chosen point
  mitk::Point3D GetPointWithGeometryMatrix(const mitk::PointSet::Pointer inputPointSet, const int pointIndex);

    // Append the geometry matrix to the pointSet
  mitk::PointSet::Pointer GetPointSetWithGeometryMatrix(const mitk::PointSet::Pointer inputPointSet);

  

};

#endif // THAPlanning_h
