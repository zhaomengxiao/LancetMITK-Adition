//udp
#ifndef TCPROBOTCOMMANDSERVER_H
#define TCPROBOTCOMMANDSERVER_H

#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>

namespace lancet
{
  /**
 * C++ doesn't actually define a byte as 8-bits, but rather a number of bits that can
 * fit the entire character set. Here we define a byte as an 8 bit integer.
 */
  using byte_t = uint8_t;
  /**
   * @brief A cross platform stream socket implementation.
   */
  class TcpRobotCommandServer
  {
  public:
    /**
     * @brief Closes any existing connection, and connects to the new device.
     * @param hostname The hostname or IP address of the device.
     * @param port The port number to connect on.
     */
    bool connect(int port);

    /** @brief Closes the socket connection */
    void disconnect();

    /** @brief Returns true if the socket connection succeeded */
    bool isConnected() const;


    /**
     * \brief 
     * \param msg 
     * \return  true if read success
     */
    bool read(std::string& msg);


    /**
     * \brief send massage to robot use stream socket
     * \param msg string send to robot
     * \return true if write success
     */
    bool write(std::string msg) const;

    /**
     * @brief gets the Client IP address as string
     */
    std::string connectionName() const;

  private:
    //static const int NUM_CONNECTION_RETRIES = 3;
    bool m_IsConnected{false};
    std::string m_ClientAddr;

    Poco::Net::ServerSocket m_ServerSocket;
    Poco::Net::StreamSocket m_StreamSocket;
  };
}
#endif // TCPROBOTCOMMANDSERVER_H
