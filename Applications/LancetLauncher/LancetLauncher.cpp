/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include <mitkBaseApplication.h>

#include "LancetLauncherDialog.h"

#include <QFileInfo>
#include <QVariant>

int main(int argc, char **argv)
{
  mitk::BaseApplication app(argc, argv);
  app.setApplicationName("LancetLauncher");
  app.setOrganizationName("Lancet");
  app.initializeQt();

  LancetLauncherDialog demoDialog;
  QString selectedConfiguration = demoDialog.getDemoConfiguration();

  if (selectedConfiguration.isEmpty())
    return EXIT_SUCCESS;

  app.setProvisioningFilePath(selectedConfiguration);

  // We create the application id relying on a convention:
  // org.mitk.lancet.<configuration-name>
  QString appId = "org.mitk.lancet.";
  QStringList appIdTokens = QFileInfo(selectedConfiguration).baseName().toLower().split('_', QString::SkipEmptyParts);
  appId += appIdTokens.size() > 1 ? appIdTokens.at(1) : appIdTokens.at(0);

	if (appId == "org.mitk.lancet.tha")
	{
		appId = "org.mitk.lancet.tha.application";
	}
  app.setProperty(mitk::BaseApplication::PROP_APPLICATION, appId);

  return app.run();
}
