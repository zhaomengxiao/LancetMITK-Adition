#include "lancetSpatialFittingNavigationToolCollector.h"

// Qt
#include <QTimer>
#include<core/lancetSpatialFittingPointAccuracyDate.h>

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
	this->imp->qtClockTrigger.setInterval(interval);
}

unsigned int NavigationToolCollector::GetNumberForMean() const
{
	return this->imp->numberForMean;
}

void NavigationToolCollector::SetNumberForMean(unsigned int number)
{
	this->imp->numberForMean = number;
}

double NavigationToolCollector::GetAccuracyRange() const
{
	return this->imp->accuracyRange;
}

void NavigationToolCollector::SetAccuracyRange(double accuracy)
{
	this->imp->accuracyRange = accuracy;
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
		mitk::NavigationData::Pointer temp = mitk::NavigationData::New();
		temp->Graft(this->GetOutput());
		this->imp->vecCollectorNavigationData.push_back(temp);

		// Fail ?
		if (false == this->imp->vecCollectorNavigationData.back()->IsDataValid())
		{
			this->Stop();
			emit this->Fail(this->imp->vecCollectorNavigationData.size());
		}

		// TODO: Tool is invaild
		//for (int index = 0; index < this->imp->vecCollectorNavigationData.size(); ++index)
		//{
		//	PointAccuracyDate pt(temp->GetPosition(), 
		//		this->imp->vecCollectorNavigationData[index]->GetPosition());
		//	if (pt.Compute() > this->GetAccuracyRange())
		//	{
		//		this->Stop();
		//		emit this->Fail(this->imp->vecCollectorNavigationData.size());
		//		break;
		//	}
		//}

		// Step ?
		emit this->Step(this->imp->vecCollectorNavigationData.size(),
			this->imp->vecCollectorNavigationData.back().GetPointer());
	}


	// Complete ?
	if (this->imp->vecCollectorNavigationData.size() >= this->GetNumberForMean())
	{
		this->Stop();
		mitk::NavigationData::Pointer data = mitk::NavigationData::New();

		mitk::Point3D point;
		for (auto& item : this->imp->vecCollectorNavigationData)
		{
			point.SetElement(0, point.GetElement(0) + item->GetPosition().GetElement(0));
			point.SetElement(1, point.GetElement(1) + item->GetPosition().GetElement(1));
			point.SetElement(2, point.GetElement(2) + item->GetPosition().GetElement(2));
		}
		int vecSize = this->imp->vecCollectorNavigationData.size();
		point.SetElement(0, point.GetElement(0) / vecSize);
		point.SetElement(1, point.GetElement(1) / vecSize);
		point.SetElement(2, point.GetElement(2) / vecSize);
		data->SetPosition(point);

		emit this->Complete(data);
	}
}

void NavigationToolCollector::GenerateData()
{
	MITK_DEBUG << "log";
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
