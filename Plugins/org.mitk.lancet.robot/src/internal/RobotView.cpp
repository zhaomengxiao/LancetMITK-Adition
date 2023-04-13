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
// mitk 
#include <mitkImage.h>
#include <mitkTrackingDeviceSource.h>

//robotapi
#include "robotapi.h"
#include "robotsocket.h"
#include "robotcontroler.h"

//micro service
#include <usGetModuleContext.h>
#include <usModule.h>
#include <usServiceProperties.h>
#include <usModuleContext.h>
#include <usModuleInitialization.h>
US_INITIALIZE_MODULE


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

  m_device = lancet::KukaRobotDevice::New();

  connect(&m_timer, SIGNAL(timeout()), this, SLOT(UpdateToolPosition()));

  connect(m_Controls.SocketConnect, SIGNAL(clicked()),
	  this, SLOT(ConnectDevice()));
  connect(m_Controls.SocketDisConnect, SIGNAL(clicked()),
	  this, SLOT(DisConnectDevice()));
  connect(m_Controls.pushButton_SelfCheck, SIGNAL(clicked()),
    this, SLOT(SelfCheck()));
  // connect(m_Controls.pushButtonMove, SIGNAL(QPushButton::click),
	 //  this, SLOT(PrintToolPosition));
  connect(m_Controls.pushbtnSendCommand, SIGNAL(clicked()),
    this, SLOT(SendCommand()));
  
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
  //try get Device from Micro service
  us::ModuleContext* context = us::GetModuleContext();
  std::vector<us::ServiceReference<mitk::NavigationDataSource> > refs = context->GetServiceReferences<mitk::NavigationDataSource>();
  if (!refs.empty())
  {
    mitk::NavigationDataSource* navigationDataSource = context->GetService<
      mitk::NavigationDataSource>(refs.front());

    auto deviceSource = dynamic_cast<mitk::TrackingDeviceSource*>(navigationDataSource);
    if (deviceSource != nullptr && deviceSource->GetTrackingDevice().IsNotNull())
    {
      auto device = deviceSource->GetTrackingDevice();
      if (device->GetTrackingDeviceName() == "Kuka")
      {
        m_device = dynamic_cast<lancet::KukaRobotDevice*>(deviceSource->GetTrackingDevice().GetPointer());
      }
    }
  }
  else
  {
    MITK_ERROR << "No NavigationDataSource service found! new device create";
  }
  
  if (m_device.IsNull())
  {
    m_device = lancet::KukaRobotDevice::New();
  }
  if (m_device->GetState()==0)
  {
	  m_device->OpenConnection();
  }
  if (m_device->GetState() == 1)
  {
	  m_Controls.robotconnectstate->setText("Ready");
  }
  else
  {
	  m_Controls.robotconnectstate->setText("Tracking");
	  m_timer.start();
  }
	
}

void RobotView::DisConnectDevice()
{
	m_device->CloseConnection();
	if (m_device->GetState()==0)
	{
		m_Controls.robotconnectstate->setText("Disconnected");
	}
	else
	{
		m_Controls.robotconnectstate->setText("Connected");		
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

void RobotView::SelfCheck()
{
  if (m_device.IsNotNull() && m_device->GetState()!=0)
  {
    m_device->RequestExecOperate(/*"Robot",*/ "setio", { "20", "20" });
  }
  else
  {
    MITK_ERROR << "robot not connect";
  }
}

void RobotView::SendCommand()
{
  MITK_INFO << "Send Command";
  MITK_INFO << QString("try send %1 at ").arg(this->m_Controls.lineEditRoboticsAPI->text()).toStdString() << this->m_Controls.lineEditRoboticsParam->text();
  if (false == m_device->RequestExecOperate( this->m_Controls.lineEditRoboticsAPI->text(), this->m_Controls.lineEditRoboticsParam->text().split(",")))
  {
    MITK_WARN << QString("call robot %1 %2 faild").arg(this->m_Controls.lineEditRoboticsAPI->text()).arg(this->m_Controls.lineEditRoboticsParam->text()).toStdString();
    return;
  }
  MITK_INFO << "success";
}
