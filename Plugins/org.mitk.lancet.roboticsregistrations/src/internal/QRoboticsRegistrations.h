/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef QRoboticsRegistrations_h
#define QRoboticsRegistrations_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>
#include <mitkNavigationData.h>

#include "ui_QRoboticsRegistrationsControls.h"

namespace lancet::spatial_fitting
{
  class RoboticsRegisterModel;

  using RoboticsRegisterModelPtr = itk::SmartPointer<RoboticsRegisterModel>;
} // namespace lancet::spatial_fitting

/**
  \brief QRoboticsRegistrations

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class QRoboticsRegistrations : public QmitkAbstractView
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
private:
  void UpdateWidgetOfService();

  lancet::spatial_fitting::RoboticsRegisterModelPtr GetServiceRoboticsModel() const;
private slots:
  void on_pushButtonRobotVerify_clicked();

  void on_pushButtonCalResult_clicked();

  void on_pushButtonAddPoint_clicked();

  void on_toolCollector_fail(int, mitk::NavigationData*);

	void on_toolCollector_complete(mitk::NavigationData*);
private:
  Ui::QRoboticsRegistrationsControls m_Controls;
};

#endif // QRoboticsRegistrations_h
