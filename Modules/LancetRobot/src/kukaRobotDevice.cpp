#include "kukaRobotDevice.h"

bool KukaRobotDevice::OpenConnection()
{
	connect(&m_robotApi, SIGNAL(signal_api_isRobotConnected(bool)),
		this, SLOT(IsRobotConnected(bool)));

	m_robotApi.connectrobot();

	return m_IsConnected;
}

bool KukaRobotDevice::CloseConnection()
{
	m_robotApi.disconnectrobot();

	return !m_IsConnected;
}

bool KukaRobotDevice::StartTracking()
{
	return false;
}

mitk::TrackingTool* KukaRobotDevice::GetTool(unsigned toolNumber) const
{
	return nullptr;
}

unsigned KukaRobotDevice::GetToolCount() const
{
	return 0;
}

void KukaRobotDevice::TrackTools()
{
	m_robotApi.requestrealtimedata();
	m_trackingData[0] = m_robotApi.realtime_data.pose.x;
	m_trackingData[1] = m_robotApi.realtime_data.pose.y;
	m_trackingData[2] = m_robotApi.realtime_data.pose.z;

	m_trackingData[3] = m_robotApi.realtime_data.pose.a;
	m_trackingData[4] = m_robotApi.realtime_data.pose.b;
	m_trackingData[5] = m_robotApi.realtime_data.pose.c;
}

std::array<double, 6> KukaRobotDevice::GetTrackingData()
{
	return m_trackingData;
}

bool KukaRobotDevice::RequestExecOperate(const QString& funname, const QStringList& param)
{
  qWarning() << "ExecOperateFunc " << funname << ", ParamLisr " << param;
  if (funname.indexOf("movep") != -1 && param.size() == 6)
  {
    QThread::msleep(150);
    this->m_robotApi.movep(param.at(0).toDouble(),
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
    this->m_robotApi.movej(param.at(0).toDouble(),
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
    this->m_robotApi.movel(param.at(0).toDouble(),
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
    this->m_robotApi.setworkmode(param.at(0).toInt());
    return true;
  }
  else if (funname.indexOf("setTcpNum") != -1 && param.size() == 2)
  {
    QThread::msleep(300);
    this->m_robotApi.setTcpNum(param.at(0).toInt(), param.at(1).toInt());
    return true;
  }
  else if (funname.indexOf("setio") != -1 && param.size() == 2)
  {
    this->m_robotApi.setio(param.at(0).toInt(), param.at(1).toInt());
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

void KukaRobotDevice::IsRobotConnected(bool isConnect)
{
	m_IsConnected = isConnect;
	if (isConnect)
	{
		m_State = TrackingDeviceState::Ready;

		m_heartbeat = QThread::create(heartbeatThreadWorker, this);
		m_heartbeat->start();
	}
	else
	{
		m_State = TrackingDeviceState::Setup;
	}
}

void KukaRobotDevice::heartbeatThreadWorker(KukaRobotDevice* _this)
{
	while (_this->m_IsConnected == true)
	{
		_this->m_robotApi.setspeed(50);
		QThread::msleep(1000);
	}
}
