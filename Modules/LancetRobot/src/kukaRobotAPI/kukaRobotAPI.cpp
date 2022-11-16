#include "kukaRobotAPI/kukaRobotAPI.h"

#include "kukaRobotAPI/defaultProtocol.h"
#include <sys/timeb.h>

#include "kukaRobotAPI/robotInfoProtocol.h"

std::string lancet::KukaRobotAPI::GetApiRevision()
{
  return "0.1.0";
}

void lancet::KukaRobotAPI::Connect()
{
  //connect tcp to send robot command and receive result
  m_TcpConnection.connect(30009);
  //Send heartbeat packets through TCP to maintain connection with kuka robot application;
  m_TcpHeartBeatThread = std::thread(&lancet::KukaRobotAPI::sendTcpHeartBeat, this);

  //connect udp to receive robot info
  m_UdpConnection.connect("172.31.1.148", 30009);
}

void lancet::KukaRobotAPI::DisConnect()
{
  //todo
}

void lancet::KukaRobotAPI::SendCommandNoPara(std::string cmd) const
{
  DefaultProtocol protocol;
  protocol.operateType = cmd;
  protocol.timestamp = timeStamp();
  //convet obj to json format string
  std::stringstream  jsnString;
  protocol.ToJsonObj().stringify(jsnString);
  std::cout << jsnString.str() << std::endl;
  //send to robot
  //todo handle write error
  auto res = m_TcpConnection.write(jsnString.str());
}

RobotInfoProtocol lancet::KukaRobotAPI::GetRobotInfo()
{
  Poco::JSON::Parser parser;
  Poco::Dynamic::Var result;
  parser.reset();
  // todo handle udp read empty or wrong
  result = parser.parse(m_UdpConnection.read());
  Poco::JSON::Object::Ptr pObj = result.extract<Poco::JSON::Object::Ptr>();

  RobotInfoProtocol info{};
  info.FromJsonObj(*pObj);
  return info;
}

void lancet::KukaRobotAPI::RunBrakeTest() const
{
  SendCommandNoPara("TestBrake");
}

void lancet::KukaRobotAPI::HandGuiding() const
{
  SendCommandNoPara("FreeHand");
}

void lancet::KukaRobotAPI::sendTcpHeartBeat() const
{
  while (true)
  {
    auto res = m_TcpConnection.write("heartBeat");
    Sleep(1000);
    //MITK_INFO << "-";
  }
}

long lancet::KukaRobotAPI::timeStamp()
{
  timeb t{};
  ftime(&t);
  return t.time * 1000 + t.millitm;
}
