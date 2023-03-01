/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef QPelvisPrecisionRegistrations_h
#define QPelvisPrecisionRegistrations_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_QPelvisPrecisionRegistrationsControls.h"

namespace mitk { class NavigationData; }
namespace lancet { class IDevicesAdministrationService; }
namespace lancet::spatial_fitting { class PelvicRoughRegistrationsModel; }

/**
  \brief QPelvisPrecisionRegistrations

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class QPelvisPrecisionRegistrations : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

	QPelvisPrecisionRegistrations();
	virtual ~QPelvisPrecisionRegistrations();
protected slots:
	void on_toolCollector_fail(int);

	void on_toolCollector_complete(mitk::NavigationData*);

	void on_toolCollector_step(int, mitk::NavigationData*);

	void on_pushButtonCapturePelvis_Registration_clicked();
protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;

  /// \brief Called when the user clicks the GUI button
  void DoImageProcessing();

private:
	using PelvicRoughRegistrationsModel = lancet::spatial_fitting::PelvicRoughRegistrationsModel;
	itk::SmartPointer<PelvicRoughRegistrationsModel> GetServiceModel() const;

	void Initialize();
	void InitializeTrackingToolsWidget();
	void InitializeQtWidgetEventAtOnly();
	void InitializeQtEventToService();
	void InitializeCollectStateForQtWidget();
	bool UpdateQtPartControlStyleSheet(const QString& qss);

	void AppendVegaPointOnBack(const mitk::Point3D&);
	void RemoveVegaPointOnBack();

	void VerifyImageRegistor();

	static lancet::IDevicesAdministrationService* GetDevicesService();
  Ui::QPelvisPrecisionRegistrationsControls m_Controls;
private:
	struct PrivateImp;
	std::shared_ptr<PrivateImp> imp;
};

#endif // QPelvisPrecisionRegistrations_h
