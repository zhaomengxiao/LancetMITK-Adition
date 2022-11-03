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

//172.31.1.147:18000:
//{
//"operateType":"test",
//"param":{"target":null,"x":3.0,"y":4.0,"z":5.0,"a":0.0,"b":1.0,"c":2.0},
//"param2":{"target":null,"x":3.0,"y":4.0,"z":5.0,"a":0.0,"b":1.0,"c":2.0},
//"timestamp":0,
//"jointPos":[0.0,0.0,0.0,0.0,0.0,0.0,0.0]
//}
class Param
{
public:
  std::string target{"null"};
  double x{};
  double y{};
  double z{};
  double a{};
  double b{};
  double c{};

  Poco::JSON::Object ToJsonObj() const
  {
    Poco::JSON::Object res;
    res.set("target", this->target);
    res.set("x", this->x);
    res.set("y", this->y);
    res.set("z", this->z);
    res.set("a", this->a);
    res.set("b", this->b);
    res.set("c", this->c);

    return res;
  }
};

class ProtocolBean
{
public:
  std::string operateType{"null"};
  Param param;
  Param param2;
  int timestamp{};
  std::array<double, 7> jointPos{};

  Poco::JSON::Object ToJsonObj()
  {
    Poco::JSON::Object res;
    res.set("operateType", this->operateType);
    res.set("Param", this->param.ToJsonObj());
    res.set("param2", this->param2.ToJsonObj());
    res.set("timestamp", this->timestamp);
    Poco::JSON::Array array;
    array.add(jointPos[0]);
    array.add(jointPos[1]);
    array.add(jointPos[2]);
    array.add(jointPos[3]);
    array.add(jointPos[4]);
    array.add(jointPos[5]);
    array.add(jointPos[6]);

    res.set("jointPos", array);

    return res;
  }
};

class kukaInfomation
{
public:
  double joint1;
  double joint2;
  double joint3;
  double joint4;
  double joint5;
  double joint6;
  double joint7;

  double Flange1;
  double Flange2;
  double Flange3;
  double Flange4;
  double Flange5;
  double Flange6;

  void FromJsonObj(Poco::JSON::Object obj)
  {
    joint1 = obj.get("joint1");
    joint2 = obj.get("joint2");
    joint3 = obj.get("joint3");
    joint4 = obj.get("joint4");
    joint5 = obj.get("joint5");
    joint6 = obj.get("joint6");
    joint7 = obj.get("joint7");

    Flange1 = obj.get("Flange1");
    Flange2 = obj.get("Flange2");
    Flange3 = obj.get("Flange3");
    Flange4 = obj.get("Flange4");
    Flange5 = obj.get("Flange5");
    Flange6 = obj.get("Flange6");
  }
};


const std::string RobotView::VIEW_ID = "org.mitk.views.robotview";

void RobotView::SetFocus()
{
  m_Controls.SocketConnect->setFocus();
}

RobotView::~RobotView()
{
}

void RobotView::CreateQtPartControl(QWidget* parent)
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

  connect(m_Controls.pushButton_startUDP, SIGNAL(clicked()),
          this, SLOT(StartUDP()));
}

void RobotView::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                   const QList<mitk::DataNode::Pointer>& nodes)
{
  // iterate all selected objects, adjust warning visibility
  foreach(mitk::DataNode::Pointer node, nodes)
  {
    if (node.IsNotNull() && dynamic_cast<mitk::Image*>(node->GetData()))
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
  std::vector<us::ServiceReference<mitk::NavigationDataSource>> refs = context->GetServiceReferences<
    mitk::NavigationDataSource>();
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
  if (m_device->GetState() == 0)
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
  if (m_device->GetState() == 0)
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
  if (m_device.IsNotNull() && m_device->GetState() != 0)
  {
    m_device->RequestExecOperate(/*"Robot",*/ "setio", {"20", "20"});
  }
  else
  {
    MITK_ERROR << "robot not connect";
  }
}

void RobotView::SendCommand()
{
  // MITK_INFO << "Send Command";
  // MITK_INFO << QString("try send %1 at ").arg(this->m_Controls.lineEditRoboticsAPI->text()).toStdString() << this->
  //   m_Controls.lineEditRoboticsParam->text();
  // if (false == m_device->RequestExecOperate(this->m_Controls.lineEditRoboticsAPI->text(),
  //                                           this->m_Controls.lineEditRoboticsParam->text().split(",")))
  // {
  //   MITK_WARN << QString("call robot %1 %2 faild").arg(this->m_Controls.lineEditRoboticsAPI->text()).arg(
  //     this->m_Controls.lineEditRoboticsParam->text()).toStdString();
  //   return;
  // }
  // MITK_INFO << "success";
  ProtocolBean bean;
  bean.operateType = this->m_Controls.lineEditRoboticsAPI->text().toStdString();
  std::stringstream  jsnString;
  bean.ToJsonObj().stringify(jsnString, 3);
  std::cout << jsnString.str() << std::endl;
  Poco::Net::StreamSocket ss = m_tcpSocket_RobotCommand.acceptConnection();
  Poco::Net::SocketStream str(ss);
  str << jsnString.str() << std::endl << std::flush;
}

void RobotView::StartUDP()
{
  //UDP server;
  Poco::Net::SocketAddress sa("172.31.1.148", 30003);
  m_udpSocket_RobotInfo.bind(sa);

  m_Thread = std::thread(&RobotView::threadUDP_RobotInfo, this);

  //TCP server;
  Poco::Net::SocketAddress sa2("172.31.1.147", 30009);
  m_tcpSocket_RobotCommand = Poco::Net::ServerSocket(30009);

  m_Thread2 = std::thread(&RobotView::threadUDP_HeartBeat, this);
}

void RobotView::threadUDP_RobotInfo()
{
  while (true)
  {
    char buffer[1024];
    Poco::Net::SocketAddress sender;
    int n = m_udpSocket_RobotInfo.receiveFrom(buffer, sizeof(buffer) - 1, sender);
    buffer[n] = '\0';

    std::cout << sender.toString() << ":" << buffer << std::endl;
    //m_Controls.j0->setText(buffer);

    //std::cout << sender.toString() << ":" << buffer << std::endl;
  

    Poco::JSON::Parser parser;
    Poco::Dynamic::Var result;
    parser.reset();

    result = parser.parse(buffer);
    Poco::JSON::Object::Ptr pObj = result.extract<Poco::JSON::Object::Ptr>();

    kukaInfomation info;
    info.FromJsonObj(*pObj);

    m_Controls.j0->setText(QString::number(info.joint1));
    m_Controls.j1->setText(QString::number(info.joint2));
    m_Controls.j2->setText(QString::number(info.joint3));
    m_Controls.j3->setText(QString::number(info.joint4));
    m_Controls.j4->setText(QString::number(info.joint5));
    m_Controls.j5->setText(QString::number(info.joint6));
    m_Controls.j6->setText(QString::number(info.joint7));

    m_Controls.x->setText(QString::number(info.Flange1));
    m_Controls.y->setText(QString::number(info.Flange2));
    m_Controls.z->setText(QString::number(info.Flange3));
    m_Controls.a->setText(QString::number(info.Flange4));
    m_Controls.b->setText(QString::number(info.Flange5));
    m_Controls.c->setText(QString::number(info.Flange6));
  }
}

void RobotView::threadUDP_HeartBeat()
{
  while (true)
  {
    std::string cmd = "heartBeat";
    Poco::Net::StreamSocket ss = m_tcpSocket_RobotCommand.acceptConnection();
    Poco::Net::SocketStream str(ss);
    str << cmd << std::endl << std::flush;
    
    Sleep(1000);
    MITK_INFO << "-";
  }
}
