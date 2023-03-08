/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef LANCETNAVIGATIONSCENEFILTER_H
#define LANCETNAVIGATIONSCENEFILTER_H
#include "mitkNavigationDataToNavigationDataFilter.h"
#include "MitkLancetIGTExports.h"
#include "lancetTreeCoords.h"
#include "mitkTrackingDeviceSource.h"

namespace lancet {

  /**Documentation
  * \brief NavigationDataReferenceTransformFilter applies a itk-landmark-transformation
  * defined by source and target NavigationDatas.
  *
  * Before executing the filter SetSourceNavigationDatas and SetTargetNavigationDatas must be called.
  * The amount of given NavigationDatas must be the same for source and target.
  * If source or target points are changed after calling SetXXXNavigationDatas, the corresponding SetXXXNavigationDatas
  * method has to be called again to apply the changes.
  * If UseICPInitialization is false (standard value, or set with SetUseICPInitialization(false) or UseICPInitializationOff())
  * then source landmarks and target landmarks with the same ID must correspondent to each other.
  * (--> source landmark with ID x will be mapped to target landmark with ID x).
  * If you do not know the correspondences, call SetUseICPInitialization(true) or UseICPInitializationOn()
  * to let the filter guess the correspondences during initialization with an iterative closest point search.
  * This is only possible, if at least 6 source and target landmarks are available.
  *
  * \ingroup IGT
  */
  class MITKLANCETIGT_EXPORT NavigationSceneFilter : public mitk::NavigationDataToNavigationDataFilter
  {
  public:
    mitkClassMacro(NavigationSceneFilter, NavigationDataToNavigationDataFilter);
    itkFactorylessNewMacro(Self);
    itkCloneMacro(Self);

    itkGetMacro(ReferenceName, std::string);
    itkSetMacro(ReferenceName, std::string);

    itkGetMacro(NavigationScene, NavigationScene::Pointer);
    itkSetMacro(NavigationScene, NavigationScene::Pointer);

    void SetTrackingDevice(mitk::TrackingDeviceSource::Pointer trackingDeviceSource);
    void AddTrackingDevice(mitk::TrackingDeviceSource::Pointer trackingDeviceSource, std::string fatherSceneNodeName);
  protected:

    /**
    * \brief Constructor
    **/
    NavigationSceneFilter();
    /**
    * \brief Destructor
    **/
    ~NavigationSceneFilter() override;

    void GenerateData() override;

    void CreateNavigationSceneFromTrackingDeviceSource(mitk::TrackingDeviceSource::Pointer trackingDeviceSource, NavigationScene::Pointer tree);
    void AppendNavigationSceneFromTrackingDeviceSource(mitk::TrackingDeviceSource::Pointer trackingDeviceSource, NavigationScene::Pointer tree, std::string fatherNodeName);
   

    std::string m_ReferenceName;
    NavigationScene::Pointer m_NavigationScene;
  };
} // namespace lancet

#endif // LANCETNAVIGATIONSCENEFILTER_H
