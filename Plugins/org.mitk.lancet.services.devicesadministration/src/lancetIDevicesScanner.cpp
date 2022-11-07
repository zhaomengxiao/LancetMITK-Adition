#include "lancetIDevicesScanner.h"

#include <QDir>

#include <mitkNavigationToolStorage.h>

namespace lancet
{
	struct IDevicesScanner::IDevicesScannerPrivateImp
	{
		int robotStatus;
		int ndiStatus;
		mitk::NavigationToolStorage::Pointer m_KukaToolStorage;
		mitk::NavigationToolStorage::Pointer m_VegaToolStorage;
	};
	IDevicesScanner::IDevicesScanner()
		: imp(std::make_shared<IDevicesScannerPrivateImp>())
	{
	}
	void IDevicesScanner::SetNDIToolStorage(itk::SmartPointer<mitk::NavigationToolStorage> m_ToolStorage)
	{
		this->imp->m_VegaToolStorage = m_ToolStorage;
	}
	itk::SmartPointer<mitk::NavigationToolStorage>  IDevicesScanner::GetNDIToolStorage() const
	{
		return this->imp->m_VegaToolStorage;
	}
	void IDevicesScanner::SetRobotToolStorage(itk::SmartPointer<mitk::NavigationToolStorage> m_ToolStorage)
	{
		this->imp->m_KukaToolStorage = m_ToolStorage;
	}
	itk::SmartPointer<mitk::NavigationToolStorage>  IDevicesScanner::GetRobotToolStorage() const
	{
		return this->imp->m_KukaToolStorage;
	}
	void IDevicesScanner::SetRobotStatus(int i)
	{
		this->imp->robotStatus = i;
	}
	int IDevicesScanner::GetRobotStatus() const
	{
		return this->imp->robotStatus;
	}
	void IDevicesScanner::SetNDIStatus(int i)
	{
		this->imp->ndiStatus = i; 
	}
	int IDevicesScanner::GetNDIStatus() const
	{
		return this->imp->ndiStatus;
	}

}