/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef ACETABULARPREPARATIONWIDGET_H
#define ACETABULARPREPARATIONWIDGET_H

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_acetabularpreparationwidget.h"

/**
  \brief Acetabularpreparationwidgetview

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class Acetabularpreparationwidgetview : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;
QWidget *m_Parent;

  Ui::AcetabularPreparationWidget m_Controls;
};

#endif // ACETABULARPREPARATIONWIDGET_H
