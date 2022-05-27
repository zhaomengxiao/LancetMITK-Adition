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
#include "SpineCTRegistration.h"

// Qt
#include "leastsquaresfit.h"
#include <QMessageBox>
#include <QPushButton>

#include "mitkImageToSurfaceFilter.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include "mitkPointSet.h"
#include "mitkSurface.h"
#include "surfaceregistraion.h"
#include "vtkConnectivityFilter.h"
#include <QPushButton>
#include <mitkImage.h>
#include <vtkSphereSource.h>

const std::string SpineCTRegistration::VIEW_ID = "org.mitk.views.spinectregistration";


void SpineCTRegistration::SetFocus()
{
  // m_Controls.buttonPerformImageProcessing->setFocus();
}
void SpineCTRegistration::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);

  // Initialize mitkNodeSelectionWidget
  InitNodeSelector(m_Controls.mitkNodeSelectWidget_CTimage);
  InitNodeSelector(m_Controls.mitkNodeSelectWidget_MovingSurface);
  InitNodeSelector(m_Controls.mitkNodeSelectWidget_LandmarkSrcPointset);
  InitNodeSelector(m_Controls.mitkNodeSelectWidget_LandmarkTargetPointset);
  InitNodeSelector(m_Controls.mitkNodeSelectWidget_IcpSrcSurface);
  InitNodeSelector(m_Controls.mitkNodeSelectWidget_IcpTargetPointset);

  // Connect the signals and slots
  connect(m_Controls.mitkNodeSelectWidget_CTimage,
          &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged,
          this,
          &SpineCTRegistration::ChangeCtImage);

  connect(m_Controls.mitkNodeSelectWidget_MovingSurface,
          &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged,
          this,
          &SpineCTRegistration::ChangeMovingSurface);

  connect(m_Controls.mitkNodeSelectWidget_LandmarkSrcPointset,
          &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged,
          this,
          &SpineCTRegistration::ChangeLandmarkSrcPointset);

  connect(m_Controls.mitkNodeSelectWidget_LandmarkTargetPointset,
          &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged,
          this,
          &SpineCTRegistration::ChangeLandmarkTargetPointset);

  connect(m_Controls.mitkNodeSelectWidget_IcpSrcSurface,
          &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged,
          this,
          &SpineCTRegistration::ChangeIcpSrcSurface);

  connect(m_Controls.mitkNodeSelectWidget_IcpTargetPointset,
          &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged,
          this,
          &SpineCTRegistration::ChangeIcpTargetPointset);

  connect(m_Controls.pushButton_ResetImageOrigin, &QPushButton::clicked, this, &SpineCTRegistration::ResetImage);

  connect(m_Controls.pushButton_Landmark, &QPushButton::clicked, this, &SpineCTRegistration::LandmarkRegistration);

  connect(m_Controls.pushButton_Icp, &QPushButton::clicked, this, &SpineCTRegistration::IcpRegistration);

  connect(m_Controls.pushButton_TransformSurface, &QPushButton::clicked, this, &SpineCTRegistration::TransformSurface);

  connect(m_Controls.pushButton_TransformImage, &QPushButton::clicked, this, &SpineCTRegistration::TransformImage);

  connect(
    m_Controls.pushButton_ExtractSteelballs, &QPushButton::clicked, this, &SpineCTRegistration::GetSteelballCenters);

  connect(
    m_Controls.pushButton_ReconstructBones, &QPushButton::clicked, this, &SpineCTRegistration::ReconstructSpineSurface);

  connect(m_Controls.pushButton_checkNDI, &QPushButton::clicked, this, &SpineCTRegistration::CheckToolValidity);

  connect(m_Controls.pushButton_OptimizeTool, &QPushButton::clicked, this, &SpineCTRegistration::OptimizeTool);
}

void SpineCTRegistration::InitNodeSelector(QmitkSingleNodeSelectionWidget *widget)
{
  widget->SetDataStorage(GetDataStorage());
  widget->SetNodePredicate(mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(
    mitk::NodePredicateProperty::New("helper object"), mitk::NodePredicateProperty::New("hidden object"))));
  widget->SetSelectionIsOptional(true);
  widget->SetAutoSelectNewNodes(true);
  widget->SetEmptyInfo(QString("Please select a node"));
  widget->SetPopUpTitel(QString("Select node"));
}

inline void SpineCTRegistration::ChangeCtImage(QmitkSingleNodeSelectionWidget::NodeList /*nodes*/)
{
  m_CtImageDataNode = m_Controls.mitkNodeSelectWidget_CTimage->GetSelectedNode();
}

inline void SpineCTRegistration::ChangeMovingSurface(QmitkSingleNodeSelectionWidget::NodeList /*nodes*/)
{
  m_MovingSurfaceDataNode = m_Controls.mitkNodeSelectWidget_MovingSurface->GetSelectedNode();
}

inline void SpineCTRegistration::ChangeLandmarkSrcPointset(QmitkSingleNodeSelectionWidget::NodeList /*nodes*/)
{
  m_LandmarkSrcPointsetDataNode = m_Controls.mitkNodeSelectWidget_LandmarkSrcPointset->GetSelectedNode();
}

inline void SpineCTRegistration::ChangeLandmarkTargetPointset(QmitkSingleNodeSelectionWidget::NodeList /*nodes*/)
{
  m_LandmarkTargetPointsetDataNode = m_Controls.mitkNodeSelectWidget_LandmarkTargetPointset->GetSelectedNode();
}

inline void SpineCTRegistration::ChangeIcpSrcSurface(QmitkSingleNodeSelectionWidget::NodeList /*nodes*/)
{
  m_IcpSrcSurfaceDataNode = m_Controls.mitkNodeSelectWidget_IcpSrcSurface->GetSelectedNode();
}

inline void SpineCTRegistration::ChangeIcpTargetPointset(QmitkSingleNodeSelectionWidget::NodeList /*nodes*/)
{
  m_IcpTargetPointsetDataNode = m_Controls.mitkNodeSelectWidget_IcpTargetPointset->GetSelectedNode();
}

//---------------------------Above: QT UI initialization------------------------------//