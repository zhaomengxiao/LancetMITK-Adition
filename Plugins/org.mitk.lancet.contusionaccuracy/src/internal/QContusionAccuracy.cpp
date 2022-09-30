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
#include "QContusionAccuracy.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

const std::string QContusionAccuracy::VIEW_ID = "org.mitk.views.qcontusionaccuracy";

void QContusionAccuracy::SetFocus()
{
}

void QContusionAccuracy::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
}

void QContusionAccuracy::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}

void QContusionAccuracy::DoImageProcessing()
{
}
