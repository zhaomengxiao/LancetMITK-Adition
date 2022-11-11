/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief A Connector abstract object that declares patient data.
 * \ingroup org_mitk_lancet_services_Devicesadministration
 * \version V1.0.0
 * \data 2022-10-11 10:43:35
 * 
 * \par Modify History
 *   -# Create Initialize Version
 * 
 * \author Sun
 * \data 2022-10-11 10:43:35
 * \remark Nothing.
 */
#ifndef LancetITrackingDeviceManage_H
#define LancetITrackingDeviceManage_H
 // c or c++
#include <memory>

// berry
#include <berryObject.h>

// itk
#include <itkSmartPointer.h>

// qt
#include <QPair>
#include <QVector>
#include <QSharedPointer>

// ORG_MITK_LANCET_SERVICES_DEVICESADMINISTRATION_PLUGIN
#include "org_mitk_lancet_services_devicesadministration_Export.h"

namespace mitk 
{
	class NavigationToolStorage;
	class TrackingDeviceSource;
	class VirtualTrackingDevice;
}

namespace lancet
{
	class NDIVegaTrackingDevice;
	class KukaRobotDevice;

class ORG_MITK_LANCET_SERVICES_DEVICESADMINISTRATION_PLUGIN
	ITrackingDeviceManage : public QObject, public berry::Object
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::ITrackingDeviceManage);
	ITrackingDeviceManage();

	virtual void SetNDIToolStorage(itk::SmartPointer<mitk::NavigationToolStorage>);
	virtual itk::SmartPointer<mitk::NavigationToolStorage> GetNDIToolStorage() const;

	virtual void SetRobotToolStorage(itk::SmartPointer<mitk::NavigationToolStorage>);
	virtual itk::SmartPointer<mitk::NavigationToolStorage> GetRobotToolStorage() const;
	
	virtual void SetRobotStatus(int);
	virtual int GetRobotStatus() const;

	virtual void SetNDIStatus(int);
	virtual int GetNDIStatus() const;

	virtual void ConnectDevice() = 0;

Q_SIGNALS:
	void IDevicesGetStatus();

private:
	struct ITrackingDeviceManagePrivateImp;
	std::shared_ptr<ITrackingDeviceManagePrivateImp> imp;
};
} // namespace lancet
#endif // !LancetITrackingDeviceManage_H
