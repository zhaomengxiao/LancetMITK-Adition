/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef QPelvisRoughRegistrations_h
#define QPelvisRoughRegistrations_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_QPelvisRoughRegistrationsControls.h"

namespace mitk{ class NavigationData; }
namespace lancet { class IDevicesAdministrationService; }
namespace lancet::spatial_fitting{ class PelvicRoughRegistrationsModel; }
/**
  \brief QPelvisRoughRegistrations

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class QPelvisRoughRegistrations : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

	QPelvisRoughRegistrations();
	virtual ~QPelvisRoughRegistrations();
protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;

  /// \brief Called when the user clicks the GUI button
  void DoImageProcessing();

  Ui::QPelvisRoughRegistrationsControls m_Controls;

protected slots:
	void on_toolCollector_fail(int);

	void on_toolCollector_complete(mitk::NavigationData*);

	void on_toolCollector_step(int, mitk::NavigationData*);

	void on_pushButtonCapturePelvis_Landmark_clicked();

	void on_pushButtonClearOne_Landmark_clicked();
private:
	using PelvicRoughRegistrationsModel = lancet::spatial_fitting::PelvicRoughRegistrationsModel;
	berry::SmartPointer<PelvicRoughRegistrationsModel> GetServiceModel() const;

	void Initialize();
	void InitializeTrackingToolsWidget();
	void InitializeQtWidgetEventAtOnly();
	void InitializeQtEventToService();
	void InitializeCollectStateForQtWidget();

	void AppendVegaPointOnBack(const mitk::Point3D&);
	void RemoveVegaPointOnBack();

	void VerifyImageRegistor();

	static lancet::IDevicesAdministrationService* GetDevicesService();
private:
	struct QPelvisRoughRegistrationsPrivateImp;
	std::shared_ptr<QPelvisRoughRegistrationsPrivateImp> imp;
};

#endif // QPelvisRoughRegistrations_h
