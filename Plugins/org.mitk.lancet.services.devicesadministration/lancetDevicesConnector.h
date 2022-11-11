#ifndef LancetDevicesConnector_H
#define LancetDevicesConnector_H

#include <lancetIDevicesConnector.h>

//IGT
#include "kukaRobotDevice.h"
#include "mitkVirtualTrackingDevice.h"
#include "mitkVirtualTrackingTool.h"
#include "lancetNavigationObjectVisualizationFilter.h"
#include "lancetApplyDeviceRegistratioinFilter.h"
#include "lancetApplySurfaceRegistratioinFilter.h"
#include "lancetTrackingDeviceSourceConfigurator.h"
#include "lancetPathPoint.h"
#include "mitkTrackingDeviceSource.h"
#include "lancetVegaTrackingDevice.h"
#include <mitkNavigationDataToPointSetFilter.h>
#include "mitkNavigationToolStorageDeserializer.h"
#include <QtWidgets\qfiledialog.h>
#include "mitkIGTIOException.h"
#include "mitkNavigationToolStorageSerializer.h"
#include "QmitkIGTCommonHelper.h"
namespace lancet
{
class ORG_MITK_LANCET_SERVICES_DEVICESADMINISTRATION_PLUGIN
	DevicesConnector: public IDevicesConnector
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::DevicesConnector);
	DevicesConnector();

public:

	virtual bool RegisterDevice(const std::string&);
	//virtual bool UnRegisterDevice(const std::string&);

	virtual bool IsConnect(const std::string& name) const;

	virtual mitk::TrackingDevice::Pointer GetDevice(const std::string& name) const;
	virtual mitk::TrackingDeviceSource::Pointer GetDeviceSource(const std::string& name) const;

public:
	virtual bool IsRegisterDevice(const std::string&) const;

	virtual void ConnectDevice() override;

protected:
	virtual bool RegisterVegaTrackingDevice(const std::string&);
	virtual bool RegisterRobotTrackingDevice(const std::string&);

Q_SIGNALS:
	void DeviceConnect_chnage(std::string, bool);

protected Q_SLOTS:
	void func_sanf();
private:
	struct DevicesConnectorPrivateImp;
	std::shared_ptr<DevicesConnectorPrivateImp> imp;
};
}

#endif // !LancetDevicesConnector_H