#include <QTest>
#include <QPluginLoader>

#include <ctkPluginActivator.h>
#include <internal/lancetScxmlStateMachineLoader.h>

class test_stateMachineState
  : public QObject 
{
    Q_OBJECT
private slots:
  void test_case1()
  {
    lancet::ScxmlStateMachineLoader laoder;

    laoder.SetFileName("Tewst");

    QCOMPARE(laoder.GetFileName(), "Tewst");
  }

  void test_loaderService()
  {
    QPluginLoader loader("Y:/LancetMitk_SBD/MITK-build/bin/plugins/Debug/liborg_mitk_lancet_statemachine_services.dll");
    QCOMPARE(loader.load(), true);
    auto loaderDllObject = qobject_cast<ctkPluginActivator*>(loader.instance());
    if (loaderDllObject)
    {
      QCOMPARE(1, 1);
      loaderDllObject->start(nullptr);
    }
  }
};

QTEST_MAIN(test_stateMachineState)
#include "test_stateMachineState.moc"