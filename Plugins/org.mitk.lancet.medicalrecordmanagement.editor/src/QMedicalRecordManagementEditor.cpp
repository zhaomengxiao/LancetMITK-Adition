/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "QMedicalRecordManagementEditor.h"
#include "internal/org_mitk_lancet_medicalrecordmanagement_editor_Activator.h"

#include <berryUIException.h>
#include <berryIWorkbenchPage.h>
#include <berryIPreferencesService.h>
#include <berryIPreferences.h>

#include <mitkColorProperty.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateProperty.h>

#include <mitkDataStorageEditorInput.h>
#include <mitkIDataStorageService.h>

// mitk qt widgets module
#include <QmitkInteractionSchemeToolBar.h>
#include <QmitkLevelWindowWidget.h>
#include <QmitkRenderWindowWidget.h>
#include <QmitkStdMultiWidget.h>

#include <ctkServiceTracker.h>

// mitk gui qt common plugin
#include <QmitkMultiWidgetDecorationManager.h>

// Qt
#include <QMessageBox>
#include <QDir>

// lancet
#include <lancetIMedicalRecordsScanner.h>
#include <lancetIMedicalRecordsProperty.h>
#include <lancetIMedicalRecordsService.h>

const QString QMedicalRecordManagementEditor::EDITOR_ID = "org_mitk_lancet_medicalrecordmanagement_editor";

struct QMedicalRecordManagementEditor::QMedicalRecordManagementEditorPrivateImp
{
	QMap<QString, int> tableDataSheet;
};
//////////////////////////////////////////////////////////////////////////
// QMedicalRecordManagementEditor
//////////////////////////////////////////////////////////////////////////
QMedicalRecordManagementEditor::QMedicalRecordManagementEditor()
  : berry::EditorPart()
	, imp(std::make_shared<QMedicalRecordManagementEditorPrivateImp>())
	, widgetInstace(nullptr)
	, m_Prefs(nullptr)
	, m_PrefServiceTracker(org_mitk_lancet_medicalrecordmanagement_editor_Activator::GetContext())
{
  // nothing here
	m_PrefServiceTracker.open();
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
}

QMedicalRecordManagementEditor::~QMedicalRecordManagementEditor()
{
	GetSite()->GetPage()->RemovePartListener(this);
	if (this->GetService())
	{
		this->GetService()->Stop();
		this->DisConnectToService();
	}
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
}

void QMedicalRecordManagementEditor::CreatePartControl(QWidget* parent)
{
	m_Controls.setupUi(parent);
	auto test_dir = QDir(":/org.mitk.lancet.medicalrecordmanagementeditor/");
	QFile qss(test_dir.absoluteFilePath("medicalrecordmanagementeditor.qss"));
	if (!qss.open(QIODevice::ReadOnly))
	{
		qWarning() << __func__ << __LINE__ << ":" << "error load file "
			<< test_dir.absoluteFilePath("medicalrecordmanagementeditor.qss") << "\n"
			<< "error: " << qss.errorString();
	}
	// pos
	qInfo() << "log.file.pos " << qss.pos();
	m_Controls.tableWidget->setStyleSheet(QLatin1String(qss.readAll()));
	qss.close();

	connect(m_Controls.tableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(ItemClick(QTableWidgetItem*)));
	if (this->GetService())
	{
		this->ConnectToService();
		this->GetService()->Start();
	}
}
void QMedicalRecordManagementEditor::SetFocus()
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	if (widgetInstace.isNull() == false)
	{
		widgetInstace->setFocus();
	}
}

void QMedicalRecordManagementEditor::DoSave()
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
}

void QMedicalRecordManagementEditor::DoSaveAs()
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
}

bool QMedicalRecordManagementEditor::IsSaveOnCloseNeeded() const
{
	return true;
}

bool QMedicalRecordManagementEditor::IsDirty() const
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	return true;
}

bool QMedicalRecordManagementEditor::IsSaveAsAllowed() const
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	return true;
}

berry::IPreferences::Pointer QMedicalRecordManagementEditor::GetPreferences() const
{
	berry::IPreferencesService* prefService = m_PrefServiceTracker.getService();
	if (prefService != nullptr)
	{
		return prefService->GetSystemPreferences()->Node(GetSite()->GetId());
	}
	return berry::IPreferences::Pointer(nullptr);
}

void QMedicalRecordManagementEditor::Init(berry::IEditorSite::Pointer site, berry::IEditorInput::Pointer input)
{
	//editorSite = site;
	//editorInput = input;
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	if (input.Cast<mitk::DataStorageEditorInput>().IsNull())
		throw berry::PartInitException("Invalid Input: Must be mitk::DataStorageEditorInput");

	this->SetSite(site);
	this->SetInput(input);

	m_Prefs = this->GetPreferences().Cast<berry::IBerryPreferences>();
	if (m_Prefs.IsNotNull())
	{
		m_Prefs->OnChanged.AddListener(berry::MessageDelegate1<QMedicalRecordManagementEditor, const berry::IBerryPreferences*>
			(this, &QMedicalRecordManagementEditor::OnPreferencesChanged));
	}

	// open service
	if (this->GetService())
	{
		auto scanner = this->GetService()->GetScanner();
		if (scanner.IsNotNull())
		{
			QDir workDir(QDir::currentPath());
			workDir.cd("..");
			workDir.cd("..");
			workDir.cd("bin");
			workDir.cd("planning");

			qDebug() << "scanner dir " << workDir.absolutePath();

			scanner->SetInterval(200);
			scanner->SetDirectory(workDir);
			scanner->SetRunTimeMode(lancet::IMedicalRecordsScanner::Thread);
		}
	}
}

berry::IPartListener::Events::Types QMedicalRecordManagementEditor::GetPartEventTypes() const
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	return Events::CLOSED | Events::OPENED | Events::HIDDEN | Events::VISIBLE;
}

lancet::IMedicalRecordsService* QMedicalRecordManagementEditor::GetService() const
{
	auto context = PluginActivator::GetContext();
	auto serviceRef = context->getServiceReference<lancet::IMedicalRecordsService>();
	return context->getService<lancet::IMedicalRecordsService>(serviceRef);
}

void QMedicalRecordManagementEditor::ConnectToService()
{
	auto sender = this->GetService();
	if (sender)
	{
		lancet::IMedicalRecordsService* o = sender;
		QObject::connect(o, &lancet::IMedicalRecordsService::MedicalRecordsPropertyTrace,
			this, &QMedicalRecordManagementEditor::Slot_MedicalRecordsPropertyTrace);
		QObject::connect(o, &lancet::IMedicalRecordsService::MedicalRecordsPropertyModify,
			this, &QMedicalRecordManagementEditor::Slot_MedicalRecordsPropertyModify);
		QObject::connect(o, &lancet::IMedicalRecordsService::MedicalRecordsPropertyDelete,
			this, &QMedicalRecordManagementEditor::Slot_MedicalRecordsPropertyDelete);
	}
}

void QMedicalRecordManagementEditor::DisConnectToService()
{
	auto sender = this->GetService();
	if (sender)
	{
		lancet::IMedicalRecordsService* o = sender;
		QObject::disconnect(o, &lancet::IMedicalRecordsService::MedicalRecordsPropertyTrace,
			this, &QMedicalRecordManagementEditor::Slot_MedicalRecordsPropertyTrace);
		QObject::disconnect(o, &lancet::IMedicalRecordsService::MedicalRecordsPropertyModify,
			this, &QMedicalRecordManagementEditor::Slot_MedicalRecordsPropertyModify);
		QObject::disconnect(o, &lancet::IMedicalRecordsService::MedicalRecordsPropertyDelete,
			this, &QMedicalRecordManagementEditor::Slot_MedicalRecordsPropertyDelete);
	}
}

QStringList QMedicalRecordManagementEditor::GetTableWidgetHeaders() const
{
	return {
		QPropertyKeys::Name,
		QPropertyKeys::Id,
		QPropertyKeys::Sex,
		QPropertyKeys::Age,
		QPropertyKeys::OperatingSurgeonName,
		QPropertyKeys::SurgicalArea,
		QPropertyKeys::CreateTime
	};
}

void QMedicalRecordManagementEditor::ItemClick(QTableWidgetItem* item)
{
	QList<QTableWidgetItem*> list_item = m_Controls.tableWidget->selectedItems();
	if (!list_item.empty())
	{
		int rowindex = m_Controls.tableWidget->item(list_item.at(0)->row(), 0)->text().toInt();
		
		auto validPropertyArray = this->GetService()->GetScanner()->GetValidPropertyArray();
		if (validPropertyArray.size() > rowindex)
		{
			this->GetService()->SetSelect(validPropertyArray[rowindex]);
		}
	}
}
void QMedicalRecordManagementEditor::CreateTableItem(int row,
	lancet::IMedicalRecordsProperty* data)
{
	if (!data && row < 0)
	{
		return;
	}
	if ((row + 1) >= this->m_Controls.tableWidget->rowCount())
	{
		this->m_Controls.tableWidget->setRowCount((row + 1));
	}
	// number
	QTableWidgetItem* cell_row_0 = this->m_Controls.tableWidget->item(row, 0);
	if (!cell_row_0)
	{
		cell_row_0 = new QTableWidgetItem();
		cell_row_0->setTextAlignment(Qt::AlignCenter);
		cell_row_0->setBackgroundColor(QColor(16, 16, 16));
		this->m_Controls.tableWidget->setItem(row, 0, cell_row_0);
	}
	cell_row_0->setText(QString::number(row));
	// data items
	QStringList listKey = this->GetTableWidgetHeaders();
	for (auto index = 0; index < listKey.size(); ++index)
	{
		QTableWidgetItem* cell_row_index = this->m_Controls.tableWidget->item(row, index + 1);
		if (!cell_row_index)
		{
			cell_row_index = new QTableWidgetItem();
			cell_row_index->setTextAlignment(Qt::AlignCenter);
			cell_row_index->setBackgroundColor(QColor(16, 16, 16));
			this->m_Controls.tableWidget->setItem(row, index + 1, cell_row_index);
		}
		cell_row_index->setText(data->GetKeyValue(listKey[index]).toString());
	}
}

void QMedicalRecordManagementEditor::ModifyTableItem(int row, 
	lancet::IMedicalRecordsProperty* data)
{
	if (!data || (row + 1) >= this->m_Controls.tableWidget->rowCount() &&
		row < 0)
	{
		return;
	}
	if (!data->GetKeyValue(QPropertyKeys::Valid).toBool())
	{
		// visible: false
		this->m_Controls.tableWidget->removeRow(row);
	}
	else
	{
		// items modify
		QStringList listKey = this->GetTableWidgetHeaders();
		for (auto index = 0; index < listKey.size(); ++index)
		{
			QTableWidgetItem* cell_row_index = this->m_Controls.tableWidget->item(row, index + 1);
			if (cell_row_index)
			{
				cell_row_index->setText(data->GetKeyValue(listKey[index]).toString());
			}
		}
	}
}

void QMedicalRecordManagementEditor::Slot_MedicalRecordsPropertyTrace(
	int index, lancet::IMedicalRecordsProperty* data, bool valid)
{
	qDebug() << __FUNCTION__ << "\n"
		<< "index: " << index << "\n"
		<< "valid: " << valid << "\n"
		<< "data: " << data->ToString();
	if (data && valid)
	{
		if (data->GetKeyValue(QPropertyKeys::Valid).toBool())
		{
			// Visible: true
			this->CreateTableItem(index, data);
		}
		data->ResetPropertyOfModify();
	}
}

void QMedicalRecordManagementEditor::Slot_MedicalRecordsPropertyModify(
	int index, lancet::IMedicalRecordsProperty* data, bool valid)
{
	qDebug() << __FUNCTION__ << "\n"
		<< "index: " << index << "\n"
		<< "valid: " << valid << "\n"
		<< "data: " << data->ToString();
	if (data && valid)
	{
		this->ModifyTableItem(index, data);
		data->ResetPropertyOfModify();
	}
}

void QMedicalRecordManagementEditor::Slot_MedicalRecordsPropertyDelete(
	int index, lancet::IMedicalRecordsProperty* data, bool valid)
{
	qDebug() << __FUNCTION__ << "\n"
		<< "index: " << index << "\n"
		<< "valid: " << valid << "\n"
		<< "data: " << data->ToString();
	if (data && valid)
	{
		this->ModifyTableItem(index, data);
		data->ResetPropertyOfModify();
	}
}

void QMedicalRecordManagementEditor::OnPreferencesChanged(const berry::IBerryPreferences*)
{
}
