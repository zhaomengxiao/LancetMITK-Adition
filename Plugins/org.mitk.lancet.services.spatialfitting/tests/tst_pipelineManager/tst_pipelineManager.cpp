#include <QTest>

// Import memory test function module.
#if defined(VLD_INSTALL_X64) || defined(VLD_INSTALL_X32)
#include <vld.h>
#endif // !defined(VLD_INSTALL_X64) || defined(VLD_INSTALL_X32)

#include <core/lancetSpatialFittingPipelineManager.h>
#include <core/lancetSpatialFittingNavigationToolCollector.h>
#include <core/lancetSpatialFittingNavigationToolToSpaceFilter.h>
#include <core/lancetSpatialFittingNavigationToolToNavigationToolFilter.h>

class tst_pipelineManager
	: public QObject
{
	Q_OBJECT
public:
	mitk::NavigationDataToNavigationDataFilter::Pointer
		CreateFilterOfFactor(int filter_type)
	{
		mitk::NavigationDataToNavigationDataFilter::Pointer filter;
		switch (filter_type)
		{
		case 0:
			filter = lancet::spatial_fitting::NavigationToolToNavigationToolFilter::New();
			break;
		case 1:
			filter = lancet::spatial_fitting::NavigationToolToSpaceFilter::New();
			break;
		case 2:
		default:
			filter = lancet::spatial_fitting::NavigationToolCollector::New();
			break;
		}
		return filter;
	}

	bool initTestFilterOfManager(lancet::spatial_fitting::PipelineManager* manager, int min, int max)
	{
		if (nullptr == manager)
		{
			return false;
		}

		bool isSuccess = true;
		for (int index = min; index < max; ++index)
		{
			mitk::NavigationDataToNavigationDataFilter::Pointer filter =
				this->CreateFilterOfFactor(rand() % 3);

			isSuccess &= manager->AddFilter(index - min, filter);
		}
		return isSuccess;
	}


private slots:
	void initTestCase()
	{

	}

	void tst_MemoryLeak_case()
	{
		using PipelineManager = lancet::spatial_fitting::PipelineManager;

		for (auto index = 0; index < 100; ++index)
		{
			PipelineManager obj1 = PipelineManager();
			std::shared_ptr<PipelineManager> obj2 = std::make_shared<PipelineManager>();
			std::shared_ptr<PipelineManager> obj3(
				new PipelineManager());
			PipelineManager::Pointer obj4 = PipelineManager::New();

			QCOMPARE(true, this->initTestFilterOfManager(&obj1, 0, rand() % 100 + 1));
			QCOMPARE(true, this->initTestFilterOfManager(obj2.get(), 0, rand() % 100 + 1));
			QCOMPARE(true, this->initTestFilterOfManager(obj3.get(), 0, rand() % 100 + 1));
			QCOMPARE(true, this->initTestFilterOfManager(obj4.GetPointer(), 0, rand() % 100 + 1));
		}
	}

	void tst_PipelineManager_AddFilter_case()
	{
		using PipelineManager = lancet::spatial_fitting::PipelineManager;

		for (auto index = 0; index < 100; ++index)
		{
			PipelineManager obj1 = PipelineManager();
			std::shared_ptr<PipelineManager> obj2 = std::make_shared<PipelineManager>();
			std::shared_ptr<PipelineManager> obj3(
				new PipelineManager());
			PipelineManager::Pointer obj4 = PipelineManager::New();

			QCOMPARE(true, this->initTestFilterOfManager(&obj1, 10, 100));
			QCOMPARE(true, this->initTestFilterOfManager(obj2.get(), 50, 100));
			QCOMPARE(true, this->initTestFilterOfManager(obj3.get(), 0, 100));
			QCOMPARE(true, this->initTestFilterOfManager(obj4.GetPointer(), 0, 100));


			// RemoveFilter of obj1
			QCOMPARE(90, obj1.GetSize());
			for (size_t index = obj1.GetSize() - 1; index < obj1.GetSize(); --index)
			{
				QCOMPARE(true, obj1.RemoveFilter(index));
			}
			QCOMPARE(true, obj1.IsEmpty());

			// RemoveFilter of obj2
			QCOMPARE(50, obj2->GetSize());
			for (size_t index = obj2->GetSize() - 1; index < obj2->GetSize(); --index)
			{
				QCOMPARE(true, obj2->RemoveFilter(index));
			}
			QCOMPARE(true, obj2->IsEmpty());

			// RemoveFilter of obj3
			QCOMPARE(100, obj3->GetSize());
			for (size_t index = obj3->GetSize() - 1; index < obj3->GetSize(); --index)
			{
				QCOMPARE(true, obj3->RemoveFilter(index));
			}
			QCOMPARE(true, obj3->IsEmpty());

			// RemoveFilter of obj4
			QCOMPARE(100, obj4->GetSize());
			for (size_t index = obj4->GetSize() - 1; index < obj4->GetSize(); --index)
			{
				QCOMPARE(true, obj4->RemoveFilter(index));
			}
			QCOMPARE(true, obj4->IsEmpty());
		}
	}

	void tst_PipelineManager_RemoveFilter_case()
	{
		using PipelineManager = lancet::spatial_fitting::PipelineManager;
		PipelineManager obj1 = PipelineManager();

		QCOMPARE(true, obj1.AddFilter(0, lancet::spatial_fitting::NavigationToolCollector::New()));
		QCOMPARE(1, obj1.GetSize());

		obj1.FindFilter(0)->SetName("tst_filter");

		QCOMPARE(true, obj1.FindFilter("tst_filter").IsNotNull());
		QCOMPARE(true, obj1.FindFilter("tst_filter_not").IsNull());

		QCOMPARE(true, obj1.RemoveFilter("tst_filter"));
		QCOMPARE(0, obj1.GetSize());
	}

	void tst_PipelineManager_ModifyFilter_case()
	{
		using PipelineManager = lancet::spatial_fitting::PipelineManager;
		PipelineManager obj1 = PipelineManager();

		QCOMPARE(true, obj1.AddFilter(0, lancet::spatial_fitting::NavigationToolCollector::New()));
		QCOMPARE(1, obj1.GetSize());
		obj1.FindFilter(0)->SetName("tst_filter");

		auto tst_filter = lancet::spatial_fitting::NavigationToolCollector::New();
		tst_filter->SetName("tst_modify_filter");
		obj1.ModifyFilter("tst_filter", tst_filter);

		QCOMPARE(true, obj1.FindFilter("tst_modify_filter").IsNotNull());
		QCOMPARE(1, obj1.GetSize());
	}
};


QTEST_MAIN(tst_pipelineManager)
#include "tst_pipelineManager.moc"