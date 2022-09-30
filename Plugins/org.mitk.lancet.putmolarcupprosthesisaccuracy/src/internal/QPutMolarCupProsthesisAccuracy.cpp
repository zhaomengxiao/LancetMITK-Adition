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
#include "QPutMolarCupProsthesisAccuracy.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

const std::string QPutMolarCupProsthesisAccuracy::VIEW_ID = "org.mitk.views.qputmolarcupprosthesisaccuracy";

void QPutMolarCupProsthesisAccuracy::SetFocus()
{
  
}

void QPutMolarCupProsthesisAccuracy::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
}

void QPutMolarCupProsthesisAccuracy::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}

void QPutMolarCupProsthesisAccuracy::DoImageProcessing()
{
}
