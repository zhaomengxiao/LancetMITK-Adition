/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef LANCETAPPLYSURFACEREGISTRATIOINSTATICIMAGEFILTER_H
#define LANCETAPPLYSURFACEREGISTRATIOINSTATICIMAGEFILTER_H
#include "mitkNavigationDataToNavigationDataFilter.h"
#include "lancetNavigationObject.h"
#include "MitkLancetIGTExports.h"

namespace lancet {

  /**Documentation
  * \brief ApplyDeviceRegistratioinFilter applies a transformation
  * defined by Registration Matrix and navigation data of reference frame.
  *
  * The tracking data of MoveDevice is converted to the ReferenceDevice coordinates
  * by using the registration matrix and the navigation data of the reference frame fixed on MoveDevice.
  *
  * Before executing the filter SetRegistrationMatrix and SetNavigationDataOfRF should be called.Otherwise it won't do anything
  * 
  *
  * \ingroup LancetIGT
  */
  class MITKLANCETIGT_EXPORT ApplySurfaceRegistratioinStaticImageFilter : public mitk::NavigationDataToNavigationDataFilter
  {
  public:
    mitkClassMacro(ApplySurfaceRegistratioinStaticImageFilter, NavigationDataToNavigationDataFilter);
    itkFactorylessNewMacro(Self);
    itkCloneMacro(Self);

    /**Documentation
   * \brief Set the registration matrix between two devices,specifically from reference device/reference frame to move device
   */
    itkSetObjectMacro(RegistrationMatrix, mitk::AffineTransform3D);
    itkGetObjectMacro(RegistrationMatrix, mitk::AffineTransform3D);

    /**Documentation
   * \brief Set The NavigationData(from Reference Device) of the reference frame which attached to MoveDevice;
   * !If Not Set!, no displacement is allowed between the two devices after the registration is completed
   */
    itkSetObjectMacro(NavigationDataOfRF, mitk::NavigationData);
    itkGetObjectMacro(NavigationDataOfRF, mitk::NavigationData);


	// itkSetObjectMacro(navigationImage, lancet::NavigationObject);
	// itkGetObjectMacro(navigationImage, lancet::NavigationObject);

  protected:

    /**
    * \brief Constructor
    **/
    ApplySurfaceRegistratioinStaticImageFilter();
    /**
    * \brief Destructor
    **/
    ~ApplySurfaceRegistratioinStaticImageFilter() override;

    /**Documentation
    * \brief filter execute method
    *
    * transforms navigation data
    */
    void GenerateData() override;

    mitk::NavigationData::Pointer m_NavigationDataOfRF;
    mitk::AffineTransform3D::Pointer m_RegistrationMatrix; /// T (surface to RF)
	//lancet::NavigationObject::Pointer m_navigationImage;
  };
} // namespace lancet

#endif // LANCETAPPLYSURFACEREGISTRATIOINSTATICIMAGEFILTER_H
