/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef MoveData_h
#define MoveData_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "QmitkSingleNodeSelectionWidget.h"
#include "ui_MoveDataControls.h"


/**
  \brief MoveData

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class MoveData : public QmitkAbstractView
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

  Ui::MoveDataControls m_Controls;


  // Variables
	//node op
  mitk::BaseData* m_baseDataToMove{ nullptr };
  mitk::DataNode* m_DirectionPointset{ nullptr };

	// slots
  void InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget);
  void PointSetDirectionChanged(QmitkSingleNodeSelectionWidget::NodeList);

	// Toolset 1
  void Translate(double direction[3], double length, mitk::BaseData* data);
  void Rotate(double center[3], double direction[3], double counterclockwiseDegree, mitk::BaseData* data);
  void TranslatePlusX();
  void TranslatePlusY();
  void TranslatePlusZ();
  void TranslateMinusX();
  void TranslateMinusY();
  void TranslateMinusZ();
  void RotatePlusX();
  void RotatePlusY();
  void RotatePlusZ();
  void RotateMinusX();
  void RotateMinusY();
  void RotateMinusZ();

  void TranslatePlus();
  void TranslateMinus();
  void RotatePlus();
  void RotateMinus();


  // Toolset 2 
  void ClearMatrixContent();
  void OverwriteOffsetMatrix();
  void AppendOffsetMatrix();
  vtkMatrix4x4* ObtainVtkMatrixFromUi();




};

#endif // MoveData_h
