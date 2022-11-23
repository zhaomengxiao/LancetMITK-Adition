#ifndef KUKAROBOTDEVICE_H
#define KUKAROBOTDEVICE_H
//mitk
#include <mitkCommon.h>
#include <mitkTrackingDevice.h>

#include "MitkLancetIGTExports.h"
#include "mitkTrackingTool.h"

//KUKA ROBOT API
#include "kukaRobotAPI/kukaRobotAPI.h"

//#include "robotapi.h"
#include "lancetRobotTrackingTool.h"
//#include "udpsocketrobotheartbeat.h" //udp

typedef mitk::Point<mitk::ScalarType, 6> xyzabc;
namespace lancet
{
  /** Documentation
  * \brief superclass for specific KUKA Robot Devices that use socket communication.
  *
  * implements the TrackingDevice interface for Kuka robot devices ()
  *
  * \ingroup Robot
 */
  class MITKLANCETIGT_EXPORT  KukaRobotDevice_New :public  mitk::TrackingDevice
  {
  public:
    mitkClassMacro(KukaRobotDevice_New, TrackingDevice);
    itkFactorylessNewMacro(Self);
    itkCloneMacro(Self);

    typedef std::vector<RobotTool::Pointer> RobotTools;

    //itkGetMacro(IsConnected, bool);
    /**
       * @brief Opens the connection to the device. This have to be done before the tracking is started.
       */

    bool OpenConnection() override;
    /**
       * @brief Closes the connection and clears all resources.
       */
    bool CloseConnection() override;
    /**
     * \brief Start the tracking.
     *
     * A new thread is created, which continuously reads the position and orientation information of each tool and stores them inside the tools.
     * Call StopTracking() to stop the tracking thread.
     */
    bool StartTracking() override;
    /**
       * \param toolNumber The number of the tool which should be given back.
       * \return Returns the tool which the number "toolNumber". Returns nullptr, if there is
       * no tool with this number.
       */
    mitk::TrackingTool* GetTool(unsigned toolNumber) const override;
    mitk::TrackingTool* GetToolByName(std::string name) const override;

    RobotTool* GetInternalTool(std::string name);

    mitk::TrackingTool* AddTool(const char* toolName, xyzabc tcp);
    unsigned GetToolCount() const override;
    mitk::Quaternion kukaABC2Quaternion(double a, double b, double c);

    /**
    * \brief TrackTools() continuously polls robotApi interface for new 6d tool positions until StopTracking is called.
    *
    * Continuously tracks the 6D position of all tools until StopTracking() is called.
    * This function is executed by the tracking thread (through StartTracking() and ThreadStartTracking()).
    * It should not be called directly.
    * @throw mitk::IGTHardwareException Throws an exception if there are errors while tracking the tools.
    */
    void TrackTools();

    std::array<double, 6> GetTrackingData();

    //todo robotDevice api, move to RobotDevice abstract class later
    //bool RequestExecOperate(const QString& funname, const QStringList& param);

    void RobotMove(vtkMatrix4x4* T_robot);

    //Robot
    KukaRobotAPI m_RobotApi;
  protected:
    KukaRobotDevice_New();
    virtual ~KukaRobotDevice_New() override;

    /**
      * \brief Add a kuka end effector tool to the list of tracked tools and add tool tcp to robot. This method is used by AddTool
      * @throw mitk::IGTHardwareException Throws an exception if there are errors while adding the tool.
      * \warning adding tools is not possible in tracking mode, only in setup and ready.
      */
    virtual bool InternalAddTool(RobotTool* tool); //todo create new tcp in robot

  // private slots:
  //   void IsRobotConnected(bool isConnect);

  private:
    void ThreadStartTracking();
  private:
    //bool m_IsConnected = false;

    std::array<double, 6> m_TrackingData = {};

    //track
    mutable std::mutex m_ToolsMutex; ///< mutex for coordinated access of tool container
    RobotTools m_RobotTools; ///< container for all tracking tools
    ///< creates tracking thread that continuously polls serial interface for new tracking data
    std::thread m_Thread;                            ///< ID of tracking thread

    
    //Device Configure
    // std::string m_IpAddress{ "172.31.1.148" };
    // QString m_Port{ "30300" };
    // std::string m_RemoteIpAddress{ "172.31.1.147" };
    // QString m_RemotePort{ "30300" };
    std::string m_DeviceName{ "Kuka" };

    //communication
    //UdpSocketRobotHeartbeat m_udp;

  };
}





#endif // KUKAROBOTDEVICE_H
