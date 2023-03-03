#ifndef KUKAROBOTDEVICE_H
#define KUKAROBOTDEVICE_H

#include <mitkCommon.h>
#include <mitkTrackingDevice.h>
#include <mitkTrackingTool.h>
#include "lancetRobotTrackingTool.h"
#include "MitkLancetIGTExports.h"
#include "kukaRobotAPI/kukaRobotAPI.h"


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
  class MITKLANCETIGT_EXPORT KukaRobotDevice_New : public mitk::TrackingDevice
  {
  public:
    mitkClassMacro(KukaRobotDevice_New, TrackingDevice);
    itkFactorylessNewMacro(Self);
    itkCloneMacro(Self);

    typedef std::vector<RobotTool::Pointer> RobotTools;

    //itkGetMacro(IsConnected, bool);
    /**
       * @brief Opens the connection to the device. This have to be done before the tracking is started.
       *
       * @details OpenConnection() establishes the connection to the kuka robot device by calling RobotAPI:
       * - establish a tcp connection to send robot command and receive result(Port 30009)
       * - Send heartbeat command through TCP to maintain connection with kuka robot application (every second);
       * - establish a udp connection to receive robot information,like joint position,tool position,force&torque...
       * - initializing all manually added tools (load tools from ToolStorage with TCP pre set,in kuka app its frames on tool)
       *
       * @return True, if connection success
       *
       * @throw mitk::IGTHardwareException Throws an exception if there are errors while connecting to the device.
       * @throw mitk::IGTException Throws a normal IGT exception if an error occures which is not related to the hardware.
       */
    bool OpenConnection() override;
    /**
       * @brief Closes the connection.
       *
       * @details CloseConnection() resets the tracking device, invalidates all tools and then closes tcp udp connection.
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

    unsigned GetToolCount() const override;


    /**
     * \brief Create a robot tool and pass it to InternalAddTool() method.
     * \param toolName name of the tool,in robot application use name to identify which frame( =tool here) to operate
     * \param tcp Transform from Flange to tool,xyz is the translate,abc rotate Z->Y-> X axis
     * \return pointer of the created tool
     *
     * @throw mitk::IGTHardwareException Throws an exception if there are errors while adding the tool.
     *
     * \warning adding tools is not possible in tracking mode, only in setup and ready.
     */
    mitk::TrackingTool* AddTool(const char* toolName, xyzabc tcp);

    /**
      *\brief Remove a passive 6D tool from the list of tracked tools.
      *
      *\warning removing tools is not possible in tracking mode, only in setup and ready modes.
      *
      */
    //todo virtual bool RemoveTool(mitk::TrackingTool* tool);

    /**
    * \brief reloads and reinitializes the tool
    */
    //todo virtual bool UpdateTool(mitk::TrackingTool* tool);

    static mitk::Quaternion kukaABC2Quaternion(double a, double b, double c);

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
	bool SafeWaitThreadQuit(int = 1000);

    KukaRobotDevice_New();
    virtual ~KukaRobotDevice_New() override;

    /**
      * \brief Add a kuka robot tool to the list of tracked tools and add tool tcp to robot. This method is used by AddTool
      * @throw mitk::IGTHardwareException Throws an exception if there are errors while adding the tool.
      * \warning adding tools is not possible in tracking mode, only in setup and ready.
      */
    virtual bool InternalAddTool(RobotTool* tool);

    RobotTool* GetInternalTool(std::string name);
    virtual void InvalidateAllTools(); ///< invalidate all tools

    // private slots:
    //   void IsRobotConnected(bool isConnect);

  private:
    void ThreadStartTracking();
  private:
    //bool m_IsConnected = false;

    std::array<double, 6> m_TrackingData = {};

    //track
    mutable std::mutex m_ToolsMutex; ///< mutex for coordinated access of tool container
    RobotTools m_RobotTools;
    ///< container for all tracking tools
       ///< creates tracking thread that continuously polls serial interface for new tracking data
	std::atomic_bool m_isRunningTrackingTool = false;
    std::thread m_Thread; ///< ID of tracking thread


    //Device Configure
    // std::string m_IpAddress{ "172.31.1.148" };
    // QString m_Port{ "30300" };
    // std::string m_RemoteIpAddress{ "172.31.1.147" };
    // QString m_RemotePort{ "30300" };
    std::string m_DeviceName{"Kuka"};

    //communication
    //UdpSocketRobotHeartbeat m_udp;
  };
}


#endif // KUKAROBOTDEVICE_H
