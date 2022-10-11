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
#include "QImplantPlanning.h"

// Qt
#include <QDir>
#include <QMessageBox>
#include <QListView>

// mitk image
#include <mitkImage.h>

// THA
//#include "IGTController.h"
//#include "ReadBackups.h"
const std::string QImplantPlanning::VIEW_ID = "org.mitk.views.qimplantplanning";

void QImplantPlanning::SetFocus()
{
}

void QImplantPlanning::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  auto test_dir = QDir(":/org.mitk.lancet.implantplanning/");
  QFile qss(test_dir.absoluteFilePath("implantplanning.qss"));
  if (!qss.open(QIODevice::ReadOnly))
  {
	  qWarning() << __func__ << __LINE__ << ":" << "error load file "
		  << test_dir.absoluteFilePath("implantplanning.qss") << "\n"
		  << "error: " << qss.errorString();
  }
  // pos
  qInfo() << "log.file.pos " << qss.pos();
  m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
  qss.close();

  //connect(IGTController::getInStance(), SIGNAL(signalReadBackups()), this, SLOT(on_signalReadBackups()));
  //connect(IGTController::getInStance(), SIGNAL(signalProsthesisType()), this, SLOT(on_signalProsthesisType()));
  m_Controls.comboBox_ProsthesisType->setView(new QListView());
  m_Controls.comboBox_PlanCupSize->setView(new QListView());
  m_Controls.comboBox_PlanFemoralSize->setView(new QListView());
  m_Controls.comboBox_PlanHeadLength->setView(new QListView());


  m_Controls.comboBox_ProsthesisType->setIconSize(QSize(100, 20));
  m_Controls.comboBox_ProsthesisType->setItemIcon(0, QIcon(test_dir.absoluteFilePath("image/ak_button.png")));
  m_Controls.comboBox_ProsthesisType->setItemIcon(1, QIcon(test_dir.absoluteFilePath("image/chunli.png")));
  m_Controls.comboBox_ProsthesisType->setItemIcon(2, QIcon(test_dir.absoluteFilePath("image/mairui.png")));

  connect((QObject*)(m_Controls.comboBox_ProsthesisType), SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_ProsthesisType_currentIndexChanged(int)));
  connect((QObject*)(m_Controls.comboBox_PlanCupSize), SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_PlanCupSize_currentIndexChanged(int)));
  connect((QObject*)(m_Controls.comboBox_PlanFemoralSize), SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_PlanFemoralSize_currentIndexChanged(int)));
  connect((QObject*)(m_Controls.comboBox_PlanHeadLength), SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_PlanHeadLength_currentIndexChanged(int)));

}

void QImplantPlanning::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}

void QImplantPlanning::on_comboBox_ProsthesisType_currentIndexChanged(int index)
{
	MITK_INFO << "QImplantPlanning:" << __func__ + QString::number(index) << ": log";
	//switch (index)
	//{
	//case 0:
	//	IGTController::ProsthesisType = "AK";
	//	break;
	//case 1:
	//	IGTController::ProsthesisType = "CL";
	//	break;
	//case 2:
	//	IGTController::ProsthesisType = "MR";
	//	break;
	//default:
	//	break;
	//}
	//MITK_INFO << "IGTController::ProsthesisType" << IGTController::ProsthesisType.toStdString();
	//this->PlanFemoralSizeChanged();
	//m_Controls.comboBox_PlanFemoralSize->setCurrentIndex(IGTController::planstemsize - 1);
	//ReadBackups::getInStance()->SetConfigData("Plan_Shell_Transformed", "ProsthesisType", IGTController::ProsthesisType);
}

void QImplantPlanning::on_comboBox_PlanCupSize_currentIndexChanged(int index)
{
	//Log::write("QImplantPlanning::on_comboBox_PlanCupSize_currentIndexChanged::" + QString::number(index));
	MITK_INFO << "QImplantPlanning:" << __func__ + QString::number(index) << ": log";
	//switch (index)
	//{
	//case 0:
	//	IGTController::planshellsize = 44;
	//	break;
	//case 1:
	//	IGTController::planshellsize = 46;
	//	break;
	//case 2:
	//	IGTController::planshellsize = 48;
	//	break;
	//case 3:
	//	IGTController::planshellsize = 50;
	//	break;
	//case 4:
	//	IGTController::planshellsize = 52;
	//	break;
	//case 5:
	//	IGTController::planshellsize = 54;
	//	break;
	//case 6:
	//	IGTController::planshellsize = 56;
	//	break;
	//case 7:
	//	IGTController::planshellsize = 58;
	//	break;
	//case 8:
	//	IGTController::planshellsize = 60;
	//	break;
	//case 9:
	//	IGTController::planshellsize = 62;
	//	break;
	//default:
	//	break;
	//}
	//if (IGTController::planshellsize == 44 || IGTController::planshellsize == 46)
	//{
	//	QVariant v(0);
	//	m_Controls.comboBox_PlanHeadLength->setItemData(3, v, Qt::UserRole - 1);
	//}
	//else
	//{
	//	QVariant v(1 | 32);
	//	m_Controls.comboBox_PlanHeadLength->setItemData(3, v, Qt::UserRole - 1);
	//}
	//IGTController::planlinersize = IGTController::planshellsize - AKLiner;
	//IGTController::getInStance()->SetPlanCupSize();
	//IGTController::getInStance()->SetPlanLinerSize();
	//if (IGTController::isInX)
	//{
	//	IGTController::getInStance()->ModeSurfaceToImage();
	//}
	//ReadBackups::getInStance()->SetConfigData("Plan_Shell_Transformed", "linersize", IGTController::planlinersize);
	//ReadBackups::getInStance()->SetConfigData("Plan_Shell_Transformed", "headsize", IGTController::planheadsize);
	//ReadBackups::getInStance()->SetConfigData("Plan_Shell_Transformed", "shellsize", IGTController::planshellsize);
}

void QImplantPlanning::on_comboBox_PlanFemoralSize_currentIndexChanged(int index)
{
	//Log::write("QImplantPlanning::on_comboBox_PlanFemoralSize_currentIndexChanged::" + QString::number(index));
	MITK_INFO << "QImplantPlanning:" << __func__ + QString::number(index) << ": log";
	//switch (index)
	//{
	//case 0:
	//	IGTController::planstemsize = 1;
	//	break;
	//case 1:
	//	IGTController::planstemsize = 2;
	//	break;
	//case 2:
	//	IGTController::planstemsize = 3;
	//	break;
	//case 3:
	//	IGTController::planstemsize = 4;
	//	break;
	//case 4:
	//	IGTController::planstemsize = 5;
	//	break;
	//case 5:
	//	IGTController::planstemsize = 6;
	//	break;
	//case 6:
	//	IGTController::planstemsize = 7;
	//	break;
	//case 7:
	//	IGTController::planstemsize = 8;
	//	break;
	//case 8:
	//	IGTController::planstemsize = 9;
	//	break;
	//case 9:
	//	IGTController::planstemsize = 10;
	//	break;
	//case 10:
	//	IGTController::planstemsize = 11;
	//	break;
	//default:
	//	break;
	//}
	//IGTController::getInStance()->SetPlanStemSize();
	//if (IGTController::isInX)
	//{
	//	IGTController::getInStance()->ModeSurfaceToImage();
	//}
	//ReadBackups::getInStance()->SetConfigData("Plan_Shell_Transformed", "stemsize", IGTController::planstemsize);
}

void QImplantPlanning::on_comboBox_PlanHeadLength_currentIndexChanged(int index)
{
	//Log::write("QImplantPlanning::on_comboBox_PlanHeadLength_currentIndexChanged" + QString::number(index));
	MITK_INFO << "QImplantPlanning:" << __func__ + QString::number(index) << ": log";
	//switch (index)
	//{
	//case 0:
	//	IGTController::planheadlength = S;
	//	break;
	//case 1:
	//	IGTController::planheadlength = M;
	//	break;
	//case 2:
	//	IGTController::planheadlength = L;
	//	break;
	//case 3:
	//	IGTController::planheadlength = XL;
	//	break;
	//default:
	//	break;
	//}
	//IGTController::getInStance()->SetPlanHeadSize();
	//IGTController::getInStance()->SetPlanStemSize();
	//if (IGTController::isInX)
	//{
	//	IGTController::getInStance()->ModeSurfaceToImage();
	//}
	//ReadBackups::getInStance()->SetConfigData("Plan_Shell_Transformed", "headlength", IGTController::planheadlength);
}

void QImplantPlanning::on_signalReadBackups()
{
	//Log::write("QImplantPlanning::on_signalReadBackups");
	MITK_INFO << "QImplantPlanning:" << __func__ << ": log";
	//QString cupsize = QString::number(IGTController::planshellsize) + " mm";
	//m_Controls.comboBox_PlanCupSize->blockSignals(true);//trueÆÁ±ÎÐÅºÅ
	//m_Controls.comboBox_PlanCupSize->setCurrentText(cupsize);
	//m_Controls.comboBox_PlanCupSize->blockSignals(false);

	//m_Controls.comboBox_PlanFemoralSize->blockSignals(true);//trueÆÁ±ÎÐÅºÅ
	//m_Controls.comboBox_PlanFemoralSize->setCurrentIndex(IGTController::planstemsize - 1);
	//m_Controls.comboBox_PlanFemoralSize->blockSignals(false);

	//QString headsize = QString::number(IGTController::planheadsize) + " mm";
	//m_Controls.comboBox_PlanHeadSize->blockSignals(true);//trueÆÁ±ÎÐÅºÅ
	//m_Controls.comboBox_PlanHeadSize->setCurrentText(headsize);
	//m_Controls.comboBox_PlanHeadSize->blockSignals(false);

	m_Controls.comboBox_PlanHeadLength->blockSignals(true);//trueÆÁ±ÎÐÅºÅ
	//switch (IGTController::planheadlength)
	//{
	//case S:
	//	m_Controls.comboBox_PlanHeadLength->setCurrentIndex(S);
	//	break;
	//case M:
	//	m_Controls.comboBox_PlanHeadLength->setCurrentIndex(M);
	//	break;
	//case L:
	//	m_Controls.comboBox_PlanHeadLength->setCurrentIndex(L);
	//	break;
	//case XL:
	//	m_Controls.comboBox_PlanHeadLength->setCurrentIndex(XL);
	//	break;
	//default:
	//	break;
	//}
	m_Controls.comboBox_PlanHeadLength->blockSignals(false);
}

void QImplantPlanning::on_signalProsthesisType()
{
	m_Controls.comboBox_ProsthesisType->blockSignals(true);//trueÆÁ±ÎÐÅºÅ
	//if (IGTController::ProsthesisType == "AK")
	//{
	//	m_Controls.comboBox_ProsthesisType->setCurrentIndex(0);
	//}
	//else if (IGTController::ProsthesisType == "CL")
	//{
	//	m_Controls.comboBox_ProsthesisType->setCurrentIndex(1);
	//}
	//else if (IGTController::ProsthesisType == "MR")
	//{
	//	m_Controls.comboBox_ProsthesisType->setCurrentIndex(2);
	//}
	m_Controls.comboBox_ProsthesisType->blockSignals(false);
	this->PlanFemoralSizeChanged();
}

void QImplantPlanning::PlanFemoralSizeChanged()
{
	m_Controls.comboBox_PlanFemoralSize->blockSignals(true);//trueÆÁ±ÎÐÅºÅ
	m_Controls.comboBox_PlanFemoralSize->clear();
	//if (IGTController::ProsthesisType == "AK")
	//{
	//	for (int i = 0; i < 11; i++)
	//	{
	//		m_Controls.comboBox_PlanFemoralSize->insertItem(i, QString::number(i + 1));
	//	}
	//}
	//else if (IGTController::ProsthesisType == "CL")
	//{
	//	for (int i = 0; i < 9; i++)
	//	{
	//		m_Controls.comboBox_PlanFemoralSize->insertItem(i, QString::number(i + 8));
	//	}
	//}
	//else if (IGTController::ProsthesisType == "MR")
	//{
	//	//m_Controls.comboBox_ProsthesisType->setCurrentIndex(2);
	//}
	m_Controls.comboBox_PlanFemoralSize->blockSignals(false);
}
