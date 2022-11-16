//udp
#ifndef TCPROBOTCOMMANDSERVER_H
#define TCPROBOTCOMMANDSERVER_H

#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/SocketAddress.h>
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/JSONException.h"
#include "Poco/StreamCopier.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Query.h"
#include "Poco/JSON/PrintHandler.h"

#include <cstdint>

namespace lancet
{
  /**
 * C++ doesn't actually define a byte as 8-bits, but rather a number of bits that can
 * fit the entire character set. Here we define a byte as an 8 bit integer.
 */
  typedef uint8_t byte_t;
  /**
   * @brief A cross platform socket implementation.
   */
  class TcpRobotCommandServer
  {
  public:
    ///** Constructs a socket object that is cross-platform. */
    //TcpRobotCommandServer();

    ///**
    // * @brief Constructs a socket object and connects immediately.
    // * @param hostname The hostname or IP address of the measurement system.
    // * @param port The port to connect on. Port 8765 is default for Vega systems.
    // */
    //TcpRobotCommandServer(const char* hostname, const char* port = "8765");

    ///**
    // * @brief Closes the socket connection and frees memory.
    // */
    //virtual ~TcpRobotCommandServer();

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
   * @brief Reads 'length' bytes from the socket into 'buffer'
   * @param buffer The buffer to read into.
   * @param length The number of bytes to read.
   */
    int read(byte_t* buffer, int length) const;

    /**
     * @brief Writes 'length' chars from 'buffer' to the socket
     * @param buffer The buffer to write from.
     * @param length The number of chars to write.
     */
    int write(std::string string) const;

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
