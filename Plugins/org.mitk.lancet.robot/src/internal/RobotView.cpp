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
#include "RobotView.h"

// Qt
#include <QMessageBox>
#include <QPushButton>
// mitk image
#include <mitkImage.h>

//robotapi
#include "robotapi.h"
#include "robotsocket.h"
#include "robotcontroler.h"

const std::string RobotView::VIEW_ID = "org.mitk.views.robotview";

void RobotView::SetFocus()
{
  m_Controls.SocketConnect->setFocus();
}

RobotView::~RobotView()
{
	
}

void RobotView::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  // timer
  m_timer.setInterval(10);

  m_device = KukaRobotDevice::New();

  connect(&m_timer, SIGNAL(timeout()), this, SLOT(UpdateToolPosition()));

  connect(m_Controls.SocketConnect, SIGNAL(clicked()),
	  this, SLOT(ConnectDevice()));
  connect(m_Controls.SocketDisConnect, SIGNAL(clicked()),
	  this, SLOT(DisConnectDevice()));
  // connect(m_Controls.pushButtonMove, SIGNAL(QPushButton::click),
	 //  this, SLOT(PrintToolPosition));

}

void RobotView::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                   const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
  foreach(mitk::DataNode::Pointer node, nodes)
  {
    if (node.IsNotNull() && dynamic_cast<mitk::Image *>(node->GetData()))
    {
      //m_Controls.labelWarning->setVisible(false);
      //m_Controls.buttonPerformImageProcessing->setEnabled(true);
      return;
    }
  }

  //m_Controls.labelWarning->setVisible(true);
  //m_Controls.buttonPerformImageProcessing->setEnabled(false);
}

void RobotView::ConnectDevice()
{
	m_device->OpenConnection();
	if(m_device->GetIsConnected())
	{
		m_Controls.robotconnectstate->setText("Connected");
		m_timer.start();
	}
	else
	{
		m_Controls.robotconnectstate->setText("Disconnected");
	}
}

void RobotView::DisConnectDevice()
{
	m_device->CloseConnection();
	if (m_device->GetIsConnected())
	{
		m_Controls.robotconnectstate->setText("Connected");
	}
	else
	{
		m_Controls.robotconnectstate->setText("Disconnected");
	}
}

void RobotView::UpdateToolPosition()
{
	m_device->TrackTools();
	std::array<double, 6> data = m_device->GetTrackingData();
	m_Controls.x->setText(QString::fromStdString(std::to_string(data[0])));
	m_Controls.y->setText(QString::fromStdString(std::to_string(data[1])));
	m_Controls.z->setText(QString::fromStdString(std::to_string(data[2])));

	m_Controls.a->setText(QString::fromStdString(std::to_string(data[3])));
	m_Controls.b->setText(QString::fromStdString(std::to_string(data[4])));
	m_Controls.c->setText(QString::fromStdString(std::to_string(data[5])));
}
