#include <QTest>
#include <QPluginLoader>

#include <internal/lancetMedicalRecordsLoader.h>
#include <internal/lancetMedicalRecordsProperty.h>

class tst_medicalRecordsLoader
	: public QObject
{
	Q_OBJECT
public:

private slots:
	void initTestCase()
	{

	}

	void tst_medicalRecordsLoaderEfficiency100()
	{
		QBENCHMARK{
		lancet::MedicalRecordsLoader loader;
		lancet::IMedicalRecordsProperty::Pointer property;

		loader.SetFileName(":/test.json");

			for(auto index =0; index < 1000; ++index)
			{
				property = loader.GetOutput();
			}			
		}
	}

	void tst_medicalRecordsLoader_normal()
	{
		lancet::MedicalRecordsProperty eproperty;

		typedef lancet::MedicalRecordsProperty::PropertyKeys Keys;
		for (auto key : Keys::ToQtList())
		{
			eproperty.SetKeyValue(key, QString("T%1").arg(key));
		}
		eproperty.SetKeyValue(Keys::Valid, "true");

		/// -----------------------------------------------------------
		lancet::MedicalRecordsLoader loader;
		lancet::IMedicalRecordsProperty::Pointer property;

		loader.SetFileName(":/test.json");

		property = loader.GetOutput();

		QCOMPARE(loader.GetJsonParseError(), 0);

		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Id),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Id));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Sex),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Sex));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Age),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Age));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Valid),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Valid));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::OperatingSurgeonName),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::OperatingSurgeonName));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::SurgicalArea),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::SurgicalArea));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Type),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Type));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::CreateTime),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::CreateTime));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::LastModifyTime),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::LastModifyTime));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::DataAddress),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::DataAddress));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::DrPreviewAddress),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::DrPreviewAddress));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::DrPositiveDicomAddress),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::DrPositiveDicomAddress));
		QCOMPARE(property->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::DrLateralDicomAddress),
			eproperty.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::DrLateralDicomAddress));
	}

	void tst_medicalRecordsLoader_fileError1()
	{
		lancet::MedicalRecordsLoader loader;

		loader.SetFileName("empty");
		auto property = loader.GetOutput();

		QCOMPARE(loader.IsFileError(), true);
	}
	void tst_medicalRecordsLoader_jsonError2()
	{
		lancet::MedicalRecordsLoader loader;

		loader.SetFileName(":/test_empty.json");
		auto property = loader.GetOutput();

		QCOMPARE(loader.IsJsonError(), true);
	}
	void tst_medicalRecordsLoader_jsonError3()
	{
		lancet::MedicalRecordsLoader loader;

		loader.SetFileName(":/test_error.json");
		auto property = loader.GetOutput();

		QCOMPARE(loader.IsJsonError(), true);
	}
	void tst_medicalRecordsLoader_jsonError4()
	{
		lancet::MedicalRecordsLoader loader;

		loader.SetFileName(QDir::currentPath());
		auto property = loader.GetOutput();

		QCOMPARE(loader.IsFileError(), true);
	}
};

QTEST_MAIN(tst_medicalRecordsLoader)
#include "tst_medicalRecordsLoader.moc"