/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// berry includes
#ifndef DEVICEPERSPECTIVE_H
#define DEVICEPERSPECTIVE_H

#include <berryIPerspectiveFactory.h>

// Qt includes
#include <QObject>

class DevicePerspective : public QObject, public berry::IPerspectiveFactory
{
  Q_OBJECT
  Q_INTERFACES(berry::IPerspectiveFactory)

public:
  DevicePerspective();

  void CreateInitialLayout(berry::IPageLayout::Pointer layout) override;
};

#endif /* MINIMUMPERSPECTIVE_H_ */
