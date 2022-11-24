#include "kukaRobotAPI/tcpRobotCommandServer.h"
#include "Poco/Net/NetException.h"
#include "mitkLogMacros.h"
bool lancet::TcpRobotCommandServer::connect(int port)
{
  Poco::Net::SocketAddress sa;
  m_ServerSocket = Poco::Net::ServerSocket(port);
  try
  {
    m_StreamSocket = m_ServerSocket.acceptConnection(sa);//The client socket's address is returned in sa.
  }
  catch (Poco::Net::NetException& e)
  {
    MITK_ERROR << "TcpRobotCommandServer Connect ERROR" << e.message();
    return false;
  }
  
  m_ClientAddr = sa.toString();
  m_IsConnected = true;
  return true;
}

void lancet::TcpRobotCommandServer::disconnect()
{
  m_ServerSocket.close();
  m_IsConnected = false;
}

bool lancet::TcpRobotCommandServer::isConnected() const
{
  return m_IsConnected;
}

bool lancet::TcpRobotCommandServer::read(std::string& msg)
{
  try
  {
    if (m_StreamSocket.available())//no block
    {
      Poco::FIFOBuffer buffer(1024);
      const int n = m_StreamSocket.receiveBytes(buffer); //block
      msg = std::string(buffer.begin(), n);
      //std::cout << "recv length : " << n << "," << "value : " << val << std::endl;
      return true;
    }
    return false;
  }
  catch (Poco::Net::NetException& e)
  {
    MITK_ERROR<<"TcpRobotCommandServer Read ERROR: " << e.message();
    return false;
  }
  catch (Poco::IOException& e)
  {
    MITK_ERROR << "TcpRobotCommandServer Read ERROR: " << e.message();
    return false;
  }
  
}

bool lancet::TcpRobotCommandServer::write(std::string msg) const
{
  try
  {
    Poco::Net::SocketStream str(m_StreamSocket);
    str << msg << std::endl << std::flush;
    return true;
  }
  catch (Poco::Net::NetException & e)
  {
    MITK_ERROR << e.message();
    return false;
  }
  catch (Poco::IOException & e)
  {
    MITK_ERROR << "TcpRobotCommandServer Read ERROR: " << e.message();
    return false;
  }
}

std::string lancet::TcpRobotCommandServer::connectionName() const
{
  return m_ClientAddr;
}
