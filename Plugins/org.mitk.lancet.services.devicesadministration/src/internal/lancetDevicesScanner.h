#ifndef LancetDevicesScanner_H
#define LancetDevicesScanner_H

#define MITK_BUILD_TESTTING
#include <lancetIDevicesScanner.h>

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
	DevicesScanner: public IDevicesScanner
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::DevicesScanner);
	DevicesScanner();

public:
	virtual void ConnectNDI() override;
	virtual void DisConnectNDI() override;

	virtual void ConnectRobot() override;
	virtual void DisConnectRobot() override;

	virtual void ConnectVirtualDeviceNDI() override;
	virtual void DisConnectVirtualDeviceNDI() override;

	virtual void ConnectVirtualDeviceRobot() override;
	virtual void DisConnectVirtualDeviceRobot() override;

protected:
	lancet::NDIVegaTrackingDevice::Pointer vegaTrackingDevice;
	lancet::KukaRobotDevice::Pointer m_KukaTrackingDevice;
	mitk::TrackingDeviceSource::Pointer m_VegaSource;
	mitk::TrackingDeviceSource::Pointer m_KukaSource;
	lancet::NavigationObjectVisualizationFilter::Pointer m_KukaVisualizer;
	lancet::NavigationObjectVisualizationFilter::Pointer m_VegaVisualizer;
	QTimer* m_KukaVisualizeTimer{ nullptr };
	QTimer* m_VegaVisualizeTimer{ nullptr };
	//mitk::NavigationToolStorage::Pointer m_KukaToolStorage;
	//mitk::NavigationToolStorage::Pointer m_VegaToolStorage;

	//filter test
	mitk::VirtualTrackingDevice::Pointer m_VirtualDevice1;
	mitk::VirtualTrackingDevice::Pointer m_VirtualDevice2;
	mitk::TrackingDeviceSource::Pointer m_VirtualDevice1Source;
	mitk::TrackingDeviceSource::Pointer m_VirtualDevice2Source;
	lancet::NavigationObjectVisualizationFilter::Pointer m_VirtualDevice1Visualizer;
	lancet::NavigationObjectVisualizationFilter::Pointer m_VirtualDevice2Visualizer;
	QTimer* m_VirtualDevice1Timer{ nullptr };
	QTimer* m_VirtualDevice2Timer{ nullptr };
	//mitk::NavigationToolStorage::Pointer m_VirtualDevice1ToolStorage;
	//mitk::NavigationToolStorage::Pointer m_VirtualDevice2ToolStorage;

private:
	struct DevicesScannerPrivateImp;
	std::shared_ptr<DevicesScannerPrivateImp> imp;
};
}

#endif // !LancetDevicesScanner_H