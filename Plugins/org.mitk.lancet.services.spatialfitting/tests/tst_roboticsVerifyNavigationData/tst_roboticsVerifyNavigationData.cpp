#include <QTest>

// Import memory test function module.
#if defined(VLD_INSTALL_X64) || defined(VLD_INSTALL_X32)
#include <vld.h>
#endif // !defined(VLD_INSTALL_X64) || defined(VLD_INSTALL_X32)

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

	void tst_MemoryLeak_case()
	{
		using RoboticsVerifyDirector = lancet::spatial_fitting::RoboticsVerifyDirector;

		for (auto index = 0; index < 100; ++index)
		{
			RoboticsVerifyDirector obj1 = RoboticsVerifyDirector();
			std::shared_ptr<RoboticsVerifyDirector> obj2 =
				std::make_shared<RoboticsVerifyDirector>();
			RoboticsVerifyDirector::Pointer obj3 = RoboticsVerifyDirector::New();
			RoboticsVerifyDirector::Pointer obj4(new RoboticsVerifyDirector);
		}
	}

	void tst_BuilderRoboticsVerifyDirector_case()
	{
		lancet::spatial_fitting::RoboticsVerifyDirector::Pointer roboticsRegisterDirector
			= lancet::spatial_fitting::RoboticsVerifyDirector::New();

		QCOMPARE(true, roboticsRegisterDirector.IsNotNull());
		QCOMPARE(true, roboticsRegisterDirector->Builder());
		QCOMPARE(true, roboticsRegisterDirector->GetBuilder()->GetOutput().IsNotNull());

		auto pipelineManage = roboticsRegisterDirector->GetBuilder()->GetOutput();

		QCOMPARE(true, pipelineManage.IsNotNull());
		QCOMPARE(false, pipelineManage->IsEmpty());

		roboticsRegisterDirector->SetBuilder(nullptr);
		QCOMPARE(true, roboticsRegisterDirector->GetBuilder().IsNull());
		QCOMPARE(false, roboticsRegisterDirector->Builder());
	}
};


QTEST_MAIN(tst_roboticsVerifyNavigationData)
#include "tst_roboticsVerifyNavigationData.moc"