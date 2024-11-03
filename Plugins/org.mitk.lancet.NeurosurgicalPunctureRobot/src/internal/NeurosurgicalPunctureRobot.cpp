/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "NeurosurgicalPunctureRobot.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

const std::string NeurosurgicalPunctureRobot::VIEW_ID = "org.mitk.views.neurosurgicalpuncturerobot";

void NeurosurgicalPunctureRobot::SetFocus()
{

}

void NeurosurgicalPunctureRobot::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  connect(m_Controls.LoadCTSeriesBtn, &QPushButton::clicked, this, &NeurosurgicalPunctureRobot::LoadCTSeriesBtnClicked);
  connect(m_Controls.SegVesselBtn, &QPushButton::clicked, this, &NeurosurgicalPunctureRobot::SegVesselBtnClicked);
  std::cout << "VTK Version: " << vtkVersion::GetVTKVersion() << std::endl;
}

void NeurosurgicalPunctureRobot::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
}


void NeurosurgicalPunctureRobot::LoadCTSeriesBtnClicked()
{
    QString filename = QFileDialog::getExistingDirectory(nullptr, "Select the Tools store folder", "");
    if (filename.isNull()) return;

    vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
    reader->SetDirectoryName(filename.toStdString().c_str());
    reader->Update();

    vtkSmartPointer<vtkImageData> data = reader->GetOutput();

    vtkSmartPointer<vtkImageCast> imageCast = vtkSmartPointer<vtkImageCast>::New();
    imageCast->SetInputData(m_vtkImageData);
    imageCast->SetOutputScalarTypeToFloat();
    imageCast->Update();
    m_vtkImageData->DeepCopy(imageCast->GetOutput());
}

void NeurosurgicalPunctureRobot::InitGlobalVariable()
{
    m_vtkImageData = vtkSmartPointer<vtkImageData>::New();
}

void NeurosurgicalPunctureRobot::SegVesselBtnClicked()
{
    vtkSmartPointer<vtkImageThreshold> thresholdFilter = vtkSmartPointer<vtkImageThreshold>::New();
    thresholdFilter->SetInputData(m_vtkImageData);
    double max = m_Controls.VesselSegMaxLineEdit->text().toDouble();
    double min = m_Controls.VesselSegMinLineEdit->text().toDouble();
    thresholdFilter->ThresholdBetween(min, max);
    thresholdFilter->SetOutValue(0);
    thresholdFilter->SetInValue(1);
    thresholdFilter->Update();

    //// Step 3: Perform connectivity filtering to keep the largest vessel region
    vtkSmartPointer<vtkImageConnectivityFilter> connectivityFilter = vtkSmartPointer<vtkImageConnectivityFilter>::New();
    connectivityFilter->SetInputData(thresholdFilter->GetOutput());
    connectivityFilter->SetExtractionModeToLargestRegion();
    connectivityFilter->Update();

    //// 设置特征图像（在实际应用中可以从不同的方式生成）
    vtkSmartPointer<vtkImageData> featureImage  = vtkSmartPointer<vtkImageData>::New();
    
    //vtkSmartPointer<vtkvmtkGeodesicActiveContourLevelSetImageFilter> levelSetFilter =
       //vtkSmartPointer<vtkvmtkGeodesicActiveContourLevelSetImageFilter>::New();
    //levelSetFilter->SetFeatureImage(featureImage);
    //levelSetFilter->SetDerivativeSigma(1.0); // 你可以根据需求调整
    //levelSetFilter->SetAutoGenerateSpeedAdvection(1);
    //levelSetFilter->SetPropagationScaling(1.0); // 传播参数
    //levelSetFilter->SetCurvatureScaling(0.5); // 曲率参数
    //levelSetFilter->SetAdvectionScaling(1.0); // 对流参数

    //// 设置输入数据
    //levelSetFilter->SetInputData(connectivityFilter->GetOutput());
    //levelSetFilter->SetNumberOfIterations(100); // 迭代次数
    //levelSetFilter->SetIsoSurfaceValue(0.0);
    //levelSetFilter->SetMaximumRMSError(1e-20);
    //levelSetFilter->SetInterpolateSurfaceLocation(1);
    //levelSetFilter->SetUseImageSpacing(1);

    //levelSetFilter->Update();

    //// 获取输出图像
    //vtkSmartPointer<vtkImageData> levelSetOutput = levelSetFilter->GetOutput();

    //vtkSmartPointer<vtkvmtkCapPolyData> capFilter = vtkSmartPointer<vtkvmtkCapPolyData>::New();
    //capFilter->SetInputData(levelSetOutput);
    //capFilter->Update();

    //vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    //polyData->DeepCopy(capFilter->GetOutput());
  
    //mitk::Surface::Pointer surface = mitk::Surface::New();
    //surface->SetVtkPolyData(polyData);

    //mitk::DataNode::Pointer dataNode = mitk::DataNode::New();
    //dataNode->SetData(surface);
    //dataNode->SetName("BloonVessel");
    //this->GetDataStorage()->Add(dataNode);
}