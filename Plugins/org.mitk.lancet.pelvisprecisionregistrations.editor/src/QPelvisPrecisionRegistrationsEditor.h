#ifndef QPelvisPrecisionRegistrationsEditor_H
#define QPelvisPrecisionRegistrationsEditor_H
#include <org_mitk_lancet_pelvisprecisionregistrations_editor_Export.h>

// Include header files for Qt.
#include <QTimer>

// Include header files for mitk.
#include <QmitkAbstractMultiWidgetEditor.h>

// define mitk namespace class.
namespace mitk { class DataStorage; } // namespace mitk::DataStorage;
namespace lancet { class IDevicesAdministrationService; } // namespace lancet::IDevicesAdministrationService;
namespace lancet::spatial_fitting { class PelvicRegistrationsModel; } // namespace lancet::spatial_fitting::PelvicRegistrationsModel;

class ORG_MITK_LANCET_PELVISPRECISIONREGISTRATIONS_EDITOR 
  QPelvisPrecisionRegistrationsEditor final : public berry::EditorPart,
   public berry::IPartListener
{
  Q_OBJECT

public:

  berryObjectMacro(QPelvisPrecisionRegistrationsEditor, berry::EditorPart, berry::IPartListener);

  static const QString EDITOR_ID;
public:
  QPelvisPrecisionRegistrationsEditor();
  virtual ~QPelvisPrecisionRegistrationsEditor() override;

public:
  /**
  * @brief Overridden from berry::EditorPart
  */
  void CreatePartControl(QWidget* parent) override;

  /**
  * @brief Overridden from berry::EditorPart
  */
  virtual void SetFocus() override;

  /** \see berry::IEditorPart::DoSave */
  void DoSave() override;

  /** \see berry::IEditorPart::DoSaveAs */
  void DoSaveAs() override;

  /** \see berry::IEditorPart::IsSaveOnCloseNeeded */
  virtual bool IsSaveOnCloseNeeded() const override;

  /** \see berry::IEditorPart::IsDirty */
  bool IsDirty() const override;

  /** \see berry::IEditorPart::IsSaveAsAllowed */
  bool IsSaveAsAllowed() const override;

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
  * @brief Overridden from berry::IPartListener
  */
  virtual berry::IPartListener::Events::Types GetPartEventTypes() const override;

	virtual int GetRegisterPointSize() const;

public slots:
	//void OnVegaPointChange();

	void onCollectModelOfCapturePoint(int);
private:
  void InitializeDataStorageForService();
  void UnInitializeDataStorageForService();

  void InitializeMitkMultiWidget();
  void UnInitializeMitkMultiWidget();
  
  void InitializeQtEventOnService();
  void UnInitializeQtEventOnService();

	void BackupDataStorageNodes(const itk::SmartPointer<mitk::DataStorage>&);
	void RecoverDataStorageNodes(const itk::SmartPointer<mitk::DataStorage>&);

	void InitializeMitkMultiWidgetOnCollectModel();
	void UnInitializeMitkMultiWidgetOnCollectModel();
	void InitializeMitkMultiWidgetOnVerifyModel();
	void UnInitializeMitkMultiWidgetOnVerifyModel();

	void UpdateSelectedPoint(int index, bool selected, float size = 8.0f);

  static lancet::IDevicesAdministrationService* GetDevicesService();

  static itk::SmartPointer<lancet::spatial_fitting::PelvicRegistrationsModel> 
    GetServiceModel();

	static itk::SmartPointer<mitk::DataStorage> GetDataStorage();
private:
  struct PrivateImp;
  std::shared_ptr<PrivateImp> imp;
};

#endif // !QPelvisPrecisionRegistrationsEditor_H