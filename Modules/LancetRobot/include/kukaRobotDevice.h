#ifndef KUKAROBOTDEVICE_H
#define KUKAROBOTDEVICE_H
//mitk
#include <mitkCommon.h>
#include <mitkTrackingDevice.h>

//KUKA ROBOT API
#include "MitkLancetRobotExports.h"
#include "robotapi.h"

//qt
#include <QObject>
#include <QThread>
#include <QMetaType>
#include <QPointer>


/** Documentation
  * \brief superclass for specific KUKA Robot Devices that use socket communication.
  *
  * implements the TrackingDevice interface for Kuka robot devices ()
  *
  * \ingroup Robot
 */
class MITKLANCETROBOT_EXPORT  KukaRobotDevice :public QObject,public  mitk::TrackingDevice
{
	Q_OBJECT
public:
	mitkClassMacro(KukaRobotDevice, TrackingDevice);
	itkFactorylessNewMacro(Self);
	itkCloneMacro(Self);

	itkGetMacro(IsConnected, bool);


	bool OpenConnection() override;

	bool CloseConnection() override;

	bool StartTracking() override;

	mitk::TrackingTool* GetTool(unsigned toolNumber) const override;

	unsigned GetToolCount() const override;

	void TrackTools();

	std::array<double,6> GetTrackingData();

private slots:
	void IsRobotConnected(bool isConnect);

private:
	static void heartbeatThreadWorker(KukaRobotDevice* _this);

private:
	bool m_IsConnected = false;
	TrackingDeviceState m_State = Setup;///< current object state (Setup, Ready or Tracking)
	RobotApi m_robotApi;
	QPointer<QThread> m_heartbeat;
	std::array<double, 6> m_trackingData = {};
};



#endif // KUKAROBOTDEVICE_H
