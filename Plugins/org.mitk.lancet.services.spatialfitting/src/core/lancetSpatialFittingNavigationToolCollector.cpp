#include "lancetSpatialFittingNavigationToolCollector.h"

// Qt
#include <QTimer>


BEGIN_SPATIAL_FITTING_NAMESPACE

struct NavigationToolCollector::NavigationToolCollectorPrivateImp
{
	QTimer qtClockTrigger;
	unsigned int numberForMean = 10;
	double accuracyRange = 0.5;
	WorkStatus state = WorkStatus::Idle;
	std::string permissionIdentificationArea;

	std::vector<mitk::NavigationData::Pointer> vecCollectorNavigationData;
};

NavigationToolCollector::NavigationToolCollector()
	: imp(std::make_shared<NavigationToolCollectorPrivateImp>())
{
	this->SetInterval(20);

	connect(&this->imp->qtClockTrigger, &QTimer::timeout, 
		this, &NavigationToolCollector::on_QtTimerTrigger_timeout);
}

NavigationToolCollector::~NavigationToolCollector()
{
	disconnect(&this->imp->qtClockTrigger, &QTimer::timeout,
		this, &NavigationToolCollector::on_QtTimerTrigger_timeout);
}

long NavigationToolCollector::GetInterval() const
{
	return this->imp->qtClockTrigger.interval();
}

void NavigationToolCollector::SetInterval(long interval)
{
}

unsigned int NavigationToolCollector::GetNumberForMean() const
{
	return 0;
}

void NavigationToolCollector::SetNumberForMean(unsigned int number)
{
}

double NavigationToolCollector::GetAccuracyRange() const
{
	return 0.0;
}

void NavigationToolCollector::SetAccuracyRange(double accuracy)
{
}

NavigationToolCollector::WorkStatus NavigationToolCollector::GetWorkState() const
{
	return this->imp->state;
}

bool NavigationToolCollector::IsRunning() const
{
	return this->GetWorkState() == WorkStatus::Working;
}

bool NavigationToolCollector::Start(long interval)
{
	if (this->GetWorkState() != WorkStatus::Idle)
	{
		// The collector is currently working.
		return false;
	}

	if (interval != 0)
	{
		this->SetInterval(interval);
	}
	this->imp->vecCollectorNavigationData.clear();
	this->imp->qtClockTrigger.start();
	this->imp->state = WorkStatus::Working;
	return true;
}

bool NavigationToolCollector::Stop()
{
	this->imp->state = WorkStatus::Idle;
	this->imp->qtClockTrigger.stop();
	return true;
}

std::string NavigationToolCollector::GetPermissionIdentificationArea() const
{
	return this->imp->permissionIdentificationArea;
}

void NavigationToolCollector::SetPermissionIdentificationArea(const std::string& area)
{
	this->imp->permissionIdentificationArea = area;
}

void NavigationToolCollector::on_QtTimerTrigger_timeout()
{
	if (this->GetOutput())
	{
		this->imp->vecCollectorNavigationData.push_back(this->GetOutput()->Clone());
		emit this->Step(this->imp->vecCollectorNavigationData.size(),
			this->imp->vecCollectorNavigationData.back().GetPointer());
	}

	if (this->imp->vecCollectorNavigationData.size() >= this->GetNumberForMean())
	{
		this->Stop();
		emit this->Complete(nullptr);
	}
}

void NavigationToolCollector::GenerateData()
{
	std::cout << __FUNCTION__ << "log" << std::endl;
	this->CreateOutputsForAllInputs();

	for (size_t index = 0; index < this->GetNumberOfInputs(); ++index)
	{
		if (this->GetInput(index) && this->GetOutput(index))
		{
			this->GetOutput(index)->Graft(this->GetInput(index));
		}
	}
}


END_SPATIAL_FITTING_NAMESPACE
