/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief $Document description$
 * \ingroup org_mitk_lancet_services_devicesadministration
 * \version V1.0.0
 * \data 2022-10-11 10:39:19
 * 
 * \par Modify History
 *
 * \author Sun
 * \data 2022-10-11 10:39:19
 * \remark $Modify Description$
 */
#ifndef LancetIDevicesAdministrationService_H
#define LancetIDevicesAdministrationService_H

// C++
#include <memory>

// berry
#include <berryObject.h>

// Qt
#include <QObject>

// ORG_MITK_LANCET_SERVICES_DEVICESADMINISTRATION_PLUGIN
#include "org_mitk_lancet_services_devicesadministration_Export.h"

namespace lancet
{
/**
	* \class lancetIDevicesAdministrationService
	* \ingroup org_mitk_lancet_services_devicesadministration
	* \namespace lancet
	* \brief 
	*
	* TODO: long description
	*
	* \pre $Pre description information$
	* \example $Description of use examples$
	* \code
	*	todo...
	* \endcoed
	*
	* \author Sun
	* \version V1.0.0
	* \date 2022-10-11 10:39:35
	* \remark todo: insert comments
	*
	* Contact: sh4a01@163.com
	*
	*/
class TrackingDeviceManage;
class ORG_MITK_LANCET_SERVICES_DEVICESADMINISTRATION_PLUGIN
	IDevicesAdministrationService: public QObject, public berry::Object
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::IDevicesAdministrationService);
	IDevicesAdministrationService();

	static berry::SmartPointer<IDevicesAdministrationService> GetService();

	virtual void Start() = 0;
	virtual void Stop() = 0;

	virtual void SetConnector(berry::SmartPointer<TrackingDeviceManage>);
	virtual berry::SmartPointer<TrackingDeviceManage> GetConnector() const;

Q_SIGNALS:
	void IDevicesGetStatus();

protected Q_SLOTS:
	void Slot_IDevicesGetStatus();

private:
	virtual void ConnectToConnector(const berry::SmartPointer<TrackingDeviceManage>&) const;
	virtual void DisConnectToConnector(const berry::SmartPointer<TrackingDeviceManage>&) const;

	struct IDevicesAdministrationServicePrivateImp;
	std::shared_ptr<IDevicesAdministrationServicePrivateImp> imp;
};
}
Q_DECLARE_INTERFACE(lancet::IDevicesAdministrationService, "org.mitk.lancet.services.IDevicesAdministrationService")
#endif // !LancetIDevicesAdministrationService_H
