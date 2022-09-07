/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "DevicePerspective.h"

// berry includes
#include "berryIViewLayout.h"

DevicePerspective::DevicePerspective()
{
}

void DevicePerspective::CreateInitialLayout(berry::IPageLayout::Pointer layout)
{
  // Editors are placed for free.
  QString editorAreaId = layout->GetEditorArea();

  //! [Visibility of editor area]
  // set editor area to visible
  layout->SetEditorAreaVisible(true);
  //! [Visibility of editor area]

  // create folder with alignment "right"
  berry::IFolderLayout::Pointer right = layout->CreateFolder("right", berry::IPageLayout::RIGHT, 0.5f, editorAreaId);
  // add emptyview1 to the folder
  right->AddView("org.mitk.views.mitkigtnavigationtoolmanager");
  right->AddView("org.mitk.views.navigationdataplayer");
  right->AddView("org.mitk.views.mitkigttrackingtoolbox");
  right->AddView("org.mitk.views.igtnavigationtoolcalibration");
  right->AddView("org.mitk.views.IGTFiducialRegistration");

  // create folder with alignment "left"
  berry::IFolderLayout::Pointer left = layout->CreateFolder("left", berry::IPageLayout::LEFT, 0.5f, editorAreaId);
  // add emptyview1 to the folder
  left->AddView("org.mitk.views.robotview");
}
