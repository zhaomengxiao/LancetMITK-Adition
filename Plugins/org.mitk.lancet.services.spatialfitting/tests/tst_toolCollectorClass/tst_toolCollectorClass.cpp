#include <QTest>

#include <lancetSpatialFittingAbstractDirector.h>
#include <core/lancetSpatialFittingPipelineBuilder.h>
#include <core/lancetSpatialFittingPipelineManager.h>
#include <core/lancetSpatialFittingNavigationToolCollector.h>

BEGIN_SPATIAL_FITTING_NAMESPACE
class tst_toolCollectorDirector : public AbstractDirector
{
public:
	mitkClassMacroItkParent(tst_toolCollectorDirector, AbstractDirector);

	itkNewMacro(tst_toolCollectorDirector)

	tst_toolCollectorDirector() 
	{
		PipelineBuilder::Pointer pipelineBuilder = PipelineBuilder::New();
		this->SetBuilder(pipelineBuilder);
	}

	virtual bool Builder() override 
	{
		PipelineBuilder::Pointer pipelineBuilder = 
			dynamic_cast<PipelineBuilder*>(this->GetBuilder().GetPointer());

		if (pipelineBuilder.IsNull())
		{
			return false;
		}

		mitk::AffineTransform3D::Pointer toolToSpaceData = mitk::AffineTransform3D::New();
		toolToSpaceData->SetIdentity();
		//pipelineBuilder->BuilderNavigationToolToSpaceFilter(0, toolToSpaceData);
		pipelineBuilder->BuilderNavigationToolCollector(0, 10);

		pipelineBuilder->GetOutput()->FindFilter(0)->SetName("tst_Filter_01");
		//pipelineBuilder->GetOutput()->FindFilter(1)->SetName("tst_Filter_02");
		return true;
	}
};

END_SPATIAL_FITTING_NAMESPACE

class tst_toolCollectorClass
	: public QObject
{
	Q_OBJECT
public:

private slots:
	void initTestCase()
	{

	}

	void tst_buildToolCollector_case()
	{
		lancet::spatial_fitting::tst_toolCollectorDirector::Pointer tstDirector =
			lancet::spatial_fitting::tst_toolCollectorDirector::New();

		tstDirector->Builder();

		QCOMPARE(true, tstDirector->GetBuilder().IsNotNull());

		//QCOMPARE(2, tstDirector->GetBuilder()->GetOutput()->GetSize());

		mitk::NavigationData::Pointer tst_data = mitk::NavigationData::New();
		tst_data->SetName("tst_Data");
		tst_data->SetDataValid(true);
		tstDirector->GetBuilder()->GetOutput()->SetInput(tst_data);
		double pt[3] = { 0,10, 20 };
		tst_data->SetPosition(mitk::Point3D(pt));


		MITK_INFO << "tst_data->Clone()->GetName(): " << tst_data->Clone()->GetName();
		QCOMPARE("tst_Data", tst_data->Clone()->GetName());

		try
		{
			tstDirector->GetBuilder()->GetOutput()->UpdateFilter();
		}
		catch (...)
		{
			MITK_ERROR << __FUNCTION__ << ": catch filter error!";
		}

		MITK_WARN << tstDirector->GetBuilder()->GetOutput()->GetOutput()->GetName();

		mitk::NavigationData::Pointer tst_data_copy = tstDirector->GetBuilder()
			->GetOutput()->FindFilter(0)->GetOutput();
		MITK_WARN << tst_data_copy->GetName();
		//mitk::NavigationData::Pointer tst_data_copy2 = tstDirector->GetBuilder()
		//	->GetOutput()->FindFilter(1)->GetOutput();

		QCOMPARE(true, nullptr != tst_data_copy);
		//QCOMPARE(true, nullptr != tst_data_copy2);
		QCOMPARE("tst_Data", tst_data_copy->GetName());
		QCOMPARE(true, tst_data_copy->IsDataValid());
	}
};

QTEST_MAIN(tst_toolCollectorClass)
#include "tst_toolCollectorClass.moc"