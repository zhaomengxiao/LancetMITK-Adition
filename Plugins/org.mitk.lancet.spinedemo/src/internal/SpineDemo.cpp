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
#include "SpineDemo.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <QFileDialog>
#include <QTimer>

#include "lancetTrackingDeviceSourceConfigurator.h"
#include "lancetVegaTrackingDevice.h"
#include "mitkNavigationToolStorageDeserializer.h"
#include "mitkNodePredicateAnd.h"
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"

const std::string SpineDemo::VIEW_ID = "org.mitk.views.spinedemo";

void SpineDemo::SetFocus()
{
  // m_Controls.buttonPerformImageProcessing->setFocus();
}

void SpineDemo::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  // connect(m_Controls.buttonPerformImageProcessing, &QPushButton::clicked, this, &SpineDemo::DoImageProcessing);
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_implant_3d);
  connect(m_Controls.pushButton_connectCamera, &QPushButton::clicked, this, &SpineDemo::on_pushButton_connectCamera_clicked);
  connect(m_Controls.pushButton_initCheckPoint_3d, &QPushButton::clicked, this, &SpineDemo::on_pushButton_initCheckPoint_3d_clicked);
  connect(m_Controls.pushButton_detectBalls_3d, &QPushButton::clicked, this, &SpineDemo::on_pushButton_detectBalls_3d_clicked);
  connect(m_Controls.pushButton_startPlan_3d, &QPushButton::clicked, this, &SpineDemo::on_pushButton_startPlan_3d_clicked);

  connect(m_Controls.pushButton_U_ax, &QPushButton::clicked, this, &SpineDemo::on_pushButton_U_ax_clicked);
  connect(m_Controls.pushButton_D_ax, &QPushButton::clicked, this, &SpineDemo::on_pushButton_D_ax_clicked);
  connect(m_Controls.pushButton_R_ax, &QPushButton::clicked, this, &SpineDemo::on_pushButton_R_ax_clicked);
  connect(m_Controls.pushButton_L_ax, &QPushButton::clicked, this, &SpineDemo::on_pushButton_L_ax_clicked);
  connect(m_Controls.pushButton_D_cor, &QPushButton::clicked, this, &SpineDemo::on_pushButton_D_cor_clicked);
  connect(m_Controls.pushButton_D_sag, &QPushButton::clicked, this, &SpineDemo::on_pushButton_D_sag_clicked);
  connect(m_Controls.pushButton_U_cor, &QPushButton::clicked, this, &SpineDemo::on_pushButton_U_cor_clicked);
  connect(m_Controls.pushButton_U_sag, &QPushButton::clicked, this, &SpineDemo::on_pushButton_U_sag_clicked);
  connect(m_Controls.pushButton_L_cor, &QPushButton::clicked, this, &SpineDemo::on_pushButton_L_cor_clicked);
  connect(m_Controls.pushButton_R_cor, &QPushButton::clicked, this, &SpineDemo::on_pushButton_R_cor_clicked);
  connect(m_Controls.pushButton_L_sag, &QPushButton::clicked, this, &SpineDemo::on_pushButton_L_sag_clicked);
  connect(m_Controls.pushButton_R_sag, &QPushButton::clicked, this, &SpineDemo::on_pushButton_R_sag_clicked);
  connect(m_Controls.pushButton_clock_cor, &QPushButton::clicked, this, &SpineDemo::on_pushButton_clock_cor_clicked);
  connect(m_Controls.pushButton_counter_cor, &QPushButton::clicked, this, &SpineDemo::on_pushButton_counter_cor_clicked);
  connect(m_Controls.pushButton_clock_sag, &QPushButton::clicked, this, &SpineDemo::on_pushButton_clock_sag_clicked);
  connect(m_Controls.pushButton_counter_sag, &QPushButton::clicked, this, &SpineDemo::on_pushButton_counter_sag_clicked);
  connect(m_Controls.pushButton_clock_ax, &QPushButton::clicked, this, &SpineDemo::on_pushButton_clock_ax_clicked);
  connect(m_Controls.pushButton_counter_ax, &QPushButton::clicked, this, &SpineDemo::on_pushButton_counter_ax_clicked);

}

void SpineDemo::InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget)
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

void SpineDemo::on_pushButton_connectCamera_clicked()
{
	//read in filename
	QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/",
		tr("Tool Storage Files (*.IGTToolStorage)"));
	if (filename.isNull()) return;

	//read tool storage from disk
	std::string errorMessage = "";
	mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
		GetDataStorage());
	m_VegaToolStorage = myDeserializer->Deserialize(filename.toStdString());
	m_VegaToolStorage->SetName(filename.toStdString());


	MITK_INFO << "Vega tracking";
	lancet::NDIVegaTrackingDevice::Pointer vegaTrackingDevice = lancet::NDIVegaTrackingDevice::New(); //instantiate

	//Create Navigation Data Source with the factory class, and the visualize filter.
	lancet::TrackingDeviceSourceConfiguratorLancet::Pointer vegaSourceFactory =
		lancet::TrackingDeviceSourceConfiguratorLancet::New(m_VegaToolStorage, vegaTrackingDevice);

	m_VegaSource = vegaSourceFactory->CreateTrackingDeviceSource(m_VegaVisualizer);
	m_VegaSource->SetToolMetaDataCollection(m_VegaToolStorage);
	m_VegaSource->Connect();

	m_VegaSource->StartTracking();

	//update visualize filter by timer
	if (m_VegaVisualizeTimer == nullptr)
	{
		m_VegaVisualizeTimer = new QTimer(this); //create a new timer
	}
	connect(m_VegaVisualizeTimer, &QTimer::timeout, this, &SpineDemo::OnVegaVisualizeTimer);

	ShowToolStatus_Vega();

	m_VegaVisualizeTimer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps

	auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
	mitk::RenderingManager::GetInstance()->InitializeViews(geo);

	m_ImageRegistrationMatrix->Identity();
}


void SpineDemo::OnVegaVisualizeTimer()
{
	//Here we call the Update() method from the Visualization Filter. Internally the filter checks if
	//new NavigationData is available. If we have a new NavigationData the cone position and orientation
	//will be adapted.

	if (m_VegaVisualizer.IsNotNull())
	{
		m_VegaVisualizer->Update();
		// auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
		// mitk::RenderingManager::GetInstance()->InitializeViews(geo);
		this->RequestRenderWindowUpdate();
	}

	m_Controls.m_StatusWidgetVegaToolToShow->Refresh();

}

void SpineDemo::ShowToolStatus_Vega()
{
	m_VegaNavigationData.clear();
	for (std::size_t i = 0; i < m_VegaSource->GetNumberOfOutputs(); i++)
	{
		m_VegaNavigationData.push_back(m_VegaSource->GetOutput(i));
	}
	//initialize widget
	m_Controls.m_StatusWidgetVegaToolToShow->RemoveStatusLabels();
	m_Controls.m_StatusWidgetVegaToolToShow->SetShowPositions(true);
	m_Controls.m_StatusWidgetVegaToolToShow->SetTextAlignment(Qt::AlignLeft);
	m_Controls.m_StatusWidgetVegaToolToShow->SetNavigationDatas(&m_VegaNavigationData);
	m_Controls.m_StatusWidgetVegaToolToShow->ShowStatusLabels();
}

