#include "QLinkingHardware.h"
#include "ui_QLinkingHardwareControls.h"
#include "org_mitk_lancet_linkinghardware_Activator.h"

// Qt
#include <QFile>
#include <QTimer>

#include <QByteArray>
#include <QJsonDocument>

// mitk
#include <mitkNavigationData.h>
#include <mitkTrackingDeviceSource.h>

// lancet
#include <lancetIDevicesAdministrationService.h>
#include <core/lancetSpatialFittingPointAccuracyDate.h>

class DervicveConfigurePrivate
{
public:
	DervicveConfigurePrivate() {}
	DervicveConfigurePrivate(const QString& file)
	{
		this->SetFileName(file);
	}

	void SetFileName(const QString& file)
	{
		this->_filename = file;
	}

	QString GetFileName() const
	{
		return this->_filename;
	}

	bool Update()
	{
		QFile file(this->GetFileName());
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			MITK_WARN << "Not open file. see " << this->GetFileName();
			return false;
		}

		return this->AnalysisJsonConfigure(file.readAll());
	}

	QString GetNDIIGTToolStorage() const
	{
		return this->_NDIIGTToolStorage;
	}

	QString GetRobotIGTToolStorage() const
	{
		return this->_RobotIGTToolStorage;
	}

protected:
	bool AnalysisJsonConfigure(const QByteArray& stream)
	{
		QJsonParseError jsonError;
		QJsonDocument jsonDoc = QJsonDocument::fromJson(stream, &jsonError);

		if (jsonError.error != QJsonParseError::NoError)
		{
			return false;
		}

		this->_NDIIGTToolStorage = jsonDoc.object()["Device"].toObject()["NDIIGTToolStorage"].toString();
		this->_RobotIGTToolStorage = jsonDoc.object()["Device"].toObject()["RobotIGTToolStorage"].toString();

		return true;
	}
private:
	QString _NDIIGTToolStorage;
	QString _RobotIGTToolStorage;
	QString _filename;
};

const std::string QLinkingHardware::VIEW_ID = "org.mitk.views.qlinkinghardware";

struct QLinkingHardware::QLinkingHardwarePrivateImp
{
	QTimer robotMoveCheckedTimer;

	mitk::Point3D robotStartPoint;

	Ui::QLinkingHardwareControls m_Controls;
};

QLinkingHardware::QLinkingHardware()
  : imp(std::make_shared<QLinkingHardwarePrivateImp>())
{
}

QLinkingHardware::~QLinkingHardware()
{
}

void QLinkingHardware::SetFocus()
{
}

void QLinkingHardware::CreateQtPartControl(QWidget *parent)
{
	this->imp->m_Controls.setupUi(parent);

	QString qssFile = ":/org.mitk.lancet.linkinghardware/linkinghardware.qss";
	this->UpdateQtPartControlStyleSheet(qssFile);
	this->ConnectedQtInteractive();
	this->ConnectedQtEventForDevicesService();

	this->imp->m_Controls.widget_3->InitializeTrackingToolVisible("Probe");
	this->imp->m_Controls.widget_3->InitializeTrackingToolVisible("Robot");
}

bool QLinkingHardware::ConnectedQtInteractive()
{
	connect(this->imp->m_Controls.pushbtnActivate, &QPushButton::clicked,
		this, &QLinkingHardware::OnPushbtnActivate);
	connect(this->imp->m_Controls.pushbtnActivateSuccess, &QPushButton::clicked,
		this, &QLinkingHardware::OnPushbtnActivateSuccess);

	connect(this->imp->m_Controls.pushbtnNew, &QPushButton::clicked,
		this, &QLinkingHardware::OnTestAppendTrackingTool);
	connect(this->imp->m_Controls.pushbtnDel, &QPushButton::clicked,
		this, &QLinkingHardware::OnTestRemoveTrackingTool);
	return true;
}

bool QLinkingHardware::DisConnectedQtInteractive()
{
	disconnect(this->imp->m_Controls.pushbtnActivate, &QPushButton::clicked,
		this, &QLinkingHardware::OnPushbtnActivate);
	disconnect(this->imp->m_Controls.pushbtnActivateSuccess, &QPushButton::clicked,
		this, &QLinkingHardware::OnPushbtnActivateSuccess);
	return true;
}

bool QLinkingHardware::ConnectedQtEventForDevicesService()
{
	lancet::IDevicesAdministrationService* sender = this->GetService();
	if (nullptr == sender)
	{
		return false;
	}
	lancet::IDevicesAdministrationService* o = sender;
	QObject::connect(sender, &lancet::IDevicesAdministrationService::TrackingDeviceStateChange,
		this, &QLinkingHardware::OnIDevicesGetStatus);

	return true;
}

bool QLinkingHardware::DisConnectedQtEventForDevicesService()
{
	lancet::IDevicesAdministrationService* sender = this->GetService();
	if (nullptr == sender)
	{
		return false;
	}
	lancet::IDevicesAdministrationService* o = sender;
	QObject::disconnect(sender, &lancet::IDevicesAdministrationService::TrackingDeviceStateChange,
		this, &QLinkingHardware::OnIDevicesGetStatus);

	return true;
}

bool QLinkingHardware::UpdateQtPartControlStyleSheet(const QString& qssFileName)
{
	QFile qss(qssFileName);

	if (!qss.open(QIODevice::ReadOnly))
	{
		MITK_WARN << "Not open file. see " << qssFileName;
		return false;
	}

	this->imp->m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
	return true;
}

lancet::IDevicesAdministrationService *QLinkingHardware::GetService()
{
	auto context = mitk::PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	return context->getService<lancet::IDevicesAdministrationService>(serviceRef);
}

void QLinkingHardware::OnIDevicesGetStatus(std::string name, 
	lancet::TrackingDeviceManage::TrackingDeviceState state)
{
	auto scanner = this->GetService()->GetConnector();
	bool isConnected = state & lancet::TrackingDeviceManage::TrackingDeviceState::Connected;

	if (name == "Vega")
	{
		this->imp->m_Controls.checkBox_startNDI->setChecked(isConnected);
	}
	else if (name == "Kuka")
	{
		auto connector = this->GetService()->GetConnector();
		auto robot = connector->GetTrackingDevice("Kuka");
		if (state == lancet::TrackingDeviceManage::TrackingDeviceState::Install)
		{
			connector->GetTrackingDevice("Kuka")->StartTracking();
			return;
		}
		this->imp->m_Controls.checkBox_startRobot->setChecked(isConnected);

		mitk::NavigationData::Pointer rob_move = connector->GetTrackingDeviceSource("Kuka")->GetOutput(0);
		this->imp->robotStartPoint = rob_move->GetPosition();
		this->imp->robotMoveCheckedTimer.start(18);
		connect(&this->imp->robotMoveCheckedTimer, &QTimer::timeout, 
			this, &QLinkingHardware::OnCheckedRobotMovePosition);
	}

	// 如果设备都连接成功，那么开放跳转通道
	bool isPass = this->imp->m_Controls.checkBox_startNDI->isChecked() 
		&& this->imp->m_Controls.checkBox_startRobot->isChecked();
	this->imp->m_Controls.pushbtnActivate->setEnabled(isPass);
	if (this->imp->m_Controls.pushbtnActivateSuccess->isEnabled())
	{
		this->imp->m_Controls.pushbtnActivate->setText(QString::fromLocal8Bit("检测成功"));
	}
	else
	{
		this->imp->m_Controls.pushbtnActivate->setEnabled(true);
		this->imp->m_Controls.pushbtnActivate->setText(QString::fromLocal8Bit("重新检测"));
	}
}

void QLinkingHardware::OnCheckedRobotMovePosition()
{
	auto connector = this->GetService()->GetConnector();

	if (connector && connector->GetTrackingDevice("Kuka"))
	{
		mitk::NavigationData::Pointer robotTracking = 
			connector->GetTrackingDeviceSource("Kuka")->GetOutput(0);

		if (robotTracking->IsDataValid())
		{
			lancet::spatial_fitting::PointAccuracyDate robotMoveAccurcy;
			robotMoveAccurcy.SetSourcePoint(this->imp->robotStartPoint);
			robotMoveAccurcy.SetTargetPoint(robotTracking->GetPosition());

			if (robotMoveAccurcy.Compute() >= 5.0)
			{
				this->imp->m_Controls.checkBox_free->setChecked(true);
				this->imp->robotMoveCheckedTimer.stop();
			}
		}
		else
		{
			MITK_WARN << "robot tracking data is invalid!";
		}
	}
}

void QLinkingHardware::OnPushbtnActivate()
{
	DervicveConfigurePrivate conf;
	conf.SetFileName(":/org.mitk.lancet.linkinghardware/config.json");
	conf.Update();

	if (this->GetService())
	{
		auto connector = this->GetService()->GetConnector();

		if (false == connector->IsInstallTrackingDevice("Vega"))
		{
			std::string ndiIGTToolStorage = conf.GetNDIIGTToolStorage().toStdString();
			if (connector->InstallTrackingDevice("Vega", ndiIGTToolStorage, this->GetDataStorage()))
			{
				MITK_INFO << "Connecting to the Vega";
				connector->GetTrackingDevice("Vega")->OpenConnection();
				connector->GetTrackingDevice("Vega")->StartTracking();
			}
			else
			{
				MITK_ERROR 
					<< "install vega tracking device faild! error: " 
					<< connector->GetErrorString();
			}
		}
		if (false == connector->IsInstallTrackingDevice("Kuka"))
		{
			std::string kukaIGTToolStorage = conf.GetNDIIGTToolStorage().toStdString();
			if (connector->InstallTrackingDevice("Kuka", kukaIGTToolStorage, this->GetDataStorage()))
			{
				MITK_INFO << "Connecting to the Kuka";
				connector->GetTrackingDevice("Kuka")->OpenConnection();
			}
			else
			{
				MITK_ERROR
					<< "install robot tracking device faild! error: "
					<< connector->GetErrorString();
			}
		}
	}
}

void QLinkingHardware::OnPushbtnActivateSuccess()
{
}

void QLinkingHardware::OnTestAppendTrackingTool()
{
	static int index = 0;

	if (index >= this->imp->m_Controls.widget_3->GetTrackingToolNameOfWidgets().size())
	{
		index = 0;
	}
	QString name = this->imp->m_Controls.widget_3->GetTrackingToolNameOfWidgets().at(index++);
	//this->imp->m_Controls.widget_3->InitializeTrackingToolVisible(name);
	this->imp->m_Controls.widget_3->SetTrackingToolEnable(name,true);
}

void QLinkingHardware::OnTestRemoveTrackingTool()
{
	static int index = 0;

	if (index >= this->imp->m_Controls.widget_3->GetTrackingToolNameOfWidgets().size())
	{
		index = 0;
	}
	QString name = this->imp->m_Controls.widget_3->GetTrackingToolNameOfWidgets().at(index++);
	//this->imp->m_Controls.widget_3->UnInitializeTrackingToolVisible(name);
	this->imp->m_Controls.widget_3->SetTrackingToolEnable(name, false);
}
