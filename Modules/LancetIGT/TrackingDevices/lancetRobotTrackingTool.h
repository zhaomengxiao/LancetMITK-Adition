/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef LANCETROBOTTRACKINGTOOL_H
#define LANCETROBOTTRACKINGTOOL_H
#include <MitkLancetIGTExports.h>
#include <mitkTrackingTool.h>
#include "mitkTrackingTypes.h"

namespace lancet
{
  class KukaRobotDevice;
}
namespace lancet
{
  class NDITrackingDevice;

  /**Documentation
  * \brief Implementation of a robot end tool
  *
  * implements the TrackingTool interface and has the ability to
  * define the TCP(tool center point) and the load
  *
  * \ingroup LancetIGT
  */
  class MITKLANCETIGT_EXPORT RobotEndTool : public mitk::TrackingTool
  {
  public:
    friend class KukaRobotDevice;
    mitkClassMacro(RobotEndTool, TrackingTool);

    itkSetMacro(LoadData, double);
    itkGetMacro(LoadData, double);

    itkGetConstObjectMacro(TCP,mitk::AffineTransform3D)
    itkSetObjectMacro(TCP, mitk::AffineTransform3D)
  protected:
    itkFactorylessNewMacro(Self);
    itkCloneMacro(Self)
    RobotEndTool();
    ~RobotEndTool() override;

    mitk::Point3D m_VerifyPoint;
    mitk::AffineTransform3D::Pointer m_TCP;
    double m_LoadData;
  };
} // namespace lancet

#endif // LANCETROBOTTRACKINGTOOL_H
