/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef QImplantPlanning_h
#define QImplantPlanning_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_QImplantPlanningControls.h"

/**
  \brief QImplantPlanning

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class QImplantPlanning : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

public Q_SLOTS:
	void on_comboBox_ProsthesisType_currentIndexChanged(int index);
	void on_comboBox_PlanCupSize_currentIndexChanged(int index);
	void on_comboBox_PlanFemoralSize_currentIndexChanged(int index);
	void on_comboBox_PlanHeadLength_currentIndexChanged(int index);
	void on_signalReadBackups();
	void on_signalProsthesisType();

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;

  void PlanFemoralSizeChanged();

  Ui::QImplantPlanningControls m_Controls;
};

#endif // QImplantPlanning_h
