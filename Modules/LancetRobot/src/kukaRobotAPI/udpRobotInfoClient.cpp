#include "kukaRobotAPI/udpRobotInfoClient.h"

bool lancet::UdpRobotInfoClient::connect(std::string ip, int port)
{
  Poco::Net::SocketAddress sa(ip, port);
  m_DatagramSocket.bind(sa);
  
  //m_Thread = std::thread(&RobotView::threadUDP_RobotInfo, this);
  return false;
}

std::string lancet::UdpRobotInfoClient::read()
{
  char buffer[1024];
  Poco::Net::SocketAddress sender;
  int n = m_DatagramSocket.receiveFrom(buffer, sizeof(buffer) - 1, sender);
  buffer[n] = '\0';

  return std::string(buffer);
}
