/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef HTOTest_h
#define HTOTest_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>
#include <vtkPolyData.h>

#include "ui_HTOTestControls.h"

/**
  \brief HTOTest

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class HTOTest : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  Ui::HTOTestControls m_Controls;


  //Cut tibia into two parts with a plane 
  bool CreateOneCutPlane();
  bool CreateCutPlane(); // create one or two cut planes
  
  bool CutPolyDataWithPlane(vtkSmartPointer<vtkPolyData> dataToCut, 
	  vtkSmartPointer<vtkPolyData> largerSubPart, 
	  vtkSmartPointer<vtkPolyData> smallerSubPart,
	  double planeOrigin[3], double planeNormal[3]);

  bool CutTibiaWithOnePlane(); // cut tibia surface with one plane
  bool CutTibiaWithTwoPlanes(); // cut tibia surface with two planes
  bool CutTibiaSurface(); // cut tibia surface with one or two planes

  bool CutTibiaImage(); // cut tibia image with one or two planes

  bool CutTibia(); // cut tibia image and surface

  bool GetPlaneProperty(vtkSmartPointer<vtkPolyData> plane, double normal[3], double center[3]);

  // Register femur, proximal tibia and distal tibia
  bool RegisterFemur();
  bool RegisterPoximalTibia();
  bool RegisterDistalTibia();


};

#endif // HTOTest_h
