#include "kukaRobotAPI/kukaRobotAPI.h"

#include "kukaRobotAPI/defaultProtocol.h"
#include <sys/timeb.h>

#include "kukaRobotAPI/robotInfoProtocol.h"

#define PI acos(-1)
std::string lancet::KukaRobotAPI::GetApiRevision()
{
  return "0.1.0";
}

bool lancet::KukaRobotAPI::Connect()
{
  //connect tcp to send robot command and receive result
  m_TcpConnection.connect(30009);
  //Send heartbeat packets through TCP to maintain connection with kuka robot application;
  m_KeepHeartBeat = true;
  m_TcpHeartBeatThread = std::thread(&lancet::KukaRobotAPI::sendTcpHeartBeat, this);

  //connect udp to receive robot info
  m_UdpConnection.connect("172.31.1.148", 30003);

  return true;
}

void lancet::KukaRobotAPI::DisConnect()
{
  m_KeepHeartBeat = false;
  m_TcpConnection.disconnect();
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

void lancet::KukaRobotAPI::ReadCommandResult()
{
  auto res = m_TcpConnection.read();
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

bool lancet::KukaRobotAPI::AddFrame(std::string name, std::array<double, 6> xyzabc) const
{
  DefaultProtocol protocol;
  protocol.operateType = "AddFrame";
  protocol.timestamp = timeStamp();
  protocol.param.target = std::move(name);
  protocol.param.FromArray(xyzabc);
  //convet obj to json format string
  std::stringstream  jsnString;
  protocol.ToJsonObj().stringify(jsnString);
  std::cout << jsnString.str() << std::endl;
  //send to robot
  //todo handle write error
  auto res = m_TcpConnection.write(jsnString.str());

  return true;
}

void lancet::KukaRobotAPI::SetMotionFrame(std::string name)
{
  DefaultProtocol protocol;
  protocol.operateType = "SetMotionFrame";
  protocol.timestamp = timeStamp();
  protocol.param.target = std::move(name);
  //convet obj to json format string
  std::stringstream  jsnString;
  protocol.ToJsonObj().stringify(jsnString);
  std::cout << jsnString.str() << std::endl;
  //send to robot
  //todo handle write error
  auto res = m_TcpConnection.write(jsnString.str());
}

void lancet::KukaRobotAPI::RunBrakeTest() const
{
  SendCommandNoPara("TestBrake");
}

void lancet::KukaRobotAPI::MovePTP(vtkMatrix4x4* target)
{
  DefaultProtocol protocol;
  protocol.operateType = "MovePTP";
  protocol.timestamp = timeStamp();
  //
  std::array<double, 6> xyzabc = kukaTransformMatrix2xyzabc(target);
  protocol.param.FromArray(xyzabc);
  //convet obj to json format string
  std::stringstream  jsnString;
  protocol.ToJsonObj().stringify(jsnString);
  std::cout << jsnString.str() << std::endl;
  //send to robot
  //todo handle write error
  auto res = m_TcpConnection.write(jsnString.str());
}

void lancet::KukaRobotAPI::HandGuiding() const
{
  SendCommandNoPara("FreeHand");
}

lancet::KukaRobotAPI::~KukaRobotAPI()
{
  this->DisConnect();
  m_TcpHeartBeatThread.join();
}

void lancet::KukaRobotAPI::sendTcpHeartBeat() const
{
  while (m_KeepHeartBeat)
  {
    auto res = m_TcpConnection.write("heartBeat");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //MITK_INFO << "-";
  }
}

long lancet::KukaRobotAPI::timeStamp()
{
  timeb t{};
  ftime(&t);
  return t.time * 1000 + t.millitm;
}

std::vector<double> lancet::KukaRobotAPI::kukamatrix2angle(const double matrix3x3[3][3])
{
  double nx = matrix3x3[0][0];
  double ny = matrix3x3[0][1];
  double nz = matrix3x3[0][2];
  double ox = matrix3x3[1][0];
  double oy = matrix3x3[1][1];
  double oz = matrix3x3[1][2];
  double ax = matrix3x3[2][0];
  double ay = matrix3x3[2][1];
  double az = matrix3x3[2][2];

  //«ÛΩ‚Rx°¢Ry°¢Rz
  //call Atan2(ax,sqrt(power(nx,2)+power(ox,2)),ry1)
  //double cry = (double)sqrt(nx * nx + ox * ox);
  double rx, ry, rz, rx2, ry2, rz2, rx_Origin, ry_Origin, rz_Origin;
  double FlagRx = 0.0, FlagRy = 0.0, FlagRz = 0.0;
  ry = (float)atan2(ax, (double)sqrt(nx * nx + ox * ox));
  if (ry == 90)
  {
    rx = 0;
    //call Atan2(ny,-nz,rz1)
    rz = (float)atan2(ny, -nz);
  }
  else if (ry == -90)
  {
    rx = 0;
    rz = (float)atan2(ny, nz);
    //call Atan2(ny,nz,rz1);
  }
  else
  {
    //call Atan2(-ay/cos(ry1),az/cos(ry1),rx1)
    //call Atan2(-ox/cos(ry1),nx/cos(ry1),rz1)
    rx = (float)atan2(-ay / cos(ry), az / cos(ry));
    rz = (float)atan2(-ox / cos(ry), nx / cos(ry));
  }

  //call Atan2(ax,-sqrt(power(nx,2)+power(ox,2)),ry2)
  ry2 = (float)atan2(ax, -(double)sqrt(nx * nx + ox * ox));
  if (ry2 == 90)
  {
    rx2 = 0;
    //call Atan2(ny,-nz,rz1)
    rz2 = (float)atan2(ny, -nz);
  }
  else if (ry2 == -90)
  {
    rx2 = 0;
    rz2 = (float)atan2(ny, nz);
    //call Atan2(ny,nz,rz1);
  }
  else
  {
    //call Atan2(-ay/cos(ry1),az/cos(ry1),rx1)
    //call Atan2(-ox/cos(ry1),nx/cos(ry1),rz1)
    rx2 = (float)atan2(-ay / cos(ry2), az / cos(ry2));
    rz2 = (float)atan2(-ox / cos(ry2), nx / cos(ry2));
  }

  std::cout << "rz11:" << -rz * (180.0f / PI) << "   ry:" << -ry * (180.0f / PI) << "   rx:" << -rx * (180.0f / PI) << "\n";
  std::cout << "rz22:" << -rz2 * (180.0f / PI) << "   ry:" << -ry2 * (180.0f / PI) << "   rx:" << -rx2 * (180.0f / PI) << "\n";
  if (abs(rx) + abs(ry) + abs(rz) <= abs(rx2) + abs(ry2) + abs(rz2))
  {
    rx_Origin = -rx;
    ry_Origin = -ry;
    rz_Origin = -rz;
  }
  else
  {
    rx_Origin = -rx2;
    ry_Origin = -ry2;
    rz_Origin = -rz2;
  }
  std::vector<double> i;


  if (abs(ry * (180.0f /PI)) < 90)
  {
    i.push_back(-rz * (180.0f / PI));
    i.push_back(-ry * (180.0f / PI));
    i.push_back(-rx * (180.0f / PI));
  }
  else
  {
    i.push_back(-rz2 * (180.0f / PI));
    i.push_back(-ry2 * (180.0f / PI));
    i.push_back(-rx2 * (180.0f / PI));
  }
  //MITK_INFO << i[0] << "," << i[1] << "," << i[2];
  return i;
}

std::array<double, 6> lancet::KukaRobotAPI::kukaTransformMatrix2xyzabc(vtkMatrix4x4* matrix4x4)
{
  const double R[3][3] = { {matrix4x4->GetElement(0, 0), matrix4x4->GetElement(0, 1), matrix4x4->GetElement(0, 2)},
      {matrix4x4->GetElement(1, 0), matrix4x4->GetElement(1, 1), matrix4x4->GetElement(1, 2)},
      {matrix4x4->GetElement(2, 0), matrix4x4->GetElement(2, 1), matrix4x4->GetElement(2, 2) }
  };

  std::vector<double> abc = kukamatrix2angle(R);
  std::array<double, 6> res{};

  res[0] = matrix4x4->GetElement(0, 3); ///< x
  res[1] = matrix4x4->GetElement(1, 3); ///< y
  res[2] = matrix4x4->GetElement(2, 3); ///< z

  res[3] = abc[0]; ///< a
  res[4] = abc[1]; ///< b
  res[5] = abc[2]; ///< c

  return res;
}
