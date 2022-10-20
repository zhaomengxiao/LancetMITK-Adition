#include <QTest>
#include <QPluginLoader>

#include <internal/lancetMedicalRecordsProperty.h>

class tst_medicalRecordsProperty
	: public QObject
{
	Q_OBJECT
public:

private slots:
	void initTestCase()
	{

	}

	void tst_MedicalRecordsPropertyNew1()
	{
		lancet::MedicalRecordsProperty property;

		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Key), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Name), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Id), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Sex), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Age), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Valid), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::OperatingSurgeonName), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::SurgicalArea), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Type), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::CreateTime), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::LastModifyTime), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::DataAddress), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::DrPreviewAddress), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::DrPositiveDicomAddress), true);
		QCOMPARE(property.HasKey(lancet::MedicalRecordsProperty::PropertyKeys::DrLateralDicomAddress), true);
	}
	void tst_MedicalRecordsPropertyNew2()
	{
		lancet::MedicalRecordsProperty::Pointer property =
			lancet::MedicalRecordsProperty::Pointer(new lancet::MedicalRecordsProperty);


		QCOMPARE(property.IsNotNull(), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Key), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Name), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Id), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Sex), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Age), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Valid), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::OperatingSurgeonName), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::SurgicalArea), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Type), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::CreateTime), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::LastModifyTime), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::DataAddress), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::DrPreviewAddress), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::DrPositiveDicomAddress), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::DrLateralDicomAddress), true);
	}
	void tst_MedicalRecordsPropertyNew3()
	{
		lancet::MedicalRecordsProperty* property = new lancet::MedicalRecordsProperty;

		QCOMPARE(property != nullptr, true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Key), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Name), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Id), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Sex), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Age), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Valid), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::OperatingSurgeonName), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::SurgicalArea), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::Type), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::CreateTime), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::LastModifyTime), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::DataAddress), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::DrPreviewAddress), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::DrPositiveDicomAddress), true);
		QCOMPARE(property->HasKey(lancet::MedicalRecordsProperty::PropertyKeys::DrLateralDicomAddress), true);
	}
	void tst_MedicalRecordsPropertyKeys()
	{
		auto listKeys = lancet::MedicalRecordsProperty::PropertyKeys::ToQtList();

		QCOMPARE(listKeys.size(), 0x0F);

		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::Key),
			QString("No matching target key. see %1").arg("'Key'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::Name),
			QString("No matching target key. see %1").arg("'Name'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::Id),
			QString("No matching target key. see %1").arg("'Id'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::Sex),
			QString("No matching target key. see %1").arg("'Sex'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::Age),
			QString("No matching target key. see %1").arg("'Age'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::Valid),
			QString("No matching target key. see %1").arg("'Valid'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::OperatingSurgeonName),
			QString("No matching target key. see %1").arg("'OperatingSurgeonName'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::SurgicalArea),
			QString("No matching target key. see %1").arg("'SurgicalArea'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::Type),
			QString("No matching target key. see %1").arg("'Type'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::CreateTime),
			QString("No matching target key. see %1").arg("'CreateTime'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::LastModifyTime),
			QString("No matching target key. see %1").arg("'LastModifyTime'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::DataAddress),
			QString("No matching target key. see %1").arg("'DataAddress'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::DrPreviewAddress),
			QString("No matching target key. see %1").arg("'DrPreviewAddress'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::DrPositiveDicomAddress),
			QString("No matching target key. see %1").arg("'DrPositiveDicomAddress'").toLatin1());
		QVERIFY2(listKeys.contains(lancet::MedicalRecordsProperty::PropertyKeys::DrLateralDicomAddress),
			QString("No matching target key. see %1").arg("'DrLateralDicomAddress'").toLatin1());

		lancet::MedicalRecordsProperty proSrc;
		lancet::MedicalRecordsProperty proDest;

		proDest.SetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Valid, "true");
		proSrc.ResetPropertyOfModify();

		QCOMPARE(proSrc.GetModifyKeys().size(), 0);

		proSrc.ModifyOf(&proDest);
		QCOMPARE(proSrc.GetModifyKeys().size(), 1);

	}
	void tst_MedicalRecordsPropertyCopy1()
	{
		lancet::MedicalRecordsProperty proSrc;
		lancet::MedicalRecordsProperty::Pointer proDest(new lancet::MedicalRecordsProperty);

		proDest->SetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name, "ProName1");

		QCOMPARE(proSrc == proDest, false);
		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key) !=
			proDest->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key),
			"Correct unique values are inconsistent.");
		proSrc = proDest;

		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key) ==
			proDest->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key),
			"Unique value mismatch, object copy error.");
		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name) ==
			proDest->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name),
			"Attribute value mismatch, object copy error.");

		QVERIFY2(proSrc == proDest, "operator==(MedicalRecordsProperty::Pointer) Comparison error.");
	}
	void tst_MedicalRecordsPropertyCopy2()
	{
		lancet::MedicalRecordsProperty proSrc;
		lancet::MedicalRecordsProperty proDest;

		proDest.SetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name, "ProName1");

		QCOMPARE(proSrc == proDest, false);
		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key) !=
			proDest.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key),
			"Correct unique values are inconsistent.");
		proSrc = proDest;

		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key) ==
			proDest.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key),
			"Unique value mismatch, object copy error.");
		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name) ==
			proDest.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name),
			"Attribute value mismatch, object copy error.");

		QVERIFY2(proSrc == proDest, "operator==(const MedicalRecordsProperty&) Comparison error.");
	}
	void tst_MedicalRecordsPropertyCopy3()
	{
		lancet::MedicalRecordsProperty proSrc;
		lancet::MedicalRecordsProperty* proDest = new lancet::MedicalRecordsProperty;

		proDest->SetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name, "ProName1");

		QCOMPARE(proSrc == proDest, false);
		QCOMPARE(proSrc == nullptr, false);
		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key) !=
			proDest->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key),
			"Correct unique values are inconsistent.");
		proSrc = proDest;

		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key) ==
			proDest->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key),
			"Unique value mismatch, object copy error.");
		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name) ==
			proDest->GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name),
			"Attribute value mismatch, object copy error.");

		QVERIFY2(proSrc == proDest, "operator==(MedicalRecordsProperty*) Comparison error.");
	}
	void tst_MedicalRecordsPropertyCopy4()
	{
		lancet::MedicalRecordsProperty proDest;

		proDest.SetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name, "ProName1");
		lancet::MedicalRecordsProperty proSrc = proDest;

		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key) ==
			proDest.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key),
			"Unique value mismatch, object copy error.");
		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name) ==
			proDest.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name),
			"Attribute value mismatch, object copy error.");

		QVERIFY2(proSrc == proDest, "MedicalRecordsProperty(const MedicalRecordsProperty&) Comparison error.");
	}
	void tst_MedicalRecordsPropertyCopy5()
	{
		lancet::MedicalRecordsProperty proDest;
		lancet::MedicalRecordsProperty proSrc;

		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key) !=
			proDest.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key),
			"Unique value mismatch, object copy error.");
		proDest.SetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name, "ProName1");

		proSrc = lancet::MedicalRecordsProperty(proDest.ToJsonString());

		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key) !=
			proDest.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Key),
			"Unique value mismatch, object copy error.");
		QVERIFY2(proSrc.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name) ==
			proDest.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name),
			"Attribute value mismatch, object copy error.");
	}
	void tst_MedicalRecordsPropertyCopy6_error()
	{
		lancet::MedicalRecordsProperty pro;
		lancet::MedicalRecordsProperty pro2;

		QCOMPARE(pro == pro2, false);
	}
	void tst_MedicalRecordsPropertyModify1()
	{
		lancet::MedicalRecordsProperty pro;

		// From 1
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::ToQtList()),
			true);

		// From 2
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Key), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Name), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Id), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Sex), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Age), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Valid), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::OperatingSurgeonName), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::SurgicalArea), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Type), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::CreateTime), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::LastModifyTime), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DataAddress), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DrPreviewAddress), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DrPositiveDicomAddress), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DrLateralDicomAddress), true);

		pro.ResetPropertyOfModify();
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Key), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Name), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Id), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Sex), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Age), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Valid), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::OperatingSurgeonName), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::SurgicalArea), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Type), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::CreateTime), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::LastModifyTime), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DataAddress), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DrPreviewAddress), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DrPositiveDicomAddress), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DrLateralDicomAddress), false);

		// Modify
		pro.SetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name, "ModifyName");
		QCOMPARE(pro.GetKeyValue(lancet::MedicalRecordsProperty::PropertyKeys::Name), "ModifyName");

		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Key), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Name), true);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Id), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Sex), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Age), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Valid), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::OperatingSurgeonName), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::SurgicalArea), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Type), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::CreateTime), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::LastModifyTime), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DataAddress), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DrPreviewAddress), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DrPositiveDicomAddress), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DrLateralDicomAddress), false);

		pro.ResetPropertyOfModify();
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Key), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Name), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Id), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Sex), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Age), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Valid), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::OperatingSurgeonName), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::SurgicalArea), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::Type), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::CreateTime), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::LastModifyTime), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DataAddress), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DrPreviewAddress), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DrPositiveDicomAddress), false);
		QCOMPARE(pro.HasModifyOf(lancet::MedicalRecordsProperty::PropertyKeys::DrLateralDicomAddress), false);

	}
	void tst_MedicalRecordsPropertyModify2_error()
	{
		lancet::MedicalRecordsProperty pro;

		QCOMPARE(pro.HasModifyOf("ErrorCode"), false);
		QCOMPARE(pro.HasModifyOf(QList<QString>()), false);
		QCOMPARE(pro.HasModifyOf({ "ErrorCode", "ErrorCode2" }), false);
		QCOMPARE(pro.GetKeyValue("ErrorCode"), QVariant());
	}
};
QTEST_MAIN(tst_medicalRecordsProperty)
#include "tst_medicalRecordsProperty.moc"