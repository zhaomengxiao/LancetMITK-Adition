/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef THAPlan_h
#define THAPlan_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "mitkPointSet.h"
#include "reduction.h"
#include "ui_THAPlanControls.h"

enum class EState
{
  NONINIT = 0,
  READY,
  Preoperative,
  Plan,
  Reduction
};

inline const char* to_string(EState e)
{
  switch (e)
  {
  case EState::NONINIT: return "NONINIT";
  case EState::READY: return "READY";
  case EState::Preoperative: return "Preoperative";
  case EState::Plan: return "Plan";
  case EState::Reduction: return "Reduction";
  default: return "unknown";
  }
}

/**
  \brief THAPlan

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class THAPlan : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:

  static const std::string VIEW_ID;

  void Init(berry::IViewSite::Pointer site, berry::IMemento::Pointer memento) override;

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  // /// \brief called by QmitkFunctionality when DataManager's selection has changed
  // virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
  //                                 const QList<mitk::DataNode::Pointer> &nodes) override;

  void onPushButton_init_clicked();

  //4 mode
  void onPushButton_preop_clicked();
  void onPushButton_cupPlan_clicked();
  void onPushButton_stemPlan_clicked();
  void onPushButton_reduction_clicked();


  //test
  void OnPushButton_test1_clicked();
  void OnPushButton_test2_clicked();
  Ui::THAPlanControls m_Controls;


private:
  bool initPelvis();
  bool initFemurR();
  bool initFemurL();
  bool initCup(std::string size = "Cup_56");
  bool initStem(std::string size = "Stem_6");
  bool initLiner(std::string size = "Liner_36");
  bool initHead(std::string size = "Head_36_M");

  //if there is no m_cupPlanMatrix,place cup to init pose
  bool restoreCupPose();
  bool restoreStemPose();

  bool getPoint(std::string name, mitk::PointSet::PointType* point, unsigned int index = 0) const;
  void HideAllNode(mitk::DataStorage* dataStorage);

  void listenCupGeoModify();
  void onCupGeoModified();
  void updateCupPlanMatrix();

  void listenStemGeoModify();
  void onStemGeoModified();
  void updateStemPlanMatrix();

  void updateUI_HipLengthAndOffset();
  void updateHipLengthAndOffset();

  void FlushAllTransform();
  void Show(Eigen::Vector3d point, std::string name);

private:
  othopedics::Pelvis::Pointer m_Pelvis = nullptr;
  othopedics::Femur::Pointer m_Femur_R = nullptr;
  othopedics::Femur::Pointer m_Femur_L = nullptr;
  othopedics::Cup::Pointer m_Cup = nullptr;
  othopedics::Stem::Pointer m_Stem = nullptr;
  othopedics::Body::Pointer m_Liner = nullptr;
  othopedics::Body::Pointer m_Head = nullptr;

  othopedics::Reduction::Pointer m_Reduction = nullptr;

  mitk::DataNode::Pointer m_dn_pelvis = nullptr;
  mitk::DataNode::Pointer m_dn_femurR = nullptr;
  mitk::DataNode::Pointer m_dn_femurL = nullptr;
  mitk::DataNode::Pointer m_dn_cup = nullptr;
  mitk::DataNode::Pointer m_dn_stem = nullptr;
  mitk::DataNode::Pointer m_dn_liner = nullptr;
  mitk::DataNode::Pointer m_dn_head = nullptr;
  //mitk::DataNode::Pointer m_dn_pelvis;

  //todo these parameter should from config file or pre steps
  othopedics::ESide m_OperationSide;

  Eigen::Matrix4d m_cupPlanMatrix;
  mitk::BaseGeometry::Pointer m_CupGeometry = nullptr;
  //! ITK tag for the observing of m_CupGeometry
  unsigned long m_CupListenTag{};

  Eigen::Matrix4d m_stemPlanMatrix;
  mitk::BaseGeometry::Pointer m_StemGeometry = nullptr;
  //! ITK tag for the observing of m_CupGeometry
  unsigned long m_StemListenTag{};

  EState m_state{EState::NONINIT};
};

#endif // THAPlan_h
