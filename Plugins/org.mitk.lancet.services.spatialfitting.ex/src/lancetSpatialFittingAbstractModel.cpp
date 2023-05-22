#include "lancetSpatialFittingAbstractModel.h"


// Include files of mitk.
#include <mitkNavigationDataSource.h>

BEGIN_SPATIAL_FITTING_NAMESPACE

struct AbstractModel::PrivateImp 
{
  QString serialNumber;

	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;
	mitk::NavigationDataSource::Pointer roboticsNavigationDataSource;

	QMap<QString, mitk::BaseProperty::Pointer> mapMitkProperty;
};

AbstractModel::AbstractModel(const QString & sn)
    : imp(std::make_shared<PrivateImp>())
{
    this->SetSerialNumber(sn);
}

QString AbstractModel::GetSerialNumber() const
{
    return this->imp->serialNumber;
}

void AbstractModel::SetSerialNumber(const QString & sn)
{
    this->imp->serialNumber = sn;
}

itk::SmartPointer<mitk::NavigationDataSource> AbstractModel::GetNdiNavigationDataSource() const
{
    return this->imp->ndiNavigationDataSource;
}

void AbstractModel::SetNdiNavigationDataSource(const itk::SmartPointer<mitk::NavigationDataSource>& source)
{
   this->imp->ndiNavigationDataSource = source;
}

itk::SmartPointer<mitk::NavigationDataSource> AbstractModel::GetRoboticsNavigationDataSource() const
{
    return this->imp->roboticsNavigationDataSource;
}

void AbstractModel::SetRoboticsNavigationDataSource(const itk::SmartPointer<mitk::NavigationDataSource>& source)
{
    this->imp->roboticsNavigationDataSource = source;
}

void AbstractModel::SetProperty(const QString& key, const itk::SmartPointer<mitk::BaseProperty>& value)
{
	this->imp->mapMitkProperty[key] = value;
}

itk::SmartPointer<mitk::BaseProperty> AbstractModel::GetProperty(const QString& key) const
{
	return this->imp->mapMitkProperty.value(key);
}

END_SPATIAL_FITTING_NAMESPACE

