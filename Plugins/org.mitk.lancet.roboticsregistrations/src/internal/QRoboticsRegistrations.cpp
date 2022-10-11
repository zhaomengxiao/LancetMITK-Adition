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
#include "QRoboticsRegistrations.h"

// Qt
#include <QDir>
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

// THA
//#include "IGTController.h"

const std::string QRoboticsRegistrations::VIEW_ID = "org.mitk.views.qroboticsregistrations";

void QRoboticsRegistrations::SetFocus()
{
}

void QRoboticsRegistrations::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  auto test_dir = QDir(":/org.mitk.lancet.roboticsregistrations/");
  QFile qss(test_dir.absoluteFilePath("roboticsregistrations.qss"));
  if (!qss.open(QIODevice::ReadOnly))
  {
	  qWarning() << __func__ << __LINE__ << ":" << "error load file "
		  << test_dir.absoluteFilePath("roboticsregistrations.qss") << "\n"
		  << "error: " << qss.errorString();
  }
  // pos
  qInfo() << "log.file.pos " << qss.pos();
  m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
  qss.close();

  connect((QObject*)(m_Controls.comboBox_ToolType), SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_ToolType_currentIndexChanged(int)));
  connect((QObject*)(m_Controls.comboBox_TCPType), SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_TCPType_currentIndexChanged(int)));
  connect(m_Controls.pushButtonAddPoint, &QPushButton::clicked, this, &QRoboticsRegistrations::on_pushButtonAddPoint_clicked);
  connect(m_Controls.pushButtonCalResult, &QPushButton::clicked, this, &QRoboticsRegistrations::on_pushButtonCalResult_clicked);
  connect(m_Controls.pushButtonRobotVerify, &QPushButton::clicked, this, &QRoboticsRegistrations::on_pushButtonRobotVerify_clicked);
 
}

void QRoboticsRegistrations::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}
void QRoboticsRegistrations::on_signalReadBackups()
{
	//Log::write("QRoboticsRegistrations::on_signalReadBackups::");
	MITK_INFO << "QRoboticsRegistrations:" << __func__ << ": log";
	m_Controls.comboBox_ToolType->blockSignals(true);//true屏蔽信号
	//switch (IGTController::ToolTcpType)
	//{
	//case TOOL21001:
	//	m_Controls.comboBox_ToolType->setCurrentIndex(0);
	//	break;
	//case TOOL21002:
	//	m_Controls.comboBox_ToolType->setCurrentIndex(1);
	//	break;
	//case TOOL21003:
	//	m_Controls.comboBox_ToolType->setCurrentIndex(2);
	//	break;
	//case TOOL21004:
	//	m_Controls.comboBox_ToolType->setCurrentIndex(3);
	//	break;
	//case TOOL21005:
	//	m_Controls.comboBox_ToolType->setCurrentIndex(4);
	//	break;
	//default:
	//	break;
	//}
	m_Controls.comboBox_ToolType->blockSignals(false);
}
void QRoboticsRegistrations::on_comboBox_ToolType_currentIndexChanged(int index)
{
	//Log::write("QRoboticsRegistrations::on_comboBox_ToolType_currentIndexChanged::" + QString::number(index));
	MITK_INFO << "QRoboticsRegistrations:" << __func__ + QString::number(index) << ": log";
	//switch (index)
	//{
	//case 0:
	//	IGTController::ToolTcpType = TOOL21001;
	//	break;
	//case 1:
	//	IGTController::ToolTcpType = TOOL21002;
	//	break;
	//case 2:
	//	IGTController::ToolTcpType = TOOL21003;
	//	break;
	//case 3:
	//	IGTController::ToolTcpType = TOOL21004;
	//	break;
	//case 4:
	//	IGTController::ToolTcpType = TOOL21005;
	//	break;
	//default:
	//	break;
	//}

	//IGTController::getInStance()->SetRobotTCP();
}
void QRoboticsRegistrations::on_comboBox_TCPType_currentIndexChanged(int index)
{
	//Log::write("QRoboticsRegistrations::on_comboBox_TCPType_currentIndexChanged::" + QString::number(index));
	MITK_INFO << "QRoboticsRegistrations:" << __func__ + QString::number(index) << ": log";
	//switch (index)
	//{
	//case 0:
	//{
	//	IGTController::RobotTcpType = LREAMERSTRAIGHT;

	//	if (IGTController::SideTcpType == RIGHT)
	//	{
	//		IGTController::RobotTcpType = RREAMERSTRAIGHT;
	//	}
	//}
	//break;
	//case 1:
	//{
	//	IGTController::RobotTcpType = LREAMERBEND;

	//	if (IGTController::SideTcpType == RIGHT)
	//	{
	//		IGTController::RobotTcpType = RREAMERBEND;
	//	}
	//}
	//break;
	//default:
	//	break;
	//}
	//IGTController::getInStance()->SetRobotTCP();
}
void QRoboticsRegistrations::on_pushButtonAddPoint_clicked()
{
	//Log::write("SurgicalRegistrationWidget::on_pushButtonAddPoint_clicked");
	MITK_INFO << "QRoboticsRegistrations:" << __func__ << ": log";
	//if (IGTController::Tool_maker == true && IGTController::Robot_maker == true)
	//{
	//	int value = IGTController::getInStance()->AddRobotMovePoint();
	//	m_Controls.rioProgressBar->setValue(value * 10);
	//	if (value == 10)
	//	{
	//		//IGTController::getInStance()->CalRobotMoveResult();
	//		m_Controls.pushButtonAddPoint->setEnabled(false);
	//		m_Controls.pushButtonCalResult->setEnabled(true);
	//	}
	//}
	//else
	//{
	//	QMessageBox::information(NULL, "错误", "请检查marker是否在视野中!", "关闭");
	//}
}
void QRoboticsRegistrations::on_pushButtonCalResult_clicked()
{
	//Log::write("QRoboticsRegistrations::on_pushButtonCalResult_clicked");
	MITK_INFO << "QRoboticsRegistrations:" << __func__ << ": log";
	//IGTController::getInStance()->CalRobotMoveResult();
	m_Controls.pushButtonAddPoint->setEnabled(true);
	m_Controls.pushButtonCalResult->setEnabled(false);
}
void QRoboticsRegistrations::on_pushButtonRobotVerify_clicked()
{
	//Log::write("QRoboticsRegistrations::on_pushButtonRobotVerify_clicked");
	MITK_INFO << "QRoboticsRegistrations:" << __func__ << ": log";
	emit signalAction(RIO_REGISTRATION);
	emit signalAction(PROBE_CHECKPOINT);
	emit signalAction(FEMORAL_CHECKPOINT);
	emit signalAction(PELVIS_CHECKPOINT);
	emit signalAction(PELVIS_LANDMARK);
	//emit signalAction(PELVIS_REGISTRATION);
	//IGTController* igt = IGTController::getInStance();
	//if (igt->Tool_maker == true && igt->Robot_maker == true)
	//{
	//	double rms = igt->VerifyRRTC();
	//	m_Controls.lcdNumber_RIO->display(QString::number(rms, 10, 3));
	//}
	//else
	//{
	//	QMessageBox::information(NULL, "错误", "请检查marker是否在视野中!", "关闭");
	//}
}
