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
#include "HansRobot.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

const std::string HansRobot::VIEW_ID = "org.mitk.views.hansrobot";

void HansRobot::SetFocus()
{
 // m_Controls.buttonPerformImageProcessing->setFocus();
}

void HansRobot::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  //connect(m_Controls.buttonPerformImageProcessing, &QPushButton::clicked, this, &HansRobot::DoImageProcessing);
}

void HansRobot::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  //// iterate all selected objects, adjust warning visibility
  //foreach (mitk::DataNode::Pointer node, nodes)
  //{
  //  if (node.IsNotNull() && dynamic_cast<mitk::Image *>(node->GetData()))
  //  {
  //    m_Controls.labelWarning->setVisible(false);
  //    m_Controls.buttonPerformImageProcessing->setEnabled(true);
  //    return;
  //  }
  //}

  //m_Controls.labelWarning->setVisible(true);
  //m_Controls.buttonPerformImageProcessing->setEnabled(false);
}
