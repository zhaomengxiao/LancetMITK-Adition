#ifndef QLinkingHardWareEditor_H
#define QLinkingHardWareEditor_H

#include <org_mitk_lancet_linkinghardware_editor_Export.h>

// Include header file for brrey.
#include <berryEditorPart.h>
#include <berryIPreferences.h>
#include <berryIPartListener.h>

// Inlcude header file for lancet.
#include <internal/lancetTrackingDeviceManage.h>

namespace lancet { class IDevicesAdministrationService; }
namespace berry { class IBerryPreferences; }

class ORG_MITK_LANCET_LINKINGHARDWARE_EDITOR QLinkingHardWareEditor final
	: public berry::EditorPart, public berry::IPartListener
{
  Q_OBJECT
public:
  berryObjectMacro(QLinkingHardWareEditor, berry::EditorPart, berry::IPartListener);

  static const QString EDITOR_ID;
  
  QLinkingHardWareEditor();
  virtual ~QLinkingHardWareEditor() override;

  void CreatePartControl(QWidget* parent) override;

  /**
   * Get the preferences for this editor.
   */
  virtual berry::IPreferences::Pointer GetPreferences() const;
  
  /**
   * Initializes this editor by checking for a valid mitk::DataStorageEditorInput as <code>input</code>.
   *
   * \see berry::IEditorPart::Init
   */
  void Init(berry::IEditorSite::Pointer site, berry::IEditorInput::Pointer input) override;
  
  /**
  * \brief Overridden from berry::IPartListener
  */
  virtual berry::IPartListener::Events::Types GetPartEventTypes() const override;

  /**
  * \brief Returns the device microservice instance object. 
  */
  static lancet::IDevicesAdministrationService* GetDevicesService();
public:
  /**
  * \brief Overridden from berry::EditorPart
  */
  virtual void SetFocus() override;

  /** \see berry::IEditorPart::DoSave */
  void DoSave() override {}

  /** \see berry::IEditorPart::DoSaveAs */
  void DoSaveAs() override {}

  /** \see berry::IEditorPart::IsSaveOnCloseNeeded */
  virtual bool IsSaveOnCloseNeeded() const override { return true; }

  /** \see berry::IEditorPart::IsDirty */
  bool IsDirty() const override  { return true; }

  /** \see berry::IEditorPart::IsSaveAsAllowed */
  bool IsSaveAsAllowed() const override  { return true; }

protected:
  bool ConnectedQtSlotForDevicesService();
  void UpdateQtWidgetStyleOfDeviceService(const QString&);

  void UpdateQtWidgetQssStyle(const QString&);

  virtual void OnPreferencesChanged(const berry::IBerryPreferences*);
protected Q_SLOTS:
  void OnDevicesStatePropertyChange(std::string, lancet::TrackingDeviceManage::TrackingDeviceState);
private:
  struct QLinkingHardWareEditorPrivateImp;
  std::shared_ptr<QLinkingHardWareEditorPrivateImp> imp;
};

#endif // !QLinkingHardWareEditor_H