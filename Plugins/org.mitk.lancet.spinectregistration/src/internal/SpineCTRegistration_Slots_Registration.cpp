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
#include "SpineCTRegistration.h"

// Qt
#include "leastsquaresfit.h"
#include "mitkApplyTransformMatrixOperation.h"
#include "mitkInteractionConst.h"
#include <QMessageBox>
#include <QPushButton>

#include "mitkImageToSurfaceFilter.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include "mitkPointSet.h"
#include "mitkSurface.h"
#include "surfaceregistraion.h"
#include "vtkConnectivityFilter.h"
#include <QPushButton>
#include <mitkImage.h>
#include <vtkSphereSource.h>

// Landmark registration (pointset to pointset)
void SpineCTRegistration::LandmarkRegistration()
{
  auto landmarkRegistrator = mitk::SurfaceRegistration::New();

  MITK_INFO << "Proceedinng Landmark registration";
  if (m_LandmarkSrcPointsetDataNode != nullptr && m_LandmarkTargetPointsetDataNode != nullptr)
  {
    auto sourcePointset = dynamic_cast<mitk::PointSet *>(m_LandmarkSrcPointsetDataNode->GetData());
    auto targetPointset = dynamic_cast<mitk::PointSet *>(m_LandmarkTargetPointsetDataNode->GetData());
    landmarkRegistrator->SetLandmarksSrc(sourcePointset);
    landmarkRegistrator->SetLandmarksTarget(targetPointset);
    landmarkRegistrator->ComputeLandMarkResult();
    
    Eigen::Matrix4d tmpRegistrationResult{landmarkRegistrator->GetResult()->GetData()};
    tmpRegistrationResult.transposeInPlace();
    m_TmpRegistrationResult = tmpRegistrationResult;
    MITK_INFO << m_TmpRegistrationResult;
    UpdateRegistrationMatrixInUI();
  }

  std::ostringstream os;
  landmarkRegistrator->GetResult()->Print(os);

  m_Controls.textBrowser_Registration->append("-------------Start landmark registration----------");
  m_Controls.textBrowser_Registration->append(QString::fromStdString(os.str()));
};

// ICP registration (surface to pointset)
void SpineCTRegistration::IcpRegistration()
{
  auto icpRegistrator = mitk::SurfaceRegistration::New();

  MITK_INFO << "Proceedinng ICP registration";

  if (m_IcpSrcSurfaceDataNode != nullptr && m_IcpTargetPointsetDataNode != nullptr)
  {
    auto icpTargetPointset = dynamic_cast<mitk::PointSet *>(m_IcpTargetPointsetDataNode->GetData());
    auto icpSrcSurface = dynamic_cast<mitk::Surface *>(m_IcpSrcSurfaceDataNode->GetData());
    icpRegistrator->SetIcpPoints(icpTargetPointset);
    icpRegistrator->SetSurfaceSrc(icpSrcSurface);
    icpRegistrator->ComputeIcpResult();

    Eigen::Matrix4d tmpRegistrationResult{icpRegistrator->GetResult()->GetData()};
    tmpRegistrationResult.transposeInPlace();

    m_TmpRegistrationResult = tmpRegistrationResult;
    MITK_INFO << m_TmpRegistrationResult;

    UpdateRegistrationMatrixInUI();
  }

  std::ostringstream os;
  icpRegistrator->GetResult()->Print(os);
  m_Controls.textBrowser_Registration->append("-------------Start ICP registration----------");
  m_Controls.textBrowser_Registration->append(QString::fromStdString(os.str()));
};

// Update the registration result in the UI
void SpineCTRegistration::UpdateRegistrationMatrixInUI()
{
  m_Controls.lineEdit_RegistrationMatrix_0->setText(QString::number(m_TmpRegistrationResult(0)));
  m_Controls.lineEdit_RegistrationMatrix_1->setText(QString::number(m_TmpRegistrationResult(1)));
  m_Controls.lineEdit_RegistrationMatrix_2->setText(QString::number(m_TmpRegistrationResult(2)));
  m_Controls.lineEdit_RegistrationMatrix_3->setText(QString::number(m_TmpRegistrationResult(3)));
  m_Controls.lineEdit_RegistrationMatrix_4->setText(QString::number(m_TmpRegistrationResult(4)));
  m_Controls.lineEdit_RegistrationMatrix_5->setText(QString::number(m_TmpRegistrationResult(5)));
  m_Controls.lineEdit_RegistrationMatrix_6->setText(QString::number(m_TmpRegistrationResult(6)));
  m_Controls.lineEdit_RegistrationMatrix_7->setText(QString::number(m_TmpRegistrationResult(7)));
  m_Controls.lineEdit_RegistrationMatrix_8->setText(QString::number(m_TmpRegistrationResult(8)));
  m_Controls.lineEdit_RegistrationMatrix_9->setText(QString::number(m_TmpRegistrationResult(9)));
  m_Controls.lineEdit_RegistrationMatrix_10->setText(QString::number(m_TmpRegistrationResult(10)));
  m_Controls.lineEdit_RegistrationMatrix_11->setText(QString::number(m_TmpRegistrationResult(11)));
  m_Controls.lineEdit_RegistrationMatrix_12->setText(QString::number(m_TmpRegistrationResult(12)));
  m_Controls.lineEdit_RegistrationMatrix_13->setText(QString::number(m_TmpRegistrationResult(13)));
  m_Controls.lineEdit_RegistrationMatrix_14->setText(QString::number(m_TmpRegistrationResult(14)));
  m_Controls.lineEdit_RegistrationMatrix_15->setText(QString::number(m_TmpRegistrationResult(15)));
};


void SpineCTRegistration::TransformSurface()
{
  auto movingObject = dynamic_cast<mitk::Surface *>(m_MovingSurfaceDataNode->GetData());

  double registrationResult[16];

  for (int m = 0; m < 16; m++)
  {
    registrationResult[m] = *(m_TmpRegistrationResult.data() + m);
  }

  vtkSmartPointer<vtkMatrix4x4> tmpVtkMatrixRegistrationResult = vtkSmartPointer<vtkMatrix4x4>::New();
  tmpVtkMatrixRegistrationResult->DeepCopy(registrationResult);
  tmpVtkMatrixRegistrationResult->Transpose();

  vtkTransform *trans = vtkTransform::New();
  trans->PostMultiply();
  trans->SetMatrix(movingObject->GetGeometry()->GetVtkMatrix());
  trans->Concatenate(tmpVtkMatrixRegistrationResult);
  trans->Update();

  mitk::Point3D ref;
  auto *mitkMatrixOperation =
    new mitk::ApplyTransformMatrixOperation(mitk::OpAPPLYTRANSFORMMATRIX, trans->GetMatrix(), ref);

  movingObject->GetGeometry()->ExecuteOperation(mitkMatrixOperation);

  delete mitkMatrixOperation;

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void SpineCTRegistration::TransformImage()
{
  auto movingObject = dynamic_cast<mitk::Image *>(m_MovingSurfaceDataNode->GetData());

  double registrationResult[16];

  for (int m = 0; m < 16; m++)
  {
    registrationResult[m] = *(m_TmpRegistrationResult.data() + m);
  }

  vtkSmartPointer<vtkMatrix4x4> tmpVtkMatrixRegistrationResult = vtkSmartPointer<vtkMatrix4x4>::New();
  tmpVtkMatrixRegistrationResult->DeepCopy(registrationResult);
  tmpVtkMatrixRegistrationResult->Transpose();

  vtkTransform *trans = vtkTransform::New();
  trans->PostMultiply();
  trans->SetMatrix(movingObject->GetGeometry()->GetVtkMatrix());
  trans->Concatenate(tmpVtkMatrixRegistrationResult);
  trans->Update();

  movingObject->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(trans->GetMatrix());

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

