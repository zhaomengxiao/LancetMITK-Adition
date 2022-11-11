#include "lancetTrackingDeviceManage.h"

#include "mitkTrackingDevice.h"
#include "mitkTrackingDeviceSource.h"
#include "mitkNavigationToolStorage.h"
#include "mitkNavigationDataToNavigationDataFilter.h"

#include <map>
#include <memory>
namespace lancet
{
	struct TrackingDeviceManage::TrackingDeviceManagePrivateImp
	{
		struct TrackingDeviceProperties
		{
			// Last connection status of the device, true: connection, false: disconnection.
			bool lastConnectedState = false;

			mitk::DataStorage::Pointer dataStorage;
			mitk::TrackingDeviceSource::Pointer trackingDeviceSource;
			mitk::NavigationToolStorage::Pointer navigationToolStorage;
			mitk::NavigationDataToNavigationDataFilter::Pointer navigationDataToNavigationDataFilter;
		};

		std::map<std::string, TrackingDeviceProperties> mapTrackingDerviceProperties;
	};

	TrackingDeviceManage::TrackingDeviceManage()
		: imp(std::make_shared<TrackingDeviceManagePrivateImp>())
	{
	}
	TrackingDeviceManage::~TrackingDeviceManage()
	{
	}

	bool TrackingDeviceManage::InstallTrackingDevice(const std::string& name, const std::string& filename, itk::SmartPointer<mitk::DataStorage> dataStorage)
	{
		MITK_INFO << "Establishing connection with" << name;
		if (this->IsInstallTrackingDevice(name))
		{
			// 已经注册了目标设备
			return true;
		}

		return false;
	}

	bool TrackingDeviceManage::UnInstallTrackingDevice(const std::string& name, itk::SmartPointer<mitk::DataStorage> dataStorage)
	{
		return false;
	}

	berry::SmartPointer<mitk::TrackingDevice> TrackingDeviceManage::GetTrackingDevice(const std::string& name) const
	{
		if (this->IsInstallTrackingDevice(name))
		{
			
		}
		return berry::SmartPointer<mitk::TrackingDevice>();
	}

	berry::SmartPointer<mitk::TrackingDeviceSource> TrackingDeviceManage::GetTrackingDeviceSource(const std::string& name) const
	{
		return berry::SmartPointer<mitk::TrackingDeviceSource>();
	}

	berry::SmartPointer<mitk::NavigationToolStorage> TrackingDeviceManage::GetNavigationToolStorage(const std::string& name) const
	{
		return berry::SmartPointer<mitk::NavigationToolStorage>();
	}

	berry::SmartPointer<mitk::NavigationDataToNavigationDataFilter> TrackingDeviceManage::GetNavigationDataToNavigationDataFilter(const std::string& name) const
	{
		return berry::SmartPointer<mitk::NavigationDataToNavigationDataFilter>();
	}

	void TrackingDeviceManage::SetNavigationDataFilterInterval(long time)
	{
	}

	long TrackingDeviceManage::GetNavigationDataFilterInterval() const
	{
		return 0;
	}

	int TrackingDeviceManage::GetInstallTrackingDeviceSize() const
	{
		return 0;
	}

	int TrackingDeviceManage::GetErrorCode() const
	{
		return 0;
	}

	std::string TrackingDeviceManage::GetErrorString() const
	{
		return std::string();
	}

	bool TrackingDeviceManage::IsInstallTrackingDevice(const std::string& name) const
	{
		return this->imp->mapTrackingDerviceProperties.find(name) != this->imp->mapTrackingDerviceProperties.end();
		//return false;
	}

	bool TrackingDeviceManage::IsTrackingDeviceConnected(const std::string& name) const
	{

		return false;
	}

	bool TrackingDeviceManage::IsStartTrackingDevice(const std::string& name) const
	{
		return false;
	}

}