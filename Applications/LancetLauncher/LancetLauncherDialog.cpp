/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "LancetLauncherDialog.h"
#include "ui_LancetLauncherDialog.h"

#include <QCoreApplication>
#include <QDir>
#include <QEventLoop>
#include <QPushButton>

#include <iostream>
LancetLauncherDialog::LancetLauncherDialog(QWidget *parent)
  : QDialog(parent), ui(new Ui::LancetLauncherDialog)
{
  ui->setupUi(this);

  connect(this, SIGNAL(accepted()), this, SLOT(configurationSelected()));
  connect(this, SIGNAL(rejected()), this, SLOT(dialogCanceled()));
  connect(ui->appList, SIGNAL(currentRowChanged(int)), this, SLOT(selectionChanged(int)));

  // Get all LancetLauncher_*.provisioning files
  QDir appDir(QApplication::applicationDirPath());
  QString appDirPath;
  
  std::cout << appDir.absolutePath().toStdString() << std::endl;
#ifdef CMAKE_INTDIR
  appDir.cdUp();
#endif

  provisioningFiles = appDir.entryList(
    QStringList(QApplication::applicationName() + "_*.provisioning"), QDir::Files | QDir::Readable, QDir::Name);
#ifdef Q_OS_MAC
  /*
   * On Mac, if started from the build directory the .provisioning file is located at:
   * <MITK-build/bin/LancetLauncher_*.provisioning>
   * but the executable path is:
   * <MITK-build/bin/LancetLauncher.app/Contents/MacOS/LancetLauncher>
   * In this case we have to cdUp threetimes.
   *
   * During packaging however the MitkWorkbench.provisioning file is placed at the same
   * level like the executable, hence nothing has to be done.
   */

  if (provisioningFiles.empty())
  {
    appDir.cdUp();
    appDir.cdUp();
    appDir.cdUp();
    provisioningFiles = appDir.entryList(
      QStringList(QApplication::applicationName() + "_*.provisioning"), QDir::Files | QDir::Readable, QDir::Name);
  }
#endif

  foreach (QString provFile, provisioningFiles)
  {
    int startIndex = provFile.indexOf('_');
    int endIndex = provFile.lastIndexOf('.');
    ui->appList->addItem(provFile.mid(startIndex + 1, endIndex - startIndex - 1));

    descriptions.push_back(QString());
    QString descriptionFileName = provFile.left(provFile.lastIndexOf('.')) + ".txt";
    QFile descriptionFile(appDir.filePath(descriptionFileName));

    if (descriptionFile.exists())
    {
      if (descriptionFile.open(QIODevice::ReadOnly))
      {
        descriptions.back() = descriptionFile.readAll();
      }
    }
  }

  // if (ui->appList->currentRow() == -1)
  //   ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

LancetLauncherDialog::~LancetLauncherDialog()
{
  delete ui;
}

QString LancetLauncherDialog::getDemoConfiguration()
{
  this->show();

  if (eventLoop.exec() == 0)
  {
    QDir appDir = QCoreApplication::applicationDirPath();

#ifdef CMAKE_INTDIR
    appDir.cdUp();
#endif

#ifdef Q_OS_MAC
    /*
     * On Mac, if started from the build directory the .provisioning file is located at:
     * <MITK-build/bin/LancetLauncher_*.provisioning>
     * but the executable path is:
     * <MITK-build/bin/LancetLauncher.app/Contents/MacOS/LancetLauncher>
     * In this case we have to cdUp threetimes.
     *
     * During packaging however the MitkWorkbench.provisioning file is placed at the same
     * level like the executable, hence nothing has to be done.
     */
    QFileInfo filePath(appDir.filePath(provisioningFiles[ui->appList->currentRow()]));

    if (!filePath.exists())
    {
      appDir.cdUp();
      appDir.cdUp();
      appDir.cdUp();
    }
#endif

    return appDir.filePath(provisioningFiles[ui->appList->currentRow()]);
  }

  return QString();
}

void LancetLauncherDialog::configurationSelected()
{
  eventLoop.exit(0);
}

void LancetLauncherDialog::dialogCanceled()
{
  eventLoop.exit(1);
}

void LancetLauncherDialog::selectionChanged(int row)
{
  if (row > -1)
  {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    ui->description->setHtml(this->descriptions[row]);
  }
  else
  {
    ui->description->clear();
  }
}
