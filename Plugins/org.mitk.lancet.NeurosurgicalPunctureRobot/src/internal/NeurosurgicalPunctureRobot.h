/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef NeurosurgicalPunctureRobot_h
#define NeurosurgicalPunctureRobot_h

#include <QFileDialog>
#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>
#include <mitkSurface.h>
#include <mitkDataNode.h>

#include "ui_NeurosurgicalPunctureRobotControls.h"
#include "vtkInclude.h"

/**
  \brief NeurosurgicalPunctureRobot

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class NeurosurgicalPunctureRobot : public QmitkAbstractView
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

  void LoadCTSeriesBtnClicked();
  void SegVesselBtnClicked();

private:
  Ui::NeurosurgicalPunctureRobotControls m_Controls;
  void InitGlobalVariable();
private:
    vtkSmartPointer<vtkImageData> m_vtkImageData = nullptr;
};

#endif // NeurosurgicalPunctureRobot_h
