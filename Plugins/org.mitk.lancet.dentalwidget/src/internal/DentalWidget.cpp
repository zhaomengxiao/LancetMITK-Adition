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
#include "DentalWidget.h"


// mitk image
#include <mitkImage.h>

#include "mitkImageToSurfaceFilter.h"
#include "mitkNodePredicateAnd.h"
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include "mitkPointSet.h"
#include "mitkSurface.h"

const std::string DentalWidget::VIEW_ID = "org.mitk.views.dentalwidget";

void DentalWidget::SetFocus()
{
  //m_Controls.buttonPerformImageProcessing->setFocus();
}

void DentalWidget::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/, const QList<mitk::DataNode::Pointer>& nodes)
{
	std::string nodeName;
	// iterate all selected objects, adjust warning visibility
	foreach(mitk::DataNode::Pointer node, nodes)
	{
		if (node.IsNull())
		{
			return;
		}

		node->GetName(nodeName);

		if (node != nullptr)
		{
			m_currentSelectedNode = node;
			m_baseDataToMove = node->GetData();
		}

	}

}

void DentalWidget::InitImageSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::Image>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select an image"));
	widget->SetPopUpTitel(QString("Select image"));
}

void DentalWidget::InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::PointSet>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a point set"));
	widget->SetPopUpTitel(QString("Select point set"));
}

void DentalWidget::InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::Surface>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a surface"));
	widget->SetPopUpTitel(QString("Select surface"));
}

void DentalWidget::InitNodeSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(
		mitk::NodePredicateProperty::New("helper object"), mitk::NodePredicateProperty::New("hidden object"))));
	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a node"));
	widget->SetPopUpTitel(QString("Select node"));
}

void DentalWidget::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);

  InitImageSelector(m_Controls.mitkNodeSelectWidget_intraopCt);
  InitImageSelector(m_Controls.mitkNodeSelectWidget_preopCT_reg);
  InitImageSelector(m_Controls.mitkNodeSelectWidget_intraopCT_reg);
  InitNodeSelector(m_Controls.mitkNodeSelectWidget_appendMatrix);

  m_NodetreeModel = new QmitkDataStorageTreeModel(this->GetDataStorage());

  //connect(m_Controls.buttonPerformImageProcessing, &QPushButton::clicked, this, &DentalWidget::DoImageProcessing);
  connect(m_Controls.checkBox_useSmooth, &QCheckBox::stateChanged, this, &DentalWidget::CheckUseSmoothing);
  connect(m_Controls.pushButton_reconstructSurface, &QPushButton::clicked, this, &DentalWidget::ReconstructSurface);

  connect(m_Controls.pushButton_xp, &QPushButton::clicked, this, &DentalWidget::TranslatePlusX);
  connect(m_Controls.pushButton_yp, &QPushButton::clicked, this, &DentalWidget::TranslatePlusY);
  connect(m_Controls.pushButton_zp, &QPushButton::clicked, this, &DentalWidget::TranslatePlusZ);
  connect(m_Controls.pushButton_xm, &QPushButton::clicked, this, &DentalWidget::TranslateMinusX);
  connect(m_Controls.pushButton_ym, &QPushButton::clicked, this, &DentalWidget::TranslateMinusY);
  connect(m_Controls.pushButton_zm, &QPushButton::clicked, this, &DentalWidget::TranslateMinusZ);
  connect(m_Controls.pushButton_rxp, &QPushButton::clicked, this, &DentalWidget::RotatePlusX);
  connect(m_Controls.pushButton_ryp, &QPushButton::clicked, this, &DentalWidget::RotatePlusY);
  connect(m_Controls.pushButton_rzp, &QPushButton::clicked, this, &DentalWidget::RotatePlusZ);
  connect(m_Controls.pushButton_rxm, &QPushButton::clicked, this, &DentalWidget::RotateMinusX);
  connect(m_Controls.pushButton_rym, &QPushButton::clicked, this, &DentalWidget::RotateMinusY);
  connect(m_Controls.pushButton_rzm, &QPushButton::clicked, this, &DentalWidget::RotateMinusZ);
  connect(m_Controls.pushButton_registerIos_, &QPushButton::clicked, this, &DentalWidget::RegisterIos_);
  connect(m_Controls.pushButton_fixIcp_, &QPushButton::clicked, this, &DentalWidget::FineTuneRegister_);
  connect(m_Controls.pushButton_regReset_, &QPushButton::clicked, this, &DentalWidget::ResetRegistration_);
  connect(m_Controls.pushButton_extractBallCenters, &QPushButton::clicked, this, &DentalWidget::GetSteelballCenters_modelCBCT);
  connect(m_Controls.pushButton_checkPrecision, &QPushButton::clicked, this, &DentalWidget::CheckPrecision);
  connect(m_Controls.pushButton_modeltoCBCT, &QPushButton::clicked, this, &DentalWidget::RegisterModeltoCBCT);
  connect(m_Controls.pushButton_regResetModel, &QPushButton::clicked, this, &DentalWidget::ResetModelRegistration);
  connect(m_Controls.pushButton_preprocessCt, &QPushButton::clicked, this, &DentalWidget::PrepareCbctImages);
  connect(m_Controls.pushButton_retrieveMatrix, &QPushButton::clicked, this, &DentalWidget::RetrieveRegistrationMatrix);
  connect(m_Controls.pushButton_appendOffsetMatrix, &QPushButton::clicked, this, &DentalWidget::AppendMatrix_regisCbct);
  connect(m_Controls.pushButton_assembleRegistrationPoints, &QPushButton::clicked, this, &DentalWidget::AssembleRegistrationPoints);
  


  // Prepare some empty pointsets for registration purposes
  if (GetDataStorage()->GetNamedNode("landmark_src") == nullptr)
  {
	  auto pset_landmark_src = mitk::PointSet::New();
	  auto node_pset_landmark_src = mitk::DataNode::New();
	  node_pset_landmark_src->SetData(pset_landmark_src);
	  node_pset_landmark_src->SetName("landmark_src");
	  GetDataStorage()->Add(node_pset_landmark_src);
  }
  
  if (GetDataStorage()->GetNamedNode("landmark_target") == nullptr)
  {
	  auto pset_landmark_target = mitk::PointSet::New();
	  auto node_pset_landmark_target = mitk::DataNode::New();
	  node_pset_landmark_target->SetData(pset_landmark_target);
	  node_pset_landmark_target->SetName("landmark_target");
	  GetDataStorage()->Add(node_pset_landmark_target);
  }

  if (GetDataStorage()->GetNamedNode("icp_fineTuning") == nullptr)
  {
	  auto pset_icp_fineTuning = mitk::PointSet::New();
	  auto node_pset_icp_fineTuning = mitk::DataNode::New();
	  node_pset_icp_fineTuning->SetData(pset_icp_fineTuning);
	  node_pset_icp_fineTuning->SetName("icp_fineTuning");
	  GetDataStorage()->Add(node_pset_icp_fineTuning);
  }

  if (GetDataStorage()->GetNamedNode("Precision_checkPoints") == nullptr)
  {
	  auto pset_precision = mitk::PointSet::New();
	  auto node_pset_precision = mitk::DataNode::New();
	  node_pset_precision->SetData(pset_precision);
	  node_pset_precision->SetName("Precision_checkPoints");
	  GetDataStorage()->Add(node_pset_precision);
  }

	
  m_eigenMatrixInitialOffset.setIdentity();

}

















