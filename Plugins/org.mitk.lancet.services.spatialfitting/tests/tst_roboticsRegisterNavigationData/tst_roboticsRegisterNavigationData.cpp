#include <QTest>

#include <lancetSpatialFittingAbstractPipelineBuilder.h>
#include <core/lancetSpatialFittingRoboticsRegisterDirector.h>
#include <core/lancetSpatialFittingPipelineManager.h>

class tst_roboticsRegisterNavigationData
	: public QObject
{
	Q_OBJECT
public:

private slots:
	void initTestCase()
	{

	}

	void tst_BuilderRoboticsRegisterDirector_case()
	{
		lancet::spatial_fitting::RoboticsRegisterDirector::Pointer roboticsRegisterDirector 
			= lancet::spatial_fitting::RoboticsRegisterDirector::New();

		roboticsRegisterDirector->Builder();

		//roboticsRegisterDirector->GetBuilder()->GetOutput()->Update();
		//roboticsRegisterDirector->GetBuilder()->GetOutput()->FindFilter(1)->Update();

		roboticsRegisterDirector->GetBuilder()->GetOutput()->UpdateFilter();
	}
};

QTEST_MAIN(tst_roboticsRegisterNavigationData)
#include "tst_roboticsRegisterNavigationData.moc"