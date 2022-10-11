/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef QMedicalRecordManagementEditor_H
#define QMedicalRecordManagementEditor_H

// mitk gui qt common plugin
#include <QmitkAbstractMultiWidgetEditor.h>

#include <mitkILinkedRenderWindowPart.h>

#include <org_mitk_lancet_medicalrecordmanagement_editor_Export.h>

#include <ctkServiceTracker.h>

#include <berryIPreferencesService.h>
// c++
#include <memory>

#include "ui_QMedicalRecordManagementEditor.h"
class QmitkStdMultiWidget;
class QMedicalRecordManagementEditorPrivate;

/**
 * @brief
 */
class ORG_MITK_LANCET_MEDICALRECORDMANAGEMENT_EDITOR QMedicalRecordManagementEditor final
	: public berry::EditorPart, public berry::IPartListener
{
  Q_OBJECT

public:

  berryObjectMacro(QMedicalRecordManagementEditor, berry::EditorPart, berry::IPartListener);

  static const QString EDITOR_ID;
public:
  QMedicalRecordManagementEditor();
  virtual ~QMedicalRecordManagementEditor() override;


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
private:
	virtual void OnPreferencesChanged(const berry::IBerryPreferences*);
	Ui::QMedicalRecordManagementEditor m_Controls;
protected:
	typedef berry::IPreferencesService berryIPreferencesService;
	//berry::IEditorInput::Pointer editorInput;
	//berry::IEditorSite::Pointer editorSite;
	QSharedPointer<QWidget> widgetInstace;
	ctkServiceTracker<berryIPreferencesService*> m_PrefServiceTracker;
	berry::IBerryPreferences::Pointer m_Prefs;
};

#endif // QMedicalRecordManagementEditor_H
