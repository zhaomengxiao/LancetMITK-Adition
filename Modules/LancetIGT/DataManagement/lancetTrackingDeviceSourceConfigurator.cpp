/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "lancetTrackingDeviceSourceConfigurator.h"

#include <mitkTrackingDeviceTypeCollection.h>
#include <usGetModuleContext.h>
#include <usModuleContext.h>

#include "mitkClaronTrackingDevice.h"
#include "mitkMatrixConvert.h"
#include "mitkOpenIGTLinkTrackingDevice.h"
#include "mitkOptitrackTrackingDevice.h"


lancet::TrackingDeviceSourceConfiguratorLancet::TrackingDeviceSourceConfiguratorLancet(
  mitk::NavigationToolStorage::Pointer NavigationTools, mitk::TrackingDevice::Pointer TrackingDevice)
{
  //make a copy of the navigation tool storage because we will modify the storage
  if (NavigationTools.IsNotNull())
  {
    m_NavigationTools = mitk::NavigationToolStorage::New();
    for (unsigned int i = 0; i < NavigationTools->GetToolCount(); i++)
    {
      m_NavigationTools->AddTool(NavigationTools->GetTool(i));
    }
  }

  m_TrackingDevice = TrackingDevice;
  m_ToolCorrespondencesInToolStorage = std::vector<int>();
  m_ErrorMessage = "";
}

lancet::TrackingDeviceSourceConfiguratorLancet::TrackingDeviceSourceConfiguratorLancet(
  mitk::NavigationToolStorage::Pointer NavigationTools, mitk::TrackingDevice::Pointer TrackingDevice,
  lancet::NavigationObject::Pointer NavigationObject)
{
  //make a copy of the navigation tool storage because we will modify the storage
  if (NavigationTools.IsNotNull())
  {
    m_NavigationTools = mitk::NavigationToolStorage::New();
    for (unsigned int i = 0; i < NavigationTools->GetToolCount(); i++)
    {
      m_NavigationTools->AddTool(NavigationTools->GetTool(i));
    }
  }

  m_TrackingDevice = TrackingDevice;
  m_NavigationObject = NavigationObject;
  m_ToolCorrespondencesInToolStorage = std::vector<int>();
  m_ErrorMessage = "";
}

mitk::NavigationToolStorage::Pointer lancet::TrackingDeviceSourceConfiguratorLancet::GetUpdatedNavigationToolStorage()
{
  return m_NavigationTools;
}


lancet::TrackingDeviceSourceConfiguratorLancet::~TrackingDeviceSourceConfiguratorLancet()
{
}

bool lancet::TrackingDeviceSourceConfiguratorLancet::IsCreateTrackingDeviceSourcePossible()
{
  if (m_NavigationTools.IsNull())
  {
    m_ErrorMessage = "NavigationToolStorage is nullptr!";
    return false;
  }
  else if (m_TrackingDevice.IsNull())
  {
    m_ErrorMessage = "TrackingDevice is nullptr!";
    return false;
  }
  else
  {
    for (unsigned int i = 0; i < m_NavigationTools->GetToolCount(); i++)
    {
      if (m_NavigationTools->GetTool(i)->GetTrackingDeviceType() != m_TrackingDevice->GetType())
      {
        m_ErrorMessage = "At least one tool is not of the same type like the tracking device.";
        return false;
      }
    }
    //TODO in case of Aurora: check if the tools are automatically detected by comparing the serial number
    return true;
  }
}

mitk::TrackingDeviceSource::Pointer lancet::TrackingDeviceSourceConfiguratorLancet::CreateTrackingDeviceSource()
{
  lancet::NavigationObjectVisualizationFilter::Pointer dummy; //this dummy is lost directly after creating the device
  return this->CreateTrackingDeviceSource(dummy);
}

mitk::TrackingDeviceSource::Pointer lancet::TrackingDeviceSourceConfiguratorLancet::CreateTrackingDeviceSource(
  lancet::NavigationObjectVisualizationFilter::Pointer& visualizationFilter)
{
  if (!this->IsCreateTrackingDeviceSourcePossible())
  {
    MITK_WARN << "Cannot create tracking decive: " << m_ErrorMessage;
    return nullptr;
  }

  mitk::TrackingDeviceSource::Pointer returnValue;

  us::ModuleContext* context = us::GetModuleContext();

  std::vector<us::ServiceReference<mitk::TrackingDeviceTypeCollection>> refs = context->GetServiceReferences<
    mitk::TrackingDeviceTypeCollection>();

  if (refs.empty())
  {
    MITK_ERROR << "No tracking device service found!";
  }

  mitk::TrackingDeviceTypeCollection* deviceTypeCollection = context->GetService<mitk::TrackingDeviceTypeCollection
  >(refs.front());

  //create tracking device source
  returnValue = deviceTypeCollection->GetTrackingDeviceTypeInformation(m_TrackingDevice->GetType())->
                                      CreateTrackingDeviceSource(m_TrackingDevice, m_NavigationTools, &m_ErrorMessage,
                                                                 &m_ToolCorrespondencesInToolStorage);

  //TODO: insert other tracking systems?
  if (returnValue.IsNull())
  {
    MITK_WARN << "Cannot create tracking decive: " << m_ErrorMessage;
    return nullptr;
  }

  //create visualization filter
  visualizationFilter = CreateNavigationDataObjectVisualizationFilter(returnValue, m_NavigationTools);
  if (visualizationFilter.IsNull())
  {
    MITK_WARN << "Cannot create tracking decive: " << m_ErrorMessage;
    return nullptr;
  }

  return returnValue;
}

mitk::TrackingDeviceSource::Pointer lancet::TrackingDeviceSourceConfiguratorLancet::CreateTrackingDeviceSource(
  lancet::NavigationObjectVisualizationFilter::Pointer& visualizationFilter,
  lancet::NavigationDataInReferenceCoordFilter::Pointer& referenceFilter)
{
  if (!this->IsCreateTrackingDeviceSourcePossible())
  {
    MITK_WARN << "Cannot create tracking decive: " << m_ErrorMessage;
    return nullptr;
  }

  mitk::TrackingDeviceSource::Pointer returnValue;

  us::ModuleContext* context = us::GetModuleContext();

  std::vector<us::ServiceReference<mitk::TrackingDeviceTypeCollection>> refs =
    context->GetServiceReferences<mitk::TrackingDeviceTypeCollection>();

  if (refs.empty())
  {
    MITK_ERROR << "No tracking device service found!";
  }

  mitk::TrackingDeviceTypeCollection* deviceTypeCollection =
    context->GetService<mitk::TrackingDeviceTypeCollection>(refs.front());

  // create tracking device source
  returnValue = deviceTypeCollection->GetTrackingDeviceTypeInformation(m_TrackingDevice->GetType())
                                    ->CreateTrackingDeviceSource(
                                      m_TrackingDevice, m_NavigationTools, &m_ErrorMessage,
                                      &m_ToolCorrespondencesInToolStorage);

  // TODO: insert other tracking systems?
  if (returnValue.IsNull())
  {
    MITK_WARN << "Cannot create tracking decive: " << m_ErrorMessage;
    return nullptr;
  }

  // create reference filter
  referenceFilter = CreateNavigationDataInReferenceCoordFilter(returnValue);

  // create visualization filter
  if (m_NavigationObject.IsNotNull())
  {
    visualizationFilter = CreateNavigationDataObjectVisualizationFilter(referenceFilter, m_NavigationTools,
                                                                        m_NavigationObject);
  }
  else
  {
    visualizationFilter = CreateNavigationDataObjectVisualizationFilter(referenceFilter, m_NavigationTools);
  }

  return returnValue;
}

std::string lancet::TrackingDeviceSourceConfiguratorLancet::GetErrorMessage()
{
  return this->m_ErrorMessage;
}

//############################ internal help methods ########################################
lancet::NavigationDataInReferenceCoordFilter::Pointer
lancet::TrackingDeviceSourceConfiguratorLancet::CreateNavigationDataInReferenceCoordFilter(
  mitk::NavigationDataSource::Pointer navigationDataSource)
{
  lancet::NavigationDataInReferenceCoordFilter::Pointer returnValue =
    lancet::NavigationDataInReferenceCoordFilter::New();

  returnValue->ConnectTo(navigationDataSource);
  returnValue->Update();
  return returnValue;
}

lancet::NavigationObjectVisualizationFilter::Pointer
lancet::TrackingDeviceSourceConfiguratorLancet::CreateNavigationDataObjectVisualizationFilter(
  mitk::NavigationDataSource::Pointer navigationDataSource, mitk::NavigationToolStorage::Pointer navigationTools)
{
  NavigationObjectVisualizationFilter::Pointer returnValue = NavigationObjectVisualizationFilter::New();
  returnValue->SetToolMetaDataCollection(navigationTools);
  returnValue->ConnectTo(navigationDataSource);
  return returnValue;
}

lancet::NavigationObjectVisualizationFilter::Pointer lancet::TrackingDeviceSourceConfiguratorLancet::
CreateNavigationDataObjectVisualizationFilter(mitk::NavigationDataSource::Pointer navigationDataSource,
                                              mitk::NavigationToolStorage::Pointer navigationTools,
                                              NavigationObject::Pointer navigationObject)
{
  NavigationObjectVisualizationFilter::Pointer returnValue = NavigationObjectVisualizationFilter::New();
  returnValue->SetToolMetaDataCollection(navigationTools);
  returnValue->ConnectTo(navigationDataSource);

  //bool navigationObjectHasMatchedReferenceFrame = false;
  returnValue->SetNavigationObject(navigationObject);
  // for (unsigned int i = 0; i < navigationDataSource->GetNumberOfIndexedOutputs(); i++)
  // {
  //   if (navigationObject->GetReferencFrameName() == navigationDataSource->GetOutput(i)->GetName())
  //   {
  //     returnValue->SetNavigationObjectData(i, navigationObject->GetDataNode()->GetData());
  //     mitk::AffineTransform3D::Pointer mat = mitk::AffineTransform3D::New();
  //     mitk::TransferVtkMatrixToItkTransform<mitk::AffineTransform3D>(navigationObject->GetT_Object2ReferenceFrame(),
  //                                                                    mat);
  //     returnValue->SetObjectRegistrationMatrix(i, mat);
  //     navigationObjectHasMatchedReferenceFrame = true;
  //   }
  // }
  // if (!navigationObjectHasMatchedReferenceFrame)
  // {
  //   MITK_WARN << "There is no Navigation Tool < " << navigationObject->GetReferencFrameName() << " >, NavigationObject can't connect to it";
  // }
  return returnValue;
}

int lancet::TrackingDeviceSourceConfiguratorLancet::GetToolNumberInToolStorage(unsigned int outputID)
{
  if (outputID < m_ToolCorrespondencesInToolStorage.size()) return m_ToolCorrespondencesInToolStorage.at(outputID);
  else return -1;
}

std::string lancet::TrackingDeviceSourceConfiguratorLancet::GetToolIdentifierInToolStorage(unsigned int outputID)
{
  if (outputID < m_ToolCorrespondencesInToolStorage.size()) return m_NavigationTools->GetTool(
    m_ToolCorrespondencesInToolStorage.at(outputID))->GetIdentifier();
  else return "";
}

std::vector<int> lancet::TrackingDeviceSourceConfiguratorLancet::GetToolNumbersInToolStorage()
{
  return m_ToolCorrespondencesInToolStorage;
}

std::vector<std::string> lancet::TrackingDeviceSourceConfiguratorLancet::GetToolIdentifiersInToolStorage()
{
  std::vector<std::string> returnValue = std::vector<std::string>();
  for (unsigned int i = 0; i < m_ToolCorrespondencesInToolStorage.size(); i++)
  {
    returnValue.push_back(m_NavigationTools->GetTool(m_ToolCorrespondencesInToolStorage.at(i))->GetIdentifier());
  }
  return returnValue;
}
