#include <QTest>

// Import memory test function module.
#if defined(VLD_INSTALL_X64) || defined(VLD_INSTALL_X32)
#include <vld.h>
#endif // !defined(VLD_INSTALL_X64) || defined(VLD_INSTALL_X32)



#include <core/lancetSpatialFittingPointAccuracyDate.h>

class tst_pointAccuracyDate
	: public QObject
{
	Q_OBJECT
public:

private slots:
	void initTestCase()
	{

	}

	void tst_MemoryLeak_case()
	{
		using PointAccuracyDate = lancet::spatial_fitting::PointAccuracyDate;

		for (auto index = 0; index < 100; ++index)
		{
			PointAccuracyDate obj1 = PointAccuracyDate();
			std::shared_ptr<PointAccuracyDate> obj2 = std::make_shared<PointAccuracyDate>();
			std::shared_ptr<PointAccuracyDate> obj3(
				new PointAccuracyDate(mitk::Point3D(), mitk::Point3D()));
		}
	}

	void tst_Accuracy_case_data()
	{
		QTest::addColumn<QVector<double>>("vecSourcePoint");
		QTest::addColumn<QVector<double>>("vecTargetPoint");
		QTest::addColumn<double>("Accuracy");

		QTest::newRow("0.0")
			<< QVector<double>({ 0.0,0.0,0.0 })
			<< QVector<double>({ 0.0,0.0,0.0 }) << 0.0;

		QTest::newRow("3.74165738677") 
			<< QVector<double>({ 0.0,0.0,0.0 })
			<< QVector<double>({ 1.0,2.0,3.0 }) << 3.74165738677;

		QTest::newRow("39.5600808897") 
			<< QVector<double>({ 0.0,0.0,0.0 })
			<< QVector<double>({ 10.0,21.0,32.0 }) << 39.5600808897;
	}

	void tst_Accuracy_case()
	{
		QFETCH(double, Accuracy);
		QFETCH(QVector<double>, vecSourcePoint);
		QFETCH(QVector<double>, vecTargetPoint);

		lancet::spatial_fitting::PointAccuracyDate ptAccuracyDate;
		ptAccuracyDate.SetSourcePoint(mitk::Point3D(vecSourcePoint.data()));
		ptAccuracyDate.SetTargetPoint(mitk::Point3D(vecTargetPoint.data()));

		QCOMPARE(QString::number(Accuracy, 'f', 6),
			QString::number(ptAccuracyDate.Compute(), 'f', 6));
	}
};

QTEST_MAIN(tst_pointAccuracyDate)
#include "tst_pointAccuracyDate.moc"