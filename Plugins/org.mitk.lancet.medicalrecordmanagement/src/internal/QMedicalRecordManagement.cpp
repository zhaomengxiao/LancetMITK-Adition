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
#include <mitkIOUtil.h>
#include <mitkProgressBar.h>
#include <mitkIDataStorageService.h>
#include <mitkProgressBarImplementation.h>

#include <lancetIMedicalRecordsScanner.h>
#include <lancetIMedicalRecordsProperty.h>
#include <lancetIMedicalRecordsService.h>

#include "org_mitk_lancet_medicalrecordmanagement_Activator.h"

// THA
#include "QMedicalRecordInfoDialog.h"
#include "QMedicalRecordManagement.h"
#include "QMedicalRecordLoaderDialog.h"
const std::string QMedicalRecordManagement::VIEW_ID = "org.mitk.views.qmedicalrecordmanagement";


mitk::IDataStorageService* GetDataStorageService()
{
	auto context = mitk::PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<mitk::IDataStorageService>();
	return context->getService<mitk::IDataStorageService>(serviceRef);
}

void QMedicalRecordManagement::SetFocus()
{
}
QMedicalRecordManagement::~QMedicalRecordManagement()
{

}
lancet::IMedicalRecordsService* QMedicalRecordManagement::GetService()
{
	auto context = mitk::PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IMedicalRecordsService>();
	return context->getService<lancet::IMedicalRecordsService>(serviceRef);
}
void QMedicalRecordManagement::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent); 
  
  auto test_dir = QDir(":/org.mitk.lancet.medicalrecordmanagement/");
  QFile qss(test_dir.absoluteFilePath("medicalrecordmanagement.qss"));
  if (!qss.open(QIODevice::ReadOnly))
  {
	  MITK_WARN << "error load file "
		  << test_dir.absoluteFilePath("medicalrecordmanagement.qss") << "\n"
		  << "error: " << qss.errorString();
  }
  // pos
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

	auto service = this->GetService();
	auto dataStorageService = GetDataStorageService();

	if (service && false == service->GetSelect().isNull())
	{
		MITK_ERROR << "log.debug.println " << service->GetSelect()->ToString().toStdString();
	}

	if (dataStorageService)
	{
		mitk::IDataStorageReference::Pointer dataStorage;
		for (auto& dataStorageItem : dataStorageService->GetDataStorageReferences())
		{
			if (dataStorageItem.IsNotNull() && dataStorageItem->GetLabel() == "THA-DataStorage")
			{
				dataStorage = dataStorageItem;
				break;
			}
		}

		if (dataStorage.IsNotNull())
		{
			dataStorageService->RemoveDataStorageReference(dataStorage);
		}

		dataStorage = dataStorageService->CreateDataStorage("THA-DataStorage");
		dataStorageService->SetActiveDataStorage(dataStorage);

		QMedicalRecordLoaderDialog loader;
		loader.SetMedicalRecordProperty(service->GetSelect());
		loader.SetDataStorage(dataStorage->GetDataStorage());
		loader.exec();
	}
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