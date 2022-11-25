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

namespace lancet
{
  class UdpRobotInfoClient
  {
  public:
    bool connect(std::string ip, int port);

    void disconnect();
    /**
     * \brief receive udp massage.
     * \return massage string.if read failed return empty string. 
     */
    std::string read();
    //void write();

    
  private:
    std::string m_ServerAddr;
    Poco::Net::DatagramSocket m_DatagramSocket;
  };
}
