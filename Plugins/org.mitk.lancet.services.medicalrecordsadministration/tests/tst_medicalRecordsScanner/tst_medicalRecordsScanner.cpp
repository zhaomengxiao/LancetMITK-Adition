#include <QTest>
#include <QTimer>
#include <QPluginLoader>

#include <internal/lancetMedicalRecordsScanner.h>
#include <internal/lancetMedicalRecordsProperty.h>
#include <lancetIMedicalRecordsProperty.h>

class tst_medicalRecordsScanner
	: public QObject
{
	Q_OBJECT
public:

#ifdef MITK_BUILD_TESTTING
	void tst_medicalRecordsScannerEfficiency(int count)
	{
		auto dir = QDir(TEST_DATAS_DIR);
		lancet::MedicalRecordsProperty tstProperty;

		// Test the scanning efficiency of 100 items of data.
		for (int index = 0; index < count; ++index)
		{
			QDir tempDir = dir;
			QCOMPARE(tempDir.mkdir(QString("%1tst_item").arg(index)), true);
			QCOMPARE(tempDir.cd(QString("%1tst_item").arg(index)), true);
			QFile fp(tempDir.absoluteFilePath(QString("test.json")));

			QCOMPARE(fp.open(QIODevice::WriteOnly), true);

			tstProperty.SetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key,
				QString("%1tst_item").arg(index));
			tstProperty.SetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Valid,
				QString("%1").arg((index < (count / 2)) ? "true" : "false"));

			fp.write(tstProperty.ToJsonString().toLatin1());
			fp.close();
		}

		lancet::MedicalRecordsScanner scanner;
		scanner.SetRunTimeMode(lancet::IMedicalRecordsScanner::Timer);
		scanner.SetDirectory(dir);

		scanner.RunningScanner(dir.absolutePath(), { "" });
		QBENCHMARK{
		scanner.UpdateProperty();
			//scanner.onTimerTimeout();
		}
		
		// Test the scanning efficiency of 100 items of data.
		for (int index = 0; index < count; ++index)
		{
			QDir tempDir = dir;
			QCOMPARE(tempDir.cd(QString("%1tst_item").arg(index)), true);
			QCOMPARE(tempDir.remove(QString("test.json")), true);
			QCOMPARE(tempDir.cd(".."), true);

			QCOMPARE(tempDir.rmdir(QString("%1tst_item").arg(index)), true);
		}
	}
#endif // !MITK_BUILD_TESTTING
private slots:
	void initTestCase()
	{
		
	}

	void tst_medicalRecordsScannerPropertys()
	{
		lancet::MedicalRecordsScanner::Pointer scanner(new lancet::MedicalRecordsScanner);

		const lancet::MedicalRecordsScanner* ptr = scanner.GetPointer();

		if (scanner.IsNull())
			QSKIP("Test target object is null, Ignore this unit.");

		// default
		QCOMPARE(scanner->GetInterval(), 100);
		QCOMPARE(scanner->GetDirectory().absolutePath(), QDir::currentPath());
		QCOMPARE(scanner->GetRunTimeMode(), lancet::IMedicalRecordsScanner::Thread);
		QCOMPARE(scanner->GetState(), lancet::IMedicalRecordsScanner::Downtime);
		QCOMPARE(scanner->IsRunning(), false);

		// Directory
		scanner->SetDirectory(QDir(TEST_DATAS_DIR));
		QCOMPARE(scanner->GetDirectory().absolutePath(), TEST_DATAS_DIR);
		scanner->SetDirectory(QDir("."));
		QCOMPARE(scanner->GetDirectory().absolutePath(), QDir::currentPath());

		// Interval
		scanner->SetInterval(9999.3);
		QCOMPARE(scanner->GetInterval(), 9999);
		scanner->SetInterval(true);
		QCOMPARE(scanner->GetInterval(), 1);

		// State
		QCOMPARE(scanner->GetState(), lancet::IMedicalRecordsScanner::Downtime);

		// RunTimeMode
		scanner->SetRunTimeMode(lancet::IMedicalRecordsScanner::Timer);
		QCOMPARE(scanner->GetRunTimeMode(), lancet::IMedicalRecordsScanner::Timer);

		// Running
		QCOMPARE(scanner->IsRunning(), false);
	}
	
	void tst_medicalRecordsScannerStartDatasInvalid()
	{
		lancet::MedicalRecordsScanner::Pointer scanner(new lancet::MedicalRecordsScanner);

		if (scanner.IsNull())
			QSKIP("Test target object is null, Ignore this unit.");

		scanner->SetDirectory(QDir(TEST_DATAS_DIR));
		scanner->SetInterval(100);
		scanner->SetRunTimeMode(lancet::IMedicalRecordsScanner::Thread);
		scanner->Start();

		//connect(scanner.GetPointer(),
		//	&lancet::IMedicalRecordsScanner::MedicalRecordsPropertyTrace,
		//	this, [=](lancet::IMedicalRecordsProperty* property) 
		//{
		//	qDebug() << "Enter " << "lancet::IMedicalRecordsScanner::MedicalRecordsPropertyTrace";
		//	if(property)
		//	{
		//		qDebug() << property->GetKeyValue("Key");
		//	}
		//});

		QTimer::singleShot(1000, this, [=]() {
			event.quit();
		});

		event.exec();
		QCOMPARE(scanner->GetValidPropertyArray().size(), 2);
		QCOMPARE(scanner->GetInvalidPropertyArray().size(), 1);
		scanner->Stop();
	}

	void tst_medicalRecordsScannerStartDatasValid()
	{
		lancet::MedicalRecordsScanner::Pointer scanner(new lancet::MedicalRecordsScanner);

		if (scanner.IsNull())
			QSKIP("Test target object is null, Ignore this unit.");

		scanner->SetDirectory(QDir(TEST_DATAS_DIR));
		scanner->SetInterval(100);
		scanner->SetRunTimeMode(lancet::IMedicalRecordsScanner::Timer);
		scanner->Start();

		QTimer::singleShot(1000, this, [=]() {
			event.quit();
		});

		event.exec();
		scanner->Stop();
		QCOMPARE(scanner->GetValidPropertyArray().size(), 2);
		QCOMPARE(scanner->GetInvalidPropertyArray().size(), 1);
	}

	void tst_medicalRecordsScannerTimerInterval()
	{
		lancet::MedicalRecordsScanner::Pointer scanner(new lancet::MedicalRecordsScanner);

		if (scanner.IsNull())
			QSKIP("Test target object is null, Ignore this unit.");

		connect(scanner.GetPointer(),
			&lancet::IMedicalRecordsScanner::MedicalRecordsPropertyTrace,
			this, [=](lancet::IMedicalRecordsProperty* property)
		{
			auto send = dynamic_cast<lancet::IMedicalRecordsScanner*>(sender());
			if (property && send)
			{
				property->ResetPropertyOfModify();
			}
		});

		scanner->SetDirectory(QDir(TEST_DATAS_DIR));
		scanner->SetInterval(100);
		scanner->SetRunTimeMode(lancet::IMedicalRecordsScanner::Timer);
		scanner->Start();

		QTimer::singleShot(1000, this, [=]() {
			event.quit();
		});

		event.exec();

		QCOMPARE(scanner->GetValidPropertyArray().size(), 2);
		QCOMPARE(scanner->GetInvalidPropertyArray().size(), 1);
		// Modify [Valid:false] to [Valid:true]
		{
			qDebug() << "Modify [Valid:false] to [Valid:true]";
			auto dir = QDir(TEST_DATAS_DIR);
			dir.cd("002_invalid");
			QFile modify(dir.absoluteFilePath("test.json"));

			QCOMPARE(QFileInfo::exists(modify.fileName()), true);

			QCOMPARE(modify.open(QIODevice::WriteOnly), true);
			scanner->GetInvalidPropertyArray()[0]->SetKeyValue(
				lancet::IMedicalRecordsProperty::PropertyKeys::Valid,
				true);
			modify.write(scanner->GetInvalidPropertyArray()[0]->ToJsonString().toLatin1());
			modify.close();

			QTimer::singleShot(500, this, [=]() {
				event.quit();
			});
			event.exec();
			scanner->Stop();
			QCOMPARE(scanner->GetValidPropertyArray().size(), 3);
			QCOMPARE(scanner->GetInvalidPropertyArray().size(), 0);
		}

		// Modify [Valid:true] to [Valid:false]
		{
			qDebug() << "Modify [Valid:true] to [Valid:false]" << "\n"
				<< "valid.size " << scanner->GetValidPropertyArray().size() << "\n"
				<< "invalid.size " << scanner->GetInvalidPropertyArray().size();
			auto dir = QDir(TEST_DATAS_DIR);
			dir.cd("002_invalid");
			QFile modify(dir.absoluteFilePath("test.json"));

			QCOMPARE(QFileInfo::exists(modify.fileName()), true);

			QCOMPARE(modify.open(QIODevice::WriteOnly), true);
			scanner->GetValidPropertyArray().back()->SetKeyValue(
				lancet::IMedicalRecordsProperty::PropertyKeys::Valid,
				false);
			modify.write(scanner->GetValidPropertyArray().back()->ToJsonString().toLatin1());
			modify.close();

			QTimer::singleShot(1000, this, [=]() {
				event.quit();
			});
			scanner->Start();
			event.exec();
			scanner->Stop();
			QCOMPARE(scanner->GetValidPropertyArray().size(), 2);
			QCOMPARE(scanner->GetInvalidPropertyArray().size(), 1);
		}
	}

#ifdef MITK_BUILD_TESTTING
	void tst_medicalRecordsScannerEfficiency_100()
	{
		this->tst_medicalRecordsScannerEfficiency(100);
	}
	void tst_medicalRecordsScannerEfficiency_1000()
	{
		this->tst_medicalRecordsScannerEfficiency(1000);
	}
	void tst_medicalRecordsScannerEfficiency_2000()
	{
		this->tst_medicalRecordsScannerEfficiency(2000);
	}
	void tst_medicalRecordsScannerEfficiency_4000()
	{
		//this->tst_medicalRecordsScannerEfficiency(4000);
	}
#endif

	void on_MedicalRecordsPropertyModify(lancet::IMedicalRecordsProperty*) {}
private:
	QEventLoop event;
};
QTEST_MAIN(tst_medicalRecordsScanner)
#include "tst_medicalRecordsScanner.moc"