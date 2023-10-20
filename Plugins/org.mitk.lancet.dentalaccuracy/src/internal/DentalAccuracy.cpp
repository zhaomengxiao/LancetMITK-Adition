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
#include "DentalAccuracy.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

const std::string DentalAccuracy::VIEW_ID = "org.mitk.views.dentalaccuracy";

void DentalAccuracy::SetFocus()
{
  // m_Controls.buttonPerformImageProcessing->setFocus();
}

void DentalAccuracy::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  // connect(m_Controls.buttonPerformImageProcessing, &QPushButton::clicked, this, &DentalAccuracy::DoImageProcessing);
}


