#include "kukaRobotDevice.h"

#include "mitkIGTTimeStamp.h"

#include "lancetKukaTrackingDeviceTypeInformation.h"

#define d2r 57.2957795130

namespace lancet
{
  bool KukaRobotDevice::OpenConnection()
  {
    m_RobotApi.connectrobot();


    /*if (GetState() == Ready)
    {
      MITK_WARN << "Ready after m_RobotApi.connectrobot();";
    }
    else {
      MITK_WARN << "Setup after m_RobotApi.connectrobot();";
    }*/

    //todo BUG:isRobotConnected will not turn ture immdiately
    //SetState(TrackingDeviceState::Ready);
    /*m_Heartbeat = QThread::create(heartbeatThreadWorker, this);
    m_Heartbeat->start();*/
    return true;
  }

  bool KukaRobotDevice::CloseConnection()
  {
    m_RobotApi.disconnectrobot();
    SetState(TrackingDeviceState::Setup);
    return true;
  }

  bool KukaRobotDevice::StartTracking()
  {
    if (this->GetState() != Ready)
      return false;

    this->SetState(Tracking);          // go to mode Tracking
    this->m_StopTrackingMutex.lock(); // update the local copy of m_StopTracking
    this->m_StopTracking = false;
    this->m_StopTrackingMutex.unlock();

    m_Thread = std::thread(&KukaRobotDevice::ThreadStartTracking, this);//todo start tracking agine crash here 
    // start a new thread that executes the TrackTools() method
    mitk::IGTTimeStamp::GetInstance()->Start(this);
    MITK_INFO << "lancet kuka robot start tracking";
    return true;
  }

  mitk::TrackingTool* KukaRobotDevice::GetTool(unsigned toolNumber) const
  {
    std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
    if (toolNumber < m_KukaEndEffectors.size())
      return m_KukaEndEffectors.at(toolNumber);
    return nullptr;
  }

  mitk::TrackingTool* KukaRobotDevice::GetToolByName(std::string name) const
  {
    std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
    auto end = m_KukaEndEffectors.end();
    for (auto iterator = m_KukaEndEffectors.begin(); iterator != end; ++iterator)
      if (name.compare((*iterator)->GetToolName()) == 0)
        return *iterator;
    return nullptr;
  }

  mitk::TrackingTool* KukaRobotDevice::GetInternalTool()
  {
    return m_KukaEndEffectors[0]; //only 1 end effector tracked;
  }

  mitk::TrackingTool* KukaRobotDevice::AddTool(const char* toolName, const char* fileName)
  {
    mitk::TrackingTool::Pointer t = mitk::TrackingTool::New();
    t->SetToolName(toolName);
    //todo SetToolTipPosition to TCP
    if (this->InternalAddTool(t) == false)
    {
      MITK_ERROR << "InternalAddTool failed";
      return nullptr;
    }

    return t.GetPointer();
  }

  unsigned KukaRobotDevice::GetToolCount() const
  {
    return static_cast<unsigned int>(this->m_KukaEndEffectors.size());
  }

  void KukaRobotDevice::TrackTools()
  {
    m_RobotApi.requestrealtimedata();
    m_TrackingData[0] = m_RobotApi.realtime_data.pose.x;
    m_TrackingData[1] = m_RobotApi.realtime_data.pose.y;
    m_TrackingData[2] = m_RobotApi.realtime_data.pose.z;

    m_TrackingData[3] = m_RobotApi.realtime_data.pose.a;
    m_TrackingData[4] = m_RobotApi.realtime_data.pose.b;
    m_TrackingData[5] = m_RobotApi.realtime_data.pose.c;
  }

  std::array<double, 6> KukaRobotDevice::GetTrackingData()
  {
    return m_TrackingData;
  }

  bool KukaRobotDevice::RequestExecOperate(const QString& funname, const QStringList& param)
  {
    MITK_INFO << "ExecOperateFunc " << funname.toStdString();
    for (auto string : param)
    {
      MITK_INFO << "ParamList " << string.toStdString();
    }
    if (funname.indexOf("movep") != -1 && param.size() == 6)
    {
      QThread::msleep(150);
      this->m_RobotApi.movep(param.at(0).toDouble(),
        param.at(1).toDouble(),
        param.at(2).toDouble(),
        param.at(3).toDouble(),
        param.at(4).toDouble(),
        param.at(5).toDouble());
      return true;
    }
    if (funname.indexOf("movej") != -1 && param.size() == 6)
    {
      QThread::msleep(150);
      this->m_RobotApi.movej(param.at(0).toDouble(),
        param.at(1).toDouble(),
        param.at(2).toDouble(),
        param.at(3).toDouble(),
        param.at(4).toDouble(),
        param.at(5).toDouble());
      return true;
    }
    else if (funname.indexOf("movel") != -1 && param.size() == 6)
    {
      QThread::msleep(150);
      this->m_RobotApi.movel(param.at(0).toDouble(),
        param.at(1).toDouble(),
        param.at(2).toDouble(),
        param.at(3).toDouble(),
        param.at(4).toDouble(),
        param.at(5).toDouble());
      return true;
    }
    else if (funname.indexOf("setworkmode") != -1 && param.size() == 1)
    {
      QThread::msleep(300);
      this->m_RobotApi.setworkmode(param.at(0).toInt());
      return true;
    }
    else if (funname.indexOf("setTcpNum") != -1 && param.size() == 2)
    {
      QThread::msleep(300);
      this->m_RobotApi.setTcpNum(param.at(0).toInt(), param.at(1).toInt());
      return true;
    }
    else if (funname.indexOf("setio") != -1 && param.size() == 2)
    {
      this->m_RobotApi.setio(param.at(0).toInt(), param.at(1).toInt());
      return true;
    }
    else if (funname.indexOf("update") != -1 && param.size() == 0)
    {
      //emit this->(this->devicename(), this->realtimeData());
      MITK_ERROR << "not support";
      return false;
    }
    else if (funname.toLower().indexOf("applytcpvalue") != -1 && param.size() == 6)
    {
      qInfo() << "call movel function " << this->RequestExecOperate("movel", param) << " param " << param;
      QThread::msleep(1000);
      qInfo() << "call setworkmode 1-11 function " << this->RequestExecOperate("setworkmode", { "11" });
      QThread::msleep(1000);
      qInfo() << "call setworkmode 1-5 function " << this->RequestExecOperate("setworkmode", { "5" });
      QThread::msleep(1000);
      return true;
    }
    return false;
  }

  void KukaRobotDevice::RobotMove(vtkMatrix4x4* T_robot)
  {

    const double R[3][3] = { {T_robot->GetElement(0, 0), T_robot->GetElement(0, 1), T_robot->GetElement(0, 2)},
      {T_robot->GetElement(1, 0), T_robot->GetElement(1, 1), T_robot->GetElement(1, 2)},
      {T_robot->GetElement(2, 0), T_robot->GetElement(2, 1), T_robot->GetElement(2, 2) }
    };

    std::vector<double> abc = kukamatrix2angle(R);

    QString str1 = QString::number(T_robot->GetElement(0, 3), 'f', 5);
    QString str2 = QString::number(T_robot->GetElement(1, 3), 'f', 5);
    QString str3 = QString::number(T_robot->GetElement(2, 3), 'f', 5);
    QString str4 = QString::number(abc[0], 'f', 5);
    QString str5 = QString::number(abc[1], 'f', 5);
    QString str6 = QString::number(abc[2], 'f', 5);

    QString param = str1 + "," + str2 + "," + str3 + "," + str4 + "," + str5 + "," + str6;
    MITK_INFO << "Robot move to:" << param.toStdString();
    RequestExecOperate( "movep", param.split(','));
    QThread::msleep(1000);
	RequestExecOperate("setio", { "2","6" });
  }

  std::vector<double> KukaRobotDevice::kukamatrix2angle(const double matrix3x3[3][3])
  {
    double nx = matrix3x3[0][0];
    double ny = matrix3x3[0][1];
    double nz = matrix3x3[0][2];
    double ox = matrix3x3[1][0];
    double oy = matrix3x3[1][1];
    double oz = matrix3x3[1][2];
    double ax = matrix3x3[2][0];
    double ay = matrix3x3[2][1];
    double az = matrix3x3[2][2];

    //«ÛΩ‚Rx°¢Ry°¢Rz
    //call Atan2(ax,sqrt(power(nx,2)+power(ox,2)),ry1)
    //double cry = (double)sqrt(nx * nx + ox * ox);
    double rx, ry, rz, rx2, ry2, rz2, rx_Origin, ry_Origin, rz_Origin;
    double FlagRx = 0.0, FlagRy = 0.0, FlagRz = 0.0;
    ry = (float)atan2(ax, (double)sqrt(nx * nx + ox * ox));
    if (ry == 90)
    {
      rx = 0;
      //call Atan2(ny,-nz,rz1)
      rz = (float)atan2(ny, -nz);
    }
    else if (ry == -90)
    {
      rx = 0;
      rz = (float)atan2(ny, nz);
      //call Atan2(ny,nz,rz1);
    }
    else
    {
      //call Atan2(-ay/cos(ry1),az/cos(ry1),rx1)
      //call Atan2(-ox/cos(ry1),nx/cos(ry1),rz1)
      rx = (float)atan2(-ay / cos(ry), az / cos(ry));
      rz = (float)atan2(-ox / cos(ry), nx / cos(ry));
    }

    //call Atan2(ax,-sqrt(power(nx,2)+power(ox,2)),ry2)
    ry2 = (float)atan2(ax, -(double)sqrt(nx * nx + ox * ox));
    if (ry2 == 90)
    {
      rx2 = 0;
      //call Atan2(ny,-nz,rz1)
      rz2 = (float)atan2(ny, -nz);
    }
    else if (ry2 == -90)
    {
      rx2 = 0;
      rz2 = (float)atan2(ny, nz);
      //call Atan2(ny,nz,rz1);
    }
    else
    {
      //call Atan2(-ay/cos(ry1),az/cos(ry1),rx1)
      //call Atan2(-ox/cos(ry1),nx/cos(ry1),rz1)
      rx2 = (float)atan2(-ay / cos(ry2), az / cos(ry2));
      rz2 = (float)atan2(-ox / cos(ry2), nx / cos(ry2));
    }

    std::cout << "rz11:" << -rz * (180.0f / PI) << "   ry:" << -ry * (180.0f / PI) << "   rx:" << -rx * (180.0f / PI) << "\n";
    std::cout << "rz22:" << -rz2 * (180.0f / PI) << "   ry:" << -ry2 * (180.0f / PI) << "   rx:" << -rx2 * (180.0f / PI) << "\n";
    if (abs(rx) + abs(ry) + abs(rz) <= abs(rx2) + abs(ry2) + abs(rz2))
    {
      rx_Origin = -rx;
      ry_Origin = -ry;
      rz_Origin = -rz;
    }
    else
    {
      rx_Origin = -rx2;
      ry_Origin = -ry2;
      rz_Origin = -rz2;
    }
    std::vector<double> i;


    if (abs(ry * (180.0f / PI)) < 90)
    {
      i.push_back(-rz * (180.0f / PI));
      i.push_back(-ry * (180.0f / PI));
      i.push_back(-rx * (180.0f / PI));
    }
    else
    {
      i.push_back(-rz2 * (180.0f / PI));
      i.push_back(-ry2 * (180.0f / PI));
      i.push_back(-rx2 * (180.0f / PI));
    }
    //MITK_INFO << i[0] << "," << i[1] << "," << i[2];
    return i;
  }

  KukaRobotDevice::KukaRobotDevice()
    :TrackingDevice()
  {
    m_Data = lancet::KukaRobotTypeInformation::GetDeviceDataLancetKukaTrackingDevice();
    m_KukaEndEffectors.clear();
    ////udp service
    m_udp.setRepetitiveHeartbeatInterval(500);
    m_udp.setRemoteHostPort(m_RemotePort.toUInt());
    m_udp.setRemoteHostAddress(QString::fromStdString(m_RemoteIpAddress));
    if (!m_udp.bind(QHostAddress(QString::fromStdString(m_IpAddress)), m_Port.toInt()))
    {
      MITK_ERROR << QString("bind to %1:%2 error!- %3").arg(QString::fromStdString(m_IpAddress)).arg(m_Port.toInt()).arg(m_udp.error()).toStdString();
    }
    MITK_INFO << QString("bind udp %1:%2 at fps:%3").arg(QString::fromStdString(m_IpAddress)).arg(m_Port.toInt()).arg(m_udp.repetitiveHeartbeatInterval()).toStdString();
    m_udp.startRepetitiveHeartbeat();

    connect(&m_RobotApi, SIGNAL(signal_api_isRobotConnected(bool)),
      this, SLOT(IsRobotConnected(bool)));//todo BUGFIX
  }

  KukaRobotDevice::~KukaRobotDevice()
  {
  }

  bool KukaRobotDevice::InternalAddTool(mitk::TrackingTool* tool)
  {
    MITK_INFO << "InternalAddTool Called!!!!";
    if (tool == nullptr)
      return false;
    mitk::TrackingTool::Pointer p = tool;
    //p->Enable();
    int returnValue;
    /* if the connection to the tracking device is already established, add the new tool to the device now */
    if (this->GetState() == Ready)
    {
      MITK_INFO << "State Ready";
      // Request a port handle to load a passive tool into
      //todo add tool tcp to robot
      /* now that the tool is added to the device, add it to list too */
      m_ToolsMutex.lock();
      this->m_KukaEndEffectors.push_back(p);
      m_ToolsMutex.unlock();
      this->Modified();
      return true;
    }
    else if (this->GetState() == Setup)
    {
      MITK_INFO << "State Setup";
      /* In Setup mode, we only add it to the list, so that OpenConnection() can add it later */
      m_ToolsMutex.lock();
      this->m_KukaEndEffectors.push_back(p);
      m_ToolsMutex.unlock();
      this->Modified();
      return true;
    }
    else // in Tracking mode, no tools can be added
      return false;
  }

  void KukaRobotDevice::IsRobotConnected(bool isConnect)
  {
    //m_IsConnected = isConnect;
    if (isConnect)
    {
      SetState(Ready);

      m_Heartbeat = QThread::create(heartbeatThreadWorker, this);
      m_Heartbeat->start();
	  MITK_INFO << "!connect!";
    }
    else
    {
      //SetState(Setup);
		MITK_INFO << "!disconnect!";
    }
  }

  void KukaRobotDevice::heartbeatThreadWorker(KukaRobotDevice* _this)
  {
    while (/*_this->m_IsConnected ==*/ true) //todo bugfix isConnected
    {
      _this->m_RobotApi.setspeed(50);
      QThread::msleep(500);
    }
  }

  void KukaRobotDevice::ThreadStartTracking()
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
      //MITK_INFO << "tracking";
      m_RobotApi.requestrealtimedata();
      m_TrackingData[0] = m_RobotApi.realtime_data.pose.x;
      m_TrackingData[1] = m_RobotApi.realtime_data.pose.y;
      m_TrackingData[2] = m_RobotApi.realtime_data.pose.z;

      m_TrackingData[3] = m_RobotApi.realtime_data.pose.a;
      m_TrackingData[4] = m_RobotApi.realtime_data.pose.b;
      m_TrackingData[5] = m_RobotApi.realtime_data.pose.c;

      Eigen::Quaterniond q;
      //kuka
      Eigen::AngleAxisd rx(m_TrackingData[3] / d2r, Eigen::Vector3d::UnitZ());
      Eigen::AngleAxisd ry(m_TrackingData[4] / d2r, Eigen::Vector3d::UnitY());
      Eigen::AngleAxisd rz(m_TrackingData[5] / d2r, Eigen::Vector3d::UnitX());
      //return rz.matrix()*ry.matrix()*rx.matrix(); // kuka

      //Staubli
      // Eigen::AngleAxisd rx(rxyz(0, 0) / d2r, Eigen::Vector3d::UnitX());
      // Eigen::AngleAxisd ry(rxyz(1, 0) / d2r, Eigen::Vector3d::UnitY());
      // Eigen::AngleAxisd rz(rxyz(2, 0) / d2r, Eigen::Vector3d::UnitZ());
      //return rx.matrix()*ry.matrix()*rz.matrix(); // staubli
      q = rx * ry * rz;

      mitk::TrackingTool::Pointer tool = GetInternalTool();
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

