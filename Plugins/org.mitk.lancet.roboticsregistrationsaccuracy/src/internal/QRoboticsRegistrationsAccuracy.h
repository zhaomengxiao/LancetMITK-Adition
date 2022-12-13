/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef QRoboticsRegistrationsAccuracy_h
#define QRoboticsRegistrationsAccuracy_h

// berry
#include <berryISelectionListener.h>

// mitk
#include <mitkNavigationData.h>
#include <QmitkAbstractView.h>

// ui
#include "ui_QRoboticsRegistrationsAccuracyControls.h"

namespace lancet::spatial_fitting
{
	class ProbeCheckPointModel;
}


/**
  \brief QRoboticsRegistrationsAccuracy

  The probe accuracy detection function is used to check the accuracy deviation of 
  the probe under the fixed tooling. This function plug-in is mainly for interface 
  interaction.

  This class is driven by the mitk plug-in framework, and external input parameters 
  are provided by the mitk microservice.

  \sa QmitkAbstractView
  \ingroup org_mitk_lancet_roboticsregistrationsaccuracy
*/
class QRoboticsRegistrationsAccuracy : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;

  /// \brief Called when the user clicks the GUI button
  void DoImageProcessing();

  void ConnectToQtWidget();

  void UpdateUiForService();

  itk::SmartPointer<lancet::spatial_fitting::ProbeCheckPointModel> GetServiceModel() const;
protected slots:
  void on_pushButtonProbeCheckPoint_clicked();

  void on_toolCollector_fail(int);

	void on_toolCollector_complete(mitk::NavigationData*);

	void on_toolCollector_step(int, mitk::NavigationData*);
private:
  Ui::QRoboticsRegistrationsAccuracyControls m_Controls;
};

#endif // QRoboticsRegistrationsAccuracy_h
