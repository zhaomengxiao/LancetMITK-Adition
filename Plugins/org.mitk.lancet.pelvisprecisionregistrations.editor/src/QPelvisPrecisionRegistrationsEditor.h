/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef QPelvisPrecisionRegistrationsEditor_H
#define QPelvisPrecisionRegistrationsEditor_H

// mitk gui qt common plugin
#include <QTImer>
#include <QmitkAbstractMultiWidgetEditor.h>

#include <mitkILinkedRenderWindowPart.h>

#include <org_mitk_lancet_pelvisprecisionregistrations_editor_Export.h>

#include <ctkServiceTracker.h>

#include <berryIPreferencesService.h>
// c++
#include <memory>

#include "ui_QPelvisPrecisionRegistrationsEditor.h"
class QmitkStdMultiWidget;
class QPelvisPrecisionRegistrationsEditorPrivate;
namespace lancet { class IDevicesAdministrationService; }
namespace lancet::spatial_fitting { class PelvicRoughRegistrationsModel; }

/**
 * @brief
 */
class ORG_MITK_LANCET_PELVISPRECISIONREGISTRATIONS_EDITOR QPelvisPrecisionRegistrationsEditor final
	: public berry::EditorPart, public berry::IPartListener
{
  Q_OBJECT

public:

  berryObjectMacro(QPelvisPrecisionRegistrationsEditor, berry::EditorPart, berry::IPartListener);

  static const QString EDITOR_ID;
public:
  QPelvisPrecisionRegistrationsEditor();
  virtual ~QPelvisPrecisionRegistrationsEditor() override;


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
public slots:
	//void onV
private:
	void InitializeDataStorageForService();

	void InitializeMitkMultiWidget();
	void UnInitializeMitkMultiWidget();

	void InitializeQtEventOnService();
	void UnInitializeQtEventOnService();

	void InitializeMitkMultiWidgetOnCollectModel();
	void InitializeMitkMultiWidgetOnVerifyModel();

	void InitializeWidgetOnlyOne();

	itk::SmartPointer<mitk::DataStorage> GetDataStorage() const;

	using PelvicRoughRegistrationsModel = lancet::spatial_fitting::PelvicRoughRegistrationsModel;
	itk::SmartPointer<PelvicRoughRegistrationsModel> GetServiceModel() const;

	static lancet::IDevicesAdministrationService* GetDevicesService();
private:
	virtual void OnPreferencesChanged(const berry::IBerryPreferences*);
	Ui::QPelvisPrecisionRegistrationsEditor m_Controls;
protected:
	QTimer updateProbeTm;
	QStringList listDataStorageNodes;
	typedef berry::IPreferencesService berryIPreferencesService;
	QSharedPointer<QWidget> widgetInstace;
	ctkServiceTracker<berryIPreferencesService*> m_PrefServiceTracker;
	berry::IBerryPreferences::Pointer m_Prefs;
};

#endif // QPelvisPrecisionRegistrationsEditor_H
