#include <QTest>

// Import memory test function module.
#if defined(VLD_INSTALL_X64) || defined(VLD_INSTALL_X32)
#include <vld.h>
#endif // !defined(VLD_INSTALL_X64) || defined(VLD_INSTALL_X32)

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

	void tst_MemoryLeak_case()
	{
		using RoboticsRegisterDirector = lancet::spatial_fitting::RoboticsRegisterDirector;

		for (auto index = 0; index < 100; ++index)
		{
			RoboticsRegisterDirector obj1 = RoboticsRegisterDirector();
			std::shared_ptr<RoboticsRegisterDirector> obj2 = 
				std::make_shared<RoboticsRegisterDirector>();
			RoboticsRegisterDirector::Pointer obj3 = RoboticsRegisterDirector::New();
			RoboticsRegisterDirector::Pointer obj4(new RoboticsRegisterDirector);
		}
	}

	void tst_BuilderRoboticsRegisterDirector_case()
	{
		lancet::spatial_fitting::RoboticsRegisterDirector::Pointer roboticsRegisterDirector 
			= lancet::spatial_fitting::RoboticsRegisterDirector::New();

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


QTEST_MAIN(tst_roboticsRegisterNavigationData)
#include "tst_roboticsRegisterNavigationData.moc"