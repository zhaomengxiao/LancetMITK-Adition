#ifndef KUKAROBOTAPI_H
#define KUKAROBOTAPI_H

#include <array>

#include "MitkLancetRobotExports.h"
#include <itkObject.h>
#include <mitkCommon.h>
#include <thread>
#include <sys/timeb.h>

#include "tcpRobotCommandServer.h"
#include "udpRobotInfoClient.h"
#include <vtkMatrix4x4.h>

/**
 * @brief This class encapsulates communication with kuka iiwa med devices.
 * @details This class encapsulates binary and string parsing required to send/receive commands.
 *          This class does not provide an exhaustive implementation of every function that robot supported, it does
 *          not implement every option available for some commmands. 
 */
class RobotToolProtocol;
class DefaultProtocol;
class RobotInformationProtocol;
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

    //The following port numbers (client or server socket) can be used in a robot application:
    //• 30, 000 to 30, 010
    bool Connect();
   
    void DisConnect();

    void SendCommandNoPara(std::string cmd) const;

    void ReadCommandResult();

    RobotInformationProtocol GetRobotInfo();

    // bool GetTrackingData();
    //
    // bool GetJointPosition(std::array<double, 6>& joints);

    //void SetLoad(RobotToolProtocol toolProtocol);
    bool AddFrame(std::string name, std::array<double, 6> xyzabc) const;
    void SetMotionFrame(std::string name);

    /**
     * \brief Check robot brake test state first,if a brake test is needed then run it.
     * A brake test must run after rebooting the robot,Otherwise the robot won't perform any action.
     */
    void RunBrakeTest() const;

    void MovePTP(vtkMatrix4x4* target);

    void HandGuiding() const;

    //void Nothing();
    ~KukaRobotAPI() override;
  protected:
    
    void sendTcpHeartBeat() const;

    static long timeStamp();

    std::vector<double> kukamatrix2angle(const double matrix3x3[3][3]);

    std::array<double, 6> kukaTransformMatrix2xyzabc(vtkMatrix4x4* matrix4x4);

    TcpRobotCommandServer m_TcpConnection;
    UdpRobotInfoClient m_UdpConnection;
    std::thread m_TcpHeartBeatThread;
    bool m_KeepHeartBeat{false};
  };
}

#endif // KUKAROBOTAPI_H
