#include "lancetDevicesScanner.h"

#include <QDir>
#include <QTime>
#include <QTimer>
#include <QThread>
#include <QDirIterator>
#include <QReadWriteLock>
#include <QSharedPointer>

#include <memory>

namespace lancet
{
	struct DevicesScanner::DevicesScannerPrivateImp
	{
		QSharedPointer<QTimer> timer;
		QReadWriteLock mutex;
	};

	DevicesScanner::DevicesScanner()
		: imp(std::make_shared<DevicesScannerPrivateImp>())
	{
	}
    void DevicesScanner::ConnectNDI()
    {
        MITK_INFO << "Establishing connection with NDI...";
        vegaTrackingDevice = lancet::NDIVegaTrackingDevice::New(); //instantiate
        lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
            lancet::TrackingDeviceSourceConfiguratorLancet::New(this->GetNDIToolStorage(), vegaTrackingDevice);

        m_VegaSource = kukaSourceFactory->CreateTrackingDeviceSource(m_VegaVisualizer);

        m_VegaSource->Connect();

        m_VegaSource->StartTracking();

        //update visualize filter by timer
        if (m_VegaVisualizeTimer == nullptr)
        {
            m_VegaVisualizeTimer = new QTimer(this); //create a new timer
        }
        m_VegaVisualizeTimer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps
        connect(m_VegaVisualizeTimer, &QTimer::timeout, [=]() {
            if (m_VegaVisualizer.IsNotNull())
            {
                this->SetNDIStatus(0);
                m_VegaVisualizer->Update();
                MITK_INFO << "NDI is connected";
                m_VegaVisualizeTimer->stop();
            }
            else
            {
                this->SetNDIStatus(1);
                MITK_INFO << "NDI is disconnected";
                m_VegaVisualizeTimer->stop();
            }
            emit this->IDevicesGetStatus();
        });
    }

    void DevicesScanner::ConnectRobot()
    {
        MITK_INFO << "Establishing connection with Robot...";
        m_KukaTrackingDevice = lancet::KukaRobotDevice::New(); //instantiate
        lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
            lancet::TrackingDeviceSourceConfiguratorLancet::New(this->GetRobotToolStorage(), m_KukaTrackingDevice);
        m_KukaSource = kukaSourceFactory->CreateTrackingDeviceSource(m_KukaVisualizer);
        m_KukaSource->Connect();
        if (m_KukaTrackingDevice->GetState() == 1) //ready
        {
            m_KukaSource->StartTracking();

            //update visualize filter by timer
            if (m_KukaVisualizeTimer == nullptr)
            {
                m_KukaVisualizeTimer = new QTimer(this); //create a new timer
            }

            m_KukaVisualizeTimer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps

            connect(m_KukaVisualizeTimer, &QTimer::timeout, [=]() {
                if (m_KukaVisualizer.IsNotNull())
                {
                    this->SetRobotStatus(0);
                    m_KukaVisualizer->Update();
                    MITK_INFO << "Robot is connected";
                    m_KukaVisualizeTimer->stop();
                }
                else
                {
                    this->SetRobotStatus(1);
                    MITK_INFO << "Robot is disconnected";
                    m_KukaVisualizeTimer->stop();
                }
                emit this->IDevicesGetStatus();
            });
        }
        else
        {
            MITK_ERROR << "Tracking not start,Device State:" << m_KukaTrackingDevice->GetState();
        }
    }

    void DevicesScanner::ConnectVirtualDeviceNDI()
    {
        MITK_INFO << "Establishing connection with VirtualDevice1...";
        m_VirtualDevice1 = mitk::VirtualTrackingDevice::New();
        lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
            lancet::TrackingDeviceSourceConfiguratorLancet::New(this->GetNDIToolStorage(), m_VirtualDevice1);

        m_VirtualDevice1Source = kukaSourceFactory->CreateTrackingDeviceSource(m_VirtualDevice1Visualizer);

        m_VirtualDevice1Source->Connect();

        m_VirtualDevice1Source->StartTracking();

        //update visualize filter by timer
        if (m_VirtualDevice1Timer == nullptr)
        {
            m_VirtualDevice1Timer = new QTimer(this); //create a new timer
        }
        m_VirtualDevice1Timer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps

        connect(m_VirtualDevice1Timer, &QTimer::timeout, [=]() {
            if (m_VirtualDevice1Visualizer.IsNotNull())
            {
                this->SetNDIStatus(0);
                m_VirtualDevice1Visualizer->Update();
                MITK_INFO << "NDI is connected";
                m_VirtualDevice1Timer->stop();
            }
            else
            {
                this->SetNDIStatus(1);
                MITK_INFO << "NDI is disconnected";
                m_VirtualDevice1Timer->stop();
            }
            emit this->IDevicesGetStatus();
        });
    }

    void DevicesScanner::ConnectVirtualDeviceRobot()
    {
        MITK_INFO << "Establishing connection with VirtualDevice2...";
        m_VirtualDevice2 = mitk::VirtualTrackingDevice::New();
        lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
            lancet::TrackingDeviceSourceConfiguratorLancet::New(this->GetRobotToolStorage(), m_VirtualDevice2);
        m_VirtualDevice2Source = kukaSourceFactory->CreateTrackingDeviceSource(m_VirtualDevice2Visualizer);
        
        try
        {
            m_VirtualDevice2Source->Connect();
            m_VirtualDevice2Source->StartTracking();
            MITK_INFO << "Robot is connected.....";
        }
        catch (std::invalid_argument e)
        {
            MITK_ERROR << "Robot is disconnected! Error " << e.what();
        }
        catch (std::runtime_error e)
        {
            MITK_ERROR << "Robot is disconnected! Error " << e.what();
        }

        //update visualize filter by timer
        if (m_VirtualDevice2Timer == nullptr)
        {
            m_VirtualDevice2Timer = new QTimer(this); //create a new timer
        }
        m_VirtualDevice2Timer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps

        connect(m_VirtualDevice2Timer, &QTimer::timeout, [=]() {
            if (m_VirtualDevice2Visualizer.IsNotNull())
            {
                this->SetRobotStatus(0);
                m_VirtualDevice2Visualizer->Update();
                MITK_INFO << "Robot is connected";
                m_VirtualDevice2Timer->stop();
            }
            else
            {
                this->SetRobotStatus(1);
                MITK_INFO << "Robot is disconnected";
                m_VirtualDevice2Timer->stop();
            }
            emit this->IDevicesGetStatus();
        });
    }

    void DevicesScanner::DisConnectNDI()
    {

    }
    void DevicesScanner::DisConnectRobot()
    {

    }
    void DevicesScanner::DisConnectVirtualDeviceNDI()
    {

    }
    void DevicesScanner::DisConnectVirtualDeviceRobot()
    {

    }
}