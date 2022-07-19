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

// Qt
#include <QMessageBox>

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
  InitNodeSelector(m_Controls.mitkNodeSelectWidget_MovingObject);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_LandmarkSrcPointset);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_LandmarkTargetPointset);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_IcpTargetPointset);
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_IcpSrcSurface);

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

  connect(m_Controls.pushButton_Landmark, &QPushButton::clicked, this, &DentalWidget::LandmarkRegistration);
  connect(m_Controls.pushButton_Icp, &QPushButton::clicked, this, &DentalWidget::IcpRegistration);
  connect(m_Controls.pushButton_ApplyRegistrationMatrix, &QPushButton::clicked, this, &DentalWidget::AppendRegistrationMatrix);



}











