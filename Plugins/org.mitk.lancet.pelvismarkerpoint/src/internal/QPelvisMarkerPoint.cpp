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
#include "QPelvisMarkerPoint.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

const std::string QPelvisMarkerPoint::VIEW_ID = "org.mitk.views.qpelvismarkerpoint";

void QPelvisMarkerPoint::SetFocus()
{
}

void QPelvisMarkerPoint::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
}

void QPelvisMarkerPoint::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}

void QPelvisMarkerPoint::DoImageProcessing()
{
 
}
