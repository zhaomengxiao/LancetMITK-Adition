#ifndef QLinkingHardware_h
#define QLinkingHardware_h

// Include header file for C++.
#include <memory>

// Include header file for Mitk.
#include <QmitkAbstractView.h>

#include <internal/lancetTrackingDeviceManage.h>

// Pre-declaration of third-party library reference class type.
namespace lancet { class IDevicesAdministrationService; }

class QLinkingHardware : public QmitkAbstractView
{
  Q_OBJECT
public:
  static const std::string VIEW_ID;
  QLinkingHardware();
  virtual ~QLinkingHardware() override;

  virtual void SetFocus() override;
  virtual void CreateQtPartControl(QWidget* parent) override;

protected:
  void InitializeTrackingToolsWidget();

  bool ConnectedQtInteractive();
  bool DisConnectedQtInteractive();

  bool ConnectedQtEventForDevicesService();
  bool DisConnectedQtEventForDevicesService();
  
  bool UpdateQtPartControlStyleSheet(const QString& qss);

  static lancet::IDevicesAdministrationService* GetService();

protected Q_SLOTS:
	void OnIDevicesGetStatus(std::string, lancet::TrackingDeviceManage::TrackingDeviceState);

	void OnCheckedRobotMovePosition();

	void OnPushbtnActivate();

	void OnPushbtnActivateSuccess();
private:
  struct QLinkingHardwarePrivateImp;
  std::shared_ptr<QLinkingHardwarePrivateImp> imp;
};


#endif // !QLinkingHardware_h