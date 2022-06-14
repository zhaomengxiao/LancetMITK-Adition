/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef SpineCArmRegistration_h
#define SpineCArmRegistration_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_SpineCArmRegistrationControls.h"
#include "QmitkSingleNodeSelectionWidget.h"

/**
  \brief SpineCArmRegistration

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class SpineCArmRegistration : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

protected:
	Ui::SpineCArmRegistrationControls m_Controls;
  virtual void CreateQtPartControl(QWidget *parent) override;
  virtual void SetFocus() override;
  void InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget);
  void InitImageSelector(QmitkSingleNodeSelectionWidget* widget);
  void InitNodeSelector(QmitkSingleNodeSelectionWidget* widget);

	// Spatial locoalization simple
  void InitSceneSpatialLocalization();
  void ConfirmApPoint();
  void ConfirmLtPoint();

  // Spatial localization demo
  double m_ArrayNdiToApImageCalibratorMarker[16];
  double m_ArrayNdiToLtImageCalibratorMarker[16];
  double m_ArrayRenderWindowToApImage[16]; // The geometry matrix of the input Ap image with spacing [1,1,1]
  double m_ArrayRenderWindowToLtImage[16]; // The geometry matrix of the input Lt image with spacing [1,1,1]
  double m_ArrayApImageCalibratorMarkerToRenderWindow[16];
  double m_ArrayLtImageCalibratorMarkerToRenderWindow[16];
  double m_ApSourceInApImage[3]; // x,y,z in Ap image coordinate (spacing [1,1,1])
  double m_LtSourceInLtImage[3]; // x,y,z in Lt image coordinate (spacing [1,1,1])
  double m_ArrayNdiToApImage[16]; // The transform from NDI coordinate to Ap image with image spacing [1,1,1]
  double m_ArrayNdiToLtImage[16]; // The transform from NDI coordinate to Lt image with image spacing [1,1,1]

  void GetMatrixApImageCalibratorMarkerToRenderWindow(); // Use landmark registration
  void GetMatrixLtImageCalibratorMarkerToRenderWindow(); // Use landmark registration 
  void GetMatrixNdiToApImage();
  void GetMatrixNdiToLtImage();
  void GetApSourceInApImage();
  void GetLtSourceInLtImage();





  
  //utilities
  void PlotCoordinate(mitk::DataStorage* ds, std::string name, double color[3]);
  void DrawLine(double start[3], double end[3], double color[3], double opacity, const char* name);
  void Get2LineIntersection(double intersection[3], double line0Start[3], double line0End[3], double line1Start[3], double line1End[3]);


};

#endif // SpineCArmRegistration_h
