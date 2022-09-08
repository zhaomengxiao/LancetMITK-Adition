/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "QLancetExamplePerspective.h"

// Berry
#include "berryIViewLayout.h"

QLancetExamplePerspective::QLancetExamplePerspective()
{
}

void QLancetExamplePerspective::CreateInitialLayout(berry::IPageLayout::Pointer layout)
{
  // Hides the editor area.
  layout->SetEditorAreaVisible(false);

  /*layout->AddView(
	  "org.mitk.views.acetabularpreparationwidget", berry::IPageLayout::LEFT, 0.2f, layout->GetEditorArea());*/

  layout->GetViewLayout(layout->GetEditorArea())->SetMoveable(false);
  layout->GetViewLayout(layout->GetEditorArea())->SetCloseable(false);
}
