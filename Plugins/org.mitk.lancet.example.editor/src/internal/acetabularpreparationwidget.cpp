/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "acetabularpreparationwidget.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

const std::string Acetabularpreparationwidgetview::VIEW_ID = "org.mitk.views.acetabularpreparationwidget";

void Acetabularpreparationwidgetview::SetFocus()
{
  
}

void Acetabularpreparationwidgetview::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
   m_Parent = parent;
  m_Controls.setupUi(parent);
  m_Parent->setEnabled(true);
  
}
