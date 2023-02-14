#include "lancetTrackingDeviceManage.h"

#include "mitkTrackingDevice.h"
#include "mitkTrackingDeviceSource.h"
#include "mitkNavigationToolStorage.h"
#include "mitkNavigationDataToNavigationDataFilter.h"
#define MITK_BUILD_TESTTING
#include "kukaRobotDevice.h"
#include "mitkVirtualTrackingDevice.h"
#include "mitkVirtualTrackingTool.h"

// TODO: Bug
// It is very likely that the Kuka mechanical arm header file has been included before, and this class has been adjusted in December 2022. Therefore, the Kuka mechanical arm header file macro is released here to add its adjustment code to the compiler's preprocessing
#ifdef KUKAROBOTDEVICE_H
#undef KUKAROBOTDEVICE_H
#endif
#include "lancetKukaRobotDevice.h"
#include "lancetNavigationObjectVisualizationFilter.h"
#include "lancetApplyDeviceRegistratioinFilter.h"
#include "lancetApplySurfaceRegistratioinFilter.h"
#include "lancetTrackingDeviceSourceConfigurator.h"
#include "lancetPathPoint.h"
#include "lancetVegaTrackingDevice.h"
#include <mitkNavigationDataToPointSetFilter.h>
#include "mitkNavigationToolStorageDeserializer.h"
#include <QtWidgets\qfiledialog.h>
#include "mitkIGTIOException.h"
#include "mitkNavigationToolStorageSerializer.h"
//#include "QmitkIGTCommonHelper.h"

#include <map>
#include <memory>
#include <QTime>
#include <QTimer>
#include <QSharedPointer>
namespace lancet
{
	struct TrackingDeviceManage::TrackingDeviceManagePrivateImp
	{
		struct TrackingDeviceProperties
		{
			// Last connection status of the device, true: connection, false: disconnection.
			TrackingDeviceState lastConnectedState = UnInstall;
			mitk::DataStorage::Pointer dataStorage;
			mitk::TrackingDeviceSource::Pointer trackingDeviceSource;
			mitk::NavigationToolStorage::Pointer navigationToolStorage; 
			lancet::NavigationObjectVisualizationFilter::Pointer navigationObjectVisualizationFilter;
		};
		QTimer tm; // 导航设备数据更新定时器
		QTimer tmScanfTrackingDeviceConnected; // 实时同步已安装设备的连接状态
		std::map<std::string, TrackingDeviceProperties> mapTrackingDerviceProperties; 
	};

	TrackingDeviceManage::TrackingDeviceManage()
		: imp(std::make_shared<TrackingDeviceManagePrivateImp>())
	{
		this->imp->tmScanfTrackingDeviceConnected.setInterval(500);
		connect(&this->imp->tmScanfTrackingDeviceConnected, &QTimer::timeout, this, [=]() {
			for (auto& item = this->imp->mapTrackingDerviceProperties.begin();
				item != this->imp->mapTrackingDerviceProperties.end(); ++item)
			{
				int tempConnectedState = item->second.lastConnectedState;
				TrackingDeviceState backupConnectedState = item->second.lastConnectedState;

				switch (item->second.lastConnectedState)
				{
				case TrackingDeviceState::UnInstall:
					if (tempConnectedState == TrackingDeviceState::UnInstall)
					{
						tempConnectedState = TrackingDeviceState::Install;
						item->second.lastConnectedState = TrackingDeviceState::Install;
					}
					break;
				case TrackingDeviceState::Install:
					if (this->GetTrackingDeviceSource(item->first)->IsConnected())
					{
						tempConnectedState = TrackingDeviceState::Install | TrackingDeviceState::Connected;
						item->second.lastConnectedState = TrackingDeviceState::Connected;
					}
					else
					{
						tempConnectedState = TrackingDeviceState::Install;
						item->second.lastConnectedState = TrackingDeviceState::Install;
					}
					break;
				case TrackingDeviceState::Connected:
					if (false == this->GetTrackingDeviceSource(item->first)->IsConnected())
					{
						tempConnectedState = TrackingDeviceState::Install;
						item->second.lastConnectedState = TrackingDeviceState::Install;
					}
					else if (this->GetTrackingDeviceSource(item->first)->IsTracking())
					{
						tempConnectedState = TrackingDeviceState::Install | TrackingDeviceState::Connected | TrackingDeviceState::Tracking;
						item->second.lastConnectedState = TrackingDeviceState::Tracking;
					}
					else
					{
						tempConnectedState = TrackingDeviceState::Install | TrackingDeviceState::Connected;
						item->second.lastConnectedState = TrackingDeviceState::Connected;
					}
					break;
				case TrackingDeviceState::Tracking:
					if (false == this->GetTrackingDeviceSource(item->first)->IsTracking())
					{
						tempConnectedState = TrackingDeviceState::Install | TrackingDeviceState::Connected;
						item->second.lastConnectedState = TrackingDeviceState::Connected;
					}
					break;
				}
				if (backupConnectedState != item->second.lastConnectedState)
				{
					emit this->TrackingDeviceStateChange(item->first, (TrackingDeviceState)tempConnectedState);
				}
			}
		});
		this->imp->tmScanfTrackingDeviceConnected.start();

		connect(&this->imp->tm, SIGNAL(timeout()), this, SLOT(onPipelineUpdateTimeout()));
		this->SetNavigationDataFilterInterval(20);
	}
	TrackingDeviceManage::~TrackingDeviceManage()
	{
		MITK_WARN << "log";
		while (this->GetInstallTrackingDeviceSize())
		{
			MITK_WARN << "Automatically recycle remote device resources! try recycle " 
				<< this->imp->mapTrackingDerviceProperties.begin()->first;
			this->UnInstallTrackingDevice(this->imp->mapTrackingDerviceProperties.begin()->first,
				this->imp->mapTrackingDerviceProperties.begin()->second.dataStorage);
		}
	}

	void TrackingDeviceManage::onPipelineUpdateTimeout()
	{
		for (auto& device : this->imp->mapTrackingDerviceProperties)
		{
			if (device.second.trackingDeviceSource.IsNotNull())
			{
				device.second.trackingDeviceSource->Update();
			}
		}
	}

	bool TrackingDeviceManage::InstallTrackingDevice(const std::string& name, const std::string& filename, itk::SmartPointer<mitk::DataStorage> dataStorage)
	{
		MITK_INFO << "Establishing connection with.." << name;
		if (this->IsInstallTrackingDevice(name))
		{
			// 已经注册了目标设备
			return true;
		}
		TrackingDeviceManagePrivateImp::TrackingDeviceProperties properties;

		// 工具数据集解析
		mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(dataStorage);
		mitk::NavigationToolStorage::Pointer ToolStorage = myDeserializer->Deserialize(filename);
		ToolStorage->SetName(filename);

		// 设备创建
		mitk::TrackingDevice::Pointer vegaTrackingDevice = this->CreateTrackingDevice(name);
																										  
		// Create Navigation Data Source with the factory class, and the visualize filter.
		lancet::TrackingDeviceSourceConfiguratorLancet::Pointer SourceFactory =
			lancet::TrackingDeviceSourceConfiguratorLancet::New(ToolStorage, vegaTrackingDevice);
		lancet::NavigationObjectVisualizationFilter::Pointer Visualizer;
		mitk::TrackingDeviceSource::Pointer trackingDeviceSource = SourceFactory->CreateTrackingDeviceSource(Visualizer);

		if (Visualizer.IsNull() || trackingDeviceSource.IsNull())
		{
			return false;
		}
		trackingDeviceSource->SetToolMetaDataCollection(ToolStorage);

		properties.dataStorage = dataStorage;
		properties.navigationObjectVisualizationFilter = Visualizer;
		properties.navigationToolStorage = ToolStorage;
		properties.trackingDeviceSource = trackingDeviceSource;
		this->imp->mapTrackingDerviceProperties[name] = properties;

		return true;
	}

	bool TrackingDeviceManage::UnInstallTrackingDevice(const std::string& name, itk::SmartPointer<mitk::DataStorage> dataStorage)
	{
		if (this->IsInstallTrackingDevice(name))
		{
			this->GetTrackingDevice(name)->StopTracking();
			this->GetTrackingDevice(name)->CloseConnection();

			this->imp->mapTrackingDerviceProperties.erase(name);
			emit this->TrackingDeviceStateChange(name, TrackingDeviceState::UnInstall);
			return true;
		}
		return false;
	}
	itk::SmartPointer<mitk::TrackingDevice> TrackingDeviceManage::CreateTrackingDevice(const std::string& name)
	{
		if (name == "Vega")
		{
			return NDIVegaTrackingDevice::New();
			//return mitk::VirtualTrackingDevice::New();
		}
		if (name == "Kuka")
		{
			return KukaRobotDevice_New::New();
			//return mitk::VirtualTrackingDevice::New();
		}

		return itk::SmartPointer<mitk::TrackingDevice>();
	}
	itk::SmartPointer<mitk::TrackingDevice> TrackingDeviceManage::GetTrackingDevice(const std::string& name)
	{
		if (this->IsInstallTrackingDevice(name))
		{
			return this->GetTrackingDeviceSource(name)->GetTrackingDevice();
		}
		return itk::SmartPointer<mitk::TrackingDevice>();
	}

	itk::SmartPointer<mitk::TrackingDeviceSource> TrackingDeviceManage::GetTrackingDeviceSource(const std::string& name) const
	{
		if (this->IsInstallTrackingDevice(name))
		{
			return this->imp->mapTrackingDerviceProperties[name].trackingDeviceSource;
		}
		return itk::SmartPointer<mitk::TrackingDeviceSource>();
	}

	itk::SmartPointer<mitk::NavigationToolStorage> TrackingDeviceManage::GetNavigationToolStorage(const std::string& name) const
	{
		if (this->IsInstallTrackingDevice(name))
		{
			return this->imp->mapTrackingDerviceProperties[name].navigationToolStorage;;
		}
		return itk::SmartPointer<mitk::NavigationToolStorage>();
	}

	itk::SmartPointer<lancet::NavigationObjectVisualizationFilter> TrackingDeviceManage::GetNavigationDataToNavigationDataFilter(const std::string& name) const
	{
		if (this->IsInstallTrackingDevice(name))
		{
			return this->imp->mapTrackingDerviceProperties[name].navigationObjectVisualizationFilter;
		}
		return itk::SmartPointer<lancet::NavigationObjectVisualizationFilter>();
	}

	void TrackingDeviceManage::SetNavigationDataFilterInterval(long time)
	{
		this->imp->tm.stop();
		this->imp->tm.setInterval(time);
		this->imp->tm.start();
	}

	long TrackingDeviceManage::GetNavigationDataFilterInterval() const
	{
		return this->imp->tm.interval();
	}

	int TrackingDeviceManage::GetInstallTrackingDeviceSize() const
	{
		return this->imp->mapTrackingDerviceProperties.size();
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
		if (this->IsInstallTrackingDevice(name))
		{
			return this->GetTrackingDeviceSource(name)->IsConnected();
		}

		return false;
	}

	bool TrackingDeviceManage::IsStartTrackingDevice(const std::string& name) const
	{
		if (this->IsInstallTrackingDevice(name))
		{
			return this->GetTrackingDeviceSource(name)->IsTracking();
		}

		return false;
	}

	
}