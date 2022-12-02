/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef LANCETROBOTTRACKINGTOOL_H
#define LANCETROBOTTRACKINGTOOL_H

#include <mitkTrackingTool.h>
#include "mitkTrackingTypes.h"
#include "MitkLancetIGTExports.h"

namespace lancet
{
  typedef mitk::Point<mitk::ScalarType, 6> xyzabc;
  /**Documentation
  * \brief Implementation of a passive NDI optical tool
  *
  * implements the TrackingTool interface and has the ability to
  * load an srom file that contains the marker configuration for that tool
  *
  * \ingroup IGT
  */
  class MITKLANCETIGT_EXPORT RobotTool : public mitk::TrackingTool
  {
  public:
    friend class KukaRobotDevice_New;
    friend class KukaRobotDevice;

    mitkClassMacro(RobotTool, TrackingTool);

    
    itkSetMacro(TCP, xyzabc);
    itkGetMacro(TCP, xyzabc);
  protected:
    itkFactorylessNewMacro(Self);
    itkCloneMacro(Self)
    RobotTool();
    ~RobotTool() override;

    xyzabc m_TCP;

  };
} // namespace lancet
#endif // LANCETROBOTTRACKINGTOOL_H
