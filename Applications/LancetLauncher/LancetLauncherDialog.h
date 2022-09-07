/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/
#ifndef LANCETLAUNCHERDIALOG_H_
#define LANCETLAUNCHERDIALOG_H_

#include <QDialog>

#include <QEventLoop>
#include <QStringList>

namespace Ui
{
  class LancetLauncherDialog;
}

class LancetLauncherDialog : public QDialog
{
  Q_OBJECT

public:
  explicit LancetLauncherDialog(QWidget *parent = nullptr);
  ~LancetLauncherDialog() override;

  QString getDemoConfiguration();

private:
  Q_SLOT void configurationSelected();
  Q_SLOT void dialogCanceled();

  Q_SLOT void selectionChanged(int row);

private:
  Ui::LancetLauncherDialog *ui;
  QStringList provisioningFiles;
  QList<QString> descriptions;
  QEventLoop eventLoop;
};

#endif
