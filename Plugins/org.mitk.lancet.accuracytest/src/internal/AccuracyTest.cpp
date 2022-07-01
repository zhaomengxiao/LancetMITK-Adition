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
#include "AccuracyTest.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <QTimer>

const std::string AccuracyTest::VIEW_ID = "org.mitk.views.accuracytest";

void AccuracyTest::SetFocus()
{
  m_Controls.m_pushButtonSetProbe->setFocus();
}

void AccuracyTest::CreateQtPartControl(QWidget *parent)
{
  //init members
  m_IDofProbe = -1;
  m_IDofRF = -1;
  m_TrackingTimer = new QTimer(this);

  m_PointSetPivoting = mitk::PointSet::New();
  m_PointSetPivotingNode = mitk::DataNode::New();
  m_PointSetPivotingNode->SetData(m_PointSetPivoting);
  


  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);

  //init widget

  m_Controls.m_PointListPivoting->SetPointSetNode(m_PointSetPivotingNode);
  //connect
  connect(m_TrackingTimer, SIGNAL(timeout()), this, SLOT(UpdateTrackingTimer()));
  connect(m_Controls.m_pushButtonSetProbe, &QPushButton::clicked, this, &AccuracyTest::SetProbe);
  connect(m_Controls.m_pushButtonSetRF, &QPushButton::clicked, this, &AccuracyTest::SetReferenceFrame);
  connect(m_Controls.m_AddPoint, &QPushButton::clicked, this, &AccuracyTest::AddPivotPoint);
}

void AccuracyTest::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
  // foreach (mitk::DataNode::Pointer node, nodes)
  // {
  //   if (node.IsNotNull() && dynamic_cast<mitk::Image *>(node->GetData()))
  //   {
  //     m_Controls.labelWarning->setVisible(false);
  //     m_Controls.buttonPerformImageProcessing->setEnabled(true);
  //     return;
  //   }
  // }
  //
  // m_Controls.labelWarning->setVisible(true);
  // m_Controls.buttonPerformImageProcessing->setEnabled(false);
}


bool AccuracyTest::CheckInitialization(bool requireRF)
{
	if ((m_IDofProbe == -1) ||
		((requireRF) &&
		(m_IDofRF == -1)
			)
		)
	{
		QMessageBox msgBox;
		msgBox.setText("Tool to calibrate and/or calibration pointer not initialized, cannot proceed!");
		msgBox.exec();
		return false;
	}
	return true;
}

void AccuracyTest::SetProbe()
{
	m_IDofProbe = m_Controls.m_SelectionWidget->GetSelectedToolID();
	if (m_IDofProbe == -1)
	{
		m_Controls.m_Label_Probe->setText("<none>");
		m_Controls.m_StatusWidgetProbe->RemoveStatusLabels();
		m_TrackingTimer->stop();
	}
	else
	{
		m_NavigationDataSourceOfProbe = m_Controls.m_SelectionWidget->GetSelectedNavigationDataSource();
		m_Controls.m_Label_Probe->setText(m_NavigationDataSourceOfProbe->GetOutput(m_IDofProbe)->GetName());
		//initialize widget
		m_Controls.m_StatusWidgetProbe->RemoveStatusLabels();
		m_Controls.m_StatusWidgetProbe->SetShowPositions(true);
		m_Controls.m_StatusWidgetProbe->SetTextAlignment(Qt::AlignLeft);
		m_Controls.m_StatusWidgetProbe->AddNavigationData(m_NavigationDataSourceOfProbe->GetOutput(m_IDofProbe));
		m_Controls.m_StatusWidgetProbe->ShowStatusLabels();
		if (!m_TrackingTimer->isActive()) m_TrackingTimer->start(100);
	}
}

void AccuracyTest::SetReferenceFrame()
{
	m_IDofRF = m_Controls.m_SelectionWidget->GetSelectedToolID();
	if (m_IDofRF == -1)
	{
		m_Controls.m_Label_RF->setText("<none>");
		m_Controls.m_StatusWidgetRF->RemoveStatusLabels();
		m_TrackingTimer->stop();
	}
	else
	{
		m_NavigationDataSourceOfRF = m_Controls.m_SelectionWidget->GetSelectedNavigationDataSource();
		m_Controls.m_Label_RF->setText(m_NavigationDataSourceOfRF->GetOutput(m_IDofRF)->GetName());
		//initialize widget
		m_Controls.m_StatusWidgetRF->RemoveStatusLabels();
		m_Controls.m_StatusWidgetRF->SetShowPositions(true);
		m_Controls.m_StatusWidgetRF->SetTextAlignment(Qt::AlignLeft);
		m_Controls.m_StatusWidgetRF->AddNavigationData(m_NavigationDataSourceOfRF->GetOutput(m_IDofRF));
		m_Controls.m_StatusWidgetRF->ShowStatusLabels();
		if (!m_TrackingTimer->isActive()) m_TrackingTimer->start(100);
	}
}

void AccuracyTest::UpdateTrackingTimer()
{
	m_Controls.m_StatusWidgetProbe->Refresh();
	m_Controls.m_StatusWidgetRF->Refresh();
}

void AccuracyTest::AddPivotPoint()
{
	if (!CheckInitialization()) { return; }
	mitk::NavigationData::Pointer navDataTool = m_NavigationDataSourceOfRF->GetOutput(m_IDofRF);
	mitk::Point3D point = m_NavigationDataSourceOfProbe->GetOutput(m_IDofProbe)->GetPosition();

	//convert to itk transform
	itk::Vector<double, 3> translation;
	for (int k = 0; k < 3; k++) translation[k] = navDataTool->GetPosition()[k];
	itk::Matrix<double, 3, 3> rotation;
	for (int k = 0; k < 3; k++) for (int l = 0; l < 3; l++) rotation[k][l] = navDataTool->GetOrientation().rotation_matrix_transpose()[k][l];
	rotation = rotation.GetTranspose();
	itk::Vector<double> point_itk;
	point_itk[0] = point[0];
	point_itk[1] = point[1];
	point_itk[2] = point[2];

	//compute point in reference frame coordinates
	itk::Matrix<double, 3, 3> rotationInverse;
	for (int k = 0; k < 3; k++) for (int l = 0; l < 3; l++) rotationInverse[k][l] = rotation.GetInverse()[k][l];
	point_itk = rotationInverse * (point_itk - translation);

	//convert back and add landmark to pointset
	point[0] = point_itk[0];
	point[1] = point_itk[1];
	point[2] = point_itk[2];
	m_PointSetPivoting->InsertPoint(m_PointSetPivoting->GetSize(), point);
}
