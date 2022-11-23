#include <QTest>

#include <lancetSpatialFittingAbstractPipelineBuilder.h>
#include <core/lancetSpatialFittingRoboticsVerifyDirector.h>
#include <core/lancetSpatialFittingPipelineManager.h>

class tst_roboticsVerifyNavigationData
	: public QObject
{
	Q_OBJECT
public:

private slots:
	void initTestCase()
	{

	}

	void tst_BuilderRoboticsVerifyDirector_case()
	{
		lancet::spatial_fitting::RoboticsVerifyDirector::Pointer roboticsVerifyDirector 
			= lancet::spatial_fitting::RoboticsVerifyDirector::New();

		roboticsVerifyDirector->Builder();

		//roboticsVerifyDirector->GetBuilder()->GetOutput()->Update();
		//roboticsVerifyDirector->GetBuilder()->GetOutput()->FindFilter(1)->Update();

		roboticsVerifyDirector->GetBuilder()->GetOutput()->UpdateFilter();
	}
};

QTEST_MAIN(tst_roboticsVerifyNavigationData)
#include "tst_roboticsVerifyNavigationData.moc"