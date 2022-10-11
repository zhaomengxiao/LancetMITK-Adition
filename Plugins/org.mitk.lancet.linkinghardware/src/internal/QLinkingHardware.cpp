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

// THA
//#include "MarkerStatusFrame.h"
//#include "IGTController.h"
//#include "Log.h"
const std::string QLinkingHardware::VIEW_ID = "org.mitk.views.qlinkinghardware";

void QLinkingHardware::SetFocus(){}
QLinkingHardware::~QLinkingHardware()
{
}
void QLinkingHardware::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent); 
  this->setStartHardware(1, 1);
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
  ////m_Controls.widget->setStyleSheet(qss.readAll());
  //if (qss.pos() != 0)
  //{
	 // qss.seek(0);
  //}

  //qDebug() << __func__ << __LINE__ << ":"
	 // << QFile::exists(test_dir.absoluteFilePath("linkinghardware.qss"))  // True
	 // << "?" << QLatin1String(qss.readAll());
  qss.close();

  connect(m_Controls.pushButton_auto, &QPushButton::clicked, this, &QLinkingHardware::on_pushButton_auto_clicked);
  connect(m_Controls.pushButton_success, &QPushButton::clicked, this, &QLinkingHardware::on_pushButton_success_clicked);
}
void QLinkingHardware::setStartHardware(int staubli, int ndi)
{
	QString str = "--staubli::" + QString::number(staubli) + "--ndi::" + QString::number(ndi);
	//Log::write("QLinkingHardware::setStartHardware" + str);
	MITK_INFO << "QLinkingHardware:" << __func__ + str << ": log";
	if (staubli == 0 && ndi == 0)
	{
		m_Controls.checkBox_startNDI->setChecked(true);
		m_Controls.checkBox_startStaubli->setChecked(true);
	}
	if (staubli == 0 && ndi == 1)
	{
		m_Controls.checkBox_startNDI->setChecked(false);
		m_Controls.checkBox_startStaubli->setChecked(true);
	}
	if (staubli == 1 && ndi == 0)
	{
		m_Controls.checkBox_startNDI->setChecked(true);
		m_Controls.checkBox_startStaubli->setChecked(false);
	}
	if (staubli == 1 && ndi == 1)
	{
		m_Controls.checkBox_startNDI->setChecked(false);
		m_Controls.checkBox_startStaubli->setChecked(false);
	}
}

void QLinkingHardware::on_pushButton_auto_clicked()
{
	MITK_INFO << "QLinkingHardware:" << __func__ << ": log";
	m_Controls.pushButton_auto->setEnabled(false);
	m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("正在检测"));
	if (isauto)
	{
	//	IGTController *igt = IGTController::getInStance(); 
	//	this->RobotStatusConnect = connect(igt, &IGTController::RobotStatus, this, &HardWareWidget::RobotStatus);
	//	this->NDIStatusConnect = connect(igt, &IGTController::NDIStatus, this, &HardWareWidget::NDIStatus);
	//	igt->Initialize();
	}
	else
	{
	//	IGTController *igt = IGTController::getInStance();
	//	igt->Initialize();
	}
}
void QLinkingHardware::RobotStatus(int i)
{
	//Log::write("QLinkingHardware::RobotStatus::" + QString::number(i));
	MITK_INFO << "QLinkingHardware:" << __func__ + QString::number(i) << ": log";
	staubliStatus = i;
}

void QLinkingHardware::NDIStatus(int i)
{
	//Log::write("QLinkingHardware::NDIStatus:" + QString::number(i));
	MITK_INFO << "QLinkingHardware:" << __func__ + QString::number(i) << ": log";
	ndiStatus = i;
	this->setStartHardware(staubliStatus, ndiStatus);
	emit signalStatus(staubliStatus, ndiStatus);
	if (staubliStatus != 0 || ndiStatus != 0)
	{
		m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("重新检测"));
		m_Controls.pushButton_auto->setEnabled(true);
		isauto = false;
	}
	else
	{
		m_Controls.pushButton_success->setEnabled(true);
		m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("检测成功"));
		disconnect(this->RobotStatusConnect);
		disconnect(this->NDIStatusConnect);
	}
}

void QLinkingHardware::on_pushButton_success_clicked()
{
	//Log::write("QLinkingHardware::on_pushButton_success_clicked");
	MITK_INFO << "QLinkingHardware:" << __func__ << ": log";
	emit signalHardWareFinished();
}
