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

const std::string SpineCArmRegistration::VIEW_ID = "org.mitk.views.spinecarmregistration";

void SpineCArmRegistration::SetFocus()
{
	// purposely not implemented
}

void SpineCArmRegistration::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_spatialApPoint);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_spatialLtPoint);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_designLballs);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_APextractedLballs);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_LTextractedLballs);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_designMballs);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_APextractedMballs);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_LTextractedMballs);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_demoApPoint);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_demoLtPoint);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_demo3Dpointset);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_designLballs_pnp);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_designMballs_pnp);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_extractedLballs_pnp);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_extractedMballs_pnp);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_designPnPballs);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_imagePnPballs);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_3Dpointset_pnp);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_ApPoint_pnp);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_LtPoint_pnp);


  InitNodeSelector(m_Controls.mitkNodeSelectWidget_ndiAp);
  InitNodeSelector(m_Controls.mitkNodeSelectWidget_ndiLt);
  InitNodeSelector(m_Controls.mitkNodeSelectWidget_ndiPnP);

  InitImageSelector(m_Controls.mitkNodeSelectWidget_ApImage);
  InitImageSelector(m_Controls.mitkNodeSelectWidget_LtImage);
  InitImageSelector(m_Controls.mitkNodeSelectWidget_image_pnp);
  InitImageSelector(m_Controls.mitkNodeSelectWidget_circleDetectInput);
  InitImageSelector(m_Controls.mitkNodeSelectWidget_search);
  InitImageSelector(m_Controls.mitkNodeSelectWidget_template);
  InitImageSelector(m_Controls.mitkNodeSelectWidget_drrCT);


  //connect(m_Controls.buttonPerformImageProcessing, &QPushButton::clicked, this, &SpineCArmRegistration::DoImageProcessing);
  connect(m_Controls.pushButton_InitSpatialScene, &QPushButton::clicked, this, &SpineCArmRegistration::InitSceneSpatialLocalization);
  connect(m_Controls.pushButton_confirmApPoint, &QPushButton::clicked, this, &SpineCArmRegistration::ConfirmApPoint);
  connect(m_Controls.pushButton_confirmLtPoint, &QPushButton::clicked, this, &SpineCArmRegistration::ConfirmLtPoint);
  connect(m_Controls.pushButton_projectionModelReg, &QPushButton::clicked, this, &SpineCArmRegistration::RegistertheProjectionModel);
  connect(m_Controls.pushButton_confirmDemoApPoint, &QPushButton::clicked, this, &SpineCArmRegistration::ConfirmDemoApPoint);
  connect(m_Controls.pushButton_confirmDemoLtPoint, &QPushButton::clicked, this, &SpineCArmRegistration::ConfirmDemoLtPoint);
  connect(m_Controls.pushButton_projectionModelReg_pnp, &QPushButton::clicked, this, &SpineCArmRegistration::RegisterProjectionModel_pnp);
  connect(m_Controls.pushButton_confirmApPoint_pnp, &QPushButton::clicked, this, &SpineCArmRegistration::ConfirmApPoint_pnp);
  connect(m_Controls.pushButton_confirmLtPoint_pnp, &QPushButton::clicked, this, &SpineCArmRegistration::ConfirmLtPoint_pnp);
  connect(m_Controls.pushButton_detectCircle, &QPushButton::clicked, this, &SpineCArmRegistration::DetectCircles);
  connect(m_Controls.pushButton_canny, &QPushButton::clicked, this, &SpineCArmRegistration::GetCannyEdge);
  connect(m_Controls.pushButton_testNCC, &QPushButton::clicked, this, &SpineCArmRegistration::TestNCC);

  connect(m_Controls.pushButton_recursiveSearch, &QPushButton::clicked, this, &SpineCArmRegistration::on_pushButton_recursiveSearch_clicked);
  connect(m_Controls.pushButton_generateDRR, &QPushButton::clicked, this, &SpineCArmRegistration::on_pushButton_generateDRR_clicked);

  connect(m_Controls.pushButton_CLAHE, &QPushButton::clicked, this, &SpineCArmRegistration::on_pushButton_CLAHE_clicked);
  connect(m_Controls.pushButton_HE, &QPushButton::clicked, this, &SpineCArmRegistration::on_pushButton_HE_clicked);

  connect(m_Controls.pushButton_outlieFilter, &QPushButton::clicked, this, &SpineCArmRegistration::on_pushButton_outlieFilter_clicked);
  connect(m_Controls.pushButton_teethDetect, &QPushButton::clicked, this, &SpineCArmRegistration::on_pushButton_teethDetect_clicked);

  connect(m_Controls.pushButton_step_1_3, &QPushButton::clicked, this, &SpineCArmRegistration::on_pushButton_step_1_3_clicked);
  connect(m_Controls.pushButton_step_4_5, &QPushButton::clicked, this, &SpineCArmRegistration::on_pushButton_step_4_5_clicked);
  connect(m_Controls.pushButton_step_6_10, &QPushButton::clicked, this, &SpineCArmRegistration::on_pushButton_step_6_10_clicked);
  connect(m_Controls.pushButton_step_11_13, &QPushButton::clicked, this, &SpineCArmRegistration::on_pushButton_step_11_13_clicked);
  connect(m_Controls.pushButton_step_14_16, &QPushButton::clicked, this, &SpineCArmRegistration::on_pushButton_step_14_16_clicked);
  connect(m_Controls.pushButton_step_17, &QPushButton::clicked, this, &SpineCArmRegistration::on_pushButton_step_17_clicked);
  connect(m_Controls.pushButton_EasyItkTest, &QPushButton::clicked, this, &SpineCArmRegistration::on_pushButton_EasyItkTest_clicked);

}
void SpineCArmRegistration::InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::PointSet>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a point set"));
	widget->SetPopUpTitel(QString("Select point set"));
}

void SpineCArmRegistration::InitImageSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::Image>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select an image"));
	widget->SetPopUpTitel(QString("Select image"));
}

void SpineCArmRegistration::InitNodeSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(
		mitk::NodePredicateProperty::New("helper object"), mitk::NodePredicateProperty::New("hidden object"))));
	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a node"));
	widget->SetPopUpTitel(QString("Select node"));
}

void SpineCArmRegistration::InitSceneSpatialLocalization()
{

	// World coordinate
	double color[3]{ 0,0,0 };
	PlotCoordinate(GetDataStorage(), "World coordinate", color);



	double resolutionPlane[2]{ m_Controls.lineEdit_planeResolutionX->text().toDouble(), m_Controls.lineEdit_planeResolutionY->text().toDouble() };
	double pixelNum[2]{ m_Controls.lineEdit_planePixNumX->text().toDouble(),m_Controls.lineEdit_planePixNumY->text().toDouble() };

	double dimensionPlane[2]{pixelNum[0]* resolutionPlane[0],pixelNum[1] * resolutionPlane[1] };

	auto imagerPlaneSource0 = vtkSmartPointer<vtkPlaneSource>::New();
	auto raySource0 = vtkSmartPointer<vtkSphereSource>::New();
	auto imagerPlaneSource1 = vtkSmartPointer<vtkPlaneSource>::New();
	auto raySource1 = vtkSmartPointer<vtkSphereSource>::New();


	// Create a plane and source for C-arm posture 0
	double color0[3]{ 1,1,1 };
	PlotCoordinate(GetDataStorage(), "AP coordinate", color0);

	

	imagerPlaneSource0->SetOrigin(0, 0, 0);
	imagerPlaneSource0->SetPoint1(dimensionPlane[0], 0, 0);
	imagerPlaneSource0->SetPoint2(0, dimensionPlane[1], 0);
	imagerPlaneSource0->Update();
	
	auto imagerPlaneNode0 = mitk::DataNode::New();
	auto imagerPlaneSurface0 = mitk::Surface::New();
	imagerPlaneSurface0->SetVtkPolyData(imagerPlaneSource0->GetOutput());
	imagerPlaneNode0->SetData(imagerPlaneSurface0);
	imagerPlaneNode0->SetName("AP imager");
	imagerPlaneNode0->SetColor(0.4, 0.0, 0.0);
	imagerPlaneNode0->SetVisibility(true);
	imagerPlaneNode0->SetOpacity(1.0);
	GetDataStorage()->Add(imagerPlaneNode0);

	raySource0->SetCenter(m_Controls.lineEdit_planeSourceX_0->text().toDouble(), m_Controls.lineEdit_planeSourceY_0->text().toDouble(), m_Controls.lineEdit_planeSourceZ_0->text().toDouble());
	raySource0->SetRadius(17);
	raySource0->Update();

	auto raySourceNode0 = mitk::DataNode::New();
	auto raySourceSurface0 = mitk::Surface::New();
	raySourceSurface0->SetVtkPolyData(raySource0->GetOutput());
	raySourceNode0->SetData(raySourceSurface0);
	raySourceNode0->SetName("AP raySource");
	raySourceNode0->SetColor(1.0, 0.0, 0.0);
	raySourceNode0->SetVisibility(true);
	raySourceNode0->SetOpacity(1.0);
	GetDataStorage()->Add(raySourceNode0);
	
	
	// Create a plane and source for C-arm posture 1

	double color1[3]{ 1,1,1 };
	PlotCoordinate(GetDataStorage(), "LT coordinate", color1);

	imagerPlaneSource1->SetOrigin(0, 0, 0);
	imagerPlaneSource1->SetPoint1(dimensionPlane[0], 0, 0);
	imagerPlaneSource1->SetPoint2(0, dimensionPlane[1], 0);
	imagerPlaneSource1->Update();

	auto imagerPlaneNode1 = mitk::DataNode::New();
	auto imagerPlaneSurface1 = mitk::Surface::New();
	imagerPlaneSurface1->SetVtkPolyData(imagerPlaneSource1->GetOutput());
	imagerPlaneNode1->SetData(imagerPlaneSurface1);
	imagerPlaneNode1->SetName("LT imager");
	imagerPlaneNode1->SetColor(0.0, 0.0, 0.4);
	imagerPlaneNode1->SetVisibility(true);
	imagerPlaneNode1->SetOpacity(0.7);
	GetDataStorage()->Add(imagerPlaneNode1);

	raySource1->SetCenter(m_Controls.lineEdit_planeSourceX_1->text().toDouble(), m_Controls.lineEdit_planeSourceY_1->text().toDouble(), m_Controls.lineEdit_planeSourceZ_1->text().toDouble());
	raySource1->SetRadius(17);
	raySource1->Update();

	auto raySourceNode1 = mitk::DataNode::New();
	auto raySourceSurface1 = mitk::Surface::New();
	raySourceSurface1->SetVtkPolyData(raySource1->GetOutput());
	raySourceNode1->SetData(raySourceSurface1);
	raySourceNode1->SetName("LT raySource");
	raySourceNode1->SetColor(0.0, 0.0, 1.0);
	raySourceNode1->SetVisibility(true);
	raySourceNode1->SetOpacity(1.0);
	GetDataStorage()->Add(raySourceNode1);

}

void SpineCArmRegistration::PlotCoordinate(mitk::DataStorage* ds, std::string name, double color[3])
{
	mitk::DataNode::Pointer coordnode = mitk::DataNode::New();
	coordnode->SetName(name);

	mitk::Surface::Pointer mySphere = mitk::Surface::New();

	double axisLength = 5.;

	vtkSmartPointer<vtkSphereSource> vtkSphere = vtkSmartPointer<vtkSphereSource>::New();
	vtkSmartPointer<vtkConeSource> vtkCone = vtkSmartPointer<vtkConeSource>::New();
	vtkSmartPointer<vtkCylinderSource> vtkCylinder = vtkSmartPointer<vtkCylinderSource>::New();
	vtkSmartPointer<vtkPolyData> axis = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkLineSource> vtkLine = vtkSmartPointer<vtkLineSource>::New();
	vtkSmartPointer<vtkLineSource> vtkLine2 = vtkSmartPointer<vtkLineSource>::New();
	vtkSmartPointer<vtkLineSource> vtkLine3 = vtkSmartPointer<vtkLineSource>::New();

	vtkSmartPointer<vtkAppendPolyData> appendPolyData = vtkSmartPointer<vtkAppendPolyData>::New();
	vtkSmartPointer<vtkPolyData> surface = vtkSmartPointer<vtkPolyData>::New();

	// Y-Axis (start with y, cause cylinder is oriented in y by vtk default...)
	vtkCone->SetDirection(0, 1, 0);
	vtkCone->SetHeight(1.0 );
	vtkCone->SetRadius(0.4f );
	vtkCone->SetResolution(16);
	vtkCone->SetCenter(0.0, axisLength, 0.0);
	vtkCone->Update();

	vtkCylinder->SetRadius(0.05 );
	vtkCylinder->SetHeight(axisLength);
	vtkCylinder->SetCenter(0.0, 0.5 * axisLength, 0.0);
	vtkCylinder->Update();

	appendPolyData->AddInputData(vtkCone->GetOutput());
	appendPolyData->AddInputData(vtkCylinder->GetOutput());
	appendPolyData->Update();
	axis->DeepCopy(appendPolyData->GetOutput());

	// y symbol
	vtkLine->SetPoint1(-0.5  , axisLength + 2.  , 0.0);
	vtkLine->SetPoint2(0.0, axisLength + 1.5  , 0.0);
	vtkLine->Update();

	vtkLine2->SetPoint1(0.5 , axisLength + 2. , 0.0);
	vtkLine2->SetPoint2(-0.5 , axisLength + 1. , 0.0);
	vtkLine2->Update();

	appendPolyData->AddInputData(vtkLine->GetOutput());
	appendPolyData->AddInputData(vtkLine2->GetOutput());
	appendPolyData->AddInputData(axis);
	appendPolyData->Update();
	surface->DeepCopy(appendPolyData->GetOutput());

	// X-axis
	vtkSmartPointer<vtkTransform> XTransform = vtkSmartPointer<vtkTransform>::New();
	XTransform->RotateZ(-90);
	vtkSmartPointer<vtkTransformPolyDataFilter> TrafoFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	TrafoFilter->SetTransform(XTransform);
	TrafoFilter->SetInputData(axis);
	TrafoFilter->Update();

	// x symbol
	vtkLine->SetPoint1(axisLength + 2. , -0.5 , 0.0);
	vtkLine->SetPoint2(axisLength + 1. , 0.5 , 0.0);
	vtkLine->Update();

	vtkLine2->SetPoint1(axisLength + 2. , 0.5 , 0.0);
	vtkLine2->SetPoint2(axisLength + 1. , -0.5 , 0.0);
	vtkLine2->Update();

	appendPolyData->AddInputData(vtkLine->GetOutput());
	appendPolyData->AddInputData(vtkLine2->GetOutput());
	appendPolyData->AddInputData(TrafoFilter->GetOutput());
	appendPolyData->AddInputData(surface);
	appendPolyData->Update();
	surface->DeepCopy(appendPolyData->GetOutput());

	// Z-axis
	vtkSmartPointer<vtkTransform> ZTransform = vtkSmartPointer<vtkTransform>::New();
	ZTransform->RotateX(90);
	TrafoFilter->SetTransform(ZTransform);
	TrafoFilter->SetInputData(axis);
	TrafoFilter->Update();

	// z symbol
	vtkLine->SetPoint1(-0.5 , 0.0, axisLength + 2. );
	vtkLine->SetPoint2(0.5 , 0.0, axisLength + 2. );
	vtkLine->Update();

	vtkLine2->SetPoint1(-0.5 , 0.0, axisLength + 2. );
	vtkLine2->SetPoint2(0.5 , 0.0, axisLength + 1. );
	vtkLine2->Update();

	vtkLine3->SetPoint1(0.5 , 0.0, axisLength + 1. );
	vtkLine3->SetPoint2(-0.5 , 0.0, axisLength + 1. );
	vtkLine3->Update();

	appendPolyData->AddInputData(vtkLine->GetOutput());
	appendPolyData->AddInputData(vtkLine2->GetOutput());
	appendPolyData->AddInputData(vtkLine3->GetOutput());
	appendPolyData->AddInputData(TrafoFilter->GetOutput());
	appendPolyData->AddInputData(surface);
	appendPolyData->Update();
	surface->DeepCopy(appendPolyData->GetOutput());

	// Center
	vtkSphere->SetRadius(0.5f );
	vtkSphere->SetCenter(0.0, 0.0, 0.0);
	vtkSphere->Update();

	appendPolyData->AddInputData(vtkSphere->GetOutput());
	appendPolyData->AddInputData(surface);
	appendPolyData->Update();
	surface->DeepCopy(appendPolyData->GetOutput());

	// Scale
	double s{ 10 };
	vtkSmartPointer<vtkTransform> ScaleTransform = vtkSmartPointer<vtkTransform>::New();
	ScaleTransform->Scale(5. * s, 5. * s, 5. * s);

	TrafoFilter->SetTransform(ScaleTransform);
	TrafoFilter->SetInputData(surface);
	TrafoFilter->Update();

	auto tmpVtkpolydata = vtkPolyData::New();
	tmpVtkpolydata = TrafoFilter->GetOutput();
	
	mySphere->SetVtkPolyData(tmpVtkpolydata);
	
	coordnode->SetData(mySphere);
	coordnode->SetColor(color[0], color[1], color[2]);
	
	ds->Add(coordnode);
}

void SpineCArmRegistration::ConfirmApPoint()
{
	// Draw the AP ray
	mitk::Point3D apPoint = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_spatialApPoint->GetSelectedNode()->GetData())->GetPoint(0);
	mitk::Point3D apSource = GetDataStorage()->GetNamedNode("AP raySource")->GetData()->GetGeometry()->GetCenter();
	double apPointArray[3]{apPoint[0],apPoint[1],apPoint[2] };
	double apSourceArray[3]{ apSource[0],apSource[1],apSource[2] };
	// double colorApRay[3]{ 1,0,0 };
	// DrawLine(apPointArray, apSourceArray, colorApRay,1, "AP ray");

	// Draw the auxiliary lines
	// Get the normal of LT imager
	auto tmpTransMatrix = GetDataStorage()->GetNamedNode("LT imager")->GetData()->GetGeometry()->GetVtkMatrix();
	double ltPlaneNormalArray[3]
	{
		tmpTransMatrix->GetElement(0,2) ,
		tmpTransMatrix->GetElement(1,2) ,
		tmpTransMatrix->GetElement(2,2) 
	};

	
	double ltPlanePointArray[3]
	{
		tmpTransMatrix->GetElement(0,3),
		tmpTransMatrix->GetElement(1,3),
		tmpTransMatrix->GetElement(2,3)
	};

	mitk::Point3D ltSource = GetDataStorage()->GetNamedNode("LT raySource")->GetData()->GetGeometry()->GetCenter();
	double ltSourceArray[3]{ ltSource[0],ltSource[1],ltSource[2] };

	double auxiliaryPoint0[3];
	double auxiliaryPoint1[3];
	lancetAlgorithm::GetLinePlaneIntersection(auxiliaryPoint0, ltSourceArray, apSourceArray, ltPlanePointArray, ltPlaneNormalArray);
	lancetAlgorithm::GetLinePlaneIntersection(auxiliaryPoint1, ltSourceArray, apPointArray, ltPlanePointArray, ltPlaneNormalArray);

	double colorProjectionLine[3]{0.7,0.7,0.7};
	DrawLine(auxiliaryPoint0, auxiliaryPoint1, colorProjectionLine, 1, "Projection line");

	double tmpColor[3]{ 0,0,1 };
	//DrawLine(auxiliaryPoint0, ltSourceArray, colorApRay, 0.2, "Auxiliary line 0");
	//DrawLine(auxiliaryPoint1, ltSourceArray, colorApRay, 0.2, "Auxiliary line 1");

}

void SpineCArmRegistration::ConfirmLtPoint()
{
	mitk::Point3D ltLinePoint = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_spatialLtPoint->GetSelectedNode()->GetData())->GetPoint(0);
	double ltLinePointArray[3]{ ltLinePoint[0], ltLinePoint[1] ,ltLinePoint[2] };
	mitk::Point3D ltSource = GetDataStorage()->GetNamedNode("LT raySource")->GetData()->GetGeometry()->GetCenter();
	double ltSourceArray[3]{ ltSource[0],ltSource[1],ltSource[2] };
	double tmpColor[3]{ 0,0,0.5 };
	DrawLine(ltLinePointArray, ltSourceArray, tmpColor, 0.2, "LT ray");


	mitk::Point3D apPoint = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_spatialApPoint->GetSelectedNode()->GetData())->GetPoint(0);
	mitk::Point3D apSource = GetDataStorage()->GetNamedNode("AP raySource")->GetData()->GetGeometry()->GetCenter();
	double apPointArray[3]{ apPoint[0],apPoint[1],apPoint[2] };
	double apSourceArray[3]{ apSource[0],apSource[1],apSource[2] };
	double colorApRay[3]{ 1,0,0 };
	DrawLine(apPointArray, apSourceArray, colorApRay, 0.2, "AP ray");

	double targetPointArray[3];
	//Get2LineIntersection(targetPointArray, apPointArray, apSourceArray, ltLinePointArray, ltSourceArray);

	// test replacing Get2LineIntersection
	auto tmpPointset = mitk::PointSet::New();
	tmpPointset->InsertPoint(apSource);
	tmpPointset->InsertPoint(ltSource);
	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Line starts");
	tmpNode->SetData(tmpPointset);
	GetDataStorage()->Add(tmpNode);
	m_linesStartsNode = tmpNode;

	auto tmpPointset1 = mitk::PointSet::New();
	tmpPointset1->InsertPoint(apPoint);
	tmpPointset1->InsertPoint(ltLinePoint);
	auto tmpNode1 = mitk::DataNode::New();
	tmpNode1->SetName("Line ends");
	tmpNode1->SetData(tmpPointset1);
	GetDataStorage()->Add(tmpNode1);
	m_linesEndsNode = tmpNode1;

	GetLinesIntersection(targetPointArray);

	GetDataStorage()->Remove(tmpNode);
	GetDataStorage()->Remove(tmpNode1);




	auto tmpSource = vtkSphereSource::New();
	tmpSource->SetCenter(targetPointArray[0], targetPointArray[1], targetPointArray[2]);
	tmpSource->SetRadius(7);
	tmpSource->Update();

	auto targetNode = mitk::DataNode::New();
	auto tmpSurface = mitk::Surface::New();
	tmpSurface->SetVtkPolyData(tmpSource->GetOutput());
	targetNode->SetData(tmpSurface);
	targetNode->SetName("Spatial Target");
	targetNode->SetColor(1.0, 1.0, 1.0);
	targetNode->SetVisibility(true);
	targetNode->SetOpacity(1.0);
	GetDataStorage()->Add(targetNode);


}

void SpineCArmRegistration::DrawLine(double start[3], double end[3], double color[3], double opacity,const char* name)
{
	
	vtkNew<vtkPoints> points_Line;
	points_Line->InsertNextPoint(start);
	points_Line->InsertNextPoint(end);
	vtkNew<vtkPolyLine> polyLine;
	polyLine->GetPointIds()->SetNumberOfIds(2);
	for (unsigned int i = 0; i < 2; i++)
	{
		polyLine->GetPointIds()->SetId(i, i);
	}

	vtkNew<vtkCellArray> cells;
	cells->InsertNextCell(polyLine);
	
	vtkNew<vtkPolyData> polyData;
	polyData->SetPoints(points_Line);
	polyData->SetLines(cells);

	auto linesNode = mitk::DataNode::New();
	auto linesSurface = mitk::Surface::New();
	linesSurface->SetVtkPolyData(polyData);
	linesNode->SetData(linesSurface);


	linesNode->SetName(name);
	linesNode->SetColor(color[0], color[1], color[2]);
	linesNode->SetVisibility(true);
	linesNode->SetOpacity(opacity);
	GetDataStorage()->Add(linesNode);
	
}

void SpineCArmRegistration::Get2LineIntersection(double intersection[3], double line0Start[3], double line0End[3], double line1Start[3], double line1End[3])
{
	unsigned int lineNumber = 2;
	double line0StartX = line0Start[0];
	double line0StartY = line0Start[1];
	double line0StartZ = line0Start[2];

	double line0EndX = line0End[0];
	double line0EndY = line0End[1];
	double line0EndZ = line0End[2];

	double line1StartX = line1Start[0];
	double line1StartY = line1Start[1];
	double line1StartZ = line1Start[2];

	double line1EndX = line1End[0];
	double line1EndY = line1End[1];
	double line1EndZ = line1End[2];

	

	Eigen::VectorXd d(3* lineNumber);
	d << line0StartX, line0StartY, line0StartZ, line1StartX, line1StartY, line1StartZ;
	Eigen::VectorXd d_End(3 * lineNumber);
	d_End << line0EndX, line0EndY, line0EndZ, line1EndX, line1EndY, line1EndZ;
	Eigen::VectorXd d_Substraction(3 * lineNumber);
	d_Substraction = d_End - d;

	Eigen::MatrixXd G(3 * lineNumber, lineNumber + 3);
	// G.Zero();

	for (int i = 0; i < 3 * lineNumber; i = i + 1)
	{
		for (int j = 0; j < 3 + lineNumber; j = j + 1)
		{
			G(i, j) = 0;

			if (i % 3 == 0 && j == 0)
			{
				G(i, j) = 1;
			}
			if (i % 3 == 1 && j == 1)
			{
				G(i, j) = 1;
			}
			if (i % 3 == 2 && j == 2)
			{
				G(i, j) = 1;
			}

			if (j - 2 > 0)
			{
				for (int q = 0; q < 3; q = q + 1)
				{
					G(q + 3 * (j - 3), j) = -d_Substraction[q + 3 * (j - 3)];
				}
			}

		}
	}

	Eigen::VectorXd m(3 + lineNumber);

	Eigen::MatrixXd G_Transpose(3 * lineNumber, lineNumber + 3);
	G_Transpose = G.transpose();

	m = (G_Transpose * G).inverse() * G_Transpose * d;

	// m_Controls.lineIntersectionTextBrowser->append(QString::number(m[0]));
	// m_Controls.lineIntersectionTextBrowser->append(QString::number(m[1]));
	// m_Controls.lineIntersectionTextBrowser->append(QString::number(m[2]));

	intersection[0] = m[0];
	intersection[1] = m[1];
	intersection[2] = m[2];
}

void SpineCArmRegistration::GetLinesIntersection(double intersection[3])
{
	if(m_linesEndsNode != nullptr && m_linesStartsNode != nullptr)
	{
		
		if(dynamic_cast<mitk::PointSet*>(m_linesEndsNode->GetData())->GetSize() == 
			dynamic_cast<mitk::PointSet*>(m_linesStartsNode->GetData())->GetSize())
		{
			auto mitkStartsPointset = dynamic_cast<mitk::PointSet*>(m_linesStartsNode->GetData());
			auto mitkEndsPointset = dynamic_cast<mitk::PointSet*>(m_linesEndsNode->GetData());

			int lineNumber = dynamic_cast<mitk::PointSet*>(m_linesStartsNode->GetData())->GetSize();

			Eigen::VectorXd d(3 * lineNumber);
			// d << line0StartX, line0StartY, line0StartZ, line1StartX, line1StartY, line1StartZ;
			Eigen::VectorXd d_End(3 * lineNumber);
			// d_End << line0EndX, line0EndY, line0EndZ, line1EndX, line1EndY, line1EndZ;
			for( int i = 0 ; i < lineNumber ; i++)
			{
				d[i * 3] = (mitkStartsPointset->GetPoint(i))[0];
				d[i * 3 + 1] = (mitkStartsPointset->GetPoint(i))[1];
				d[i * 3 + 2] = (mitkStartsPointset->GetPoint(i))[2];

				d_End[i * 3] = (mitkEndsPointset->GetPoint(i))[0];
				d_End[i * 3 + 1] = (mitkEndsPointset->GetPoint(i))[1];
				d_End[i * 3 + 2] = (mitkEndsPointset->GetPoint(i))[2];
			}

			Eigen::VectorXd d_Substraction(3 * lineNumber);
			d_Substraction = d_End - d;

			Eigen::MatrixXd G(3 * lineNumber, lineNumber + 3);

			for (int i = 0; i < 3 * lineNumber; i = i + 1)
			{
				for (int j = 0; j < 3 + lineNumber; j = j + 1)
				{
					G(i, j) = 0;

					if (i % 3 == 0 && j == 0)
					{
						G(i, j) = 1;
					}
					if (i % 3 == 1 && j == 1)
					{
						G(i, j) = 1;
					}
					if (i % 3 == 2 && j == 2)
					{
						G(i, j) = 1;
					}

					if (j - 2 > 0)
					{
						for (int q = 0; q < 3; q = q + 1)
						{
							G(q + 3 * (j - 3), j) = -d_Substraction[q + 3 * (j - 3)];
						}
					}

				}
			}

			Eigen::VectorXd m(3 + lineNumber);

			Eigen::MatrixXd G_Transpose(3 * lineNumber, lineNumber + 3);
			G_Transpose = G.transpose();

			m = (G_Transpose * G).inverse() * G_Transpose * d;

			// m_Controls.lineIntersectionTextBrowser->append(QString::number(m[0]));
			// m_Controls.lineIntersectionTextBrowser->append(QString::number(m[1]));
			// m_Controls.lineIntersectionTextBrowser->append(QString::number(m[2]));

			intersection[0] = m[0];
			intersection[1] = m[1];
			intersection[2] = m[2];

		}else
		{
			m_Controls.textBrowser->append("~~~ The start and end pointset do not have the same size ~~~");
			
		}
	}


}





