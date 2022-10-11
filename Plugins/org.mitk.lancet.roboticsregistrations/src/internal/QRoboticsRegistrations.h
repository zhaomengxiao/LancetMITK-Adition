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

#include "ui_QRoboticsRegistrationsControls.h"

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

Q_SIGNALS:
  void signalAction(int);
private Q_SLOTS:
  void on_comboBox_ToolType_currentIndexChanged(int index);
  void on_comboBox_TCPType_currentIndexChanged(int index);
  void on_signalReadBackups();

  void on_pushButtonAddPoint_clicked();
  void on_pushButtonCalResult_clicked();
  void on_pushButtonRobotVerify_clicked();

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;

  /// \brief Called when the user clicks the GUI button
  void DoImageProcessing();

  Ui::QRoboticsRegistrationsControls m_Controls;
  enum
  {
	  RIO_REGISTRATION = 3,
	  PROBE_CHECKPOINT,
	  FEMORAL_CHECKPOINT,
	  FEMUR_LANDMARK,
	  FEMUR_REGISTRATION,
	  PELVIS_CHECKPOINT,
	  PELVIS_LANDMARK,
	  PELVIS_REGISTRATION,
  };
};

#endif // QRoboticsRegistrations_h
