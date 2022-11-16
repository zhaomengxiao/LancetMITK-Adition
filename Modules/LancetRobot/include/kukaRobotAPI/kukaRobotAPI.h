#ifndef KUKAROBOTAPI_H
#define KUKAROBOTAPI_H

#include "MitkLancetRobotExports.h"
#include <itkObject.h>
#include <mitkCommon.h>
#include <thread>
#include <sys/timeb.h>

#include "tcpRobotCommandServer.h"
#include "udpRobotInfoClient.h"
/**
 * @brief This class encapsulates communication with kuka iiwa med devices.
 * @details This class encapsulates binary and string parsing required to send/receive commands.
 *          This class does not provide an exhaustive implementation of every function that robot supported, it does
 *          not implement every option available for some commmands. 
 */
class RobotToolProtocol;
class DefaultProtocol;
class RobotInfoProtocol;
namespace lancet
{
  class MITKLANCETROBOT_EXPORT KukaRobotAPI :public itk::Object
  {
  public:
    mitkClassMacroItkParent(KukaRobotAPI, itk::Object);
    itkFactorylessNewMacro(Self); //New() and CreateAnother()
    itkCloneMacro(Self);

    //External control via UDP interface
    // void StartRobotApplication();
    // void ResumeRobotApplication();
    // void PauseRobotApplication();

    static std::string GetApiRevision();

    void Connect();
   
    void DisConnect();

    void SendCommandNoPara(std::string cmd) const;

    RobotInfoProtocol GetRobotInfo();

    // void AddTool(RobotToolProtocol toolProtocol);
    // void SelectTool(unsigned int i);

    /**
     * \brief Check robot brake test state first,if a brake test is needed then run it.
     * A brake test must run after rebooting the robot,Otherwise the robot won't perform any action.
     */
    void RunBrakeTest() const;

    //void MovePTP();

    void HandGuiding() const;

    //void Nothing();

  protected:
    void sendTcpHeartBeat() const;

    static long timeStamp();


    TcpRobotCommandServer m_TcpConnection;
    UdpRobotInfoClient m_UdpConnection;
    std::thread m_TcpHeartBeatThread;
  };
}

#endif // KUKAROBOTAPI_H
