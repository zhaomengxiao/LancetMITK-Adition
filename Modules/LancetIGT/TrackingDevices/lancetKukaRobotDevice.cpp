#include "lancetKukaRobotDevice.h"

#include "lancetKukaTrackingDeviceTypeInformation.h"
#include "mitkIGTException.h"
#include "mitkIGTHardwareException.h"
#include "mitkIGTTimeStamp.h"
#include "kukaRobotAPI/robotInfoProtocol.h"

#define d2r 57.2957795130

namespace lancet
{
  bool KukaRobotDevice_New::OpenConnection()
  {
    if (this->GetState() != Setup)
    {
      mitkThrowException(mitk::IGTException) << "Can only try to open the connection if in setup mode";
    }
    if (!m_RobotApi.Connect(30009, "172.31.1.148", 30003))
    {
      mitkThrowException(mitk::IGTHardwareException) << "Can not connect to Kuka Robot";
    }
    // todo compare to robot side api revision,if not match throw exception and exit
    MITK_INFO << "Robot API Revision: " << lancet::KukaRobotAPI::GetApiRevision();

    // initialize the tools that were added manually
    {
      std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
      int returnValue;
      for (RobotTool* robotTool : m_RobotTools)
      {
        // add tool tcp to robot as a frame in Flange coords
        std::array<double, 6> tcp{};
        robotTool->GetTCP().ToArray(tcp); //MITK Point toArray has bug,&
		/*MITK_INFO << "tcp:" << robotTool->GetTCP();
		MITK_INFO << "-tcp:" << tcp[0] << "," << tcp[1] << "," << tcp[2] << "," << tcp[3];*/
        if (!m_RobotApi.AddFrame(robotTool->GetToolName(), tcp))
        {
          mitkThrowException(mitk::IGTHardwareException) << "Add Frame to Robot Failed";
          return false;
        }
      }
    } // end of toolsmutexlockholder scope
    SetState(Ready);
    return true;
  }

  bool KukaRobotDevice_New::CloseConnection()
  {
    if (this->GetState() != Setup)
    {
      //todo init before closing to force robot to clean some stuff and work properly when reconnect
      //m_RobotApi.INIT();
      StopTracking();
      m_RobotApi.DisConnect();/*close the connection*/
      this->InvalidateAllTools();/* invalidate all tools */
      this->SetState(TrackingDeviceState::Setup);/* return to setup mode */
      MITK_INFO << "Robot Device Connection Closed";
      return true;
    }
    MITK_INFO << "Robot Device not in Setup State,CloseConnection() not work.";
    return false;
  }

  bool KukaRobotDevice_New::StartTracking()
  {
    if (this->GetState() != Ready)
      return false;

    this->SetState(Tracking);          // go to mode Tracking
    this->m_StopTrackingMutex.lock(); // update the local copy of m_StopTracking
    this->m_StopTracking = false;
    this->m_StopTrackingMutex.unlock();

    this->m_Thread = std::thread(&KukaRobotDevice_New::ThreadStartTracking, this);//todo start tracking agine crash here 
	//thread.detach();
    // start a new thread that executes the TrackTools() method
    mitk::IGTTimeStamp::GetInstance()->Start(this);
    MITK_INFO << "lancet kuka robot start tracking";
    return true;
  }

  mitk::TrackingTool* KukaRobotDevice_New::GetTool(unsigned toolNumber) const
  {
    std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
    if (toolNumber < m_RobotTools.size())
      return m_RobotTools.at(toolNumber);
    return nullptr;
  }

  mitk::TrackingTool* KukaRobotDevice_New::GetToolByName(std::string name) const
  {
    std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
    auto end = m_RobotTools.end();
    for (auto iterator = m_RobotTools.begin(); iterator != end; ++iterator)
      if (name.compare((*iterator)->GetToolName()) == 0)
        return *iterator;
    return nullptr;
  }

  RobotTool* KukaRobotDevice_New::GetInternalTool(std::string name)
  {
    std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
    auto end = m_RobotTools.end();
    for (auto iterator = m_RobotTools.begin(); iterator != end; ++iterator)
      if (name == (*iterator)->GetToolName())
        return *iterator;
    return nullptr;
  }

  void KukaRobotDevice_New::InvalidateAllTools()
  {
    std::lock_guard<std::mutex> lock(m_ToolsMutex);
    auto end = m_RobotTools.end();
    for (auto iterator = m_RobotTools.begin(); iterator != end; ++iterator)
      (*iterator)->SetDataValid(false);
  }

  mitk::TrackingTool* KukaRobotDevice_New::AddTool(const char* toolName, xyzabc tcp)
  {
    RobotTool::Pointer t = RobotTool::New();
    t->SetToolName(toolName);
    t->SetTCP(tcp);
    if (this->InternalAddTool(t) == false)
    {
      MITK_ERROR << "InternalAddTool failed";
      return nullptr;
    }

    return t.GetPointer();
  }

  // bool KukaRobotDevice_New::RemoveTool(mitk::TrackingTool* tool)
  // {
  //   return true;
  //   auto robotTool = dynamic_cast<RobotTool*>(tool);
  //   if (robotTool == nullptr)
  //     return false;
  //   
  //   std::string toolName = robotTool->GetToolName();
  //   /* a valid portHandle has length 2. If a valid handle exists, the tool is already added to the tracking device, so we have to remove it there
  //   if the connection to the tracking device has already been established.
  //   */
  //   if ((portHandle.length() == 2) && (this->GetState() == Ready))  // do not remove a tool in tracking mode
  //   {
  //     mitk::NDIErrorCode returnvalue;
  //     returnvalue = m_DeviceProtocol->PHF(&portHandle);
  //     if (returnvalue != mitk::NDIOKAY)
  //       return false;
  //     /* Now that the tool is removed from the tracking device, remove it from our tool list too */
  //     std::lock_guard<std::mutex> lock(m_ToolsMutex);
  //     auto end = m_6DTools.end();
  //     for (auto iterator = m_6DTools.begin(); iterator != end; ++iterator)
  //     {
  //       if (iterator->GetPointer() == ndiTool)
  //       {
  //         m_6DTools.erase(iterator);
  //         this->Modified();
  //         return true;
  //       }
  //     }
  //     return false;
  //   }
  //   else if (this->GetState() == Setup)  // in Setup Mode, we are not connected to the tracking device, so we can just remove the tool from the tool list
  //   {
  //     std::lock_guard<std::mutex> lock(m_ToolsMutex);
  //     auto end = m_6DTools.end();
  //     for (auto iterator = m_6DTools.begin(); iterator != end; ++iterator)
  //     {
  //       if ((*iterator).GetPointer() == ndiTool)
  //       {
  //         m_6DTools.erase(iterator);
  //         this->Modified();
  //         return true;
  //       }
  //     }
  //     return false;
  //   }
  //   return false;
  // }

  unsigned KukaRobotDevice_New::GetToolCount() const
  {
    return static_cast<unsigned int>(this->m_RobotTools.size());
  }

  mitk::Quaternion KukaRobotDevice_New::kukaABC2Quaternion(double a,double b,double c)
  {
    Eigen::AngleAxisd rx(a, Eigen::Vector3d::UnitZ());
    Eigen::AngleAxisd ry(b, Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd rz(c, Eigen::Vector3d::UnitX());
    Eigen::Quaterniond q = rx * ry * rz;

    return { q.x(),q.y(),q.z(),q.w() };
  }

  void KukaRobotDevice_New::TrackTools()
  {
    MITK_INFO << "tracktools called";
	this->m_isRunningTrackingTool = true;
    /* lock the TrackingFinishedMutex to signal that the execution rights are now transfered to the tracking thread */
    // keep lock until end of scope
    std::lock_guard<std::mutex> lock(m_TrackingFinishedMutex);
    if (this->GetState() != Tracking)
    {
      MITK_WARN << "TrackTools Return: state not tracking";
      return;
    }

    bool localStopTracking;
    // Because m_StopTracking is used by two threads, access has to be guarded by a mutex. To minimize thread locking, a local copy is used here
    this->m_StopTrackingMutex.lock(); // update the local copy of m_StopTracking
    localStopTracking = this->m_StopTracking;
    this->m_StopTrackingMutex.unlock();

    while ((this->GetState() == Tracking) && (localStopTracking == false))
    {
      //robotAPI GetRobotInfo return flange xyzabc in Robot Base Coords
      auto frames = m_RobotApi.GetRobotInfo().frames;
      if(frames.empty())
      {
        MITK_WARN << "Tracking Loop Start, but tracking frames empty,check if the tool added successfully?";
        break;
      }

      for (auto frame : frames)
      {
        lancet::RobotTool::Pointer tool = GetInternalTool(frame.name);
        // mitk::Quaternion quaternion{ toolData.transform.qx, toolData.transform.qy, toolData.transform.qz,
        //                             toolData.transform.q0 };
		if (tool.IsNotNull())
		{
			mitk::Quaternion q = kukaABC2Quaternion(frame.position[3], frame.position[4], frame.position[5]);
			tool->SetOrientation(q);
			mitk::Point3D position;
			position[0] = frame.position[0];
			position[1] = frame.position[1];
			position[2] = frame.position[2];

			tool->SetPosition(position);
			//todo more tool imformation to be provide
			//tool->SetTrackingError(toolData.transform.error);
			//tool->SetErrorMessage("");
			//tool->SetFrameNumber(toolData.frameNumber);
			tool->SetDataValid(true);
		}
       
      }
      
      /* Update the local copy of m_StopTracking */
      this->m_StopTrackingMutex.lock();
      localStopTracking = m_StopTracking;
      this->m_StopTrackingMutex.unlock();
    }
	this->m_isRunningTrackingTool = false;
    /* StopTracking was called, thus the mode should be changed back to Ready now that the tracking loop has ended. */
    //todo robotAPI stop tracking support
    // returnvalue = m_capi.stopTracking();
    // if (warningOrError(returnvalue, "m_capi.stopTracking()"))
    // {
    //   MITK_INFO << "m_capi.stopTracking() err";
    //   return;
    // }
    // MITK_INFO << "m_capi stopTracking()";
    return;
    // returning from this function (and ThreadStartTracking()) this will end the thread and transfer control back to main thread by releasing trackingFinishedLockHolder
  }

  std::array<double, 6> KukaRobotDevice_New::GetTrackingData()
  {
    return m_TrackingData;
  }

  // bool KukaRobotDevice_New::RequestExecOperate(const QString& funname, const QStringList& param)
  // {
  //   return false;
  // }

  void KukaRobotDevice_New::RobotMove(vtkMatrix4x4* T_robot)
  {
    m_RobotApi.MovePTP(T_robot);
  }

  bool KukaRobotDevice_New::SafeWaitThreadQuit(int waitTime)
  {
	  clock_t startTime = clock();
	  while (this->m_isRunningTrackingTool)
	  {
		  if ((clock() - startTime) >= waitTime)
		  {
			  return false;
		  }
		  std::this_thread::sleep_for(std::chrono::milliseconds(50));
	  }
	  return true;
  }

  KukaRobotDevice_New::KukaRobotDevice_New()
    :TrackingDevice()
  {
    m_Data = lancet::KukaRobotTypeInformation::GetDeviceDataLancetKukaTrackingDevice();
    // m_RobotTools.clear();
    // ////udp service
    // m_udp.setRepetitiveHeartbeatInterval(500);
    // m_udp.setRemoteHostPort(m_RemotePort.toUInt());
    // m_udp.setRemoteHostAddress(QString::fromStdString(m_RemoteIpAddress));
    // if (!m_udp.bind(QHostAddress(QString::fromStdString(m_IpAddress)), m_Port.toInt()))
    // {
    //   MITK_ERROR << QString("bind to %1:%2 error!- %3").arg(QString::fromStdString(m_IpAddress)).arg(m_Port.toInt()).arg(m_udp.error()).toStdString();
    // }
    // MITK_INFO << QString("bind udp %1:%2 at fps:%3").arg(QString::fromStdString(m_IpAddress)).arg(m_Port.toInt()).arg(m_udp.repetitiveHeartbeatInterval()).toStdString();
    // m_udp.startRepetitiveHeartbeat();
    //
    // connect(&m_RobotApi, SIGNAL(signal_api_isRobotConnected(bool)),
    //   this, SLOT(IsRobotConnected(bool)));
  }

  KukaRobotDevice_New::~KukaRobotDevice_New()
  {
	  //m_udp.disconnect();
	  this->StopTracking();
	  this->CloseConnection();
	  while (!this->SafeWaitThreadQuit())
	  {
		  std::this_thread::sleep_for(std::chrono::milliseconds(100));
	  }
	  this->m_Thread.join();
  }

  bool KukaRobotDevice_New::InternalAddTool(RobotTool* tool)
  {
    MITK_INFO << "[RobotTool Added]";
    if (tool == nullptr)
      return false;
    RobotTool::Pointer p = tool;
    //p->Enable();
    int returnValue;
    /* if the connection to the tracking device is already established, add the new tool to the device now */
    if (this->GetState() == Ready)
    {
      MITK_INFO << "State Ready";
      // add tool tcp to robot as a frame in Flange coords
      std::array<double, 6> tcp{};
      p->GetTCP().ToArray(tcp);
      if (!m_RobotApi.AddFrame(p->GetToolName(), tcp))
      {
        MITK_ERROR << "Add Frame to Robot Failed";
        return false;
      }
      /* now that the tool is added to the device, add it to list too */
      m_ToolsMutex.lock();
      this->m_RobotTools.push_back(p);
      m_ToolsMutex.unlock();
      this->Modified();
      return true;
    }
    else if (this->GetState() == Setup)
    {
      MITK_INFO << "State Setup";
      /* In Setup mode, we only add it to the list, so that OpenConnection() can add it later */
      m_ToolsMutex.lock();
      this->m_RobotTools.push_back(p);
      m_ToolsMutex.unlock();
      this->Modified();
      return true;
    }
    else // in Tracking mode, no tools can be added
      return false;
  }

  // void KukaRobotDevice_New::IsRobotConnected(bool isConnect)
  // {
  //   isConnect = true;
  // }

  void KukaRobotDevice_New::ThreadStartTracking()
  {
    TrackTools();
  }
}

