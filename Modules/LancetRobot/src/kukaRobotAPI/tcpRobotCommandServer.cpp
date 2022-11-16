#include "kukaRobotAPI/tcpRobotCommandServer.h"

bool lancet::TcpRobotCommandServer::connect(int port)
{
  Poco::Net::SocketAddress sa;
  m_ServerSocket = Poco::Net::ServerSocket(port);
  m_StreamSocket = m_ServerSocket.acceptConnection(sa);//The client socket's address is returned in sa.
  m_ClientAddr = sa.toString();
  //todo Check whether the connection is successful
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

int lancet::TcpRobotCommandServer::read(byte_t* buffer, int length) const
{
  //todo
  return 0;
}

int lancet::TcpRobotCommandServer::write(std::string string) const
{
  Poco::Net::SocketStream str(m_StreamSocket);
  str << string << std::endl << std::flush;
  //todo Check whether the write is successful
  return 0;
}

std::string lancet::TcpRobotCommandServer::connectionName() const
{
  return m_ClientAddr;
}
