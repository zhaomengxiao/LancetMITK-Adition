/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef LANCETTRACKINGDEVICESOURCECONFIGURATOR_H
#define LANCETTRACKINGDEVICESOURCECONFIGURATOR_H
#include <MitkLancetIGTExports.h>

//itk includes
#include <itkObject.h>

//mitk IGT includes
#include "lancetNavigationDataInReferenceCoordFilter.h"
//#include "mitkNavigationDataObjectVisualizationFilter.h"
#include "lancetNavigationObjectVisualizationFilter.h"
#include "mitkNavigationToolStorage.h"
#include "mitkTrackingDeviceSource.h"
#include "lancetNavigationObject.h"
namespace lancet {
  /**Documentation
  * \brief This class offers a factory method for objects of the class TrackingDeviceSource. It initializes this TrackingDeviceSource with
  *        the given navigation tools and the given tracking device. The factory method also checks if all tools are valid and of the same
  *        type like the TrackingDevice. You can do this check before trying to create the TrackingDeviceSource by calling the method
  *        IsCreateTrackingDeviceSourcePossible(), if it returns false you might want to get the error message by calling the method
  *        GetErrorMessage().
  * \ingroup IGT
  */
  class MITKLANCETIGT_EXPORT TrackingDeviceSourceConfiguratorLancet : public itk::Object
  {
  public:
    mitkClassMacroItkParent(TrackingDeviceSourceConfiguratorLancet,  itk::Object);
    mitkNewMacro2Param(Self,mitk::NavigationToolStorage::Pointer,mitk::TrackingDevice::Pointer);
    mitkNewMacro3Param(Self, mitk::NavigationToolStorage::Pointer, mitk::TrackingDevice::Pointer,lancet::NavigationObject::Pointer);

    /** @return Returns if it's possible to create a tracking device source, which means the tools are checked
     *          if they are of the same type like the tracking device, etc. If it returns false you can get
     *          the reason for this by getting the error message.
     */
    bool IsCreateTrackingDeviceSourcePossible();

    /** @return Returns a new TrackingDeviceSource. Returns nullptr if there was an error on creating the
     *          TrackingDeviceSource. If there was an error it's availiable as error message.
     */
    mitk::TrackingDeviceSource::Pointer CreateTrackingDeviceSource();

    /** @return Returns a new TrackingDeviceSource. Returns nullptr if there was an error on creating the
     *          TrackingDeviceSource. If there was an error it's availiable as error message.
     *  @param visualizationFilter (return value) returns a visualization filter which is already connected to the tracking device source.
     *                             This filter visualises the surfaces which are availiable by the navigation tool storage.
     */
    mitk::TrackingDeviceSource::Pointer CreateTrackingDeviceSource(lancet::NavigationObjectVisualizationFilter::Pointer &visualizationFilter);

    /** @return Returns a new TrackingDeviceSource. Returns nullptr if there was an error on creating the
     *          TrackingDeviceSource. If there was an error it's availiable as error message.
     *  @param visualizationFilter (return value) returns a visualization filter which is already connected to the
     * tracking device source. This filter visualises the surfaces which are availiable by the navigation tool storage.
     */
    mitk::TrackingDeviceSource::Pointer CreateTrackingDeviceSource(
      lancet::NavigationObjectVisualizationFilter::Pointer &visualizationFilter,lancet::NavigationDataInReferenceCoordFilter::Pointer &referenceFilter);

    /** @return Returns the internal number of the corresponding tool in the tool storage of a output navigation data. Returns -1 if there was an error. */
    int GetToolNumberInToolStorage(unsigned int outputID);

    /** @return Returns the identifier of the corresponding tool in the tool storage of a output navigation data. Returns an empty string if there was an error.*/
    std::string GetToolIdentifierInToolStorage(unsigned int outputID);

    /** @return Returns a vector with all internal numbers of the corresponding tools in the tool storage of all outputs.
      *         The order is the same like the order of the outputs. Returns an empty vector if there was an error.
      */
    std::vector<int> GetToolNumbersInToolStorage();

    /** @return Returns a vector with all identifier of the corresponding tools in the tool storage of all outputs.
      *         The order is the same like the order of the outputs. Returns an empty vector if there was an error.
      */
    std::vector<std::string> GetToolIdentifiersInToolStorage();

    /** @return Returns a modified navigation tool storage which holds the tools currently in use in
      *         the same order like the output ids of the pipline.
      */
    mitk::NavigationToolStorage::Pointer GetUpdatedNavigationToolStorage();

    /** @return Returns the current error message. Returns an empty string if there was no error.
     */
    std::string GetErrorMessage();

  protected:
    TrackingDeviceSourceConfiguratorLancet(mitk::NavigationToolStorage::Pointer NavigationTools, mitk::TrackingDevice::Pointer TrackingDevice);
    TrackingDeviceSourceConfiguratorLancet(mitk::NavigationToolStorage::Pointer NavigationTools, mitk::TrackingDevice::Pointer TrackingDevice,lancet::NavigationObject::Pointer NavigationObject);
    ~TrackingDeviceSourceConfiguratorLancet() override;

    mitk::NavigationToolStorage::Pointer m_NavigationTools;
    mitk::TrackingDevice::Pointer m_TrackingDevice;
    lancet::NavigationObject::Pointer m_NavigationObject;
    std::string m_ErrorMessage;
    std::vector<int> m_ToolCorrespondencesInToolStorage;

    lancet::NavigationDataInReferenceCoordFilter::Pointer CreateNavigationDataInReferenceCoordFilter(
      mitk::NavigationDataSource::Pointer trackingDeviceSource);

    lancet::NavigationObjectVisualizationFilter::Pointer CreateNavigationDataObjectVisualizationFilter(mitk::NavigationDataSource::Pointer navigationDataSource, mitk::NavigationToolStorage::Pointer navigationTools);

    lancet::NavigationObjectVisualizationFilter::Pointer CreateNavigationDataObjectVisualizationFilter(mitk::NavigationDataSource::Pointer navigationDataSource, mitk::NavigationToolStorage::Pointer navigationTools,lancet::NavigationObject::Pointer navigationObject);
  };
} // namespace mitk
#endif // LANCETTRACKINGDEVICESOURCECONFIGURATOR_H
