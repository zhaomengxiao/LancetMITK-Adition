#pragma execution_character_set("utf-8")
// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qt
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
//#include "ReadBackups.h"

#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QRegExpValidator>
#include "QMedicalRecordInfoDialog.h"
#include "Ui_QMedicalRecordInfoDialog.h"
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QJsonValue> 
#include <QJsonObject> 
#include <QJsonDocument>
#include <QJsonParseError>

// lancet
#include <QPluginLoader>
#include <QTimer>
#include <QPluginLoader>
#include <lancetIMedicalRecordsScanner.h>
#include <lancetIMedicalRecordsService.h>
#include <lancetIMedicalRecordsProperty.h>
#include <internal/lancetMedicalRecordsProperty.h>
#include "QMedicalRecordManagement.h"

QString getChinese(QString str)
{
	//    QString str;
	QString chineseStr;
	int nCount = str.count();
	for (int i = 0; i < nCount; i++)
	{
		QChar cha = str.at(i);
		ushort uni = cha.unicode();
		if (uni >= 0x4E00 && uni <= 0x9FA5)
		{
			chineseStr.append(uni);
		}
	}
	return chineseStr;
}
QMedicalRecordInfoDialog::QMedicalRecordInfoDialog(QWidget* parent)
	: QDialog(parent), m_Controls(new Ui::QMedicalRecordInfoDialog)
{
	m_Controls->setupUi(this);
	auto test_dir = QDir(":/org.mitk.lancet.medicalrecordmanagement/");
	QFile qss(test_dir.absoluteFilePath("medicalrecordinfodialog.qss"));
	if (!qss.open(QIODevice::ReadOnly))
	{
		qWarning() << __func__ << __LINE__ << ":" << "error load file "
			<< test_dir.absoluteFilePath("medicalrecordinfodialog.qss") << "\n"
			<< "error: " << qss.errorString();
	}
	// pos
	qInfo() << "log.file.pos " << qss.pos();
	m_Controls->widget->setStyleSheet(QLatin1String(qss.readAll()));
	qss.close();

	QRegExp regx("[1-9][0-9]+$");
	QValidator* validator_patientAge = new QRegExpValidator(regx, m_Controls->lineEdit_patientAge);
	m_Controls->lineEdit_patientAge->setValidator(validator_patientAge);
	connect(m_Controls->pushButton_browse, &QPushButton::clicked, this, &QMedicalRecordInfoDialog::SelectedCaseDirectory);
	connect(m_Controls->pushButton_done, &QPushButton::clicked, this, &QMedicalRecordInfoDialog::SaveCaseDirectory);
	connect(m_Controls->pushButton_cancel, &QPushButton::clicked, this, &QMedicalRecordInfoDialog::Cancel);

}

QMedicalRecordInfoDialog::~QMedicalRecordInfoDialog()
{
	delete m_Controls;
}

QStringList QMedicalRecordInfoDialog::GetKeys() const
{
	return {
		QPropertyKeys::Name,
		QPropertyKeys::Id,
		QPropertyKeys::Sex,
		QPropertyKeys::Age,
		QPropertyKeys::OperatingSurgeonName,
		QPropertyKeys::SurgicalArea,
		QPropertyKeys::DataAddress,
		QPropertyKeys::CreateTime
	};
}
void QMedicalRecordInfoDialog::SetInfo(lancet::IMedicalRecordsProperty* data)
{
	qDebug() << "QMedicalRecordInfoDialog:" << __func__ << ": log"; 
	QStringList listKey = this->GetKeys();
	if (!data)
	{
		return;
	}
	if (!data->GetKeyValue(QPropertyKeys::Valid).toBool())
	{
		// visible: false
	}
	else
	{
		m_Controls->lineEdit_patientName->setText(data->GetKeyValue(listKey[0]).toString());
		m_Controls->lineEdit_patientID->setText(data->GetKeyValue(listKey[1]).toString());
		m_Controls->comboBox_patientGender->setCurrentText(data->GetKeyValue(listKey[2]).toString());
		m_Controls->lineEdit_patientAge->setText(data->GetKeyValue(listKey[3]).toString());
		m_Controls->lineEdit_surgeryOperator->setText(data->GetKeyValue(listKey[4]).toString());
		m_Controls->comboBox_patientGender->setCurrentText(data->GetKeyValue(listKey[5]).toString());
		m_Controls->lineEdit_caseDirectory->setText(data->GetKeyValue(listKey[6]).toString());
	}
}
void QMedicalRecordInfoDialog::SelectedCaseDirectory()
{
	qDebug() << "QMedicalRecordInfoDialog:" << __func__ << ": log";
	QFileDialog* fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle("File");
	fileDialog->setDirectory(".");
	fileDialog->setNameFilter(tr("mitk files(*.mitk);;All files(*.*)"));
	fileDialog->setFileMode(QFileDialog::ExistingFiles);
	fileDialog->setViewMode(QFileDialog::Detail);
	QStringList fileNames;
	if (fileDialog->exec()) {
		fileNames = fileDialog->selectedFiles();
	}
	if (fileNames.size() > 0)
	{
		QString strPath = fileNames.at(0);
		if (!getChinese(strPath).isEmpty())
		{
			QMessageBox::information(nullptr, "提示", "患者数据文件路径包含中文！", "提示");
			return;
		}
		m_Controls->lineEdit_caseDirectory->setText(strPath);
	}
}
void QMedicalRecordInfoDialog::SaveCaseDirectory()
{
	qDebug() << "QMedicalRecordInfoDialog:" << __func__ << ": log";
	if (m_Controls->lineEdit_patientName->text().isEmpty())
	{
		QMessageBox::information(NULL, "提示", "请输入患者姓名！", "关闭");
		return;
	}
	if (m_Controls->lineEdit_patientID->text().isEmpty())
	{
		QMessageBox::information(NULL, "提示", "请输入患者ID！", "关闭");
		return;
	}
	if (m_Controls->lineEdit_patientAge->text().isEmpty())
	{
		QMessageBox::information(NULL, "提示", "请输入患者年龄！", "关闭");
		return;
	}
	if (m_Controls->lineEdit_surgeryOperator->text().isEmpty())
	{
		QMessageBox::information(NULL, "提示", "请输入医生姓名！", "关闭");
		return;
	}
	if (m_Controls->lineEdit_caseDirectory->text().isEmpty())
	{
		QMessageBox::information(NULL, "提示", "请输入患者CT数据地址！", "关闭");
		return;
	}
	
	QStringList listKey = this->GetKeys();
	QDateTime curDateTime = QDateTime::currentDateTime();
	auto service = QMedicalRecordManagement::GetService();
	if (service)
	{
		// create or modify
		if (createNew == true)
		{
			// New
			auto dir = service->GetScanner()->GetDirectory();
			if (dir.exists())
			{
				QString dirName = QString::number(curDateTime.toTime_t());
				if (!dir.exists(dirName))
				{
					dir.mkdir(dirName);
				}
				lancet::MedicalRecordsProperty eproperty;
				QString fileName = dir.absoluteFilePath(dirName+"./test.json");
				QFile file(fileName);
				// Write
				if (!file.open(QIODevice::WriteOnly))
				{
					qDebug() << "can't open error!";
					return;
				}
				eproperty.SetKeyValue(QPropertyKeys::Valid, "true");
				eproperty.SetKeyValue(listKey[0], m_Controls->lineEdit_patientName->text());
				eproperty.SetKeyValue(listKey[1], m_Controls->lineEdit_patientID->text());
				eproperty.SetKeyValue(listKey[2], m_Controls->comboBox_patientGender->currentText());
				eproperty.SetKeyValue(listKey[3], m_Controls->lineEdit_patientAge->text());
				eproperty.SetKeyValue(listKey[4], m_Controls->lineEdit_surgeryOperator->text());
				eproperty.SetKeyValue(listKey[5], m_Controls->comboBox_surgerySide->currentText());
				eproperty.SetKeyValue(listKey[6], m_Controls->lineEdit_caseDirectory->text());
				eproperty.SetKeyValue(listKey[7], curDateTime.toString("yyyy-MM-dd HH:mm:ss"));
				file.write(eproperty.ToJsonString().toLatin1());
				file.close();
			}
		}
		else
		{
			auto select = service->GetSelect();
			if (select.isNull())
			{
				return;
			}
			qDebug() << "" << select->GetDirectory();
			// Modify
			QFile file(select->GetDirectory() + "./test.json");
			select->SetKeyValue(listKey[0], m_Controls->lineEdit_patientName->text());
			select->SetKeyValue(listKey[1], m_Controls->lineEdit_patientID->text());
			select->SetKeyValue(listKey[2], m_Controls->comboBox_patientGender->currentText());
			select->SetKeyValue(listKey[3], m_Controls->lineEdit_patientAge->text());
			select->SetKeyValue(listKey[4], m_Controls->lineEdit_surgeryOperator->text());
			select->SetKeyValue(listKey[5], m_Controls->comboBox_surgerySide->currentText());
			select->SetKeyValue(listKey[6], m_Controls->lineEdit_caseDirectory->text());
			select->SetKeyValue(listKey[7], curDateTime.toString("yyyy-MM-dd HH:mm:ss"));
			// Write
			if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
				qDebug() << "can't open error!";
				return;
			}
			file.write(select->ToJsonString().toLatin1());
			file.close();
		}
		
		this->accept();
	}	
}
void QMedicalRecordInfoDialog::Cancel()
{
	this->close();
}

void QMedicalRecordInfoDialog::clearData()
{
	m_Controls->lineEdit_patientName->setText("");
	m_Controls->lineEdit_patientID->setText("");
	m_Controls->comboBox_patientGender->setCurrentIndex(0);
	m_Controls->lineEdit_patientAge->setText("");
	m_Controls->lineEdit_surgeryOperator->setText("");
	m_Controls->comboBox_surgerySide->setCurrentIndex(0);
	m_Controls->lineEdit_caseDirectory->setText("");
}