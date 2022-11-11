#include "lancetITrackingDeviceManage.h"

#include <QDir>

//IGT
#include <mitkNavigationToolStorage.h>
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
	struct ITrackingDeviceManage::ITrackingDeviceManagePrivateImp
	{
		int robotStatus;
		int ndiStatus;

		mitk::NavigationToolStorage::Pointer m_NDIToolStorage;
		mitk::NavigationToolStorage::Pointer m_RobotToolStorage;
	};
	ITrackingDeviceManage::ITrackingDeviceManage()
		: imp(std::make_shared<ITrackingDeviceManagePrivateImp>())
	{

	}
	void ITrackingDeviceManage::SetNDIToolStorage(itk::SmartPointer<mitk::NavigationToolStorage> m_ToolStorage)
	{
		this->imp->m_NDIToolStorage = m_ToolStorage;
	}
	itk::SmartPointer<mitk::NavigationToolStorage>  ITrackingDeviceManage::GetNDIToolStorage() const
	{
		return this->imp->m_NDIToolStorage;
	}

	void ITrackingDeviceManage::SetRobotToolStorage(itk::SmartPointer<mitk::NavigationToolStorage> m_ToolStorage)
	{
		this->imp->m_RobotToolStorage = m_ToolStorage;
	}
	itk::SmartPointer<mitk::NavigationToolStorage>  ITrackingDeviceManage::GetRobotToolStorage() const
	{
		return this->imp->m_RobotToolStorage;
	}

	void ITrackingDeviceManage::SetRobotStatus(int i)
	{
		this->imp->robotStatus = i;
	}
	int ITrackingDeviceManage::GetRobotStatus() const
	{
		return this->imp->robotStatus;
	}

	void ITrackingDeviceManage::SetNDIStatus(int i)
	{
		this->imp->ndiStatus = i; 
	}
	int ITrackingDeviceManage::GetNDIStatus() const
	{
		return this->imp->ndiStatus;
	}
}