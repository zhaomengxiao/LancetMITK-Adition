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
#include "QMedicalRecordManagement.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

const std::string QMedicalRecordManagement::VIEW_ID = "org.mitk.views.qmedicalrecordmanagement";

void QMedicalRecordManagement::SetFocus()
{
}
QMedicalRecordManagement::~QMedicalRecordManagement()
{

}
void QMedicalRecordManagement::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
}

void QMedicalRecordManagement::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}

void QMedicalRecordManagement::DoImageProcessing()
{
  QList<mitk::DataNode::Pointer> nodes = this->GetDataManagerSelection();
}
