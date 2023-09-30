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
#include <opencv2/calib3d/calib3d.hpp>
#include <itkHoughTransform2DCirclesImageFilter.h>

#include <itkAddImageFilter.h>
// #include <itkBinaryBallStructuringElement.h>
// #include <itkBinaryThresholdImageFilter.h>
// #include <itkCannyEdgeDetectionImageFilter.h>
// #include <itkFlipImageFilter.h>
// #include <itkGrayscaleMorphologicalClosingImageFilter.h>
// #include <itkInvertIntensityImageFilter.h>
// #include <itkResampleImageFilter.h>
// #include <itkRescaleIntensityImageFilter.h>
// #include <itkShiftScaleImageFilter.h>
// #include <itkSubtractImageFilter.h>
// #include "itkDiscreteGaussianImageFilter.h"
// #include "itkTotalVariationDenoisingImageFilter.h"

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

  // Spatial localization demo using NDI marker on the 2-deck image calibrator
  double m_ArrayNdiToApImageCalibratorMarker[16]{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
  double m_ArrayNdiToLtImageCalibratorMarker[16]{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
  double m_ArrayRenderWindowToApImage[16]{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }; // The geometry matrix of the input Ap image with spacing [1,1,1]
  double m_ArrayRenderWindowToLtImage[16]{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }; // The geometry matrix of the input Lt image with spacing [1,1,1]
  double m_ArrayApImageCalibratorMarkerToRenderWindow[16];
  double m_ArrayLtImageCalibratorMarkerToRenderWindow[16];
  double m_ApSourceInApImage[3]{ 104.96, 104.96, 980 }; // x,y,z in Ap image coordinate (spacing [1,1,1])
  double m_LtSourceInLtImage[3]{ 104.96, 104.96, 980 }; // x,y,z in Lt image coordinate (spacing [1,1,1])
  double m_ArrayNdiToApImage[16]{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }; // The transform from NDI coordinate to Ap image with image spacing [1,1,1]
  double m_ArrayNdiToLtImage[16]{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }; // The transform from NDI coordinate to Lt image with image spacing [1,1,1]

  bool GetMatrixApImageCalibratorMarkerToRenderWindow(); // Use landmark registration
  bool GetMatrixLtImageCalibratorMarkerToRenderWindow(); // Use landmark registration 
  bool GetMatrixNdiToApImage();
  bool GetMatrixNdiToLtImage();
  bool GetApSourceInApImage();
  bool GetLtSourceInLtImage();
  void RegistertheProjectionModel();
  void InitDemoScene();
  void ApplyRegistration();
  void ConfirmDemoApPoint();
  void ConfirmDemoLtPoint();


  // C arm registration using PnP algorithm

  double m_ArrayNdiToPnpDrfMarker_pnp[16]{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }; // from NavigationData
  //double m_ArrayNdiToLtPnpDrfMarker_pnp[16]{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }; // from NavigationData
  double m_ArrayApImageToCamera_pnp[16]{ 1,0,0,104.96, 0,1,0,104.96, 0,0,1,0, 0,0,0,1 }; // Need source location, for mitk::Surface setup
  //double m_ArrayLtImageToLtCamera_pnp[16]{ 1,0,0,104.96, 0,1,0,104.96, 0,0,1,0, 0,0,0,1 }; // Need source location, for mitk::Surface setup
  double m_ArrayIntrinsicMatrix_pnp[9]; // Need source location
  //double m_ArrayLtIntrinsicMatrix_pnp[9]; // Need source location
  double m_ArrayRenderWindowToImage_pnp[16]{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }; // The geometry matrix of the input Ap image with spacing [1,1,1]
  //double m_ArrayRenderWindowToLtImage_pnp[16]{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }; // The geometry matrix of the input Lt image with spacing [1,1,1]
  double m_ArrayImageCalibratorMarkerToRenderWindow_pnp[16]; // obtained by landmark registration, for source localization
  //double m_ArrayLtImageCalibratorMarkerToRenderWindow_pnp[16]; // obtained by landmark registration, for source localization
  double m_SourceInImage_pnp[3]{ 104.96, 104.96, 980 }; // x,y,z in Ap image coordinate (spacing [1,1,1])
  //double m_LtSourceInLtImage_pnp[3]{ 104.96, 104.96, 980 }; // x,y,z in Lt image coordinate (spacing [1,1,1])
  double m_ArrayNdiToImage_pnp[16]{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }; // Need PnP, the transform from NDI coordinate to Ap image with image spacing [1,1,1]
  //double m_ArrayNdiToLtImage_pnp[16]{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }; // Need PnP, the transform from NDI coordinate to Lt image with image spacing [1,1,1]

  bool GetMatrixImageCalibratorMarkerToRenderWindow_pnp(); // Use landmark registration
  bool GetSourceInImage_pnp();
  bool GetMatrixNdiToImage_pnp();
  void RegisterProjectionModel_pnp();
  void InitScene_pnp();
  void ApplyRegistration_pnp();
  void ConfirmApPoint_pnp();
  void ConfirmLtPoint_pnp();


//---------------DR steelball detection -------------------
  
  // Hough circle detection
  void DetectCircles();

  // Canny edge detection
  void GetCannyEdge();

	
  // Test NCC
  int TestNCC();

  // Recursive search
  int on_pushButton_recursiveSearch_clicked();

  // Utilities
  void PlotCoordinate(mitk::DataStorage* ds, std::string name, double color[3]);
  void DrawLine(double start[3], double end[3], double color[3], double opacity, const char* name);
  void Get2LineIntersection(double intersection[3], double line0Start[3], double line0End[3], double line1Start[3], double line1End[3]);

  void GetLinesIntersection(double intersection[3]);
  mitk::DataNode* m_linesStartsNode{ nullptr };
  mitk::DataNode* m_linesEndsNode{ nullptr };


  // DRR simulation
  void on_pushButton_generateDRR_clicked();


  // Apply CLAHE
  void on_pushButton_CLAHE_clicked();

  // Apply Histogram equalization
  void on_pushButton_HE_clicked();



  // Apply self-made outlier filter
  void on_pushButton_outlieFilter_clicked();

	// Return converted result  
  unsigned short ConvertOutlier3X3(unsigned short potentialOutlier, unsigned short neighbors[8]);

  // Apply teeth detection to a binary image containing the original edges
  void on_pushButton_teethDetect_clicked();


  // Dental DR post-processing Demo
  void DrEnhanceType2();
  void DrEnhanceType1_intermediate();
  void DrEnhanceType1();

  void DrEnhanceType2_test();
  void DrEnhanceType1_intermediate_test();
  void DrEnhanceType1_test();

  void on_pushButton_step_1_3_clicked();
  void on_pushButton_step_4_5_clicked();
  void on_pushButton_step_6_10_clicked();
  void on_pushButton_step_11_13_clicked();
  void on_pushButton_step_14_16_clicked();
  void on_pushButton_step_17_clicked();

  void on_pushButton_EasyItkTest_clicked();

  using us_short_ImageType = itk::Image<unsigned short, 2>;
  using double_ImageType = itk::Image<double, 2>;

  void ApplyAddFilter(us_short_ImageType::Pointer inputImage_1,
	  us_short_ImageType::Pointer inputImage_2, 
	  us_short_ImageType::Pointer outputImage);

  void ApplySubtractFilter(us_short_ImageType::Pointer inputImage_1,
	  us_short_ImageType::Pointer inputImage_2,
	  us_short_ImageType::Pointer outputImage);

  void ApplyGaussianFilter(us_short_ImageType::Pointer inputImage, double variance,
	  us_short_ImageType::Pointer outputImage);

  void ApplyRescaleToIntervalFilter(us_short_ImageType::Pointer inputImage, 
	  int upperLimit,
	  int lowerLimit,
	  us_short_ImageType::Pointer outputImage);

	// Shift the image first and scale it 
  void ApplyShiftAndScaleFilter(us_short_ImageType::Pointer inputImage,
	  int shiftFactor,
	  int scaleFactor,
	  us_short_ImageType::Pointer outputImage);

  void ApplyMultiplyFilter(us_short_ImageType::Pointer inputImage_1,
	  us_short_ImageType::Pointer inputImage_2,
	  us_short_ImageType::Pointer outputImage);

  void ApplyLaplacianFilter(us_short_ImageType::Pointer inputImage,
	  double_ImageType::Pointer outputImage);

};

#endif // SpineCArmRegistration_h
