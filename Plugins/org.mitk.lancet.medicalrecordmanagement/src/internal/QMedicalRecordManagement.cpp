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
#include <lancetIMedicalRecordsService.h>

#include "org_mitk_lancet_medicalrecordmanagement_Activator.h"

const std::string QMedicalRecordManagement::VIEW_ID = "org.mitk.views.qmedicalrecordmanagement";

void QMedicalRecordManagement::SetFocus()
{
}
QMedicalRecordManagement::~QMedicalRecordManagement()
{

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

lancet::IMedicalRecordsService* QMedicalRecordManagement::GetService() const
{
  auto context = mitk::PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IMedicalRecordsService>();
  return context->getService<lancet::IMedicalRecordsService>(serviceRef);
}

void QMedicalRecordManagement::ConnectToService()
{
	auto sender = this->GetService();
	if (sender)
	{
		lancet::IMedicalRecordsService* o = sender;
		QObject::connect(o, &lancet::IMedicalRecordsService::MedicalRecordsPropertySelect,
			this, &QMedicalRecordManagement::Slot_MedicalRecordsPropertySelect);
	}
}

void QMedicalRecordManagement::DisConnectToService()
{
	auto sender = this->GetService();
	if (sender)
	{
		lancet::IMedicalRecordsService* o = sender;
		QObject::disconnect(o, &lancet::IMedicalRecordsService::MedicalRecordsPropertySelect,
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
	}
}