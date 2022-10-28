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
#include "QMedicalRecordManagement.h"

// Qt
#include <QDir>
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

#include <lancetIMedicalRecordsScanner.h>
#include <lancetIMedicalRecordsProperty.h>
#include <lancetIMedicalRecordsAdministrationService.h>

#include "org_mitk_lancet_medicalrecordmanagement_Activator.h"

// THA
#include "QMedicalRecordInfoDialog.h"
#include "QMedicalRecordManagement.h"
const std::string QMedicalRecordManagement::VIEW_ID = "org.mitk.views.qmedicalrecordmanagement";

void QMedicalRecordManagement::SetFocus()
{
}
QMedicalRecordManagement::~QMedicalRecordManagement()
{

}
lancet::IMedicalRecordsAdministrationService* QMedicalRecordManagement::GetService()
{
	auto context = mitk::PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IMedicalRecordsAdministrationService>();
	return context->getService<lancet::IMedicalRecordsAdministrationService>(serviceRef);
}
void QMedicalRecordManagement::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent); 
  
  auto test_dir = QDir(":/org.mitk.lancet.medicalrecordmanagement/");
  QFile qss(test_dir.absoluteFilePath("medicalrecordmanagement.qss"));
  if (!qss.open(QIODevice::ReadOnly))
  {
	  qWarning() << __func__ << __LINE__ << ":" << "error load file "
		  << test_dir.absoluteFilePath("medicalrecordmanagement.qss") << "\n"
		  << "error: " << qss.errorString();
  }
  // pos
  qInfo() << "log.file.pos " << qss.pos();
  m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
  qss.close();
  this->ConnectToService(); 
  connect(m_Controls.toolButton_new, &QToolButton::clicked, this, &QMedicalRecordManagement::NewMedicalRecordInfoDialog);
  connect(m_Controls.toolButton_edit, &QToolButton::clicked, this, &QMedicalRecordManagement::ModifMedicalRecordInfoDialog);
  connect(m_Controls.toolButton_open, &QToolButton::clicked, this, &QMedicalRecordManagement::OpenMedicalRecord);
  connect(m_Controls.toolButton_close, &QToolButton::clicked, this, &QMedicalRecordManagement::CloseMedicalRecord);
  m_MedicalRecordInfoDialog = new QMedicalRecordInfoDialog();
  m_MedicalRecordInfoDialog->setWindowModality(Qt::ApplicationModal);
}

void QMedicalRecordManagement::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}

void QMedicalRecordManagement::DoImageProcessing()
{
  QList<mitk::DataNode::Pointer> nodes = this->GetDataManagerSelection();
}

void QMedicalRecordManagement::ConnectToService()
{
	auto sender = this->GetService();
	if (sender)
	{
		lancet::IMedicalRecordsAdministrationService* o = sender;
		QObject::connect(o, &lancet::IMedicalRecordsAdministrationService::MedicalRecordsPropertySelect,
			this, &QMedicalRecordManagement::Slot_MedicalRecordsPropertySelect);
	}
}

void QMedicalRecordManagement::DisConnectToService()
{
	auto sender = this->GetService();
	if (sender)
	{
		lancet::IMedicalRecordsAdministrationService* o = sender;
		QObject::disconnect(o, &lancet::IMedicalRecordsAdministrationService::MedicalRecordsPropertySelect,
			this, &QMedicalRecordManagement::Slot_MedicalRecordsPropertySelect);
	}
}

void QMedicalRecordManagement::Slot_MedicalRecordsPropertySelect(lancet::IMedicalRecordsProperty* data)
{
  qDebug() << __FUNCTION__ << "\n"
           << "data " << data->ToString();
	if (data)
	{
		data->ResetPropertyOfModify();
		m_MedicalRecordInfoDialog->SetInfo(data);
	}
}
void QMedicalRecordManagement::NewMedicalRecordInfoDialog()
{
	MITK_INFO << "QMedicalRecordManagement:" << __func__ << ": log";
	m_MedicalRecordInfoDialog->show();
	m_MedicalRecordInfoDialog->setWindowTitle("Create A New MedicalRecord");
	m_MedicalRecordInfoDialog->clearData();
	m_MedicalRecordInfoDialog->createNew = true;
	if (QDialog::Rejected == m_MedicalRecordInfoDialog->exec())
	{
		return;
	}
}
void QMedicalRecordManagement::ModifMedicalRecordInfoDialog()
{
	MITK_INFO << "QMedicalRecordManagement:" << __func__ << ": log";
	auto select = QMedicalRecordManagement::GetService()->GetSelect();
	if (select.isNull())
	{
		QMessageBox::information(nullptr, "warning", "Please select a MedicalRecord first!", "close");
		return;
	}
	m_MedicalRecordInfoDialog->show();
	m_MedicalRecordInfoDialog->setWindowTitle("Modif A MedicalRecord");
	m_MedicalRecordInfoDialog->createNew = false;
	int ret = m_MedicalRecordInfoDialog->exec();
	if (ret == QDialog::Rejected)
	{
		return;
	}
	
}
void QMedicalRecordManagement::OpenMedicalRecord()
{
	MITK_INFO << "QMedicalRecordManagement:" << __func__ << ": log";
	this->setCaseOpened(false);
}
void QMedicalRecordManagement::CloseMedicalRecord()
{
	MITK_INFO << "QMedicalRecordManagement:" << __func__ << ": log";
}
void QMedicalRecordManagement::setCaseOpened(bool b)
{
	//Update Table
	//this->ui->tableWidget->setEnabled(!b);
	//this->ui->tableWidget->clearSelection();

	//Enable + Disable Button
	m_Controls.toolButton_new->setEnabled(!b);
	m_Controls.toolButton_open->setEnabled(!b);
	m_Controls.toolButton_edit->setEnabled(!b);
	m_Controls.toolButton_close->setEnabled(b);
	isClose = b;
}