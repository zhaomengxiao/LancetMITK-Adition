/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef QLinkingHardware_h
#define QLinkingHardware_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_QLinkingHardwareControls.h"
#include "kukaRobotDevice.h"
#include "mitkVirtualTrackingDevice.h"
#include "mitkVirtualTrackingTool.h"
#include "lancetNavigationObjectVisualizationFilter.h"
#include "lancetApplyDeviceRegistratioinFilter.h"
#include "lancetApplySurfaceRegistratioinFilter.h"
#include "lancetTrackingDeviceSourceConfigurator.h"
#include "lancetPathPoint.h"
#include "mitkTrackingDeviceSource.h"
#include "lancetVegaTrackingDevice.h"
#include <mitkNavigationDataToPointSetFilter.h>
#include "mitkNavigationToolStorageDeserializer.h"
#include <QtWidgets\qfiledialog.h>
#include "mitkIGTIOException.h"
#include "mitkNavigationToolStorageSerializer.h"
//#include "QmitkIGTCommonHelper.h"

#include <internal/lancetTrackingDeviceManage.h>

/**
  \brief QLinkingHardware

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
namespace lancet
{
	class IDevicesAdministrationService;
}
class QLinkingHardware : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
	static const std::string VIEW_ID;
	~QLinkingHardware() override;

	virtual void CreateQtPartControl(QWidget* parent) override;
	virtual void SetFocus() override;

protected:
	void ConnectToService();
	bool isauto;
	void ReadFileName();
	lancet::IDevicesAdministrationService* GetService() const;
	void setStartHardware(std::string, bool);
	void ShowToolStatus();
	mitk::NavigationToolStorage::Pointer m_ToolStorage;
	QString filename; 
	QTimer m_updateTimer;
protected Q_SLOTS:
	void on_pb_auto_clicked();
	void on_pb_success_clicked();
	void startCheckRobotMove();
	void Slot_IDevicesGetStatus(std::string, lancet::TrackingDeviceManage::TrackingDeviceState);

private:
	mitk::Point3D m_RobotStartPosition;
	Ui::QLinkingHardwareControls m_Controls;
};

#endif // QLinkingHardware_h
