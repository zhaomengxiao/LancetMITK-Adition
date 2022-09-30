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
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

#include <berryIQtStyleManager.h>
#include "org_mitk_lancet_linkinghardware_Activator.h"

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
  m_Controls.pushButton_success->setEnabled(true);
  QFile qss(":/resources/linkinghardware.qss");
  qApp->setStyleSheet(qss.readAll());
  qss.close();
}
void QLinkingHardware::setStartHardware(int staubli, int ndi)
{
	QString str = "--staubli::" + QString::number(staubli) + "--ndi::" + QString::number(ndi);
	//Log::write("HardWareWidget::setStartHardware" + str);
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
	//Log::write("HardWareWidget::on_pushButton_auto_clicked");
	m_Controls.pushButton_auto->setEnabled(false);
	m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("正在检测"));
	//if (isauto)
	//{
	//	IGTController *igt = IGTController::getInStance(); 
	//	this->RobotStatusConnect = connect(igt, &IGTController::RobotStatus, this, &HardWareWidget::RobotStatus);
	//	this->NDIStatusConnect = connect(igt, &IGTController::NDIStatus, this, &HardWareWidget::NDIStatus);
	//	igt->Initialize();
	//}
	//else
	//{
	//	IGTController *igt = IGTController::getInStance();
	//	igt->Initialize();
	//}
}
void QLinkingHardware::RobotStatus(int i)
{
	//Log::write("HardWareWidget::RobotStatus::" + QString::number(i));
	staubliStatus = i;
}

void QLinkingHardware::NDIStatus(int i)
{
	//Log::write("HardWareWidget::NDIStatus:" + QString::number(i));
	//ndiStatus = i;
	//this->setStartHardware(staubliStatus, ndiStatus);
	//emit signalStatus(staubliStatus, ndiStatus);
	//if (staubliStatus != 0 || ndiStatus != 0)
	//{
	//	m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("重新检测"));
	//	m_Controls.pushButton_auto->setEnabled(true);
	//	isauto = false;
	//}
	//else
	//{
	//	m_Controls.pushButton_success->setEnabled(true);
	//	m_Controls.pushButton_auto->setText(QString::fromLocal8Bit("检测成功"));
	//	disconnect(this->RobotStatusConnect);
	//	disconnect(this->NDIStatusConnect);
	//}
}

void QLinkingHardware::on_pushButton_success_clicked()
{
	//Log::write("HardWareWidget::on_pushButton_success_clicked");
	emit signalHardWareFinished();
}
