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
#include "QDisclaimerAgreement.h"

// Qt
#include <QDir>
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

// THA
//#include "Log.h"
//#include "IGTController.h"
const std::string QDisclaimerAgreement::VIEW_ID = "org.mitk.views.qdisclaimeragreement";

void QDisclaimerAgreement::SetFocus()
{
}

void QDisclaimerAgreement::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);  
  auto test_dir = QDir(":/org.mitk.lancet.disclaimeragreement/");
  QFile qss(test_dir.absoluteFilePath("disclaimeragreement.qss"));
  if (!qss.open(QIODevice::ReadOnly))
  {
	  qWarning() << __func__ << __LINE__ << ":" << "error load file "
		  << test_dir.absoluteFilePath("disclaimeragreement.qss") << "\n"
		  << "error: " << qss.errorString();
  }
  // pos
  qInfo() << "log.file.pos " << qss.pos();
  m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
  qss.close(); 
  
  connect(m_Controls.pushButtonAgree, &QPushButton::clicked, this, &QDisclaimerAgreement::onButtonAgree_clicked);
}

void QDisclaimerAgreement::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}
void QDisclaimerAgreement::onButtonAgree_clicked(bool checked)
{
	//Log::write("QDisclaimerAgreement::on_pushButtonAgree_clicked");
	MITK_INFO << "QDisclaimerAgreement:" << __func__ << ": log";
	Q_UNUSED(checked);
	m_Controls.pushButtonAgree->setText("");
	m_Controls.pushButtonAgree->setDisabled(true);
	emit signalHardWare();
}
void QDisclaimerAgreement::on_signalOpen(QString name, QString id)
{
	//Log::write("QDisclaimerAgreement::on_signalOpen");
	MITK_INFO << "QDisclaimerAgreement:" << __func__ << ": log";
	m_Controls.label_14->setText(name);
	m_Controls.label_15->setText(id);
	m_Controls.label_14->update();
	m_Controls.label_15->update();
	//if (IGTController::side == RIGHT)
	//{
	//	m_Controls.label_16->setText("ÓÒ÷Å¾Ê");
	//}
	//else
	//{
	//	m_Controls.label_16->setText("×ó÷Å¾Ê");
	//}
	m_Controls.label_16->update();
}