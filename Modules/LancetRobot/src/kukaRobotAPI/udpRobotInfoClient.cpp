#include "kukaRobotAPI/udpRobotInfoClient.h"

#include <Poco/Net/NetException.h>

#include "mitkLogMacros.h"

bool lancet::UdpRobotInfoClient::connect(std::string ip, int port)
{
  try
  {
    Poco::Net::SocketAddress sa(ip, port);
    m_DatagramSocket.bind(sa);
    return true;
  }
  catch (Poco::Net::NetException & e)
  {
    MITK_ERROR << "UdpRobotInfoClient connect ERROR: " << e.message();
    return false;
  }
}

std::string lancet::UdpRobotInfoClient::read()
{
  try
  {
    char buffer[1024];
    Poco::Net::SocketAddress sender;
    int n = m_DatagramSocket.receiveFrom(buffer, sizeof(buffer) - 1, sender);
    buffer[n] = '\0';

    return std::string(buffer);
  }
  catch (Poco::Net::NetException & e)
  {
    MITK_ERROR << "UdpRobotInfoClient Read ERROR: " << e.message();
    return {};
  }
}
