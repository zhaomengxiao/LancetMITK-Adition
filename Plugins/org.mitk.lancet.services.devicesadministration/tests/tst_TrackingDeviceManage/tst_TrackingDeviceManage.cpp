#include <QTest>

#include <src/internal/lancetTrackingDeviceManage.h>

#include <mitkTrackingDevice.h>
#include <mitkNavigationToolStorage.h>

/// 外部输入的测试数据目录
/// eg. TEST_DATAS_DIR_PATH >> Y:/LancetMitk_SBD/MITK-build/bin/tst_datas
#define TEST_DATAS_DIR_PATH	QDir("../../../../bin/tst_datas").absolutePath()

/// 根据参数输入生成外部输入的测试数据文件的绝对路径
/// eg. TEST_DATAS_FILENAME_PATH("test.IGT") >> Y:/LancetMitk_SBD/MITK-build/bin/tst_datas/test.IGT
#define TEST_DATAS_FILENAME_PATH(filename)	QDir("../../../../bin/tst_datas").absoluteFilePath(filename)

class tst_TrackingDeviceManage 
  : public QObject 
{
  Q_OBJECT
public:
  struct TestTrackingDeviceManageData
  {
		QString deviceName;
    int loadErrorCode; // *.IGT文件加载错误代码
    QString toolDataStroageFileName;

		TestTrackingDeviceManageData()
			: loadErrorCode(0), deviceName(), toolDataStroageFileName() {}
		TestTrackingDeviceManageData(QString devName, QString toolDataStroageFile, int ec = 0) 
			: deviceName(devName)
			, toolDataStroageFileName(toolDataStroageFile)
			, loadErrorCode(ec)
		{
		}
	};

  /**
   * 工具集测试数据结构体
   */
	struct TestToolDataStorageData
	{
		int toolCount; // 个数
    int loadErrorCode; // *.IGT文件加载错误代码
		QString toolDataStroageFileName; // *.IGT 文件
    QList<QString> listToolNames; // 工具包中的工具元素名称，如{"Probe"、"Markers"}

    TestToolDataStorageData()
      : toolCount(0), loadErrorCode(0), toolDataStroageFileName(), listToolNames(){}

    TestToolDataStorageData(int c, QString fp, QList<QString> tools, int ec = 0) {
      this->loadErrorCode = ec;
      this->toolCount = c;
      this->toolDataStroageFileName = fp;
      this->listToolNames = tools;
    }
	};
private slots:
  void initTestCase()
  {
		qDebug() << "QDir.tst.TEST_DATAS_DIR_PATH " << TEST_DATAS_DIR_PATH;
		qDebug() << "QDir.tst.TEST_DATAS_FILENAME_PATH(fp.IGT) " << TEST_DATAS_FILENAME_PATH("fp.IGT");
  }

  void tst_InstallTrackingDevice_ToolDataStorage_Success_data()
  {
    QTest::addColumn<TestToolDataStorageData>("ToolDataStorageFiles");

    QTest::newRow("ToolDataStorage_Success_001") << TestToolDataStorageData(1, "", {});
		QTest::newRow("ToolDataStorage_Success_002") << TestToolDataStorageData(1, "", {});
		QTest::newRow("ToolDataStorage_Success_003") << TestToolDataStorageData(1, "", {});
		QTest::newRow("ToolDataStorage_Success_004") << TestToolDataStorageData(1, "", {});
  }
  void tst_InstallTrackingDevice_ToolDataStorage_Faild_data()
  {
		QTest::addColumn<TestToolDataStorageData>("ToolDataStorageFiles");

		QTest::newRow("ToolDataStorage_Faild_001") << TestToolDataStorageData(1, "", {});
		QTest::newRow("ToolDataStorage_Faild_002") << TestToolDataStorageData(1, "", {});
		QTest::newRow("ToolDataStorage_Faild_003") << TestToolDataStorageData(1, "", {});
		QTest::newRow("ToolDataStorage_Faild_004") << TestToolDataStorageData(1, "", {});
	}
	void tst_InstallTrackingDevice_Success_data()
	{
		QTest::addColumn<TestTrackingDeviceManageData>("TrackingDeviceManageData");

		QTest::newRow("InstallTrackingDevice_Success_001") << TestTrackingDeviceManageData("", "");
		QTest::newRow("InstallTrackingDevice_Success_002") << TestTrackingDeviceManageData("", "");
		QTest::newRow("InstallTrackingDevice_Success_003") << TestTrackingDeviceManageData("", "");
		QTest::newRow("InstallTrackingDevice_Success_004") << TestTrackingDeviceManageData("", "");
	}
	void tst_InstallTrackingDevice_Faild_data()
	{
		QTest::addColumn<TestTrackingDeviceManageData>("TrackingDeviceManageData");

		QTest::newRow("InstallTrackingDevice_Faild_001") << TestTrackingDeviceManageData("", "");
		QTest::newRow("InstallTrackingDevice_Faild_002") << TestTrackingDeviceManageData("", "");
		QTest::newRow("InstallTrackingDevice_Faild_003") << TestTrackingDeviceManageData("", "");
		QTest::newRow("InstallTrackingDevice_Faild_004") << TestTrackingDeviceManageData("", "");
	}

  void tst_InstallTrackingDevice_ToolDataStorage_Success()
  {
    QFETCH(TestToolDataStorageData, ToolDataStorageData);

    std::string deviceName = "Robot";

    lancet::TrackingDeviceManage::Pointer trackingDeviceManage = 
      lancet::TrackingDeviceManage::Pointer(new lancet::TrackingDeviceManage());

    /// <1: 安全监测被测试目标
    QCOMPARE(true, trackingDeviceManage.IsNotNull());

    /// <2: 测试被测试设备安装成功
    QCOMPARE(true, trackingDeviceManage->InstallTrackingDevice(deviceName, ToolDataStorageData.toolDataStroageFileName.toStdString()));

		/// <3: 验证设备管理器的各种属性
		auto deviceInstance = trackingDeviceManage->GetTrackingDevice(deviceName);
    /// <3.1: 测试被测试设备安装成功后的设备实例对象内存, GetTrackingDevice 返回不可能为Null
		QCOMPARE(true, deviceInstance.IsNotNull());
		/// <3.2: 设备安装成功，IsInstallTrackingDevice 返回true
		QCOMPARE(true, trackingDeviceManage->IsInstallTrackingDevice(deviceName));
		/// <3.3: 设备安装成功，但没有使设备建立通信通道，所以 IsTrackingDeviceConnected 返回false
		QCOMPARE(false, trackingDeviceManage->IsTrackingDeviceConnected(deviceName));
		/// <3.3: 设备安装成功，但没有使设备开始工作，所以 IsStartTrackingDevice 返回false
		QCOMPARE(false, trackingDeviceManage->IsStartTrackingDevice(deviceName));
		/// <3.4: 设备安装成功，所以设备数量由0变为1
		QCOMPARE(1, trackingDeviceManage->GetInstallTrackingDeviceSize());
    /// <3.5: 设备安装成功，管理器安装设备没有产生任何错误代码
    QCOMPARE(trackingDeviceManage->GetErrorCode(), ToolDataStorageData.loadErrorCode);

		/// <4: 测试被测试设备安装成功后的工具数据集是否正确
    auto deviceToolDataStorageInstance = trackingDeviceManage->GetNavigationToolStorage(deviceName);
    /// <4.1: 测试目标设备的工具数据集内存对象是否正确
		QCOMPARE(true, deviceToolDataStorageInstance.IsNotNull());
		/// <4.2: 测试目标设备的工具数据集工具数量是否正确
    QCOMPARE(ToolDataStorageData.toolCount, deviceToolDataStorageInstance->GetToolCount());
    /// <4.3: 测试目标设备的工具数据集的工具集是否正确
    for (QString toolName : ToolDataStorageData.listToolNames) 
    {
      QCOMPARE(true, deviceToolDataStorageInstance->GetToolByName(toolName.toStdString()).IsNotNull());
    }

    /// <5: 验证设备管理器退出设备接口
    QCOMPARE(true, trackingDeviceManage->UnInstallTrackingDevice(deviceName));
    /// <5.1: 设备卸载成功，设备管理数量由 1 变为 0
    QCOMPARE(0, trackingDeviceManage->GetInstallTrackingDeviceSize());
  }

  void tst_InstallTrackingDevice_ToolDataStorage_Faild()
	{
		QFETCH(TestToolDataStorageData, ToolDataStorageData);
		std::string deviceName = "Robot";

		lancet::TrackingDeviceManage::Pointer trackingDeviceManage =
			lancet::TrackingDeviceManage::Pointer(new lancet::TrackingDeviceManage());

		/// <1: 安全监测被测试目标
		QCOMPARE(true, trackingDeviceManage.IsNotNull());

		/// <2: 测试被测试设备安装失败
		QCOMPARE(true, trackingDeviceManage->InstallTrackingDevice(deviceName, ToolDataStorageData.toolDataStroageFileName.toStdString()));

    /// <2.1: 对比错误代码
    QCOMPARE(trackingDeviceManage->GetErrorCode(), ToolDataStorageData.loadErrorCode);

    /// <2.2: 设备安装失败，则 IsInstallTrackingDevice 接口返回false
    QCOMPARE(false ,trackingDeviceManage->IsInstallTrackingDevice(deviceName));

		/// <2.3: 设备安装失败，则 IsTrackingDeviceConnected 接口返回false
		QCOMPARE(false, trackingDeviceManage->IsTrackingDeviceConnected(deviceName));

		/// <2.4: 设备安装失败，则 GetInstallTrackingDeviceSize 接口返回 0
		QCOMPARE(0, trackingDeviceManage->GetInstallTrackingDeviceSize());
  }

  void tst_InstallTrackingDevice_Success()
  {
		QFETCH(TestTrackingDeviceManageData, TrackingDeviceManageData);
		std::string deviceName = "Robot";

		lancet::TrackingDeviceManage::Pointer trackingDeviceManage =
			lancet::TrackingDeviceManage::Pointer(new lancet::TrackingDeviceManage());

		/// <1: 安全监测被测试目标
		QCOMPARE(true, trackingDeviceManage.IsNotNull());

		/// <2: 测试被测试设备安装成功
		QCOMPARE(true, trackingDeviceManage->InstallTrackingDevice(deviceName, TrackingDeviceManageData.toolDataStroageFileName.toStdString()));

		/// <3: 验证设备管理器的各种属性
		auto deviceInstance = trackingDeviceManage->GetTrackingDevice(deviceName);
		/// <3.1: 测试被测试设备安装成功后的设备实例对象内存, GetTrackingDevice 返回不可能为Null
		QCOMPARE(true, deviceInstance.IsNotNull());
		/// <3.2: 设备安装成功，IsInstallTrackingDevice 返回true
		QCOMPARE(true, trackingDeviceManage->IsInstallTrackingDevice(deviceName));
		/// <3.3: 设备安装成功，但没有使设备建立通信通道，所以 IsTrackingDeviceConnected 返回false
		QCOMPARE(false, trackingDeviceManage->IsTrackingDeviceConnected(deviceName));
		/// <3.3: 设备安装成功，但没有使设备开始工作，所以 IsStartTrackingDevice 返回false
		QCOMPARE(false, trackingDeviceManage->IsStartTrackingDevice(deviceName));
		/// <3.4: 设备安装成功，所以设备数量由0变为1
		QCOMPARE(1, trackingDeviceManage->GetInstallTrackingDeviceSize());
		/// <3.5: 设备安装成功，管理器安装设备没有产生任何错误代码
		QCOMPARE(trackingDeviceManage->GetErrorCode(), TrackingDeviceManageData.loadErrorCode);
		/// <3.6: 验证其他属性-可视化过滤器更新间隔时间
		QCOMPARE(500, trackingDeviceManage->GetNavigationDataFilterInterval());

		/// <4: 验证设备资源对象集
		/// <4.1: 验证设备可视化过滤器对象
		QCOMPARE(true, trackingDeviceManage->GetNavigationDataToNavigationDataFilter(deviceName).IsNotNull());
		/// <4.2: 验证设备数据管道源对象
		QCOMPARE(true, trackingDeviceManage->GetTrackingDeviceSource(deviceName).IsNotNull());
		/// <4.3: 验证设备工具包对象
		QCOMPARE(true, trackingDeviceManage->GetNavigationToolStorage(deviceName).IsNotNull());


		/// <5: 验证设备管理器退出设备接口
		QCOMPARE(true, trackingDeviceManage->UnInstallTrackingDevice(deviceName));
		/// <5.1: 设备卸载成功，设备管理数量由 1 变为 0
		QCOMPARE(0, trackingDeviceManage->GetInstallTrackingDeviceSize());
		/// <5.2: 设备卸载成功，管理器内部错误状态为0
		QCOMPARE(0, trackingDeviceManage->GetErrorCode());
  }

	void tst_InstallTrackingDevice_Faild()
	{
		QFETCH(TestTrackingDeviceManageData, TrackingDeviceManageData);
		std::string deviceName = "Robot";

		lancet::TrackingDeviceManage::Pointer trackingDeviceManage =
			lancet::TrackingDeviceManage::Pointer(new lancet::TrackingDeviceManage());

		/// <1: 安全监测被测试目标
		QCOMPARE(true, trackingDeviceManage.IsNotNull());

		/// <2: 测试被测试设备安装成功
		QCOMPARE(false, trackingDeviceManage->InstallTrackingDevice(deviceName, TrackingDeviceManageData.toolDataStroageFileName.toStdString()));

		/// <3: 验证设备管理器的各种属性
		auto deviceInstance = trackingDeviceManage->GetTrackingDevice(deviceName);
		/// <3.1: 测试被测试设备安装失败后的设备实例对象内存, GetTrackingDevice 返回为Null
		QCOMPARE(false, deviceInstance.IsNotNull());
		/// <3.2: 设备安装失败，IsInstallTrackingDevice 返回false
		QCOMPARE(false, trackingDeviceManage->IsInstallTrackingDevice(deviceName));
		/// <3.3: 设备安装失败，所以 IsTrackingDeviceConnected 返回false
		QCOMPARE(false, trackingDeviceManage->IsTrackingDeviceConnected(deviceName));
		/// <3.3: 设备安装失败，所以 IsStartTrackingDevice 返回false
		QCOMPARE(false, trackingDeviceManage->IsStartTrackingDevice(deviceName));
		/// <3.4: 设备安装失败，所以设备数量为0
		QCOMPARE(0, trackingDeviceManage->GetInstallTrackingDeviceSize());
		/// <3.5: 设备安装失败，管理器安装设备产生错误代码
		QCOMPARE(trackingDeviceManage->GetErrorCode() != 0, true);
		/// <3.6: 验证其他属性-可视化过滤器更新间隔时间
		QCOMPARE(500, trackingDeviceManage->GetNavigationDataFilterInterval());

		/// <4: 验证设备资源对象集
		/// <4.1: 验证设备可视化过滤器对象
		QCOMPARE(false, trackingDeviceManage->GetNavigationDataToNavigationDataFilter(deviceName).IsNotNull());
		/// <4.2: 验证设备数据管道源对象
		QCOMPARE(false, trackingDeviceManage->GetTrackingDeviceSource(deviceName).IsNotNull());
		/// <4.3: 验证设备工具包对象
		QCOMPARE(false, trackingDeviceManage->GetNavigationToolStorage(deviceName).IsNotNull());


		/// <5: 验证设备管理器退出设备接口
		QCOMPARE(false, trackingDeviceManage->UnInstallTrackingDevice(deviceName));
	}

	void tst_ConnectedOrWorkingToTrackingDevice_Success()
	{
		// TODO: 
	}

	void tst_ConnectedOrWorkingToTrackingDevice_Faild()
	{
		// TODO:
	}
};

Q_DECLARE_METATYPE(tst_TrackingDeviceManage::TestToolDataStorageData)
Q_DECLARE_METATYPE(tst_TrackingDeviceManage::TestTrackingDeviceManageData)
QTEST_MAIN(tst_TrackingDeviceManage)
#include "tst_TrackingDeviceManage.moc"