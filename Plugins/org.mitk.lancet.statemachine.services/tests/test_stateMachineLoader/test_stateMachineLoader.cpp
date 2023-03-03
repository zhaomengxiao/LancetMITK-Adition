#include <QTest>
#include <QPluginLoader>

#include <ctkPluginActivator.h>

#include <internal/lancetScxmlStateMachineLoader.h>
#include <internal/lancetScxmlStateMachineLoader.h>

class test_stateMachineLoader 
  : public QObject 
{
  Q_OBJECT
public:
  struct TestDataItem 
  {
    QString id;
		QString scxmlFilePath;
		QString propertyFilePath;
    lancet::IScxmlStateMachineState::StateType type;
	};
	
private slots:
  void initTestCase()
  {

  }

  void test_loaderErrorDataCase_data()
  {
		QTest::addColumn<TestDataItem>("StateMachineTestDatas");

		QTest::newRow("1") << TestDataItem{
		  "org_mitk_lancet_tha_application", // id
		  "qrc:/tst_error_statemachine.scxml", 
		  "qrc:/tst_error_statemachine_subsidiary.json",
		  lancet::IScxmlStateMachineState::StateType::Ordinary
		};
  }

  void test_loaderErrorDataCase()
	{
		QFETCH(TestDataItem, StateMachineTestDatas);
    lancet::ScxmlStateMachineLoader loader;
		loader.SetStateId(StateMachineTestDatas.id);
		loader.SetStateType(StateMachineTestDatas.type);
		loader.SetFileName(StateMachineTestDatas.scxmlFilePath);
		loader.SetPropertyFileName(StateMachineTestDatas.propertyFilePath);

    auto state = loader.GetOutput();

		QCOMPARE(state.IsNotNull(), true);
		QCOMPARE(state->GetStateId(), StateMachineTestDatas.id);
		QCOMPARE(state->GetManageStateMachine().IsNull(), true);
		QCOMPARE(state->GetQtStateMachine().isNull(), true);
		QCOMPARE(state->GetStateFeatures(), lancet::IScxmlStateMachineState::StateFeatures::Unknown);
    QCOMPARE(state->GetStateType(), StateMachineTestDatas.type);
  }
};
Q_DECLARE_METATYPE(test_stateMachineLoader::TestDataItem)
QTEST_MAIN(test_stateMachineLoader)
#include "test_stateMachineLoader.moc"