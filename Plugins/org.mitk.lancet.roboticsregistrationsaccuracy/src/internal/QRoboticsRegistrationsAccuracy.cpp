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
#include "QRoboticsRegistrationsAccuracy.h"

// Qt
#include <QDir>
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

const std::string QRoboticsRegistrationsAccuracy::VIEW_ID = "org.mitk.views.qroboticsregistrationsaccuracy";

void QRoboticsRegistrationsAccuracy::SetFocus()
{
}

void QRoboticsRegistrationsAccuracy::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent); 
  auto test_dir = QDir(":/org.mitk.lancet.roboticsregistrationsaccuracy/");
  QFile qss(test_dir.absoluteFilePath("roboticsregistrationsaccuracy.qss"));
  if (!qss.open(QIODevice::ReadOnly))
  {
	  qWarning() << __func__ << __LINE__ << ":" << "error load file "
		  << test_dir.absoluteFilePath("roboticsregistrationsaccuracy.qss") << "\n"
		  << "error: " << qss.errorString();
  }
  // pos
  qInfo() << "log.file.pos " << qss.pos();
  m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
  qss.close();
}

void QRoboticsRegistrationsAccuracy::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}

void QRoboticsRegistrationsAccuracy::DoImageProcessing()
{
  
}
