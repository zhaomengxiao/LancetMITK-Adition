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
#include "SpineCArmRegistration.h"


// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkPlaneSource.h>
#include <vtkPolyLine.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

#include "mitkNodePredicateAnd.h"
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include "mitkPointSet.h"
#include "mitkSurface.h"
#include "QmitkDataStorageTreeModel.h"
#include "basic.h"
#include "surfaceregistraion.h"
#include "drr.h"


void SpineCArmRegistration::on_pushButton_generateDRR_clicked()
{
	auto inputImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_drrCT->GetSelectedNode()->GetData());

	double rx = m_Controls.lineEdit_drrRx->text().toDouble();
	double ry = m_Controls.lineEdit_drrRy->text().toDouble();
	double rz = m_Controls.lineEdit_drrRz->text().toDouble();

	double tx = m_Controls.lineEdit_drrTx->text().toDouble();
	double ty = m_Controls.lineEdit_drrTy->text().toDouble();
	double tz = m_Controls.lineEdit_drrTz->text().toDouble();

	double cx = m_Controls.lineEdit_drrCx->text().toDouble();
	double cy = m_Controls.lineEdit_drrCy->text().toDouble();
	double cz = m_Controls.lineEdit_drrCz->text().toDouble();

	double sid = m_Controls.lineEdit_drrSid->text().toDouble();

	double thres = m_Controls.lineEdit_drrThres->text().toDouble();

	double sx = m_Controls.lineEdit_drrSx->text().toDouble();
	double sy = m_Controls.lineEdit_drrSy->text().toDouble();

	int dx = m_Controls.lineEdit_drrDx->text().toInt();
	int dy = m_Controls.lineEdit_drrDy->text().toInt();

	double o2Dx = m_Controls.lineEdit_drrO2Dx->text().toDouble();
	double o2Dy = m_Controls.lineEdit_drrO2Dy->text().toDouble();


	// Generate DRR
	itk::SmartPointer<DrrFilter> drrFilter = DrrFilter::New();

	drrFilter->Setsid(sid);
	drrFilter->SetObjRotate(rx, ry, rz);
	drrFilter->SetObjTranslate(tx, ty, tz);
	drrFilter->Setcx(cx);
	drrFilter->Setcy(cy);
	drrFilter->Setcz(cz);
	drrFilter->Setthreshold(thres);
	drrFilter->Setsx(sx);
	drrFilter->Setsy(sy);
	drrFilter->Setdx(dx);
	drrFilter->Setdy(dy);
	drrFilter->Seto2Dx(o2Dx);
	drrFilter->Seto2Dy(o2Dy);

	drrFilter->SetInput(inputImage);

	drrFilter->Update();

	auto newNode_1 = mitk::DataNode::New();
	newNode_1->SetName("DRR_static");
	newNode_1->SetData(drrFilter->GetOutput());

	GetDataStorage()->Add(newNode_1);

	// Simulate an image plane
	auto imagerPlaneSource = vtkSmartPointer<vtkPlaneSource>::New();
	auto raySource = vtkSmartPointer<vtkSphereSource>::New();

	//------------ ToDo ----------------
	// imagerPlaneSource->SetOrigin(0, 0, 0);
	// imagerPlaneSource->SetPoint1(dimensionPlane0[0], 0, 0);
	// imagerPlaneSource->SetPoint2(0, dimensionPlane0[1], 0);
	// imagerPlaneSource->Update();
	//
	// auto imagerPlaneNode = mitk::DataNode::New();
	// auto imagerPlaneSurface = mitk::Surface::New();
	// imagerPlaneSurface0->SetVtkPolyData(imagerPlaneSource0->GetOutput());
	// imagerPlaneNode0->SetData(imagerPlaneSurface0);

}

