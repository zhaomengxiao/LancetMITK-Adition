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
