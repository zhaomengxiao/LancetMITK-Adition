/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef Mitk_AimooeToolCoordinateTransform_h
#define Mitk_AimooeToolCoordinateTransform_h

#include <berryISelectionListener.h>
#include <QmitkRenderWindow.h>
#include <QmitkStdMultiWidget.h>
#include <QmitkAbstractView.h>
#include <mitkPointSet.h>
#include <mitkRenderWindow.h>
#include <mitkRenderingManager.h>
#include <mitkPointSetDataInteractor.h>
#include <mitkSurface.h>
#include <mitkDataNode.h>

#include <vector>

#include "ui_Mitk_AimooeToolCoordinateTransformControls.h"

/**
  \brief Mitk_AimooeToolCoordinateTransform

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class Mitk_AimooeToolCoordinateTransform : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

  // the open file's file path
  QString filePath;

  //the input position
  mitk::PointSet::Pointer pointSet;
  mitk::DataNode::Pointer pointSetNode;

  //the three axis vector and index
  Eigen::Vector3d axis_0;
  Eigen::Vector3d axis_1;
  Eigen::Vector3d axis_2;
  std::vector<int> axis_index = { 0,1,2 }; //0:x+,1:y+,2:z+

  //Axes
  mitk::Surface::Pointer Axes;
  mitk::DataNode::Pointer AxesNode;


  //the Transform matrix
  Eigen::Matrix4d Transform;

  //the After Transform positon
  std::vector<Eigen::Vector3d> t_pointSet;


  //some func flag to run or return
  bool Readfileinfo = false;
  bool ShowPoint = false;
  bool Compute = false;
  bool Apply= false;
  bool SaveNew = false;
  bool SaveOld = false;

  //initial position choice
  int origin_point_index = 0;
  int primer_axis_point_index = 0;
  int quadrant_point_index = 0;

  //parameters for read and save file
 int PositionStart = 3;  //from 0 rows to start to count 
 int PositionNumRows = 2; //from 0 rows to start to count 
 int PointNum = 0;

 const int precision = 6;

 mitk::Surface::Pointer linesurface;
 mitk::DataNode::Pointer linenode;

 //global renderWindow for dispaly
 //mitk::RenderWindow::Pointer renderWindow;


void LoadCoordinate();

public slots:
    // some slots to change the qt controls 
    void ChangePrimerLabel(const QString& text);

    void ChangeSeconderyLabel(const QString& text);

    void ChangeSeconderyCB(const QString& text);

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;

  // our funcs
  void OpenFile();

  void ReadFIleinfo();

  void ShowPoints();

  void ComputeTransform();

  void ApplyTransform();

  void SaveAtOldFile();

  void SaveAtNewFile();

  void SetSecond();

  void Coordinate_Transform(Eigen::Matrix4d EigenTransform);

  void Undo();

  Ui::Mitk_AimooeToolCoordinateTransformControls m_Controls;
};

//override the std cout << signs
std::ostream& operator<<(std::ostream& os, const mitk::Point3D& point);

#endif // Mitk_AimooeToolCoordinateTransform_h
