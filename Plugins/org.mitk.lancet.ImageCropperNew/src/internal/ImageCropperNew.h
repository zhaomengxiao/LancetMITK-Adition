/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef ImageCropperNew_h
#define ImageCropperNew_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>
#include <mitkPointSet.h>
#include <mitkDataNode.h>
#include <mitkDataStorage.h>
#include <mitkSurface.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <mitkImage.h>
#include <tuple>
#include <mitkMoveObjectInteractor.h>
#include "ui_ImageCropperNewControls.h"

/**
  \brief ImageCropperNew

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class ImageCropperNew : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;
  ImageCropperNew(QObject* parent = nullptr);
  ~ImageCropperNew() override;
private:
	QWidget* m_ParentWidget;
	mitk::ScalarType m_CropOutsideValue;
	mitk::MoveObjectInteractor::Pointer m_MoveObjectInteractor;

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  void InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget);

  void OnImageSelectionChanged(QList<mitk::DataNode::Pointer> nodes);

  void TurnOffAllNodesVisibility();

  void ResetView();

  // <int, 6> is the diagonal pts in the index space, the 2 mitk::Point3Ds are the corresponding pts in the world space  
  std::tuple<std::array<int, 6>, mitk::Point3D, mitk::Point3D> CalculateOverlapImageBound(const mitk::Image::Pointer& image, const mitk::Surface::Pointer& surface);

  mitk::Image::Pointer ConvertVtkToMitk(vtkImageData* vtkImage);

  vtkSmartPointer<vtkImageData> ExtractImageRegionByBound(vtkSmartPointer<vtkImageData>& inputImage, int xmin, int xmax, int ymin, int ymax, int zmin, int zmax, const mitk::Point3D minPoint, const mitk::Point3D maxPoint);

  void on_pushButton_ImageCropper_clicked();

  void on_pushButton_ImageCropper_showboundingshape_clicked();

  void CreateBoundingShapeInteractor();

  void CreateMoveableBoundingBox();

  void on_pushButton_AddInteractor_clicked();

  void on_pushButton_resample_clicked();

  template <typename ITKImageType>
  mitk::Image::Pointer ResampleITKImage(typename ITKImageType::Pointer itkImage);

  void on_pushButton_resample_type2_clicked();

  Ui::ImageCropperNewControls m_Controls;
};

#endif // ImageCropperNew_h
