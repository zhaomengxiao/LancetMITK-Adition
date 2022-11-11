/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "QLinkingHardware.h"

// Qt

#include <QDir>
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

#include <berryIQtStyleManager.h>
#include "org_mitk_lancet_linkinghardware_Activator.h"
#include <lancetIDevicesAdministrationService.h>
#include <internal/lancetTrackingDeviceManage.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QJsonValue> 
#include <QJsonObject> 
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>

const std::string QLinkingHardware::VIEW_ID = "org.mitk.views.qlinkinghardware";

void QLinkingHardware::SetFocus(){}
QLinkingHardware::~QLinkingHardware()
{
}
void QLinkingHardware::CreateQtPartControl(QWidget *parent)
{ 
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent); 
  m_Controls.pushButton_success->setEnabled(true);
  auto test_dir = QDir(":/org.mitk.lancet.linkinghardware/");
  QFile qss(test_dir.absoluteFilePath("linkinghardware.qss"));

  if (!qss.open(QIODevice::ReadOnly))
  {
	  
	  qWarning() << __func__ << __LINE__ << ":" << "error load file "
		  << test_dir.absoluteFilePath("linkinghardware.qss") << "\n"
		  << "error: " << qss.errorString();
  }
  // pos
  qInfo() << "log.file.pos " << qss.pos();

  m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
  qss.close();

  connect(m_Controls.pushButton_auto, &QPushButton::clicked, this, &QLinkingHardware::on_pb_auto_clicked);
  connect(m_Controls.pushButton_success, &QPushButton::clicked, this, &QLinkingHardware::on_pb_success_clicked);
  
  if (this->GetService())
  {
	  this->ConnectToService();
  }
}
lancet::IDevicesAdministrationService* QLinkingHardware::GetService() const
{
	auto context = mitk::PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	return context->getService<lancet::IDevicesAdministrationService>(serviceRef);
}
void QLinkingHardware::ConnectToService()
{
	auto sender = this->GetService();
	if (sender)
	{
		lancet::IDevicesAdministrationService* o = sender;
		QObject::connect(o, &lancet::IDevicesAdministrationService::IDevicesGetStatus,
			this, &QLinkingHardware::Slot_IDevicesGetStatus);
	}
}
void QLinkingHardware::Slot_IDevicesGetStatus()
{
	auto scanner = this->GetService()->GetConnector();
	//this->setStartHardware(scanner->GetRobotStatus(), scanner->GetNDIStatus());
}
void QLinkingHardware::setStartHardware(int robot, int ndi)
{
	QString str = "--robot::" + QString::number(robot) + "--ndi::" + QString::number(ndi);
	//Log::write("QLinkingHardware::setStartHardware" + str);
	MITK_INFO << "QLinkingHardware:" << __func__ + str << ": log";
	if (robot == 1 && ndi == 1)
	{
		m_Controls.checkBox_startNDI->setChecked(true);
		m_Controls.checkBox_startRobot->setChecked(true);
		m_Controls.pushButton_success->setEnabled(true);
		m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("检测成功"));
	}
	if (robot == 1 && ndi == 0)
	{
		m_Controls.checkBox_startNDI->setChecked(false);
		m_Controls.checkBox_startRobot->setChecked(true);
		m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("重新检测"));
		m_Controls.pushButton_auto->setEnabled(true);
	}
	if (robot == 0 && ndi == 1)
	{
		m_Controls.checkBox_startNDI->setChecked(true);
		m_Controls.checkBox_startRobot->setChecked(false);
		m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("重新检测"));
		m_Controls.pushButton_auto->setEnabled(true);
	}
	if (robot == 0 && ndi == 0)
	{
		m_Controls.checkBox_startNDI->setChecked(false);
		m_Controls.checkBox_startRobot->setChecked(false);
		m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("重新检测"));
		m_Controls.pushButton_auto->setEnabled(true);
	}	
}

void QLinkingHardware::ReadFileName()
{
    
    auto test_dir = QDir(":/org.mitk.lancet.linkinghardware/");
    QFile file(test_dir.absoluteFilePath("config.json"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QByteArray array = file.readAll();
    QJsonParseError json_error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(array, &json_error);

    if (json_error.error == QJsonParseError::NoError)
    {
        QJsonObject rootObj = jsonDoc.object();
        QJsonArray  JsonArray;
        if (rootObj.contains("Device") && rootObj.value("Device").isObject())
        {
            QJsonObject  childJson = rootObj.value("Device").toObject();
            JsonArray = childJson.value("IGTToolStorage").toArray();
            auto connector = this->GetService()->GetConnector(); 
            for (int i = 0; i < JsonArray.size(); i++)
            {
                QJsonObject childObj = JsonArray[i].toObject();
                if (childObj.contains("ndifilename") && childObj.value("ndifilename").isString())
                {
                    filename = childObj.value("ndifilename").toString();
                    MITK_INFO  << filename;
                    mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(GetDataStorage());
                    if (false == connector->IsInstallTrackingDevice("NDI"))
                    {
                        if (false == connector->InstallTrackingDevice("NDI", filename.toStdString(), this->GetDataStorage()))
                        {
                            std::cout << "install robot tracking device faild! error: " << connector->GetErrorString();
                        }
                        else
                        {
                            connector->GetTrackingDevice("NDI")->OpenConnection();
                        }
                    }
                }

                if (childObj.contains("robfilename") && childObj.value("robfilename").isString())
                {
                    filename = childObj.value("robfilename").toString();
                    MITK_INFO  << filename; 
                    if (false == connector->IsInstallTrackingDevice("Robot"))
                    {
                        if (false == connector->InstallTrackingDevice("Robot", filename.toStdString(), this->GetDataStorage()))
                        {
                            std::cout << "install robot tracking device faild! error: " << connector->GetErrorString();
                        }
                        else
                        {
                            connector->GetTrackingDevice("Robot")->OpenConnection();
                        }
                    }
                }
                auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
                mitk::RenderingManager::GetInstance()->InitializeViews(geo);
            }
        }
        //read in filename
        //QString ndifilename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/",
        //    tr("Tool Storage Files (*.IGTToolStorage)")); 
        //QString robfilename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/",
        //        tr("Tool Storage Files (*.IGTToolStorage)"));
        if (filename.isNull()) return;
        //read tool storage from disk
    }
}

void QLinkingHardware::on_pb_auto_clicked()
{
    MITK_INFO << "QLinkingHardware:" << __func__ << ": log";
    m_Controls.pushButton_auto->setEnabled(false);
    m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("正在检测"));
   
   
	if (isauto)
    {
        ReadFileName();
    }
    else
    {
        auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
        mitk::RenderingManager::GetInstance()->InitializeViews(geo);
    }
}
void QLinkingHardware::on_pb_success_clicked()
{
	//Log::write("QLinkingHardware::on_pushButton_success_clicked");
	MITK_INFO << "QLinkingHardware:" << __func__ << ": log";
}