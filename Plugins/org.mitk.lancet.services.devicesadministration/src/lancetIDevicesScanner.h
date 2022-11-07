/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief A scanner abstract object that declares patient data.
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
#ifndef LancetIDevicesScanner_H
#define LancetIDevicesScanner_H
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

class QDir;

namespace mitk 
{
	class NavigationToolStorage;
}

namespace lancet
{
class ORG_MITK_LANCET_SERVICES_DEVICESADMINISTRATION_PLUGIN
	IDevicesScanner : public QObject, public berry::Object
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::IDevicesScanner);
	IDevicesScanner();

	virtual void SetNDIToolStorage(itk::SmartPointer<mitk::NavigationToolStorage>);
	virtual itk::SmartPointer<mitk::NavigationToolStorage> GetNDIToolStorage() const;

	virtual void SetRobotToolStorage(itk::SmartPointer<mitk::NavigationToolStorage>);
	virtual itk::SmartPointer<mitk::NavigationToolStorage> GetRobotToolStorage() const;

	virtual void SetRobotStatus(int);
	virtual int GetRobotStatus() const;

	virtual void SetNDIStatus(int);
	virtual int GetNDIStatus() const;

	virtual void ConnectNDI() = 0;
	virtual void ConnectRobot() = 0;
	virtual void DisConnectNDI() = 0;
	virtual void DisConnectRobot() = 0;

	virtual void ConnectVirtualDeviceNDI() = 0;
	virtual void ConnectVirtualDeviceRobot() = 0;
	virtual void DisConnectVirtualDeviceNDI() = 0;
	virtual void DisConnectVirtualDeviceRobot() = 0;
Q_SIGNALS:
	void IDevicesGetStatus();
private:
	struct IDevicesScannerPrivateImp;
	std::shared_ptr<IDevicesScannerPrivateImp> imp;
};
} // namespace lancet
#endif // !LancetIDevicesScanner_H
