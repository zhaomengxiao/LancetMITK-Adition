#include "lancetKukaRobotDevice.h"

#include "mitkIGTTimeStamp.h"

#include "lancetKukaTrackingDeviceTypeInformation.h"
#include "mitkIGTException.h"
#include "mitkIGTHardwareException.h"
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
    if (!m_RobotApi.Connect())
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
        robotTool->GetTCP().ToArray(tcp);
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
    m_RobotApi.DisConnect();
    SetState(TrackingDeviceState::Setup);
    return true;
  }

  bool KukaRobotDevice_New::StartTracking()
  {
    if (this->GetState() != Ready)
      return false;

    this->SetState(Tracking);          // go to mode Tracking
    this->m_StopTrackingMutex.lock(); // update the local copy of m_StopTracking
    this->m_StopTracking = false;
    this->m_StopTrackingMutex.unlock();

    m_Thread = std::thread(&KukaRobotDevice_New::ThreadStartTracking, this);//todo start tracking agine crash here 
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

  RobotTool* KukaRobotDevice_New::GetInternalTool(unsigned toolNumber)
  {
    std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
    if (toolNumber < m_RobotTools.size())
      return m_RobotTools.at(toolNumber);
    return nullptr;
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

  unsigned KukaRobotDevice_New::GetToolCount() const
  {
    return static_cast<unsigned int>(this->m_RobotTools.size());
  }

  void KukaRobotDevice_New::TrackTools()
  {
    auto info = m_RobotApi.GetRobotInfo();
    m_TrackingData[0] = info.Flange1;
    m_TrackingData[1] = info.Flange2;
    m_TrackingData[2] = info.Flange3;

    m_TrackingData[3] = info.Flange4;
    m_TrackingData[4] = info.Flange5;
    m_TrackingData[5] = info.Flange6;
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
  }

  bool KukaRobotDevice_New::InternalAddTool(RobotTool* tool)
  {
    MITK_INFO << "InternalAddTool Called!!!!";
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
    MITK_INFO << "tracktools called";
    /* lock the TrackingFinishedMutex to signal that the execution rights are now transfered to the tracking thread */
    // keep lock until end of scope
    std::lock_guard<std::mutex> lock(m_TrackingFinishedMutex);
    if (this->GetState() != Tracking)
    {
      MITK_INFO << "TrackTools Return: state not tracking";
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
      auto info = m_RobotApi.GetRobotInfo();
      m_TrackingData[0] = info.Flange1;
      m_TrackingData[1] = info.Flange2;
      m_TrackingData[2] = info.Flange3;

      m_TrackingData[3] = info.Flange4;
      m_TrackingData[4] = info.Flange5;
      m_TrackingData[5] = info.Flange6;

      Eigen::Quaterniond q;
      //kuka
      Eigen::AngleAxisd rx(m_TrackingData[3] , Eigen::Vector3d::UnitZ());
      Eigen::AngleAxisd ry(m_TrackingData[4] , Eigen::Vector3d::UnitY());
      Eigen::AngleAxisd rz(m_TrackingData[5] , Eigen::Vector3d::UnitX());
      //return rz.matrix()*ry.matrix()*rx.matrix(); // kuka

      q = rx * ry * rz;
      Eigen::Matrix3d rotate;
      q = rotate;
      for (auto tool : m_RobotTools)
      {
        mitk::Quaternion quaternion{ q.x(),q.y(),q.z(),q.w() };
        tool->SetOrientation(quaternion);
        mitk::Point3D position;
        position[0] = m_TrackingData[0];
        position[1] = m_TrackingData[1];
        position[2] = m_TrackingData[2];

        tool->SetPosition(position);
        tool->SetTrackingError(0);
        tool->SetErrorMessage("");
        //tool->SetFrameNumber(/*toolData.frameNumber*/); //todo add frameNumber in robot data for debug and frame-rate count ;
        tool->SetDataValid(true);
      }
      /* Update the local copy of m_StopTracking */
      this->m_StopTrackingMutex.lock();
      localStopTracking = m_StopTracking;
      this->m_StopTrackingMutex.unlock();
    }
    /* StopTracking was called, thus the mode should be changed back to Ready now that the tracking loop has ended. */

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
}

