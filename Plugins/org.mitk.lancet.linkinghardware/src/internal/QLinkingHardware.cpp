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
#include <lancetIDevicesScanner.h>
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
	auto scanner = this->GetService()->GetScanner();
	this->setStartHardware(scanner->GetRobotStatus(), scanner->GetNDIStatus());
}
void QLinkingHardware::setStartHardware(int robot, int ndi)
{
	QString str = "--robot::" + QString::number(robot) + "--ndi::" + QString::number(ndi);
	//Log::write("QLinkingHardware::setStartHardware" + str);
	MITK_INFO << "QLinkingHardware:" << __func__ + str << ": log";
	if (robot == 0 && ndi == 0)
	{
		m_Controls.checkBox_startNDI->setChecked(true);
		m_Controls.checkBox_startRobot->setChecked(true);
		m_Controls.pushButton_success->setEnabled(true);
		m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("检测成功"));
	}
	if (robot == 0 && ndi == 1)
	{
		m_Controls.checkBox_startNDI->setChecked(false);
		m_Controls.checkBox_startRobot->setChecked(true);
		m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("重新检测"));
		m_Controls.pushButton_auto->setEnabled(true);
	}
	if (robot == 1 && ndi == 0)
	{
		m_Controls.checkBox_startNDI->setChecked(true);
		m_Controls.checkBox_startRobot->setChecked(false);
		m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("重新检测"));
		m_Controls.pushButton_auto->setEnabled(true);
	}
	if (robot == 1 && ndi == 1)
	{
		m_Controls.checkBox_startNDI->setChecked(false);
		m_Controls.checkBox_startRobot->setChecked(false);
		m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("重新检测"));
		m_Controls.pushButton_auto->setEnabled(true);
	}	
}

void QLinkingHardware::on_pb_auto_clicked()
{
    MITK_INFO << "QLinkingHardware:" << __func__ << ": log";
    m_Controls.pushButton_auto->setEnabled(false);
    m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("正在检测"));
    auto scanner = this->GetService()->GetScanner();
	if (isauto)
    {
        //read in filename
        QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/",
            tr("Tool Storage Files (*.IGTToolStorage)"));
        if (filename.isNull()) return;
        //read tool storage from disk
        std::string errorMessage = "";
        mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(GetDataStorage());
		m_RobotToolStorage = myDeserializer->Deserialize(filename.toStdString());
        m_RobotToolStorage->SetName(filename.toStdString()); 
		scanner->SetRobotToolStorage(m_RobotToolStorage);
		//scanner->ConnectRobot();
		scanner->ConnectVirtualDeviceRobot(); 
		
		m_NDIToolStorage = myDeserializer->Deserialize(filename.toStdString());
		m_NDIToolStorage->SetName(filename.toStdString());
		scanner->SetNDIToolStorage(m_NDIToolStorage);
		//scanner->ConnectNDI();
		scanner->ConnectVirtualDeviceNDI();
        auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
        mitk::RenderingManager::GetInstance()->InitializeViews(geo);
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