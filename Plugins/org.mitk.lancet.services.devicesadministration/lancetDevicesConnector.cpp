#include "lancetDevicesConnector.h"
#include <QDir>
#include <QTime>
#include <QTimer>
#include <QThread>
#include <QDirIterator>
#include <QReadWriteLock>
#include <QSharedPointer>

#include <map>
#include <memory>

namespace lancet
{
    struct DevicesConnector::DevicesConnectorPrivateImp
    {
        QTimer tm;
        std::map<std::string, bool> mapDeviceConnect;
        std::map<std::string, mitk::TrackingDeviceSource::Pointer> mapDeviceSource;
    };

    DevicesConnector::DevicesConnector()
        : imp(std::make_shared<DevicesConnectorPrivateImp>())
    {
        this->imp->tm.setInterval(1000);
        connect(&this->imp->tm, &QTimer::timeout, this, &DevicesConnector::func_sanf);
        this->imp->tm.start();
    }
    bool DevicesConnector::RegisterDevice(const std::string& name)
    {
        if (this->IsRegisterDevice(name))
        {
            // 已经注册了目标设备
            return true;
        }

        bool retval = false;

        // 00001
        // 00000
        //=00001
        retval |= this->RegisterVegaTrackingDevice(name);
        retval |= this->RegisterRobotTrackingDevice(name);

        return retval;
    }
    bool DevicesConnector::IsConnect(const std::string& name) const
    {
        if (this->IsRegisterDevice(name))
        {
            return this->GetDeviceSource(name)->IsConnected();
        }
        return false;
    }
    mitk::TrackingDevice::Pointer DevicesConnector::GetDevice(const std::string& name) const
    {
        if (this->IsRegisterDevice(name))
        {
            return this->GetDeviceSource(name)->GetTrackingDevice();
        }
        return mitk::TrackingDevice::Pointer();
    }
    mitk::TrackingDeviceSource::Pointer DevicesConnector::GetDeviceSource(const std::string& name) const
    {
        if (this->IsRegisterDevice(name))
        {
            return this->imp->mapDeviceSource[name];
        }
        return mitk::TrackingDeviceSource::Pointer();
    }
    bool DevicesConnector::IsRegisterDevice(const std::string& name) const
    {
        return this->imp->mapDeviceSource.find(name) != this->imp->mapDeviceSource.end();
    }
    void DevicesConnector::ConnectDevice()
    {

    }
    bool DevicesConnector::RegisterVegaTrackingDevice(const std::string&)
    {
        return false;
    }
    bool DevicesConnector::RegisterRobotTrackingDevice(const std::string&)
    {
        return false;
    }
    void DevicesConnector::func_sanf()
    {
        for (auto& item = this->imp->mapDeviceSource.begin();
            item != this->imp->mapDeviceSource.end(); ++item)
        {
            auto findConnectItem = this->imp->mapDeviceConnect.find(item->first);
            if (findConnectItem != this->imp->mapDeviceConnect.end())
            {
                // 本地状态与服务上状态不一致；以服务上的状态为基准更新
                if (findConnectItem->second != item->second->IsConnected())
                {
                    this->imp->mapDeviceConnect[findConnectItem->first] = item->second->IsConnected();
                    emit this->DeviceConnect_chnage(findConnectItem->first, item->second->IsConnected());
                }
            }
        }
    }
}