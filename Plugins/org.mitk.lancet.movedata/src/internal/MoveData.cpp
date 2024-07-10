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
#include "MoveData.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkCLUtil.h>
#include <mitkImage.h>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkCardinalSpline.h>
#include <vtkSplineFilter.h>
#include <vtkProbeFilter.h>
#include <vtkImageSlabReslice.h>
#include <vtkImageAppend.h>

#include "mitkSurfaceToImageFilter.h"
#include <ep/include/vtk-9.1/vtkTransformFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkColorTransferFunction.h>
#include <vtkConnectivityFilter.h>
#include <vtkCutter.h>
#include <vtkDataSetMapper.h>
#include <vtkFillHolesFilter.h>
#include <vtkFloatArray.h>
#include <vtkImageCast.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkLine.h>
#include <vtkLineSource.h>
#include <vtkOBBTree.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkRendererCollection.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkWarpVector.h>

#include "mitkApplyTransformMatrixOperation.h"
#include "mitkInteractionConst.h"
#include "mitkNodePredicateAnd.h"
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include "mitkPointSet.h"
#include "mitkRotationOperation.h"
#include "mitkSurface.h"
#include "mitkSurfaceToImageFilter.h"
#include "QmitkSingleNodeSelectionWidget.h"
#include "QmitkDataStorageTreeModel.h"
#include "QmitkDataStorageTreeModelInternalItem.h"
#include "QmitkRenderWindow.h"
#include "surfaceregistraion.h"
#include <vtkWindowLevelLookupTable.h>

#include "mitkMapper.h"
#include "QmitkRenderWindow.h"
#include "mitkSmartPointerProperty.h"
#include "mitkSurfaceVtkMapper3D.h"
#include "mitkTransferFunctionProperty.h"
#include "mitkVtkInterpolationProperty.h"
#include "mitkVtkMapper.h"
#include "vtkIntersectionPolyDataFilter.h"
#include "vtkClipClosedSurface.h"

#include "vtkPolyDataBooleanFilter.h"

#include <vtkExtractSurface.h>
#include <vtkFeatureEdges.h>
#include <vtkPCANormalEstimation.h>
#include <vtkSignedDistance.h>

#include <iostream>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkTriangle.h>
#include <MRMesh/MRVector3.h>

#include "MRMesh/MRMesh.h"
#include "MRMesh/MRUVSphere.h"
#include "MRMesh/MRMeshBoolean.h"
#include "MRMesh/MRMeshSave.h"
#include "MRMesh/MRMeshLoad.h"
#include "MRMesh/MRMeshDecimate.h"

const std::string MoveData::VIEW_ID = "org.mitk.views.movedata";


void MoveData::SetFocus()
{
	// Purposely not implemented
}

void MoveData::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  InitPointSetSelector(m_Controls.mitkSelectWidget_directionPointSet);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_LandmarkSrcPointset);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_LandmarkTargetPointset);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_IcpTargetPointset);
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_IcpSrcSurface);
  InitNodeSelector(m_Controls.mitkNodeSelectWidget_MovingObject);
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_srcSurface);
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_targetSurface);

  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_inverNormal);
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_fixHole);
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_normalWarp);
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_surfaceboolA);
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_surfaceboolB);
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_smooth);


  m_NodetreeModel = new QmitkDataStorageTreeModel(this->GetDataStorage());

  connect(m_Controls.mitkSelectWidget_directionPointSet, &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged, this, &MoveData::PointSetDirectionChanged);
  connect(m_Controls.mitkNodeSelectWidget_LandmarkSrcPointset, &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged, this, &MoveData::PointSetLandmarkTargetChanged);
  connect(m_Controls.mitkNodeSelectWidget_LandmarkTargetPointset, &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged, this, &MoveData::PointSetLandmarkSourceChanged);
  connect(m_Controls.mitkNodeSelectWidget_IcpTargetPointset, &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged, this, &MoveData::PointSetIcpTargetChanged);
  connect(m_Controls.mitkNodeSelectWidget_IcpSrcSurface, &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged, this, &MoveData::SurfaceIcpSourceChanged);
  connect(m_Controls.mitkNodeSelectWidget_MovingObject, &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged, this, &MoveData::SurfaceIcpSourceChanged);


  connect(m_Controls.pushButton_clearMatrixContent, &QPushButton::clicked, this, &MoveData::ClearMatrixContent);
  connect(m_Controls.pushButton_overwriteOffset, &QPushButton::clicked, this, &MoveData::OverwriteOffsetMatrix);
  connect(m_Controls.pushButton_AppendOffsetMatrix, &QPushButton::clicked, this, &MoveData::AppendOffsetMatrix);

  connect(m_Controls.pushButton_xp, &QPushButton::clicked, this, &MoveData::TranslatePlusX);
  connect(m_Controls.pushButton_yp, &QPushButton::clicked, this, &MoveData::TranslatePlusY);
  connect(m_Controls.pushButton_zp, &QPushButton::clicked, this, &MoveData::TranslatePlusZ);
  connect(m_Controls.pushButton_xm, &QPushButton::clicked, this, &MoveData::TranslateMinusX);
  connect(m_Controls.pushButton_ym, &QPushButton::clicked, this, &MoveData::TranslateMinusY);
  connect(m_Controls.pushButton_zm, &QPushButton::clicked, this, &MoveData::TranslateMinusZ);
  connect(m_Controls.pushButton_rxp, &QPushButton::clicked, this, &MoveData::RotatePlusX);
  connect(m_Controls.pushButton_ryp, &QPushButton::clicked, this, &MoveData::RotatePlusY);
  connect(m_Controls.pushButton_rzp, &QPushButton::clicked, this, &MoveData::RotatePlusZ);
  connect(m_Controls.pushButton_rxm, &QPushButton::clicked, this, &MoveData::RotateMinusX);
  connect(m_Controls.pushButton_rym, &QPushButton::clicked, this, &MoveData::RotateMinusY);
  connect(m_Controls.pushButton_rzm, &QPushButton::clicked, this, &MoveData::RotateMinusZ);
  connect(m_Controls.pushButton_translatePlus, &QPushButton::clicked, this, &MoveData::TranslatePlus);
  connect(m_Controls.pushButton_translateMinus, &QPushButton::clicked, this, &MoveData::TranslateMinus);
  connect(m_Controls.pushButton_RotatePlus, &QPushButton::clicked, this, &MoveData::RotatePlus);
  connect(m_Controls.pushButton_RotateMinus, &QPushButton::clicked, this, &MoveData::RotateMinus);
  connect(m_Controls.pushButton_RealignImage, &QPushButton::clicked, this, &MoveData::RealignImage);
  connect(m_Controls.pushButton_Landmark, &QPushButton::clicked, this, &MoveData::LandmarkRegistration);
  connect(m_Controls.pushButton_ApplyRegistrationMatrix, &QPushButton::clicked, this, &MoveData::AppendRegistrationMatrix);
  connect(m_Controls.pushButton_Icp, &QPushButton::clicked, this, &MoveData::IcpRegistration);
  connect(m_Controls.pushButton_getgeometryWithSpacing, &QPushButton::clicked, this, &MoveData::GetObjectGeometryWithSpacing);
  connect(m_Controls.pushButton_getgeometryWithoutSpacing, &QPushButton::clicked, this, &MoveData::GetObjectGeometryWithoutSpacing);
  connect(m_Controls.pushButton_Icp_surfaceToSurface, &QPushButton::clicked, this, &MoveData::SurfaceToSurfaceIcp);

  connect(m_Controls.pushButton_ApplyStencil, &QPushButton::clicked, this, &MoveData::on_pushButton_ApplyStencil_clicked);
  connect(m_Controls.pushButton_implantStencil, &QPushButton::clicked, this, &MoveData::on_pushButton_implantStencil_clicked);
  connect(m_Controls.pushButton_level, &QPushButton::clicked, this, &MoveData::on_pushButton_level_clicked);
  connect(m_Controls.pushButton_combine, &QPushButton::clicked, this, &MoveData::on_pushButton_combine_clicked);
  connect(m_Controls.pushButton_hardenData, &QPushButton::clicked, this, &MoveData::on_pushButton_hardenData_clicked);

  connect(m_Controls.pushButton_addGizmo, &QPushButton::clicked, this, &MoveData::on_pushButton_addGizmo_clicked);

  connect(m_Controls.pushButton_testCrosshair, &QPushButton::clicked, this, &MoveData::on_pushButton_testCrosshair_clicked);

  m_Controls.horizontalSlider_testCrosshair->setMinimum(0);
  m_Controls.horizontalSlider_testCrosshair->setMaximum(100);
  connect(m_Controls.horizontalSlider_testCrosshair,&QSlider::valueChanged, this, &MoveData::horizontalSlider_testCrosshair_value_changed);


  connect(m_Controls.pushButton_testCPR, &QPushButton::clicked, this, &MoveData::on_pushButton_testCPR_clicked);

  connect(m_Controls.pushButton_getCameraAngle, &QPushButton::clicked, this, &MoveData::on_pushButton_getCameraAngle_clicked);

  connect(m_Controls.pushButton_testStencil, &QPushButton::clicked, this, &MoveData::on_pushButton_testStencil_clicked);

  connect(m_Controls.pushButton_gen5planes, &QPushButton::clicked, this, &MoveData::on_pushButton_gen5planes_clicked);

  connect(m_Controls.pushButton_testIntersect, &QPushButton::clicked, this, &MoveData::on_pushButton_testIntersect_clicked);
  connect(m_Controls.pushButto_interpolation, &QPushButton::clicked, this, &MoveData::on_pushButto_interpolation_clicked);

  connect(m_Controls.pushButton_testClip, &QPushButton::clicked, this, &MoveData::on_pushButton_testClip_clicked);

  connect(m_Controls.pushButton_vtkBool, &QPushButton::clicked, this, &MoveData::on_pushButton_vtkBool_clicked);

  connect(m_Controls.pushButton_inverNormal, &QPushButton::clicked, this, &MoveData::on_pushButton_inverNormal_clicked);
  connect(m_Controls.pushButton_fixhole, &QPushButton::clicked, this, &MoveData::on_pushButton_fixhole_clicked);
  connect(m_Controls.pushButton_smooth, &QPushButton::clicked, this, &MoveData::on_pushButton_smooth_clicked);
  connect(m_Controls.pushButton_warp, &QPushButton::clicked, this, &MoveData::on_pushButton_warp_clicked);
  connect(m_Controls.pushButton_intersect, &QPushButton::clicked, this, &MoveData::on_pushButton_intersect_clicked);
  connect(m_Controls.pushButton_union, &QPushButton::clicked, this, &MoveData::on_pushButton_union_clicked);
  connect(m_Controls.pushButton_diff, &QPushButton::clicked, this, &MoveData::on_pushButton_diff_clicked);
  connect(m_Controls.pushButton_implicitClip, &QPushButton::clicked, this, &MoveData::on_pushButton_implicitClip_clicked);
  connect(m_Controls.pushButton_clipAway, &QPushButton::clicked, this, &MoveData::on_pushButton_clipAway_clicked);
  connect(m_Controls.pushButton_append, &QPushButton::clicked, this, &MoveData::on_pushButton_append_clicked);


  connect(m_Controls.pushButton_initTHAcutting, &QPushButton::clicked, this, &MoveData::on_pushButton_initTHAcutting_clicked);
  connect(m_Controls.pushButton_testCut, &QPushButton::clicked, this, &MoveData::on_pushButton_testCut_clicked);

  connect(m_Controls.pushButton_gen3Region, &QPushButton::clicked, this, &MoveData::on_pushButton_gen3Region_clicked);
  connect(m_Controls.pushButton_testCutV2, &QPushButton::clicked, this, &MoveData::on_pushButton_testCutV2_clicked);

  connect(m_Controls.pushButton_colorPolyData, &QPushButton::clicked, this, &MoveData::on_pushButton_colorPolyData_clicked);

  connect(m_Controls.pushButton_debugRenderer, &QPushButton::clicked, this, &MoveData::on_pushButton_debugRenderer_clicked);

  connect(m_Controls.pushButton_surfaceReconstruct, &QPushButton::clicked, this, &MoveData::on_pushButton_surfaceReconstruct_clicked);
  connect(m_Controls.pushButton_diff_type2, &QPushButton::clicked, this, &MoveData::on_pushButton_diff_type2_clicked);

  connect(m_Controls.pushButton_gen2Stencils, &QPushButton::clicked, this, &MoveData::on_pushButton_gen2Stencils_clicked);
  connect(m_Controls.pushButton_testCutV3, &QPushButton::clicked, this, &MoveData::on_pushButton_testCutV3_clicked);

  connect(m_Controls.pushButton_cutInitV4, &QPushButton::clicked, this, &MoveData::on_pushButton_cutInitV4_clicked);
  connect(m_Controls.pushButton_cutV4, &QPushButton::clicked, this, &MoveData::on_pushButton_cutV4_clicked);

  connect(m_Controls.pushButton_meshlibTest, &QPushButton::clicked, this, &MoveData::on_pushButton_meshlibTest_clicked);

  connect(m_Controls.pushButton_cutInitV5, &QPushButton::clicked, this, &MoveData::on_pushButton_cutInitV5_clicked);
  connect(m_Controls.pushButton_cutV5, &QPushButton::clicked, this, &MoveData::on_pushButton_cutV5_clicked);


}

void MoveData::on_pushButton_cutInitV5_clicked()
{
	if (GetDataStorage()->GetNamedNode("cup") == nullptr || GetDataStorage()->GetNamedNode("cup+") == nullptr ||
		GetDataStorage()->GetNamedNode("bone") == nullptr ||
		GetDataStorage()->GetNamedNode("cutter") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("cup, cup+, bone or cutter is missing");
		return;
	}

	//--------- Retrieve the data ----------------
	auto surface_cup = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("cup")->GetData());
	auto surface_cupPlus = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("cup+")->GetData());
	auto surface_bone = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("bone")->GetData());
	auto surface_cutter = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("cutter")->GetData());

	GetDataStorage()->GetNamedNode("bone")->SetVisibility(false);

	auto polyData_cutter = surface_cutter->GetVtkPolyData();
	auto matrix_cutter = surface_cutter->GetGeometry()->GetVtkMatrix();
	auto trans_cutter = vtkTransform::New();
	trans_cutter->SetMatrix(matrix_cutter);

	vtkNew<vtkTransformPolyDataFilter> transFilter_cutter;
	transFilter_cutter->SetTransform(trans_cutter);
	transFilter_cutter->SetInputData(polyData_cutter);
	transFilter_cutter->Update();

	auto movedPolyData_cutter = transFilter_cutter->GetOutput();

	auto polyData_cup = surface_cup->GetVtkPolyData();
	auto matrix_cup = surface_cup->GetGeometry()->GetVtkMatrix();
	vtkNew<vtkTransform> trans_cup;
	trans_cup->SetMatrix(matrix_cup);

	vtkNew<vtkTransformPolyDataFilter> transFilter_cup;
	transFilter_cup->SetTransform(trans_cup);
	transFilter_cup->SetInputData(polyData_cup);
	transFilter_cup->Update();

	auto movedPolyData_cup = transFilter_cup->GetOutput();

	auto polyData_cupPlus = surface_cupPlus->GetVtkPolyData();
	auto matrix_cupPlus = surface_cupPlus->GetGeometry()->GetVtkMatrix();
	vtkNew<vtkTransform> trans_cupPlus;
	trans_cupPlus->SetMatrix(matrix_cupPlus);

	vtkNew<vtkTransformPolyDataFilter> transFilter_cupPlus;
	transFilter_cupPlus->SetTransform(trans_cupPlus);
	transFilter_cupPlus->SetInputData(polyData_cupPlus);
	transFilter_cupPlus->Update();

	auto movedPolyData_cupPlus = transFilter_cupPlus->GetOutput();

	auto polyData_bone = surface_bone->GetVtkPolyData();
	auto matrix_bone = surface_bone->GetGeometry()->GetVtkMatrix();
	vtkNew<vtkTransform> trans_bone;
	trans_bone->SetMatrix(matrix_bone);

	vtkNew<vtkTransformPolyDataFilter> transFilter_bone;
	transFilter_bone->SetTransform(trans_bone);
	transFilter_bone->SetInputData(polyData_bone);
	transFilter_bone->Update();

	auto movedPolyData_bone = transFilter_bone->GetOutput();


	clock_t start = clock();
	//---------- Convert the vtkPolyData to MR::Mesh by .stl intermediate -----------------
	// --------- save the .stl files --------------
	vtkNew<vtkSTLWriter> stlWriter;
	stlWriter->SetFileName("E:/tmp/test/bone.stl");
	stlWriter->SetInputData(movedPolyData_bone);
	stlWriter->Write();

	stlWriter->SetFileName("E:/tmp/test/cup.stl");
	stlWriter->SetInputData(movedPolyData_cup);
	stlWriter->Write();

	stlWriter->SetFileName("E:/tmp/test/cup+.stl");
	stlWriter->SetInputData(movedPolyData_cupPlus);
	stlWriter->Write();

	stlWriter->SetFileName("E:/tmp/test/cutter.stl");
	stlWriter->SetInputData(movedPolyData_cutter);
	stlWriter->Write();

	// --------- load the .stl files --------------
	MR::Mesh boneMesh = MR::MeshLoad::fromAnySupportedFormat("E:/tmp/test/bone.stl").value();
	MR::Mesh cupMesh = MR::MeshLoad::fromAnySupportedFormat("E:/tmp/test/cup.stl").value();
	MR::Mesh cupPlusMesh = MR::MeshLoad::fromAnySupportedFormat("E:/tmp/test/cup+.stl").value();

	m_Cutter_mesh = MR::MeshLoad::fromAnySupportedFormat("E:/tmp/test/cutter.stl").value();

	clock_t meshSrcReady = clock();

	//----------- Generate the different meshes by boolean---------------
	m_Green_mesh = *(MR::boolean(boneMesh, cupMesh, MR::BooleanOperation::Intersection));
	m_Red_mesh = *(MR::boolean(boneMesh, cupPlusMesh, MR::BooleanOperation::DifferenceAB));
	m_Shell_mesh = *(MR::boolean(boneMesh, cupPlusMesh, MR::BooleanOperation::OutsideA));
	MR::Mesh tmp_mesh = *(MR::boolean(boneMesh, cupPlusMesh, MR::BooleanOperation::Intersection));
	m_Buffer_mesh = *(MR::boolean(tmp_mesh, cupMesh, MR::BooleanOperation::DifferenceAB));

	clock_t meshCutReady = clock();

	//----------- Turn the MR::Mesh into vtkPolyData ----------------
	vtkNew<vtkPolyData> greenPolyData;
	TurnMRMeshIntoPolyData(m_Green_mesh, greenPolyData);

	vtkNew<vtkPolyData>  redPolyData;
	TurnMRMeshIntoPolyData(m_Red_mesh, redPolyData);

	vtkNew<vtkPolyData>  shellPolyData;
	TurnMRMeshIntoPolyData(m_Shell_mesh, shellPolyData);
	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputData(shellPolyData);
	normals->SplittingOff();
	normals->Update();

	// Warp using the normals
	vtkNew<vtkWarpVector> warper;
	warper->SetInputData(normals->GetOutput());
	warper->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS,
		vtkDataSetAttributes::NORMALS);
	warper->SetScaleFactor(0.05);
	warper->Update();

	vtkNew<vtkPolyDataNormals> normals_;
	normals_->SetInputData(warper->GetPolyDataOutput());
	normals_->Update();


	auto bufferPolyData = vtkPolyData::New();
	TurnMRMeshIntoPolyData(m_Buffer_mesh, bufferPolyData);

	clock_t polyDataReady = clock();

	//----------- Add mitk data Nodes and save the polydata---------------
	stlWriter->SetFileName("E:/tmp/test/green.stl");
	stlWriter->SetInputData(greenPolyData);
	stlWriter->Write();

	stlWriter->SetFileName("E:/tmp/test/shell.stl");
	stlWriter->SetInputData(normals_->GetOutput());
	stlWriter->Write();

	stlWriter->SetFileName("E:/tmp/test/buffer.stl");
	stlWriter->SetInputData(bufferPolyData);
	stlWriter->Write();

	stlWriter->SetFileName("E:/tmp/test/red.stl");
	stlWriter->SetInputData(redPolyData);
	stlWriter->Write();

	auto redSurface = mitk::Surface::New();
	redSurface->SetVtkPolyData(redPolyData);
	auto redNode = mitk::DataNode::New();
	redNode->SetData(redSurface);
	redNode->SetColor(1, 0, 0);
	redNode->SetName("red");
	redNode->SetFloatProperty("material.specularCoefficient", 0);
	GetDataStorage()->Add(redNode);

	auto bufferSurface = mitk::Surface::New();
	bufferSurface->SetVtkPolyData(bufferPolyData);
	auto bufferNode = mitk::DataNode::New();
	bufferNode->SetData(bufferSurface);
	bufferNode->SetName("buffer");
	bufferNode->SetFloatProperty("material.specularCoefficient", 0);
	GetDataStorage()->Add(bufferNode);

	auto greenSurface = mitk::Surface::New();
	greenSurface->SetVtkPolyData(greenPolyData);
	auto greenNode = mitk::DataNode::New();
	greenNode->SetData(greenSurface);
	greenNode->SetColor(0, 1, 0);
	greenNode->SetName("green");
	greenNode->SetFloatProperty("material.specularCoefficient", 0);
	GetDataStorage()->Add(greenNode);

	auto shellSurface = mitk::Surface::New();
	shellSurface->SetVtkPolyData(shellPolyData);
	auto shellNode = mitk::DataNode::New();
	shellNode->SetData(shellSurface);
	shellNode->SetName("shell");
	shellNode->SetFloatProperty("material.specularCoefficient", 0);
	GetDataStorage()->Add(shellNode, redNode);

	clock_t polyDataSaved = clock();

	m_Controls.textBrowser_moveData->append("Mesh cutting time:" + QString::number(meshCutReady - meshSrcReady));
	m_Controls.textBrowser_moveData->append("Converting time:" + QString::number(polyDataReady - meshCutReady));

}
                      

void MoveData::TurnMRMeshIntoPolyData(MR::Mesh MRMesh, vtkSmartPointer<vtkPolyData> PolyData)
{
	//------------- Convert the cut result to vtkPolyData for display purpose ---------------
	// all vertices of valid triangles
	const std::vector<std::array<MR::VertId, 3>> triangles = MRMesh.topology.getAllTriVerts();

	size_t cellNum = triangles.size();
	// m_Controls.textBrowser_moveData->append("Cell num:" + QString::number(cellNum));

	// all point coordinates
	const std::vector<MR::Vector3f>& points = MRMesh.points.vec_;

	size_t ptNum = points.size();
	// m_Controls.textBrowser_moveData->append("Point num:" + QString::number(ptNum));

	// triangle vertices as tripples of ints (pointing to elements in points vector)
	const int* vertexTripples = reinterpret_cast<const int*>(triangles.data());

	// Step 1: Create points
	vtkNew<vtkPoints> vtkPoints;

	for (int i{ 0 }; i < ptNum; i++)
	{
		vtkPoints->InsertNextPoint(points[i].x, points[i].y, points[i].z);
	}


	// Step 2: Create a triangle and collect all the triangles

	vtkNew<vtkCellArray> vtkTriangles;

	for (int i{ 0 }; i < cellNum; i++)
	{
		vtkNew<vtkTriangle> vtkTriangle;
		for (int j{ 0 }; j < 3; j++)
		{
			vtkTriangle->GetPointIds()->SetId(j, vertexTripples[3 * i + j]);
		}
		vtkTriangles->InsertNextCell(vtkTriangle);
	}

	// Step 3: Create a polydata object and add the points and triangles to it
	vtkNew<vtkPolyData> newPolyData;
	newPolyData->SetPoints(vtkPoints);
	newPolyData->SetPolys(vtkTriangles);

	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputData(newPolyData);
	normals->Update();

	PolyData->DeepCopy(normals->GetOutput());


}

void MoveData::on_pushButton_cutV5_clicked()
{
	if (GetDataStorage()->GetNamedNode("green") == nullptr || GetDataStorage()->GetNamedNode("buffer") == nullptr ||
		GetDataStorage()->GetNamedNode("shell") == nullptr ||
		GetDataStorage()->GetNamedNode("red") == nullptr ||
		GetDataStorage()->GetNamedNode("cutter") == nullptr )
	{
		m_Controls.textBrowser_moveData->append("Initialization is not ready");
		return;
	}

	clock_t start = clock();

	//------------ Move m_Cutter_mesh with geometry obtained from the cutter surface ----------------
	auto cutterMatrix = GetDataStorage()->GetNamedNode("cutter")->GetData()->GetGeometry()->GetVtkMatrix();

	double matrixDiff{ 0 };

	for(int i{0}; i < 4; i++)
	{
		for (int j{ 3 }; j < 4; j++)
		{
			if(abs(m_LastRoundMatrix->GetElement(i,j)-cutterMatrix->GetElement(i,j))>matrixDiff)
			{
				matrixDiff = abs(m_LastRoundMatrix->GetElement(i, j) - cutterMatrix->GetElement(i, j));
			}
		}
	}

	m_LastRoundMatrix->DeepCopy(cutterMatrix);

	if(matrixDiff < 0.01)
	{
		clock_t end = clock();

		m_Controls.textBrowser_moveData->append("Cutting time: " + QString::number(end - start));

		return;
	}

	MR::Vector3f x(cutterMatrix->GetElement(0,0), cutterMatrix->GetElement(1, 0), cutterMatrix->GetElement(2, 0));
	MR::Vector3f y(cutterMatrix->GetElement(0, 1), cutterMatrix->GetElement(1, 1), cutterMatrix->GetElement(2, 1));
	MR::Vector3f z(cutterMatrix->GetElement(0, 2), cutterMatrix->GetElement(1, 2), cutterMatrix->GetElement(2, 2));
	MR::Vector3f t(cutterMatrix->GetElement(0, 3), cutterMatrix->GetElement(1, 3), cutterMatrix->GetElement(2, 3));
	MR::Matrix3f r(x,y,z);
	MR::AffineXf3f T(r, t);

	m_Cutter_mesh.transform(T);



	// MR::DecimateSettings settings;
	// settings.maxError = 0.2f;

	//------------ MeshLib Boolean -------------------
	//------------ Green -----------
	if(GetDataStorage()->GetNamedObject<mitk::Surface>("green")->GetVtkPolyData()->GetPoints()->GetNumberOfPoints() > 0)
	{
		auto intersectResult = (MR::boolean(m_Green_mesh, m_Cutter_mesh, MR::BooleanOperation::Intersection));

		MR::Mesh intersectMesh = *intersectResult;

		if (intersectMesh.points.vec_.size() > 0)
		{
			auto diffResult = (MR::boolean(m_Green_mesh, m_Cutter_mesh, MR::BooleanOperation::DifferenceAB));

			MR::Mesh diffMesh = *diffResult;

			m_Green_mesh = diffMesh;
			m_Green_mesh.invalidateCaches();
			m_Cutter_mesh.invalidateCaches();
			vtkNew<vtkPolyData> greenPolyData;
			TurnMRMeshIntoPolyData(m_Green_mesh, greenPolyData);
			// auto tmpSurface = mitk::Surface::New();
			// tmpSurface->SetVtkPolyData(greenPolyData);
			// GetDataStorage()->GetNamedNode("green")->SetData(tmpSurface);
			GetDataStorage()->GetNamedObject<mitk::Surface>("green")->ReleaseData();
			GetDataStorage()->GetNamedObject<mitk::Surface>("green")->SetVtkPolyData(greenPolyData);
		}

	}

	//------------ buffer -----------
	if (GetDataStorage()->GetNamedObject<mitk::Surface>("buffer")->GetVtkPolyData()->GetPoints()->GetNumberOfPoints() > 0)
	{
		auto intersectResult = (MR::boolean(m_Buffer_mesh, m_Cutter_mesh, MR::BooleanOperation::Intersection));

		MR::Mesh intersectMesh = *intersectResult;

		if (intersectMesh.points.vec_.size() > 0)
		{
			auto diffResult = (MR::boolean(m_Buffer_mesh, m_Cutter_mesh, MR::BooleanOperation::DifferenceAB));

			MR::Mesh diffMesh = *diffResult;

			m_Buffer_mesh = diffMesh;
			m_Buffer_mesh.invalidateCaches();
			m_Cutter_mesh.invalidateCaches();
			vtkNew<vtkPolyData> BufferPolyData;
			TurnMRMeshIntoPolyData(m_Buffer_mesh, BufferPolyData);
			// auto tmpSurface = mitk::Surface::New();
			// tmpSurface->SetVtkPolyData(BufferPolyData);
			// GetDataStorage()->GetNamedNode("Buffer")->SetData(tmpSurface);
			GetDataStorage()->GetNamedObject<mitk::Surface>("buffer")->ReleaseData();
			GetDataStorage()->GetNamedObject<mitk::Surface>("buffer")->SetVtkPolyData(BufferPolyData);
		}

	}


	//------------- Shell and Red should be considered together ----------------------
	if (GetDataStorage()->GetNamedObject<mitk::Surface>("red")->GetVtkPolyData()->GetPoints()->GetNumberOfPoints() > 0)
	{
		auto intersectResult = (MR::boolean(m_Red_mesh, m_Cutter_mesh, MR::BooleanOperation::Intersection));
	
		MR::Mesh intersectMesh = *intersectResult;
	
		if (intersectMesh.points.vec_.size() > 0)
		{
			//------------- Red part ----------------
			auto diffResult = (MR::boolean(m_Red_mesh, m_Cutter_mesh, MR::BooleanOperation::DifferenceAB));
	
			MR::Mesh diffMesh = *diffResult;
	
			m_Red_mesh = diffMesh;
			m_Red_mesh.invalidateCaches();
			m_Cutter_mesh.invalidateCaches();
			vtkNew<vtkPolyData> RedPolyData;
			TurnMRMeshIntoPolyData(m_Red_mesh, RedPolyData);
			GetDataStorage()->GetNamedObject<mitk::Surface>("red")->ReleaseData();
			GetDataStorage()->GetNamedObject<mitk::Surface>("red")->SetVtkPolyData(RedPolyData);
	
			//------------- Shell part --------------
			auto diffResult_2 = (MR::boolean(m_Shell_mesh, m_Cutter_mesh, MR::BooleanOperation::InsideA));
			MR::Mesh testMesh = *diffResult_2;
			
			if(testMesh.points.vec_.size() > 0)
			{
				auto cutResult = (MR::boolean(m_Shell_mesh, m_Cutter_mesh, MR::BooleanOperation::OutsideA));

				MR::Mesh shellMesh = *cutResult;

				m_Shell_mesh = shellMesh;
				m_Shell_mesh.invalidateCaches();
				m_Cutter_mesh.invalidateCaches();
				vtkNew<vtkPolyData> ShellPolyData;
				TurnMRMeshIntoPolyData(m_Shell_mesh, ShellPolyData);
				// auto tmpSurface = mitk::Surface::New();
				// tmpSurface->SetVtkPolyData(ShellPolyData);
				// GetDataStorage()->GetNamedNode("Shell")->SetData(tmpSurface);
				GetDataStorage()->GetNamedObject<mitk::Surface>("shell")->ReleaseData();
				GetDataStorage()->GetNamedObject<mitk::Surface>("shell")->SetVtkPolyData(ShellPolyData);
			}

			
	
		}
	}

	// //------------ Shell -----------
	// if (GetDataStorage()->GetNamedObject<mitk::Surface>("shell")->GetVtkPolyData()->GetPoints()->GetNumberOfPoints() > 0)
	// {
	// 	auto intersectResult = (MR::boolean(m_Red_mesh, m_Cutter_mesh, MR::BooleanOperation::Intersection));
	// 	
	// 	MR::Mesh intersectMesh = *intersectResult;
	// 	
	// 	if (intersectMesh.points.vec_.size() > 0)
	// 	{
	// 		auto diffResult = (MR::boolean(m_Shell_mesh, m_Cutter_mesh, MR::BooleanOperation::OutsideA));
	// 	
	// 		MR::Mesh diffMesh = *diffResult;
	// 	
	// 		m_Shell_mesh = diffMesh;
	// 		m_Shell_mesh.invalidateCaches();
	// 		vtkNew<vtkPolyData> ShellPolyData;
	// 		TurnMRMeshIntoPolyData(m_Shell_mesh, ShellPolyData);
	// 		// auto tmpSurface = mitk::Surface::New();
	// 		// tmpSurface->SetVtkPolyData(ShellPolyData);
	// 		// GetDataStorage()->GetNamedNode("Shell")->SetData(tmpSurface);
	// 		GetDataStorage()->GetNamedObject<mitk::Surface>("shell")->ReleaseData();
	// 		GetDataStorage()->GetNamedObject<mitk::Surface>("shell")->SetVtkPolyData(ShellPolyData);
	// 	}
	//
	// }
	//
	// //------------ Red -----------
	// if (GetDataStorage()->GetNamedObject<mitk::Surface>("red")->GetVtkPolyData()->GetPoints()->GetNumberOfPoints() > 0)
	// {
	// 	auto intersectResult = (MR::boolean(m_Red_mesh, m_Cutter_mesh, MR::BooleanOperation::Intersection));
	//
	// 	MR::Mesh intersectMesh = *intersectResult;
	//
	// 	if (intersectMesh.points.vec_.size() > 0)
	// 	{
	// 		auto diffResult = (MR::boolean(m_Red_mesh, m_Cutter_mesh, MR::BooleanOperation::DifferenceAB));
	//
	// 		MR::Mesh diffMesh = *diffResult;
	//
	// 		m_Red_mesh = diffMesh;
	// 		m_Red_mesh.invalidateCaches();
	// 		vtkNew<vtkPolyData> RedPolyData;
	// 		TurnMRMeshIntoPolyData(m_Red_mesh, RedPolyData);
	// 		// auto tmpSurface = mitk::Surface::New();
	// 		// tmpSurface->SetVtkPolyData(RedPolyData);
	// 		// GetDataStorage()->GetNamedNode("Red")->SetData(tmpSurface);
	// 		GetDataStorage()->GetNamedObject<mitk::Surface>("red")->ReleaseData();
	// 		GetDataStorage()->GetNamedObject<mitk::Surface>("red")->SetVtkPolyData(RedPolyData);
	// 	}
	//
	// }

	

	//------------- Move the cutter back to initial --------------
	MR::AffineXf3f T_invert = T.inverse();
	m_Cutter_mesh.transform(T_invert);

	clock_t end = clock();

	m_Controls.textBrowser_moveData->append("Cutting time: " + QString::number(end - start));

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}


void MoveData::on_pushButton_meshlibTest_clicked()
{
	clock_t start_0 = clock();

	//-------------- Load mesh ----------------------
	if(bone.topology.getAllTriVerts().size() == 0)
	{
		bone = MR::MeshLoad::fromAnySupportedFormat("E:/tmp/bone.stl").value();
		cutter = MR::MeshLoad::fromAnySupportedFormat("E:/tmp/cutter.stl").value();
	}

	
	clock_t start = clock();

	//-------------- Apply translation to the cutter ------------------------
	auto cutterMatrix = GetDataStorage()->GetNamedNode("cutter")->GetData()->GetGeometry()->GetVtkMatrix();
	MR::AffineXf3f translation = MR::AffineXf3f::translation(MR::Vector3f(cutterMatrix->GetElement(0,3),
		cutterMatrix->GetElement(1, 3) 
		, cutterMatrix->GetElement(2, 3)));

	

	cutter.transform(translation);

	//------------- Perform boolean operation ----------------------------
	MR::BooleanResult result_cut = MR::boolean(bone, cutter, MR::BooleanOperation::DifferenceAB);
	MR::Mesh resultMesh_cut = *result_cut;
	if (!result_cut.valid())
	{
		std::cerr << result_cut.errorString << "\n";
		return;
	}

	bone = resultMesh_cut;

	//------------ Move the cutter to the initial place ------------------
	MR::AffineXf3f translation_minus = MR::AffineXf3f::translation(MR::Vector3f(-cutterMatrix->GetElement(0, 3),
		-cutterMatrix->GetElement(1, 3)
		, -cutterMatrix->GetElement(2, 3)));

	cutter.transform(translation_minus);


	clock_t cutEnd = clock();

	
	//------------- Convert the cut result to vtkPolyData for display purpose ---------------
	// all vertices of valid triangles
	const std::vector<std::array<MR::VertId, 3>> triangles = resultMesh_cut.topology.getAllTriVerts();

	size_t cellNum = triangles.size();
	// m_Controls.textBrowser_moveData->append("Cell num:" + QString::number(cellNum));

	// all point coordinates
	const std::vector<MR::Vector3f>& points = resultMesh_cut.points.vec_;

	size_t ptNum = points.size();
	// m_Controls.textBrowser_moveData->append("Point num:" + QString::number(ptNum));

	// triangle vertices as tripples of ints (pointing to elements in points vector)
	const int* vertexTripples = reinterpret_cast<const int*>(triangles.data());

	// Step 1: Create points
	auto vtkPoints = vtkPoints::New();

	for (int i{ 0 }; i < ptNum; i++)
	{
		vtkPoints->InsertNextPoint(points[i].x, points[i].y, points[i].z);
	}


	// Step 2: Create a triangle and collect all the triangles

	auto vtkTriangles = vtkCellArray::New();

	for (int i{ 0 }; i < cellNum; i++)
	{
		auto vtkTriangle = vtkTriangle::New();
		for (int j{ 0 }; j < 3; j++)
		{
			vtkTriangle->GetPointIds()->SetId(j, vertexTripples[3 * i + j]);
		}
		vtkTriangles->InsertNextCell(vtkTriangle);
	}

	// Step 3: Create a polydata object and add the points and triangles to it
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(vtkPoints);
	polyData->SetPolys(vtkTriangles);

	clock_t convertEnd = clock();


    //------------------- Coloring --------------------------
	// ------------------- Apply normal filter for Gouraud display --------------------
	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputData(polyData);
	normals->Update();

	vtkNew<vtkWarpVector> warper;
	warper->SetInputData(normals->GetOutput());
	warper->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS,
		vtkDataSetAttributes::NORMALS);
	warper->SetScaleFactor(0.04);
	warper->Update();

	vtkNew<vtkPolyDataNormals> normals_;
	normals_->SetInputData(warper->GetPolyDataOutput());
	normals_->Update();

	auto cutResult = normals_->GetOutput();

	
	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints_G = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
	selectEnclosedPoints_G->SetInputData(cutResult);
	selectEnclosedPoints_G->SetSurfaceData(GetDataStorage()->GetNamedObject<mitk::Surface>("Green_stencil")->GetVtkPolyData());
	// selectEnclosedPoints_G->SetTolerance(1);
	selectEnclosedPoints_G->Update();

	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints_R = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
	selectEnclosedPoints_R->SetInputData(cutResult);
	selectEnclosedPoints_R->SetSurfaceData(GetDataStorage()->GetNamedObject<mitk::Surface>("Red_stencil")->GetVtkPolyData());
	selectEnclosedPoints_R->Update();

	// vtkSmartPointer<vtkPoints> insidePoints = vtkSmartPointer<vtkPoints>::New();
	// vtkSmartPointer<vtkCellArray> insideCells = vtkSmartPointer<vtkCellArray>::New();

	vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
	scalars->SetNumberOfComponents(1);
	scalars->SetName("Scalars");

	for (vtkIdType i = 0; i < cutResult->GetNumberOfPoints(); ++i)
	{
		if (selectEnclosedPoints_G->IsInside(i))
		{
			// insidePoints->InsertNextPoint(bone->GetPoint(i));
			scalars->InsertNextValue(100);
			continue;
		}

		if (selectEnclosedPoints_R->IsInside(i))
		{
			// insidePoints->InsertNextPoint(bone->GetPoint(i));
			scalars->InsertNextValue(250);
			continue;
		}

		scalars->InsertNextValue(0);

	}

	cutResult->GetPointData()->SetScalars(scalars);

	//------------------- Coloring End ----------------------

	clock_t colorEnd = clock();

	if(GetDataStorage()->GetNamedNode("Test")==nullptr)
	{
		auto newNode = mitk::DataNode::New();
		auto newSurface = mitk::Surface::New();
		newSurface->SetVtkPolyData(cutResult);
		newNode->SetName("Test");
		newNode->SetData(newSurface);
		GetDataStorage()->Add(newNode);

		mitk::TransferFunctionProperty::Pointer transferProp0;
		newNode->GetProperty(transferProp0, "Surface.TransferFunction");

		// Create a transfer function
		mitk::TransferFunction::Pointer transferFunction = mitk::TransferFunction::New();

		// Modify the transfer function (add control points, adjust properties, etc.)
		// For example, you can add control points for opacity and color:
		transferFunction->AddRGBPoint(0, 1.0, 1.0, 1.0);
		transferFunction->AddRGBPoint(100, 0.0, 1.0, 0.0);
		transferFunction->AddRGBPoint(255, 1.0, 0.0, 0.0);

		if (transferProp0 != nullptr)
		{
			transferProp0->SetValue(transferFunction);
		}
		else
		{
			transferProp0 = mitk::TransferFunctionProperty::New();
			transferProp0->SetValue(transferFunction);
		}

		newNode->SetProperty("Surface.TransferFunction", transferProp0);
		newNode->SetBoolProperty("scalar visibility", true);
		newNode->SetFloatProperty("material.specularCoefficient", 0);

	}else
	{
		GetDataStorage()->GetNamedObject<mitk::Surface>("Test")->SetVtkPolyData(cutResult);
	}

	m_Controls.textBrowser_moveData->append("Read time: " + QString::number(start - start_0));
	m_Controls.textBrowser_moveData->append("Cutting time: " + QString::number(cutEnd - start));
	m_Controls.textBrowser_moveData->append("Convert time: " + QString::number(convertEnd - cutEnd));
	m_Controls.textBrowser_moveData->append("Color time: " + QString::number(colorEnd - convertEnd));


	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void MoveData::on_pushButton_cutInitV4_clicked()
{
	if (GetDataStorage()->GetNamedNode("cup") == nullptr || GetDataStorage()->GetNamedNode("cup+") == nullptr ||
		GetDataStorage()->GetNamedNode("bone") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("cup, cup+ or bone is missing");
		return;
	}
	
	//--------- Retrieve the data ----------------
	auto surface_cup = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("cup")->GetData());
	auto surface_cupPlus = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("cup+")->GetData());
	auto surface_bone = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("bone")->GetData());
	GetDataStorage()->GetNamedNode("bone")->SetVisibility(false);
	
	auto polyData_cup = surface_cup->GetVtkPolyData();
	auto matrix_cup = surface_cup->GetGeometry()->GetVtkMatrix();
	auto trans_cup = vtkTransform::New();
	trans_cup->SetMatrix(matrix_cup);
	
	auto transFilter_cup = vtkTransformPolyDataFilter::New();
	transFilter_cup->SetTransform(trans_cup);
	transFilter_cup->SetInputData(polyData_cup);
	transFilter_cup->Update();
	
	auto movedPolyData_cup = transFilter_cup->GetOutput();
	
	auto polyData_cupPlus = surface_cupPlus->GetVtkPolyData();
	auto matrix_cupPlus = surface_cupPlus->GetGeometry()->GetVtkMatrix();
	auto trans_cupPlus = vtkTransform::New();
	trans_cupPlus->SetMatrix(matrix_cupPlus);
	
	auto transFilter_cupPlus = vtkTransformPolyDataFilter::New();
	transFilter_cupPlus->SetTransform(trans_cupPlus);
	transFilter_cupPlus->SetInputData(polyData_cupPlus);
	transFilter_cupPlus->Update();
	
	auto movedPolyData_cupPlus = transFilter_cupPlus->GetOutput();
	
	auto polyData_bone = surface_bone->GetVtkPolyData();
	auto matrix_bone = surface_bone->GetGeometry()->GetVtkMatrix();
	auto trans_bone = vtkTransform::New();
	trans_bone->SetMatrix(matrix_bone);
	
	auto transFilter_bone = vtkTransformPolyDataFilter::New();
	transFilter_bone->SetTransform(trans_bone);
	transFilter_bone->SetInputData(polyData_bone);
	transFilter_bone->Update();
	
	auto movedPolyData_bone = transFilter_bone->GetOutput();
	
	
	//--------- Generate the green part using vtkbool-----------------
	auto bf_green = vtkSmartPointer<vtkPolyDataBooleanFilter>::New();
	bf_green->SetInputData(0, movedPolyData_bone);
	bf_green->SetInputData(1, movedPolyData_cup);
	bf_green->SetOperModeToIntersection();
	bf_green->Update();
	
	if (bf_green->CheckHasContact() == 0)
	{
		m_Controls.textBrowser_moveData->append("Green generation failed");
		return;
	}
	
	vtkNew<vtkPolyDataNormals> normals_green;
	normals_green->SetInputData(bf_green->GetOutput());
	normals_green->ComputePointNormalsOn();
	normals_green->ComputeCellNormalsOff();
	// normals_green->SetFeatureAngle(20);
	// normals_green->SplittingOn();
	normals_green->Update();
	
	// vtkNew<vtkWarpVector> warper_green;
	// warper_green->SetInputData(normals_green->GetOutput());
	// warper_green->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS,
	// 	vtkDataSetAttributes::NORMALS);
	// warper_green->SetScaleFactor(0);
	// warper_green->Update();
	//
	// vtkNew<vtkPolyDataNormals> normals_green_;
	// normals_green_->SetInputData(warper_green->GetPolyDataOutput());
	// normals_green_->ComputePointNormalsOn();
	// normals_green_->ComputeCellNormalsOff();
	// // normals_green->SetFeatureAngle(20);
	// // normals_green->SplittingOn();
	// normals_green_->Update();
	
	if (normals_green->GetOutput()->GetNumberOfCells() > 0)
	{
		auto newNode = mitk::DataNode::New();
		auto newSurface = mitk::Surface::New();
		newSurface->SetVtkPolyData(normals_green->GetOutput());
		newNode->SetName("Green region");
		newNode->SetData(newSurface);
		newNode->SetFloatProperty("material.specularCoefficient", 0);
		newNode->SetColor(0, 1, 0);
		GetDataStorage()->Add(newNode);
	}
	
	
	//--------- Generate the white buffer zone using vtkbool----------
	auto bf_buffer = vtkSmartPointer<vtkPolyDataBooleanFilter>::New();
	bf_buffer->SetInputData(0, movedPolyData_bone);
	bf_buffer->SetInputData(1, movedPolyData_cupPlus);
	bf_buffer->SetOperModeToIntersection();
	bf_buffer->Update();
	
	if (bf_buffer->CheckHasContact() == 0)
	{
		m_Controls.textBrowser_moveData->append("Green generation failed");
		return;
	}
	
	vtkNew<vtkPolyDataNormals> normals_buffer;
	normals_buffer->SetInputData(bf_buffer->GetOutput());
	normals_buffer->ComputePointNormalsOn();
	normals_buffer->ComputeCellNormalsOff();
	// normals_green->SetFeatureAngle(20);
	// normals_green->SplittingOn();
	normals_buffer->Update();
	
	if (normals_buffer->GetOutput()->GetNumberOfCells() > 0)
	{
		vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance;
		implicitPolyDataDistance->SetInput(movedPolyData_cup);
	
		vtkNew<vtkClipPolyData> clipper;
		clipper->SetInputData(normals_buffer->GetOutput());
		// clipper_green->GenerateClippedOutputOn();
		clipper->SetClipFunction(implicitPolyDataDistance);
		clipper->Update();
	
		auto newNode = mitk::DataNode::New();
		auto newSurface = mitk::Surface::New();
		newSurface->SetVtkPolyData(clipper->GetOutput());
		newNode->SetName("Buffer region");
		newNode->SetData(newSurface);
		newNode->SetFloatProperty("material.specularCoefficient", 0);
		GetDataStorage()->Add(newNode);
	}
	
	//--------- Generate the red shell using clipping ---------
	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance;
	implicitPolyDataDistance->SetInput(movedPolyData_cupPlus);
	
	vtkNew<vtkClipPolyData> clipper;
	clipper->SetInputData(movedPolyData_bone);
	clipper->GenerateClippedOutputOn();
	clipper->SetClipFunction(implicitPolyDataDistance);
	clipper->Update();
	
	vtkNew<vtkPolyData> tmpOutput;
	tmpOutput->DeepCopy(clipper->GetOutput());
	
	auto newNode = mitk::DataNode::New();
	auto newSurface = mitk::Surface::New();
	newSurface->SetVtkPolyData(tmpOutput);
	// newSurface->SetVtkPolyData(clipper->GetClippedOutput());
	newNode->SetName("Red core");
	newNode->SetData(newSurface);
	newNode->SetFloatProperty("material.specularCoefficient", 0);
	newNode->SetFloatProperty("material.diffuseCoefficient", 0);
	newNode->SetFloatProperty("material.ambientCoefficient", 1);
	newNode->SetColor(0.6, 0, 0);
	GetDataStorage()->Add(newNode);
	
	
	//--------- Generate the white shell using clipping ---------
	auto newNode_ = mitk::DataNode::New();
	// newSurface->SetVtkPolyData(clipper->GetClippedOutput());
	newNode_->SetName("White shell");
	newNode_->SetData(newSurface);
	newNode_->SetFloatProperty("material.specularCoefficient", 0);
	newNode_->SetColor(1, 1, 1);
	newNode_->SetBoolProperty("Backface Culling", true);
	GetDataStorage()->Add(newNode_);

}

void MoveData::on_pushButton_cutV4_clicked()
{
	clock_t start = clock();

	if (GetDataStorage()->GetNamedNode("cutter") == nullptr || GetDataStorage()->GetNamedNode("Buffer region") == nullptr ||
		GetDataStorage()->GetNamedNode("Red core") == nullptr || GetDataStorage()->GetNamedNode("Green region") == nullptr
		|| GetDataStorage()->GetNamedNode("White shell") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("cutter, Red core, Buffer region, Green region or White shell is missing");
		return;
	}

	//--------- Retrieve the data --------------
	auto surface_cutter = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("cutter")->GetData());
	auto surface_green = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("Green region")->GetData());
	auto surface_red = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("Red core")->GetData());
	auto surface_buffer = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("Buffer region")->GetData());
	auto surface_white = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("White shell")->GetData());

	auto polyData_cutter = surface_cutter->GetVtkPolyData();
	auto matrix_cutter = surface_cutter->GetGeometry()->GetVtkMatrix();
	auto trans_cutter = vtkTransform::New();
	trans_cutter->SetMatrix(matrix_cutter);

	auto transFilter_cutter = vtkTransformPolyDataFilter::New();
	transFilter_cutter->SetTransform(trans_cutter);
	transFilter_cutter->SetInputData(polyData_cutter);
	transFilter_cutter->Update();

	auto movedPolyData_cutter = transFilter_cutter->GetOutput();

	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance;
	implicitPolyDataDistance->SetInput(movedPolyData_cutter);

	//--------- Cut the Green part -------------
	vtkNew<vtkClipPolyData> clipper_green;
	clipper_green->SetInputData(surface_green->GetVtkPolyData());
	// clipper_green->GenerateClippedOutputOn();
	clipper_green->SetClipFunction(implicitPolyDataDistance);
	clipper_green->Update();

	surface_green->SetVtkPolyData(clipper_green->GetOutput());

	//--------- Cut the buffer zone -------------
	vtkNew<vtkClipPolyData> clipper_buffer;
	clipper_buffer->SetInputData(surface_buffer->GetVtkPolyData());
	// clipper_green->GenerateClippedOutputOn();
	clipper_buffer->SetClipFunction(implicitPolyDataDistance);
	clipper_buffer->Update();

	surface_buffer->SetVtkPolyData(clipper_buffer->GetOutput());



	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	clock_t endTimer = clock();

	m_Controls.textBrowser_moveData->append("Cutting time: " + QString::number(endTimer - start) + "ms");
}


bool MoveData::CheckCapCoverage(vtkSmartPointer<vtkPolyData> cap, vtkSmartPointer<vtkPolyData> polyData, double distanceThres, double outLierpercentageThres)
{
	auto boundaryEdges = vtkSmartPointer<vtkFeatureEdges>::New();
	boundaryEdges->BoundaryEdgesOn();
	boundaryEdges->FeatureEdgesOff();
	boundaryEdges->ManifoldEdgesOff();
	boundaryEdges->NonManifoldEdgesOff();
	boundaryEdges->SetInputData(cap);
	boundaryEdges->Update();

	auto edges = boundaryEdges->GetOutput();

	// Set up a KD-tree locator for polydata B
	auto kdTree = vtkSmartPointer<vtkKdTreePointLocator>::New();
	kdTree->SetDataSet(polyData);
	kdTree->BuildLocator();

	// Iterate over edge points and check the distance

	// auto tmpPset = mitk::PointSet::New();

	int totalEdgeNum = edges->GetPoints()->GetNumberOfPoints();
	double outlierNum{ 0 };

	for (vtkIdType i = 0; i < edges->GetPoints()->GetNumberOfPoints(); ++i) {
		double pointA[3];
		edges->GetPoints()->GetPoint(i, pointA);

		// mitk::Point3D a;
		// a[0] = pointA[0];
		// a[1] = pointA[1];
		// a[2] = pointA[2];
		// tmpPset->InsertPoint(a);

		vtkIdType closestPointId = kdTree->FindClosestPoint(pointA);

		double pointB[3];
		polyData->GetPoints()->GetPoint(closestPointId, pointB);

		// Compute the distance
		double distance = vtkMath::Distance2BetweenPoints(pointA, pointB);
		if (distance > distanceThres) {

			// auto tmpNode = mitk::DataNode::New();
			// tmpNode->SetData(tmpPset);
			// tmpNode->SetName("debugPset");
			// GetDataStorage()->Add(tmpNode);

			//return false;
			outlierNum += 1;
		}
	}

	// auto tmpNode = mitk::DataNode::New();
	// tmpNode->SetData(tmpPset);
	// tmpNode->SetName("debugPset");
	// GetDataStorage()->Add(tmpNode);

	if( outlierNum/totalEdgeNum < outLierpercentageThres)
	{
		return true;
	}
	return false;

}


void MoveData::on_pushButton_gen2Stencils_clicked()
{
	// Step 0: Check data availability
	if(GetDataStorage()->GetNamedNode("cup") == nullptr ||
		GetDataStorage()->GetNamedNode("cup+") == nullptr ||
		GetDataStorage()->GetNamedNode("bone") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("bone, cup or cup+ is missing");
		return;
	}
		
	auto surface_cup = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("cup")->GetData());
	auto surface_cupPlus = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("cup+")->GetData());
	auto surface_bone = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("bone")->GetData());
	GetDataStorage()->GetNamedNode("bone")->SetVisibility(false);

	auto polyData_cup = surface_cup->GetVtkPolyData();
	auto matrix_cup = surface_cup->GetGeometry()->GetVtkMatrix();
	auto trans_cup = vtkTransform::New();
	trans_cup->SetMatrix(matrix_cup);

	auto transFilter_cup = vtkTransformPolyDataFilter::New();
	transFilter_cup->SetTransform(trans_cup);
	transFilter_cup->SetInputData(polyData_cup);
	transFilter_cup->Update();

	auto movedPolyData_cup = transFilter_cup->GetOutput();

	auto polyData_cupPlus = surface_cupPlus->GetVtkPolyData();
	auto matrix_cupPlus = surface_cupPlus->GetGeometry()->GetVtkMatrix();
	auto trans_cupPlus = vtkTransform::New();
	trans_cupPlus->SetMatrix(matrix_cupPlus);

	auto transFilter_cupPlus = vtkTransformPolyDataFilter::New();
	transFilter_cupPlus->SetTransform(trans_cupPlus);
	transFilter_cupPlus->SetInputData(polyData_cupPlus);
	transFilter_cupPlus->Update();

	auto movedPolyData_cupPlus = transFilter_cupPlus->GetOutput();

	auto polyData_bone = surface_bone->GetVtkPolyData();
	auto matrix_bone = surface_bone->GetGeometry()->GetVtkMatrix();
	auto trans_bone = vtkTransform::New();
	trans_bone->SetMatrix(matrix_bone);

	auto transFilter_bone = vtkTransformPolyDataFilter::New();
	transFilter_bone->SetTransform(trans_bone);
	transFilter_bone->SetInputData(polyData_bone);
	transFilter_bone->Update();

	auto movedPolyData_bone = transFilter_bone->GetOutput();

	// Step 1: Generate the green stencil
	auto bf_green = vtkSmartPointer<vtkPolyDataBooleanFilter>::New();
	bf_green->SetInputData(0, movedPolyData_bone);
	bf_green->SetInputData(1, movedPolyData_cup);
	bf_green->SetOperModeToIntersection();
	bf_green->Update();

	vtkNew<vtkPolyDataNormals> normals_green;
	normals_green->SetInputData(bf_green->GetOutput());
	normals_green->ComputePointNormalsOn();
	normals_green->ComputeCellNormalsOn();
	normals_green->SetFeatureAngle(20);
	normals_green->Update();

	// Step 2: Generate the red stencil
	auto bf_red = vtkSmartPointer<vtkPolyDataBooleanFilter>::New();
	bf_red->SetInputData(0, movedPolyData_bone);
	bf_red->SetInputData(1, movedPolyData_cupPlus);
	bf_red->SetOperModeToDifference();
	bf_red->Update();

	vtkNew<vtkPolyDataNormals> normals_red;
	normals_red->SetInputData(bf_red->GetOutput());
	normals_red->ComputePointNormalsOn();
	normals_red->ComputeCellNormalsOn();
	normals_red->SetFeatureAngle(20);
	normals_red->Update();

	

	if (normals_green->GetOutput()->GetNumberOfCells() > 0 && normals_red->GetOutput()->GetNumberOfCells() > 0)
	{
		vtkNew<vtkWarpVector> warper_G;
		warper_G->SetInputData(normals_green->GetOutput());
		warper_G->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS,
			vtkDataSetAttributes::NORMALS);
		warper_G->SetScaleFactor(0.07);
		warper_G->Update();

		auto newNode = mitk::DataNode::New();
		auto newSurface = mitk::Surface::New();
		newSurface->SetVtkPolyData(warper_G->GetPolyDataOutput());
		newNode->SetName("Green_stencil");
		newNode->SetData(newSurface);
		GetDataStorage()->Add(newNode);
		newNode->SetVisibility(false);

		// vtkNew<vtkWarpVector> warper_R;
		// warper_R->SetInputData(normals_red->GetOutput());
		// warper_R->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS,
		// 	vtkDataSetAttributes::NORMALS);
		// warper_R->SetScaleFactor(-0.1);
		// warper_R->Update();

		auto newNode_ = mitk::DataNode::New();
		auto newSurface_ = mitk::Surface::New();
		newSurface_->SetVtkPolyData(normals_red->GetOutput());
		newNode_->SetName("Red_stencil");
		newNode_->SetData(newSurface_);
		GetDataStorage()->Add(newNode_);
		newNode_->SetVisibility(false);

	}else
	{
		m_Controls.textBrowser_moveData->append("No intersection between the implant and the bone!");
		return;
	}

	// Generate the bone to display
	vtkNew<vtkWarpVector> warper_bone;
	warper_bone->SetInputData(polyData_bone);
	warper_bone->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS,
		vtkDataSetAttributes::NORMALS);
	warper_bone->SetScaleFactor(0.05);
	warper_bone->Update();

	vtkNew<vtkPolyDataNormals> normals_warped_bone;
	normals_warped_bone->SetInputData(warper_bone->GetPolyDataOutput());
	normals_warped_bone->ComputePointNormalsOn();
	normals_warped_bone->ComputeCellNormalsOn();
	normals_warped_bone->SetFeatureAngle(20);
	normals_warped_bone->Update();

	auto warped_bone = normals_warped_bone->GetOutput();

	auto newNode = mitk::DataNode::New();
	auto newSurface = mitk::Surface::New();
	newSurface->SetVtkPolyData(warped_bone);
	newNode->SetName("bone_display");
	newNode->SetData(newSurface);
	GetDataStorage()->Add(newNode);
	


	// Step 4 (optional): Do the coloring
	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints_G = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
	selectEnclosedPoints_G->SetInputData(warped_bone);
	selectEnclosedPoints_G->SetSurfaceData(GetDataStorage()->GetNamedObject<mitk::Surface>("Green_stencil")->GetVtkPolyData());
	// selectEnclosedPoints_G->SetTolerance(1);
	selectEnclosedPoints_G->Update();

	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints_R = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
	selectEnclosedPoints_R->SetInputData(warped_bone);
	selectEnclosedPoints_R->SetSurfaceData(GetDataStorage()->GetNamedObject<mitk::Surface>("Red_stencil")->GetVtkPolyData());
	selectEnclosedPoints_R->Update();

	// vtkSmartPointer<vtkPoints> insidePoints = vtkSmartPointer<vtkPoints>::New();
	// vtkSmartPointer<vtkCellArray> insideCells = vtkSmartPointer<vtkCellArray>::New();

	vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
	scalars->SetNumberOfComponents(1);
	scalars->SetName("Scalars");

	for (vtkIdType i = 0; i < warped_bone->GetNumberOfPoints(); ++i)
	{
		if (selectEnclosedPoints_G->IsInside(i))
		{
			// insidePoints->InsertNextPoint(bone->GetPoint(i));
			scalars->InsertNextValue(100);
			continue;
		}

		if (selectEnclosedPoints_R->IsInside(i))
		{
			// insidePoints->InsertNextPoint(bone->GetPoint(i));
			scalars->InsertNextValue(250);
			continue;
		}

		scalars->InsertNextValue(0);

	}

	warped_bone->GetPointData()->SetScalars(scalars);

	mitk::TransferFunctionProperty::Pointer transferProp0;
	GetDataStorage()->GetNamedNode("bone_display")->GetProperty(transferProp0, "Surface.TransferFunction");

	// Create a transfer function
	mitk::TransferFunction::Pointer transferFunction = mitk::TransferFunction::New();

	// Modify the transfer function (add control points, adjust properties, etc.)
	// For example, you can add control points for opacity and color:
	transferFunction->AddRGBPoint(0, 1.0, 1.0, 1.0);
	transferFunction->AddRGBPoint(100, 0.0, 1.0, 0.0);
	transferFunction->AddRGBPoint(255, 1.0, 0.0, 0.0);

	if (transferProp0 != nullptr)
	{
		transferProp0->SetValue(transferFunction);
	}
	else
	{
		transferProp0 = mitk::TransferFunctionProperty::New();
		transferProp0->SetValue(transferFunction);
	}

	GetDataStorage()->GetNamedNode("bone_display")->SetProperty("Surface.TransferFunction", transferProp0);
	GetDataStorage()->GetNamedNode("bone_display")->SetBoolProperty("scalar visibility", true);
	GetDataStorage()->GetNamedNode("bone_display")->SetFloatProperty("material.specularCoefficient", 0);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void MoveData::on_pushButton_testCutV3_clicked()
{
	// Both inputs' normal vectors should be outward !!
	auto inputSurfaceNode_a = GetDataStorage()->GetNamedNode("bone_display");
	if (inputSurfaceNode_a == nullptr)
	{
		m_Controls.textBrowser_moveData->append("bone_display is missing!");
		return;
	}
	auto inputSurface_a = dynamic_cast<mitk::Surface*>(inputSurfaceNode_a->GetData());

	auto inputSurfaceNode_b = GetDataStorage()->GetNamedNode("cutter");
	if (inputSurfaceNode_b == nullptr)
	{
		m_Controls.textBrowser_moveData->append("cutter is missing!");
		return;
	}
	auto inputSurface_b = dynamic_cast<mitk::Surface*>(inputSurfaceNode_b->GetData());

	auto inputpolyData_a = inputSurface_a->GetVtkPolyData();
	auto inputMatrix_a = inputSurface_a->GetGeometry()->GetVtkMatrix();
	auto trans_a = vtkTransform::New();
	trans_a->SetMatrix(inputMatrix_a);

	auto transFilter_a = vtkTransformPolyDataFilter::New();
	transFilter_a->SetTransform(trans_a);
	transFilter_a->SetInputData(inputpolyData_a);
	transFilter_a->Update();

	auto movedPolyData_a = transFilter_a->GetOutput();

	auto inputpolyData_b = inputSurface_b->GetVtkPolyData();
	auto inputMatrix_b = inputSurface_b->GetGeometry()->GetVtkMatrix();
	auto trans_b = vtkTransform::New();
	trans_b->SetMatrix(inputMatrix_b);

	auto transFilter_b = vtkTransformPolyDataFilter::New();
	transFilter_b->SetTransform(trans_b);
	transFilter_b->SetInputData(inputpolyData_b);
	transFilter_b->Update();

	auto movedPolyData_b = transFilter_b->GetOutput();

	clock_t start = clock();

	//--------- Step 1: Use B to pierce through A, and get the broken part of A---------
	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance_b;
	implicitPolyDataDistance_b->SetInput(movedPolyData_b);

	vtkNew<vtkClipPolyData> clipper_0;
	clipper_0->SetInputData(movedPolyData_a);
	// clipper_0->GenerateClippedOutputOn();
	clipper_0->SetClipFunction(implicitPolyDataDistance_b);

	clipper_0->Update();
	vtkNew<vtkPolyData> pierced_A;
	pierced_A->DeepCopy(clipper_0->GetOutput());

	//------- Step 2: Use A to crop B, and get the cropped away part of B-------------
	//-------------- 0621 backup -----------------
	// vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance_a;
	// implicitPolyDataDistance_a->SetInput(movedPolyData_a);
	//
	// vtkNew<vtkClipPolyData> clipper_1;
	// clipper_1->SetInputData(movedPolyData_b);
	// clipper_1->GenerateClippedOutputOn();
	// clipper_1->SetClipFunction(implicitPolyDataDistance_a);

	//-------------- 0621 trial  ------------------
	// vtkNew<vtkPolyDataNormals> normals;
	// normals->SetInputData(movedPolyData_a);
	// normals->SplittingOff();
	//
	// double warpFactor = m_Controls.lineEdit_warpFactor->text().toDouble();
	//
	// if (warpFactor < 0)
	// {
	// 	normals->SetFlipNormals(true);
	// }
	//
	// normals->Update();

	// Warp using the normals
	vtkNew<vtkWarpVector> warper;
	warper->SetInputData(movedPolyData_a);
	warper->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS,
		vtkDataSetAttributes::NORMALS);
	warper->SetScaleFactor(abs(0.04));
	warper->Update();

	vtkNew<vtkPolyDataNormals> normals_;
	normals_->SetInputData(warper->GetPolyDataOutput());
	normals_->Update();

	vtkNew<vtkTriangleFilter> triangles;
	triangles->SetInputData(normals_->GetOutput());
	triangles->Update();


	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance_a;
	implicitPolyDataDistance_a->SetInput(triangles->GetOutput());

	vtkNew<vtkClipPolyData> clipper_1;
	clipper_1->SetInputData(movedPolyData_b);
	clipper_1->GenerateClippedOutputOn();
	clipper_1->SetClipFunction(implicitPolyDataDistance_a);

	// --------------------------------------------

	clipper_1->Update();

	auto testsurface_debug = mitk::Surface::New();
	testsurface_debug->SetVtkPolyData(clipper_1->GetClippedOutput());
	auto testNode_debug = mitk::DataNode::New();
	testNode_debug->SetData(testsurface_debug);
	testNode_debug->SetName("without connectivity");
	GetDataStorage()->Add(testNode_debug);

	// ------------- Check if the cropped away part of the cutter has contact with the bone and only keep the contacting part -------------
	vtkNew<vtkCleanPolyData> cleaner_test;
	cleaner_test->SetInputData(clipper_1->GetClippedOutput());
	cleaner_test->Update();
	
	vtkNew<vtkCleanPolyData> cleaner_test_;
	cleaner_test_->SetInputData(pierced_A);
	cleaner_test_->Update();

	vtkNew<vtkPolyData> cropped_B_;
	vtkNew<vtkAppendPolyData> tmpAppender;
	vtkNew<vtkConnectivityFilter> vtkConnectivityFilter;
	vtkConnectivityFilter->SetInputData(cleaner_test->GetOutput());
	vtkConnectivityFilter->SetExtractionModeToAllRegions();

	vtkConnectivityFilter->Update();
	int numOfSubparts = vtkConnectivityFilter->GetNumberOfExtractedRegions();

	// vtkConnectivityFilter->SetExtractionModeToLargestRegion();
	vtkConnectivityFilter->SetExtractionModeToSpecifiedRegions();

	for(int i{0}; i < numOfSubparts; i++)
	{
		vtkConnectivityFilter->InitializeSpecifiedRegionList();
		vtkConnectivityFilter->AddSpecifiedRegion(i);
		vtkConnectivityFilter->Update();
		auto tmpPolydata = vtkPolyData::New();
		tmpPolydata->DeepCopy(vtkConnectivityFilter->GetPolyDataOutput());

		// tmpPolydata->Print(std::cout);

		////////
		// auto testsurface = mitk::Surface::New();
		// testsurface->SetVtkPolyData(tmpPolydata);
		// auto testNode = mitk::DataNode::New();
		// testNode->SetData(testsurface);
		// testNode->SetName("clipped subpart");
		// GetDataStorage()->Add(testNode);
		////////

		if(CheckCapCoverage(tmpPolydata, cleaner_test_->GetOutput(), 0.2, 0.2))
		{
			tmpAppender->AddInputData(tmpPolydata);
		}

	}

	tmpAppender->Update();

	vtkNew<vtkPolyData> cropped_B;
	cropped_B->DeepCopy(tmpAppender->GetOutput());
	//cropped_B->DeepCopy(clipper_1->GetClippedOutput());

	//--------- Step 3: The normal vectors of the cropped away part of B should be inverted before combining--------
	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputData(cropped_B);
	normals->ComputePointNormalsOn();
	normals->ComputeCellNormalsOn();
	// normals->SetFeatureAngle(20);
	normals->FlipNormalsOn();
	// normals->SplittingOn();
	normals->Update();

	vtkNew<vtkPolyData> croppedNormalInverted_B;
	croppedNormalInverted_B->DeepCopy(normals->GetOutput());

	//--------- Step 4: Append and clean the polyData------------
	auto appender = vtkAppendPolyData::New();
	appender->AddInputData(croppedNormalInverted_B);
	appender->AddInputData(pierced_A);
	appender->Update();

	auto cleaner = vtkCleanPolyData::New();
	cleaner->SetInputData(appender->GetOutput());
	cleaner->Update();

	auto warped_bone = cleaner->GetOutput();

	clock_t cuttingEnd = clock();

	m_Controls.textBrowser_moveData->append("Cutting time: " + QString::number(cuttingEnd - start) + "ms");

	// -----------Step 5: Coloring------------
	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints_G = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
	selectEnclosedPoints_G->SetInputData(warped_bone);
	selectEnclosedPoints_G->SetSurfaceData(GetDataStorage()->GetNamedObject<mitk::Surface>("Green_stencil")->GetVtkPolyData());
	// selectEnclosedPoints_G->SetTolerance(1);
	selectEnclosedPoints_G->Update();

	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints_R = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
	selectEnclosedPoints_R->SetInputData(warped_bone);
	selectEnclosedPoints_R->SetSurfaceData(GetDataStorage()->GetNamedObject<mitk::Surface>("Red_stencil")->GetVtkPolyData());
	selectEnclosedPoints_R->Update();

	// vtkSmartPointer<vtkPoints> insidePoints = vtkSmartPointer<vtkPoints>::New();
	// vtkSmartPointer<vtkCellArray> insideCells = vtkSmartPointer<vtkCellArray>::New();

	vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
	scalars->SetNumberOfComponents(1);
	scalars->SetName("Scalars");

	for (vtkIdType i = 0; i < warped_bone->GetNumberOfPoints(); ++i)
	{
		if (selectEnclosedPoints_G->IsInside(i))
		{
			// insidePoints->InsertNextPoint(bone->GetPoint(i));
			scalars->InsertNextValue(100);
			continue;
		}

		if (selectEnclosedPoints_R->IsInside(i))
		{
			// insidePoints->InsertNextPoint(bone->GetPoint(i));
			scalars->InsertNextValue(250);
			continue;
		}

		scalars->InsertNextValue(0);

	}

	warped_bone->GetPointData()->SetScalars(scalars);

	dynamic_cast<mitk::Surface*>(inputSurfaceNode_a->GetData())->SetVtkPolyData(warped_bone);
	// auto newNode = mitk::DataNode::New();
	// auto newSurface = mitk::Surface::New();
	// newSurface->SetVtkPolyData(cleaner->GetOutput());
	// newNode->SetName(inputSurfaceNode_a->GetName() + "_diffType2");
	// newNode->SetData(newSurface);
	// GetDataStorage()->Add(newNode, inputSurfaceNode_a);

	clock_t colorEnd = clock();

	m_Controls.textBrowser_moveData->append("Coloring time: " + QString::number(colorEnd - cuttingEnd) + "ms");

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void MoveData::on_pushButton_diff_type2_clicked()
{
	// Both inputs' normal vectors should be outward !!
	auto inputSurfaceNode_a = m_Controls.mitkNodeSelectWidget_surfaceboolA->GetSelectedNode();
	if (inputSurfaceNode_a == nullptr)
	{
		return;
	}
	auto inputSurface_a = dynamic_cast<mitk::Surface*>(inputSurfaceNode_a->GetData());

	auto inputSurfaceNode_b = m_Controls.mitkNodeSelectWidget_surfaceboolB->GetSelectedNode();
	if (inputSurfaceNode_b == nullptr)
	{
		return;
	}
	auto inputSurface_b = dynamic_cast<mitk::Surface*>(inputSurfaceNode_b->GetData());

	auto inputpolyData_a = inputSurface_a->GetVtkPolyData();
	auto inputMatrix_a = inputSurface_a->GetGeometry()->GetVtkMatrix();
	auto trans_a = vtkTransform::New();
	trans_a->SetMatrix(inputMatrix_a);

	auto transFilter_a = vtkTransformPolyDataFilter::New();
	transFilter_a->SetTransform(trans_a);
	transFilter_a->SetInputData(inputpolyData_a);
	transFilter_a->Update();

	auto movedPolyData_a = transFilter_a->GetOutput();

	auto inputpolyData_b = inputSurface_b->GetVtkPolyData();
	auto inputMatrix_b = inputSurface_b->GetGeometry()->GetVtkMatrix();
	auto trans_b = vtkTransform::New();
	trans_b->SetMatrix(inputMatrix_b);

	auto transFilter_b = vtkTransformPolyDataFilter::New();
	transFilter_b->SetTransform(trans_b);
	transFilter_b->SetInputData(inputpolyData_b);
	transFilter_b->Update();

	auto movedPolyData_b = transFilter_b->GetOutput();

	//--------- Step 1: Use B to pierce through A, and get the broken part of A---------
	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance_b;
	implicitPolyDataDistance_b->SetInput(movedPolyData_b);

	vtkNew<vtkClipPolyData> clipper_0;
	clipper_0->SetInputData(movedPolyData_a);
	// clipper_0->GenerateClippedOutputOn();
	clipper_0->SetClipFunction(implicitPolyDataDistance_b);

	clipper_0->Update();
	vtkNew<vtkPolyData> pierced_A;
	pierced_A->DeepCopy(clipper_0->GetOutput());

	//------- Step 2: Use A to crop B, and get the cropped away part of B-------------
	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance_a;
	implicitPolyDataDistance_a->SetInput(movedPolyData_a);

	vtkNew<vtkClipPolyData> clipper_1;
	clipper_1->SetInputData(movedPolyData_b);
	clipper_1->GenerateClippedOutputOn();
	clipper_1->SetClipFunction(implicitPolyDataDistance_a);

	clipper_1->Update();
	vtkNew<vtkPolyData> cropped_B;
	cropped_B->DeepCopy(clipper_1->GetClippedOutput());

	//--------- Step 3: The normal vectors of the cropped away part of B should be inverted before combining--------
	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputData(cropped_B);
	normals->ComputePointNormalsOn();
	normals->ComputeCellNormalsOn();
	// normals->SetFeatureAngle(20);
	normals->FlipNormalsOn();
	// normals->SplittingOn();
	normals->Update();

	vtkNew<vtkPolyData> croppedNormalInverted_B;
	croppedNormalInverted_B->DeepCopy(normals->GetOutput());

	//--------- Step 4: Append and clean the polyData------------
	auto appender = vtkAppendPolyData::New();
	appender->AddInputData(croppedNormalInverted_B);
	appender->AddInputData(pierced_A);
	appender->Update();

	auto cleaner = vtkCleanPolyData::New();
	cleaner->SetInputData(appender->GetOutput());
	cleaner->Update();

	auto newNode = mitk::DataNode::New();
	auto newSurface = mitk::Surface::New();
	newSurface->SetVtkPolyData(cleaner->GetOutput());
	newNode->SetName(inputSurfaceNode_a->GetName() + "_diffType2");
	newNode->SetData(newSurface);
	GetDataStorage()->Add(newNode, inputSurfaceNode_a);

	

}


void MoveData::on_pushButton_append_clicked()
{
	auto inputSurfaceNode_a = m_Controls.mitkNodeSelectWidget_surfaceboolA->GetSelectedNode();
	if (inputSurfaceNode_a == nullptr)
	{
		return;
	}
	auto inputSurface_a = dynamic_cast<mitk::Surface*>(inputSurfaceNode_a->GetData());

	auto inputSurfaceNode_b = m_Controls.mitkNodeSelectWidget_surfaceboolB->GetSelectedNode();
	if (inputSurfaceNode_b == nullptr)
	{
		return;
	}
	auto inputSurface_b = dynamic_cast<mitk::Surface*>(inputSurfaceNode_b->GetData());

	auto inputpolyData_a = inputSurface_a->GetVtkPolyData();
	auto inputMatrix_a = inputSurface_a->GetGeometry()->GetVtkMatrix();
	auto trans_a = vtkTransform::New();
	trans_a->SetMatrix(inputMatrix_a);

	auto transFilter_a = vtkTransformPolyDataFilter::New();
	transFilter_a->SetTransform(trans_a);
	transFilter_a->SetInputData(inputpolyData_a);
	transFilter_a->Update();

	auto movedPolyData_a = transFilter_a->GetOutput();

	auto inputpolyData_b = inputSurface_b->GetVtkPolyData();
	auto inputMatrix_b = inputSurface_b->GetGeometry()->GetVtkMatrix();
	auto trans_b = vtkTransform::New();
	trans_b->SetMatrix(inputMatrix_b);

	auto transFilter_b = vtkTransformPolyDataFilter::New();
	transFilter_b->SetTransform(trans_b);
	transFilter_b->SetInputData(inputpolyData_b);
	transFilter_b->Update();

	auto movedPolyData_b = transFilter_b->GetOutput();

	auto appender = vtkAppendPolyData::New();
	appender->AddInputData(movedPolyData_a);
	appender->AddInputData(movedPolyData_b);
	appender->Update();

	auto cleaner = vtkCleanPolyData::New();
	cleaner->SetInputData(appender->GetOutput());
	cleaner->Update();

	auto newNode = mitk::DataNode::New();
	auto newSurface = mitk::Surface::New();
	newSurface->SetVtkPolyData(cleaner->GetOutput());
	newNode->SetName(inputSurfaceNode_a->GetName() + "Appended");
	newNode->SetData(newSurface);
	GetDataStorage()->Add(newNode, inputSurfaceNode_a);

}

int MoveData::on_pushButton_clipAway_clicked()
{
	auto inputSurfaceNode_a = m_Controls.mitkNodeSelectWidget_surfaceboolA->GetSelectedNode();
	if (inputSurfaceNode_a == nullptr)
	{
		// m_Controls.textBrowser_moveData->append("Clip: input missing");
		return 2;
	}
	auto inputSurface_a = dynamic_cast<mitk::Surface*>(inputSurfaceNode_a->GetData());

	auto inputSurfaceNode_b = m_Controls.mitkNodeSelectWidget_surfaceboolB->GetSelectedNode();
	if (inputSurfaceNode_b == nullptr)
	{
		// m_Controls.textBrowser_moveData->append("Clip: input missing");
		return 2;
	}
	auto inputSurface_b = dynamic_cast<mitk::Surface*>(inputSurfaceNode_b->GetData());

	auto inputpolyData_a = inputSurface_a->GetVtkPolyData();
	auto inputMatrix_a = inputSurface_a->GetGeometry()->GetVtkMatrix();
	auto trans_a = vtkTransform::New();
	trans_a->SetMatrix(inputMatrix_a);

	auto transFilter_a = vtkTransformPolyDataFilter::New();
	transFilter_a->SetTransform(trans_a);
	transFilter_a->SetInputData(inputpolyData_a);
	transFilter_a->Update();

	auto movedPolyData_a = transFilter_a->GetOutput();

	auto inputpolyData_b = inputSurface_b->GetVtkPolyData();
	auto inputMatrix_b = inputSurface_b->GetGeometry()->GetVtkMatrix();
	auto trans_b = vtkTransform::New();
	trans_b->SetMatrix(inputMatrix_b);

	auto transFilter_b = vtkTransformPolyDataFilter::New();
	transFilter_b->SetTransform(trans_b);
	transFilter_b->SetInputData(inputpolyData_b);
	transFilter_b->Update();

	auto movedPolyData_b = transFilter_b->GetOutput();

	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance;
	implicitPolyDataDistance->SetInput(movedPolyData_b);

	vtkNew<vtkClipPolyData> clipper;
	clipper->SetInputData(movedPolyData_a);
	clipper->GenerateClippedOutputOn();
	clipper->SetClipFunction(implicitPolyDataDistance);


	clipper->Update();
	vtkNew<vtkPolyData> tmpOutput;
	tmpOutput->DeepCopy(clipper->GetOutput());


	auto newNode = mitk::DataNode::New();
	auto newSurface = mitk::Surface::New();
	// newSurface->SetVtkPolyData(tmpOutput);
	newSurface->SetVtkPolyData(clipper->GetClippedOutput());
	newNode->SetName(inputSurfaceNode_a->GetName() + "_clippedAway");
	newNode->SetData(newSurface);
	GetDataStorage()->Add(newNode, inputSurfaceNode_a);

	if (clipper->GetClippedOutput()->GetNumberOfCells() > 0)
	{
		mitk::RenderingManager::GetInstance()->RequestUpdateAll();
		//m_Controls.textBrowser_moveData->append("Clip: has contact and clipped something");
		return 0;
	}

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	//m_Controls.textBrowser_moveData->append("Clip: has no contact");
	return 1;

}


void MoveData::on_pushButton_surfaceReconstruct_clicked()
{
	auto pointPolyData = GetDataStorage()->GetNamedObject<mitk::Surface>("points")->GetVtkPolyData();

	vtkNew<vtkSignedDistance> distance;

	vtkNew<vtkPCANormalEstimation> normals;
	normals->SetInputData(pointPolyData);
	normals->SetSampleSize(3);
	normals->SetNormalOrientationToGraphTraversal();
	normals->FlipNormalsOn();
	distance->SetInputConnection(normals->GetOutputPort());

	// double radius{ 100 };

	distance->SetRadius(2);

	vtkNew<vtkExtractSurface> surface;
	surface->SetInputConnection(distance->GetOutputPort());
	surface->SetRadius(3);
	surface->Update();

	auto outputPolyData = surface->GetOutput();

	auto newNode = mitk::DataNode::New();
	auto newSurface = mitk::Surface::New();
	newSurface->SetVtkPolyData(outputPolyData);
	newNode->SetName("Testing");
	newNode->SetData(newSurface);
	GetDataStorage()->Add(newNode);
}

void MoveData::on_pushButton_debugRenderer_clicked()
{
	
	auto polyData = GetDataStorage()->GetNamedObject<mitk::Surface>("probe")->GetVtkPolyData();


	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(polyData);
	mapper->SetResolveCoincidentTopologyToPolygonOffset();
	mapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(0, -36000);

	vtkSmartPointer<vtkActor>  actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetSpecular(0);
	actor->GetProperty()->SetAmbient(1);
	// actor->GetProperty()->SetAmbient(0.05);

	actor->GetProperty()->EdgeVisibilityOn();
	actor->GetProperty()->BackfaceCullingOn();
	actor->GetProperty()->SetOpacity(0.5);
	//actor->GetProperty()->SetRepresentationToWireframe();

	auto iRenderWindowPart = GetRenderWindowPart();

	QmitkRenderWindow* mitkRenderWindow = iRenderWindowPart->GetQmitkRenderWindow("3d");

	auto renderWindow = mitkRenderWindow->GetVtkRenderWindow();

	auto testMapper = GetDataStorage()->GetNamedNode("probe")->GetMapper(mitkRenderWindow->GetRenderer()->GetMapperID());
	//
	mitk::VtkMapper* vtkMapper = dynamic_cast<mitk::VtkMapper*>(mapper.GetPointer());
	if (!vtkMapper) {
		std::cerr << "The mapper is not a VTK mapper" << std::endl;
	}


	vtkSmartPointer<vtkRenderer> renderer;

	renderer = renderWindow->GetRenderers()->GetFirstRenderer();

	m_Controls.textBrowser_moveData->append("Num of renders: " + QString::number(renderWindow->GetRenderers()->GetNumberOfItems()));


	// renderWindow->SetAlphaBitPlanes(1);
	// renderWindow->SetMultiSamples(0);
	//
	// renderer->UseDepthPeelingOn();
	// renderer->UseDepthPeelingForVolumesOn();
	renderer->RemoveActor(renderer->GetActors()->GetLastActor());

	renderer->AddActor(actor);
	
	// renderer->SetBackground(255, 255, 255);

	m_Controls.textBrowser_moveData->append("Num of actors: " + QString::number(renderer->GetActors()->GetNumberOfItems()));

	//renderer->GetActors()->GetLastActor()->GetMapper()->SetResolveCoincidentTopologyToPolygonOffset();
	//renderer->GetActors()->GetLastActor()->GetMapper()->SetRelativeCoincidentTopologyPolygonOffsetParameters(1, -100);
}

void MoveData::on_pushButton_colorPolyData_clicked()
{
	if (GetDataStorage()->GetNamedNode("Green") == nullptr ||
		GetDataStorage()->GetNamedNode("bone") == nullptr ||
		GetDataStorage()->GetNamedNode("Red_warped") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("Red_warped, Green or bone is missing");
		return;
	}

	auto bone = GetDataStorage()->GetNamedObject<mitk::Surface>("bone")->GetVtkPolyData();
	auto greenRegion = GetDataStorage()->GetNamedObject<mitk::Surface>("Green")->GetVtkPolyData();
	auto redRegion = GetDataStorage()->GetNamedObject<mitk::Surface>("Red_warped")->GetVtkPolyData();

	vtkNew<vtkPolyData> bone_copy;
	bone_copy->DeepCopy(bone);

	vtkNew<vtkPolyData> bone_back;
	bone_back->DeepCopy(bone);

	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints_G = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
	selectEnclosedPoints_G->SetInputData(bone);
	selectEnclosedPoints_G->SetSurfaceData(greenRegion);
	selectEnclosedPoints_G->Update();

	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints_R = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
	selectEnclosedPoints_R->SetInputData(bone_copy);
	selectEnclosedPoints_R->SetSurfaceData(redRegion);
	selectEnclosedPoints_R->Update();

	// vtkSmartPointer<vtkPoints> insidePoints = vtkSmartPointer<vtkPoints>::New();
	// vtkSmartPointer<vtkCellArray> insideCells = vtkSmartPointer<vtkCellArray>::New();

	vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
	scalars->SetNumberOfComponents(1);
	// scalars->GetNumberOfComponents();
	scalars->SetName("Scalars");

	for (vtkIdType i = 0; i < bone->GetNumberOfPoints(); ++i)
	{
		if (selectEnclosedPoints_G->IsInside(i))
		{
			// insidePoints->InsertNextPoint(bone->GetPoint(i));
			scalars->InsertNextValue(100);
			continue;
		}

		if (selectEnclosedPoints_R->IsInside(i))
		{
			// insidePoints->InsertNextPoint(bone->GetPoint(i));
			scalars->InsertNextValue(250);
			continue;
		}
		
		scalars->InsertNextValue(0);
		
	}

	// vtkSmartPointer<vtkPolyData> insidePolydata = vtkSmartPointer<vtkPolyData>::New();
	// insidePolydata->SetPoints(insidePoints);


	// Extract the cells (polygons) of polydata_A that have all their points inside polydata_B
	// for (vtkIdType i = 0; i < bone->GetNumberOfCells(); ++i)
	// {
	// 	vtkCell* cell = bone->GetCell(i);
	// 	bool inside = true;
	// 	for (vtkIdType j = 0; j < cell->GetNumberOfPoints(); ++j)
	// 	{
	// 		if (!selectEnclosedPoints_G->IsInside(cell->GetPointId(j)))
	// 		{
	// 			inside = false;
	// 			break;
	// 		}
	// 	}
	// 	if (inside)
	// 	{
	// 		insideCells->InsertNextCell(cell);
	// 	}
	// }

	// insidePolydata->SetPolys(insideCells);

	vtkSmartPointer<vtkDataArray> tmpScalars = bone_copy->GetPointData()->GetNormals();
	// vtkSmartPointer<vtkDataArray> testArray ;
	// testArray->Initialize();
	// testArray->DeepCopy(tmpScalars);
	bone_back->GetPointData()->SetScalars(scalars);
	bone_back->GetPointData()->SetNormals(tmpScalars);
	

	auto newNode = mitk::DataNode::New();
	auto newSurface = mitk::Surface::New();
	newSurface->SetVtkPolyData(bone_back);
	newNode->SetName("Testing");
	newNode->SetData(newSurface);
	GetDataStorage()->Add(newNode);

	mitk::TransferFunctionProperty::Pointer transferProp0;
	GetDataStorage()->GetNamedNode("Testing")->GetProperty(transferProp0, "Surface.TransferFunction");
	// transferProp0->SetInterpolationToFlat();
	// GetDataStorage()->GetNamedNode("pros")->SetProperty("material.interpolation", interpolationProp0);
	
	// Create a transfer function
	mitk::TransferFunction::Pointer transferFunction = mitk::TransferFunction::New();
	
	// Modify the transfer function (add control points, adjust properties, etc.)
	// For example, you can add control points for opacity and color:
	transferFunction->AddRGBPoint(0, 1.0, 1.0, 1.0);
	transferFunction->AddRGBPoint(100, 0.0, 1.0, 0.0);
	transferFunction->AddRGBPoint(255, 1.0, 0.0, 0.0);

	if(transferProp0 != nullptr)
	{
		transferProp0->SetValue(transferFunction);
	}
	else
	{
		transferProp0 = mitk::TransferFunctionProperty::New();
		transferProp0->SetValue(transferFunction);
	}


	GetDataStorage()->GetNamedNode("Testing")->SetProperty("Surface.TransferFunction", transferProp0);

	GetDataStorage()->GetNamedNode("Testing")->SetBoolProperty("scalar visibility", true);

	GetDataStorage()->GetNamedNode("Testing")->SetFloatProperty("material.specularCoefficient", 0);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

}

void MoveData::on_pushButton_gen3Region_clicked()
{
	if (GetDataStorage()->GetNamedNode("cup") == nullptr || GetDataStorage()->GetNamedNode("cup+") == nullptr ||
		GetDataStorage()->GetNamedNode("bone") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("cup, cup+ or bone is missing");
		return;
	}

	// Generate the green part
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("bone"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("cup"));
	on_pushButton_intersect_clicked();
	GetDataStorage()->GetNamedNode("bone_intersection")->SetFloatProperty("material.specularCoefficient", 0.1);
	GetDataStorage()->GetNamedNode("bone_intersection")->SetColor(0, 1, 0);
	GetDataStorage()->GetNamedNode("bone_intersection")->SetName("Green");


	// Generate the white buffer zone
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("bone"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("cup+"));
	on_pushButton_intersect_clicked();
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("bone_intersection"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("Green"));
	on_pushButton_diff_clicked();
	GetDataStorage()->GetNamedNode("bone_intersection_difference")->SetFloatProperty("material.specularCoefficient", 0.1);
	GetDataStorage()->GetNamedNode("bone_intersection_difference")->SetColor(1, 1, 1);
	GetDataStorage()->GetNamedNode("bone_intersection_difference")->SetName("Buffer");

	// Generate the red core
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("bone"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("cup+"));
	on_pushButton_diff_clicked();
	GetDataStorage()->GetNamedNode("bone_difference")->SetFloatProperty("material.specularCoefficient", 0.1);
	GetDataStorage()->GetNamedNode("bone_difference")->SetColor(1, 0, 0);
	GetDataStorage()->GetNamedNode("bone_difference")->SetName("Red");

	m_Controls.mitkNodeSelectWidget_normalWarp->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("Red"));
	m_Controls.lineEdit_warpFactor->setText("-0.02");

	on_pushButton_warp_clicked();
	GetDataStorage()->GetNamedNode("Red_warped")->SetFloatProperty("material.specularCoefficient", 0.1);
	GetDataStorage()->GetNamedNode("Red_warped")->SetColor(1, 0, 0);


	// Generate the white shell
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("bone"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("cup+"));
	on_pushButton_diff_clicked();
	GetDataStorage()->GetNamedNode("bone_difference")->SetFloatProperty("material.specularCoefficient", 0.1);
	GetDataStorage()->GetNamedNode("bone_difference")->SetColor(1, 1, 1);
	GetDataStorage()->GetNamedNode("bone_difference")->SetName("White");

	// Turn off all node visibility
	auto dataNodes = GetDataStorage()->GetAll();
	for (auto item = dataNodes->begin(); item != dataNodes->end(); ++item)
	{
		(*item)->SetVisibility(false);
	}
	GetDataStorage()->GetNamedNode("stdmulti.widget0.plane")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("stdmulti.widget1.plane")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("stdmulti.widget2.plane")->SetVisibility(true);

	GetDataStorage()->GetNamedNode("White")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("Red_warped")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("Buffer")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("Green")->SetVisibility(true);


	m_Controls.mitkNodeSelectWidget_normalWarp->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("Green"));
	m_Controls.lineEdit_warpFactor->setText("0.02");
	on_pushButton_warp_clicked();
	GetDataStorage()->GetNamedNode("Green_warped")->SetFloatProperty("material.specularCoefficient", 0);
	GetDataStorage()->GetNamedNode("Green_warped")->SetColor(0, 1, 0);
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Green"));
	GetDataStorage()->GetNamedNode("Green_warped")->SetName("Green");


	m_Controls.mitkNodeSelectWidget_normalWarp->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("bone"));
	m_Controls.lineEdit_warpFactor->setText("0.01");
	on_pushButton_warp_clicked();
	GetDataStorage()->GetNamedNode("bone_warped")->SetFloatProperty("material.specularCoefficient", 0);
	GetDataStorage()->GetNamedNode("bone_warped")->SetColor(1, 1, 1);
	
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void MoveData::on_pushButton_testCutV2_clicked()
{
	if (GetDataStorage()->GetNamedNode("bone_warped") == nullptr || GetDataStorage()->GetNamedNode("Green") == nullptr ||
		GetDataStorage()->GetNamedNode("Red") == nullptr ||
		GetDataStorage()->GetNamedNode("bone") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("bone, bone_warp, Green or Red is missing");
		return;
	}

	// Cut "bone"
	// Update the green part
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("bone"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("cutter"));
	int error_bone = on_pushButton_diff_clicked();

	if (error_bone == 0)
	{
		GetDataStorage()->GetNamedNode("bone_difference")->SetFloatProperty("material.specularCoefficient", 0.1);
		m_Controls.mitkNodeSelectWidget_normalWarp->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("bone_difference"));
		m_Controls.lineEdit_warpFactor->setText("0.01");
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("bone_warped"));
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("bone"));
		on_pushButton_warp_clicked();
		GetDataStorage()->GetNamedNode("bone_difference_warped")->SetFloatProperty("material.specularCoefficient", 0);
		GetDataStorage()->GetNamedNode("bone_difference_warped")->SetColor(1, 1, 1);
		GetDataStorage()->GetNamedNode("bone_difference_warped")->SetName("bone_warped");
		GetDataStorage()->GetNamedNode("bone_difference")->SetName("bone");
	}

	if (error_bone == 1)
	{
		int error_bone_clip = on_pushButton_implicitClip_clicked();
		if (error_bone_clip == 0)
		{
			m_Controls.textBrowser_moveData->append("Has intersection but bone cutting failed!");
		}

		if (error_bone_clip == 1)
		{
			GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("bone_clipped"));
		}

	}

	if (error_bone == 3)
	{
		m_Controls.textBrowser_moveData->append("bone cutting error:" + QString::number(error_bone));
		m_Controls.textBrowser_moveData->append("bone cutting failed");
	}

	//-------------------------------- Coloring----------------------------------------
	if (GetDataStorage()->GetNamedNode("Green") == nullptr ||
		GetDataStorage()->GetNamedNode("bone_warped") == nullptr ||
		GetDataStorage()->GetNamedNode("Red") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("Coloring: Red_warped, Green or bone is missing");
		return;
	}

	auto bone = GetDataStorage()->GetNamedObject<mitk::Surface>("bone_warped")->GetVtkPolyData();
	auto greenRegion = GetDataStorage()->GetNamedObject<mitk::Surface>("Green")->GetVtkPolyData();
	auto redRegion = GetDataStorage()->GetNamedObject<mitk::Surface>("Red")->GetVtkPolyData();

	vtkNew<vtkPolyData> bone_copy;
	bone_copy->DeepCopy(bone);

	vtkNew<vtkPolyData> bone_back;
	bone_back->DeepCopy(bone);

	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints_G = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
	selectEnclosedPoints_G->SetInputData(bone);
	selectEnclosedPoints_G->SetSurfaceData(greenRegion);
	selectEnclosedPoints_G->Update();

	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints_R = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
	selectEnclosedPoints_R->SetInputData(bone_copy);
	selectEnclosedPoints_R->SetSurfaceData(redRegion);
	selectEnclosedPoints_R->Update();

	// vtkSmartPointer<vtkPoints> insidePoints = vtkSmartPointer<vtkPoints>::New();
	// vtkSmartPointer<vtkCellArray> insideCells = vtkSmartPointer<vtkCellArray>::New();

	vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
	scalars->SetNumberOfComponents(1);
	scalars->SetName("Scalars");

	for (vtkIdType i = 0; i < bone->GetNumberOfPoints(); ++i)
	{
		if (selectEnclosedPoints_G->IsInside(i))
		{
			// insidePoints->InsertNextPoint(bone->GetPoint(i));
			scalars->InsertNextValue(100);
			continue;
		}

		if (selectEnclosedPoints_R->IsInside(i))
		{
			// insidePoints->InsertNextPoint(bone->GetPoint(i));
			scalars->InsertNextValue(250);
			continue;
		}

		scalars->InsertNextValue(0);

	}


	bone->GetPointData()->SetScalars(scalars);

	// auto newNode = mitk::DataNode::New();
	// auto newSurface = mitk::Surface::New();
	// newSurface->SetVtkPolyData(bone_back);
	// newNode->SetName("Testing");
	// newNode->SetData(newSurface);
	// GetDataStorage()->Add(newNode);

	mitk::TransferFunctionProperty::Pointer transferProp0;
	GetDataStorage()->GetNamedNode("bone_warped")->GetProperty(transferProp0, "Surface.TransferFunction");

	// Create a transfer function
	mitk::TransferFunction::Pointer transferFunction = mitk::TransferFunction::New();

	// Modify the transfer function (add control points, adjust properties, etc.)
	// For example, you can add control points for opacity and color:
	transferFunction->AddRGBPoint(0, 1.0, 1.0, 1.0);
	transferFunction->AddRGBPoint(100, 0.0, 1.0, 0.0);
	transferFunction->AddRGBPoint(255, 1.0, 0.0, 0.0);

	if (transferProp0 != nullptr)
	{
		transferProp0->SetValue(transferFunction);
	}
	else
	{
		transferProp0 = mitk::TransferFunctionProperty::New();
		transferProp0->SetValue(transferFunction);
	}


	GetDataStorage()->GetNamedNode("bone_warped")->SetProperty("Surface.TransferFunction", transferProp0);

	GetDataStorage()->GetNamedNode("bone_warped")->SetBoolProperty("scalar visibility", true);

	GetDataStorage()->GetNamedNode("bone_warped")->SetFloatProperty("material.specularCoefficient", 0);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	// Turn off all node visibility
	GetDataStorage()->GetNamedNode("White")->SetVisibility(false);
	GetDataStorage()->GetNamedNode("Red_warped")->SetVisibility(false);
	GetDataStorage()->GetNamedNode("Buffer")->SetVisibility(false);
	GetDataStorage()->GetNamedNode("Green")->SetVisibility(false);

	GetDataStorage()->GetNamedNode("bone_warped")->SetVisibility(true);

}

void MoveData::on_pushButton_testCut_clicked()
{
	if (GetDataStorage()->GetNamedNode("White") == nullptr || GetDataStorage()->GetNamedNode("Buffer") == nullptr ||
		GetDataStorage()->GetNamedNode("Red") == nullptr ||
		GetDataStorage()->GetNamedNode("Green") == nullptr ||
		GetDataStorage()->GetNamedNode("cutter") == nullptr ||
		GetDataStorage()->GetNamedNode("Red_warped") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("White, Buffer, Red, Red_warped, cutter or Green is missing");
		return;
	}

	// Update the green part
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("Green"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("cutter"));
	int error_Green = on_pushButton_diff_clicked();
	
	if (error_Green == 0)
	{
		GetDataStorage()->GetNamedNode("Green_difference")->SetFloatProperty("material.specularCoefficient", 0.1);
		GetDataStorage()->GetNamedNode("Green_difference")->SetColor(0, 1, 0);
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Green"));
		GetDataStorage()->GetNamedNode("Green_difference")->SetName("Green");
	}

	if (error_Green == 1)
	{
		int error_Green_clip = on_pushButton_implicitClip_clicked();
		if(error_Green_clip == 0)
		{
			GetDataStorage()->GetNamedNode("Green_clipped")->SetFloatProperty("material.specularCoefficient", 0.1);
			GetDataStorage()->GetNamedNode("Green_clipped")->SetColor(0, 1, 0);
			GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Green"));
			GetDataStorage()->GetNamedNode("Green_clipped")->SetName("Green");
			m_Controls.textBrowser_moveData->append("Green clipping is used in stead of boolean!");
		}

		if (error_Green_clip == 1)
		{
			GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Green_clipped"));
		}

	}

	if (error_Green == 3)
	{
		m_Controls.textBrowser_moveData->append("Green cutting error:" + QString::number(error_Green));
		m_Controls.textBrowser_moveData->append("Green cutting failed");
	}


	// Update the buffer part
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("Buffer"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("cutter"));
	int error_buffer = on_pushButton_diff_clicked();
	// m_Controls.textBrowser_moveData->append("Buffer cutting error:" + QString::number(error_buffer));
	if (error_buffer == 0)
	{
		GetDataStorage()->GetNamedNode("Buffer_difference")->SetFloatProperty("material.specularCoefficient", 0.1);
		GetDataStorage()->GetNamedNode("Buffer_difference")->SetColor(1, 1, 1);
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Buffer"));
		GetDataStorage()->GetNamedNode("Buffer_difference")->SetName("Buffer");
	}

	if (error_buffer == 1)
	{
		int error_buffer_clip = on_pushButton_implicitClip_clicked();
		if (error_buffer_clip == 0)
		{
			GetDataStorage()->GetNamedNode("Buffer_clipped")->SetFloatProperty("material.specularCoefficient", 0.1);
			GetDataStorage()->GetNamedNode("Buffer_clipped")->SetColor(1, 1, 1);
			GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Buffer"));
			GetDataStorage()->GetNamedNode("Buffer_clipped")->SetName("Buffer");
			m_Controls.textBrowser_moveData->append("Buffer clipping is used in stead of boolean!");
		}

		if (error_buffer_clip == 1)
		{
			GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Buffer_clipped"));
		}

	}

	if (error_buffer == 3)
	{
		m_Controls.textBrowser_moveData->append("Buffer cutting error:" + QString::number(error_buffer));
		m_Controls.textBrowser_moveData->append("Buffer cutting failed");
	}
	
	

	// Update the red part
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("Red"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("cutter"));

	int error_red = on_pushButton_diff_clicked();
	// m_Controls.textBrowser_moveData->append("Red cutting error:" + QString::number(error_red));
	if (error_red != 1)
	{
		if (GetDataStorage()->GetNamedNode("Red_difference") != nullptr)
		{
			GetDataStorage()->GetNamedNode("Red_difference")->SetFloatProperty("material.specularCoefficient", 0.1);
			GetDataStorage()->GetNamedNode("Red_difference")->SetColor(1, 0, 0);
			GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Red"));
			GetDataStorage()->GetNamedNode("Red_difference")->SetName("Red");

			m_Controls.mitkNodeSelectWidget_normalWarp->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("Red"));
			m_Controls.lineEdit_warpFactor->setText("-0.02");

			GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Red_warped"));

			on_pushButton_warp_clicked();
			GetDataStorage()->GetNamedNode("Red_warped")->SetFloatProperty("material.specularCoefficient", 0.1);
			GetDataStorage()->GetNamedNode("Red_warped")->SetColor(1, 0, 0);
			GetDataStorage()->GetNamedNode("Red")->SetVisibility(false);

		}
		else
		{
			m_Controls.textBrowser_moveData->append("Red cutting error:" + QString::number(error_red));
			m_Controls.textBrowser_moveData->append("Red cutting failed");

		}
	}

	

	// Update the white part
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("White"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("cutter"));
	if(on_pushButton_implicitClip_clicked() != 2)
	{
		if (GetDataStorage()->GetNamedNode("White_clipped") != nullptr)
		{
			GetDataStorage()->GetNamedNode("White_clipped")->SetFloatProperty("material.specularCoefficient", 0.1);
			GetDataStorage()->GetNamedNode("White_clipped")->SetColor(1, 1, 1);
			GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("White"));
			GetDataStorage()->GetNamedNode("White_clipped")->SetName("White");
		}
		else
		{
			m_Controls.textBrowser_moveData->append("White cutting failed");

		}
	}

	

	// GetDataStorage()->GetNamedNode("White")->SetVisibility(false);
	// GetDataStorage()->GetNamedNode("Buffer")->SetVisibility(false);
	// GetDataStorage()->GetNamedNode("Red_warped")->SetVisibility(false);

}

void MoveData::on_pushButton_initTHAcutting_clicked()
{
	if(GetDataStorage()->GetNamedNode("cup") == nullptr || GetDataStorage()->GetNamedNode("cup+") == nullptr ||
		GetDataStorage()->GetNamedNode("bone") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("cup, cup+ or bone is missing");
		return;
	}

	// Generate the green part
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("bone"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("cup"));
	on_pushButton_intersect_clicked();
	GetDataStorage()->GetNamedNode("bone_intersection")->SetFloatProperty("material.specularCoefficient", 0.1);
	GetDataStorage()->GetNamedNode("bone_intersection")->SetColor(0, 1, 0);
	GetDataStorage()->GetNamedNode("bone_intersection")->SetName("Green");

	// Generate the white buffer zone
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("bone"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("cup+"));
	on_pushButton_intersect_clicked();
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("bone_intersection"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("Green"));
	on_pushButton_diff_clicked();
	GetDataStorage()->GetNamedNode("bone_intersection_difference")->SetFloatProperty("material.specularCoefficient", 0.1);
	GetDataStorage()->GetNamedNode("bone_intersection_difference")->SetColor(1, 1, 1);
	GetDataStorage()->GetNamedNode("bone_intersection_difference")->SetName("Buffer");

	// Generate the red core
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("bone"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("cup+"));
	on_pushButton_diff_clicked();
	GetDataStorage()->GetNamedNode("bone_difference")->SetFloatProperty("material.specularCoefficient", 0.1);
	GetDataStorage()->GetNamedNode("bone_difference")->SetColor(1, 0, 0);
	GetDataStorage()->GetNamedNode("bone_difference")->SetName("Red");

	m_Controls.mitkNodeSelectWidget_normalWarp->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("Red"));
	m_Controls.lineEdit_warpFactor->setText("-0.02");
	
	on_pushButton_warp_clicked();
	GetDataStorage()->GetNamedNode("Red_warped")->SetFloatProperty("material.specularCoefficient", 0.1);
	GetDataStorage()->GetNamedNode("Red_warped")->SetColor(1, 0, 0);


	// Generate the white shell
	m_Controls.mitkNodeSelectWidget_surfaceboolA->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("bone"));
	m_Controls.mitkNodeSelectWidget_surfaceboolB->SetCurrentSelectedNode(GetDataStorage()->GetNamedNode("cup+"));
	on_pushButton_diff_clicked();
	GetDataStorage()->GetNamedNode("bone_difference")->SetFloatProperty("material.specularCoefficient", 0.1);
	GetDataStorage()->GetNamedNode("bone_difference")->SetColor(1, 1, 1);
	GetDataStorage()->GetNamedNode("bone_difference")->SetName("White");

	// Turn off all node visibility
	auto dataNodes = GetDataStorage()->GetAll();
	for (auto item = dataNodes->begin(); item != dataNodes->end(); ++item)
	{
		(*item)->SetVisibility(false);
	}
	GetDataStorage()->GetNamedNode("stdmulti.widget0.plane")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("stdmulti.widget1.plane")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("stdmulti.widget2.plane")->SetVisibility(true);

	GetDataStorage()->GetNamedNode("White")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("Red_warped")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("Buffer")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("Green")->SetVisibility(true);


	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void MoveData::on_pushButton_inverNormal_clicked()
{
	auto inputSurfaceNode = m_Controls.mitkNodeSelectWidget_inverNormal->GetSelectedNode();

	if(inputSurfaceNode == nullptr)
	{
		return;
	}

	auto inputSurface = dynamic_cast<mitk::Surface*>(inputSurfaceNode->GetData());

	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputData(inputSurface->GetVtkPolyData());
	normals->ComputePointNormalsOn();
	normals->ComputeCellNormalsOn();
	//normals->SetFeatureAngle(20);
	normals->FlipNormalsOn();
	normals->SplittingOn();
	normals->Update();
	
	inputSurface->SetVtkPolyData(normals->GetOutput());

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void MoveData::on_pushButton_fixhole_clicked()
{
	auto inputSurfaceNode = m_Controls.mitkNodeSelectWidget_fixHole->GetSelectedNode();

	if (inputSurfaceNode == nullptr)
	{
		return;
	}

	auto inputSurface = dynamic_cast<mitk::Surface*>(inputSurfaceNode->GetData());

	vtkNew<vtkFillHolesFilter> holeFiller;
	holeFiller->SetInputData(inputSurface->GetVtkPolyData());
	holeFiller->SetHoleSize(m_Controls.lineEdit_holeSize->text().toDouble());
	holeFiller->Update();
	vtkNew<vtkPolyData> fixedPolyData;
	fixedPolyData->DeepCopy(holeFiller->GetOutput());

	auto newNode = mitk::DataNode::New();
	auto newSurface = mitk::Surface::New();
	newSurface->SetVtkPolyData(fixedPolyData);
	newNode->SetName(inputSurfaceNode->GetName() +"_fixed");
	newNode->SetData(newSurface);
	GetDataStorage()->Add(newNode,inputSurfaceNode);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void MoveData::on_pushButton_smooth_clicked()
{
	auto inputSurfaceNode = m_Controls.mitkNodeSelectWidget_smooth->GetSelectedNode();

	if (inputSurfaceNode == nullptr)
	{
		return;
	}

	auto inputSurface = dynamic_cast<mitk::Surface*>(inputSurfaceNode->GetData());

	auto smoother = vtkSmoothPolyDataFilter::New();

	smoother->SetInputData(inputSurface->GetVtkPolyData());
	smoother->SetRelaxationFactor(m_Controls.lineEdit_smoothFactor->text().toDouble());
	smoother->SetNumberOfIterations(m_Controls.lineEdit_warpFactor_smoothIter->text().toInt());
	smoother->BoundarySmoothingOn();
	smoother->Update();

	auto newNode = mitk::DataNode::New();
	auto newSurface = mitk::Surface::New();
	newSurface->SetVtkPolyData(smoother->GetOutput());
	newNode->SetName(inputSurfaceNode->GetName() + "_smoothed");
	newNode->SetData(newSurface);
	GetDataStorage()->Add(newNode, inputSurfaceNode);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void MoveData::on_pushButton_warp_clicked()
{
	auto inputSurfaceNode = m_Controls.mitkNodeSelectWidget_normalWarp->GetSelectedNode();

	if (inputSurfaceNode == nullptr)
	{
		return;
	}

	auto inputSurface = dynamic_cast<mitk::Surface*>(inputSurfaceNode->GetData());

	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputData(inputSurface->GetVtkPolyData());
	normals->SplittingOff();

	double warpFactor = m_Controls.lineEdit_warpFactor->text().toDouble();

	if (warpFactor < 0)
	{
		normals->SetFlipNormals(true);
	}
	
	normals->Update();

	// Warp using the normals
	vtkNew<vtkWarpVector> warper;
	warper->SetInputData(normals->GetOutput());
	warper->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS,
		vtkDataSetAttributes::NORMALS);
	warper->SetScaleFactor(abs(warpFactor));
	warper->Update();

	vtkNew<vtkPolyDataNormals> normals_;
	normals_->SetInputData(warper->GetPolyDataOutput());
	normals_->Update();

	vtkNew<vtkTriangleFilter> triangles;
	triangles->SetInputData(normals_->GetOutput());
	triangles->Update();

	auto newNode = mitk::DataNode::New();
	auto newSurface = mitk::Surface::New();
	newSurface->SetVtkPolyData(triangles->GetOutput());
	newNode->SetName(inputSurfaceNode->GetName() + "_warped");
	newNode->SetData(newSurface);
	GetDataStorage()->Add(newNode, inputSurfaceNode);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void MoveData::on_pushButton_intersect_clicked()
{
	auto inputSurfaceNode_a = m_Controls.mitkNodeSelectWidget_surfaceboolA->GetSelectedNode();
	if (inputSurfaceNode_a == nullptr)
	{
		return;
	}
	auto inputSurface_a = dynamic_cast<mitk::Surface*>(inputSurfaceNode_a->GetData());

	auto inputSurfaceNode_b = m_Controls.mitkNodeSelectWidget_surfaceboolB->GetSelectedNode();
	if (inputSurfaceNode_b == nullptr)
	{
		return;
	}
	auto inputSurface_b = dynamic_cast<mitk::Surface*>(inputSurfaceNode_b->GetData());

	auto inputpolyData_a = inputSurface_a->GetVtkPolyData();
	auto inputMatrix_a = inputSurface_a->GetGeometry()->GetVtkMatrix();
	auto trans_a = vtkTransform::New();
	trans_a->SetMatrix(inputMatrix_a);

	auto transFilter_a = vtkTransformPolyDataFilter::New();
	transFilter_a->SetTransform(trans_a);
	transFilter_a->SetInputData(inputpolyData_a);
	transFilter_a->Update();

	auto movedPolyData_a = transFilter_a->GetOutput();

	auto inputpolyData_b = inputSurface_b->GetVtkPolyData();
	auto inputMatrix_b = inputSurface_b->GetGeometry()->GetVtkMatrix();
	auto trans_b = vtkTransform::New();
	trans_b->SetMatrix(inputMatrix_b);

	auto transFilter_b = vtkTransformPolyDataFilter::New();
	transFilter_b->SetTransform(trans_b);
	transFilter_b->SetInputData(inputpolyData_b);
	transFilter_b->Update();

	auto movedPolyData_b = transFilter_b->GetOutput();

	clock_t start = clock();

	auto bf = vtkSmartPointer<vtkPolyDataBooleanFilter>::New();
	bf->SetInputData(0, movedPolyData_a);
	bf->SetInputData(1, movedPolyData_b);
	bf->SetOperModeToIntersection();
	bf->Update();

	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputData(bf->GetOutput());
	normals->ComputePointNormalsOn();
	normals->ComputeCellNormalsOn();
	normals->SetFeatureAngle(20);
	normals->SplittingOn();
	normals->Update();

	MITK_WARN << "vtkbool time: " << (clock() - start);

	if (normals->GetOutput()->GetNumberOfCells() > 0)
	{
		auto newNode = mitk::DataNode::New();
		auto newSurface = mitk::Surface::New();
		newSurface->SetVtkPolyData(normals->GetOutput());
		newNode->SetName(inputSurfaceNode_a->GetName() + "_intersection");
		newNode->SetData(newSurface);
		GetDataStorage()->Add(newNode, inputSurfaceNode_a);
	}

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void MoveData::on_pushButton_union_clicked()
{
	auto inputSurfaceNode_a = m_Controls.mitkNodeSelectWidget_surfaceboolA->GetSelectedNode();
	if (inputSurfaceNode_a == nullptr)
	{
		return;
	}
	auto inputSurface_a = dynamic_cast<mitk::Surface*>(inputSurfaceNode_a->GetData());

	auto inputSurfaceNode_b = m_Controls.mitkNodeSelectWidget_surfaceboolB->GetSelectedNode();
	if (inputSurfaceNode_b == nullptr)
	{
		return;
	}
	auto inputSurface_b = dynamic_cast<mitk::Surface*>(inputSurfaceNode_b->GetData());

	auto inputpolyData_a = inputSurface_a->GetVtkPolyData();
	auto inputMatrix_a = inputSurface_a->GetGeometry()->GetVtkMatrix();
	auto trans_a = vtkTransform::New();
	trans_a->SetMatrix(inputMatrix_a);

	auto transFilter_a = vtkTransformPolyDataFilter::New();
	transFilter_a->SetTransform(trans_a);
	transFilter_a->SetInputData(inputpolyData_a);
	transFilter_a->Update();

	auto movedPolyData_a = transFilter_a->GetOutput();

	auto inputpolyData_b = inputSurface_b->GetVtkPolyData();
	auto inputMatrix_b = inputSurface_b->GetGeometry()->GetVtkMatrix();
	auto trans_b = vtkTransform::New();
	trans_b->SetMatrix(inputMatrix_b);

	auto transFilter_b = vtkTransformPolyDataFilter::New();
	transFilter_b->SetTransform(trans_b);
	transFilter_b->SetInputData(inputpolyData_b);
	transFilter_b->Update();

	auto movedPolyData_b = transFilter_b->GetOutput();

	clock_t start = clock();

	auto bf = vtkSmartPointer<vtkPolyDataBooleanFilter>::New();
	bf->SetInputData(0, movedPolyData_a);
	bf->SetInputData(1, movedPolyData_b);
	bf->SetOperModeToUnion();
	bf->Update();

	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputData(bf->GetOutput());
	normals->ComputePointNormalsOn();
	normals->ComputeCellNormalsOn();
	normals->SetFeatureAngle(20);
	normals->SplittingOn();
	normals->Update();

	MITK_WARN << "vtkbool time: " << (clock() - start);

	if (normals->GetOutput()->GetNumberOfCells() > 0)
	{
		auto newNode = mitk::DataNode::New();
		auto newSurface = mitk::Surface::New();
		newSurface->SetVtkPolyData(normals->GetOutput());
		newNode->SetName(inputSurfaceNode_a->GetName() + "_union");
		newNode->SetData(newSurface);
		GetDataStorage()->Add(newNode, inputSurfaceNode_a);
	}

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

int MoveData::on_pushButton_diff_clicked()
{
	auto inputSurfaceNode_a = m_Controls.mitkNodeSelectWidget_surfaceboolA->GetSelectedNode();
	if (inputSurfaceNode_a == nullptr)
	{
		return 2;
	}
	auto inputSurface_a = dynamic_cast<mitk::Surface*>(inputSurfaceNode_a->GetData());

	auto inputSurfaceNode_b = m_Controls.mitkNodeSelectWidget_surfaceboolB->GetSelectedNode();
	if (inputSurfaceNode_b == nullptr)
	{
		return 2;
	}
	auto inputSurface_b = dynamic_cast<mitk::Surface*>(inputSurfaceNode_b->GetData());

	auto inputpolyData_a = inputSurface_a->GetVtkPolyData();
	auto inputMatrix_a = inputSurface_a->GetGeometry()->GetVtkMatrix();
	auto trans_a = vtkTransform::New();
	trans_a->SetMatrix(inputMatrix_a);

	auto transFilter_a = vtkTransformPolyDataFilter::New();
	transFilter_a->SetTransform(trans_a);
	transFilter_a->SetInputData(inputpolyData_a);
	transFilter_a->Update();

	auto movedPolyData_a = transFilter_a->GetOutput();

	auto inputpolyData_b = inputSurface_b->GetVtkPolyData();
	auto inputMatrix_b = inputSurface_b->GetGeometry()->GetVtkMatrix();
	auto trans_b = vtkTransform::New();
	trans_b->SetMatrix(inputMatrix_b);

	auto transFilter_b = vtkTransformPolyDataFilter::New();
	transFilter_b->SetTransform(trans_b);
	transFilter_b->SetInputData(inputpolyData_b);
	transFilter_b->Update();

	auto movedPolyData_b = transFilter_b->GetOutput();

	clock_t start = clock();

	auto bf = vtkSmartPointer<vtkPolyDataBooleanFilter>::New();
	bf->SetInputData(0, movedPolyData_a);
	bf->SetInputData(1, movedPolyData_b);
	bf->SetOperModeToDifference();
	bf->Update();

	// m_Controls.textBrowser_moveData->append("Diff: "+ QString::number(bf->CheckHasContact()));

	if(bf->CheckHasContact() == 0)
	{
		// m_Controls.textBrowser_moveData->append("Diff: no contact");
		return 1;
	}

	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputData(bf->GetOutput());
	normals->ComputePointNormalsOn();
	normals->ComputeCellNormalsOn();
	normals->SetFeatureAngle(20);
	normals->SplittingOn();
	normals->Update();

	MITK_WARN << "vtkbool time: " << (clock() - start);

	if (normals->GetOutput()->GetNumberOfCells() > 0)
	{
		auto newNode = mitk::DataNode::New();
		auto newSurface = mitk::Surface::New();
		newSurface->SetVtkPolyData(normals->GetOutput());
		newNode->SetName(inputSurfaceNode_a->GetName() + "_difference");
		newNode->SetData(newSurface);
		GetDataStorage()->Add(newNode, inputSurfaceNode_a);

		mitk::RenderingManager::GetInstance()->RequestUpdateAll();

		return 0;
	}

	return 3;
}

int MoveData::on_pushButton_implicitClip_clicked()
{
	auto inputSurfaceNode_a = m_Controls.mitkNodeSelectWidget_surfaceboolA->GetSelectedNode();
	if (inputSurfaceNode_a == nullptr)
	{
		// m_Controls.textBrowser_moveData->append("Clip: input missing");
		return 2;
	}
	auto inputSurface_a = dynamic_cast<mitk::Surface*>(inputSurfaceNode_a->GetData());

	auto inputSurfaceNode_b = m_Controls.mitkNodeSelectWidget_surfaceboolB->GetSelectedNode();
	if (inputSurfaceNode_b == nullptr)
	{
		// m_Controls.textBrowser_moveData->append("Clip: input missing");
		return 2;
	}
	auto inputSurface_b = dynamic_cast<mitk::Surface*>(inputSurfaceNode_b->GetData());

	auto inputpolyData_a = inputSurface_a->GetVtkPolyData();
	auto inputMatrix_a = inputSurface_a->GetGeometry()->GetVtkMatrix();
	auto trans_a = vtkTransform::New();
	trans_a->SetMatrix(inputMatrix_a);

	auto transFilter_a = vtkTransformPolyDataFilter::New();
	transFilter_a->SetTransform(trans_a);
	transFilter_a->SetInputData(inputpolyData_a);
	transFilter_a->Update();

	auto movedPolyData_a = transFilter_a->GetOutput();

	auto inputpolyData_b = inputSurface_b->GetVtkPolyData();
	auto inputMatrix_b = inputSurface_b->GetGeometry()->GetVtkMatrix();
	auto trans_b = vtkTransform::New();
	trans_b->SetMatrix(inputMatrix_b);

	auto transFilter_b = vtkTransformPolyDataFilter::New();
	transFilter_b->SetTransform(trans_b);
	transFilter_b->SetInputData(inputpolyData_b);
	transFilter_b->Update();

	auto movedPolyData_b = transFilter_b->GetOutput();

	clock_t start = clock();

	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance;
	implicitPolyDataDistance->SetInput(movedPolyData_b);

	vtkNew<vtkClipPolyData> clipper;
	clipper->SetInputData(movedPolyData_a);
	clipper->GenerateClippedOutputOn();
	clipper->SetClipFunction(implicitPolyDataDistance);
	

	clipper->Update();
	vtkNew<vtkPolyData> tmpOutput;
	tmpOutput->DeepCopy(clipper->GetOutput());

	clock_t end = clock();

	auto newNode = mitk::DataNode::New();
	auto newSurface = mitk::Surface::New();
	newSurface->SetVtkPolyData(tmpOutput);
	// newSurface->SetVtkPolyData(clipper->GetClippedOutput());
	newNode->SetName(inputSurfaceNode_a->GetName() + "_clipped");
	newNode->SetData(newSurface);
	GetDataStorage()->Add(newNode, inputSurfaceNode_a);

	if (clipper->GetClippedOutput()->GetNumberOfCells() > 0)
	{
		mitk::RenderingManager::GetInstance()->RequestUpdateAll();
		//m_Controls.textBrowser_moveData->append("Clip: has contact and clipped something");
		m_Controls.textBrowser_moveData->append("Clipping time: " + QString::number(end - start));

		return 0;
	}

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	//m_Controls.textBrowser_moveData->append("Clip: has no contact");

	
	return 1;

}



void MoveData::on_pushButton_vtkBool_clicked()
{
	if (GetDataStorage()->GetNamedNode("bone") == nullptr || GetDataStorage()->GetNamedNode("saw") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("bone or saw is missing");
		return;
	}

	// get the bone polyData
	auto bonePolyData = GetDataStorage()->GetNamedObject<mitk::Surface>("bone")->GetVtkPolyData();

	// vtkNew<vtkFillHolesFilter> holeFiller0;
	// holeFiller0->SetInputData(bonePolyData);
	// holeFiller0->SetHoleSize(10);
	// holeFiller0->Update();
	// vtkNew<vtkPolyData> bonePolyDataFix;
	// bonePolyDataFix->DeepCopy(holeFiller0->GetOutput());

	// vtkNew<vtkPolyDataNormals> normals_bone;
	// normals_bone->SetInputData(bonePolyDataFix);
	// normals_bone->ComputePointNormalsOn();
	// normals_bone->ComputeCellNormalsOn();
	// normals_bone->SetFeatureAngle(20);
	// normals_bone->FlipNormalsOn();
	// normals_bone->SplittingOn();
	// normals_bone->Update();




	// get the saw polyData
	auto sawpolyData = GetDataStorage()->GetNamedObject<mitk::Surface>("saw")->GetVtkPolyData();
	auto sawMatrix = GetDataStorage()->GetNamedObject<mitk::Surface>("saw")->GetGeometry()->GetVtkMatrix();
	auto sawTrans = vtkTransform::New();
	sawTrans->SetMatrix(sawMatrix);

	// apply the transform
	auto tmpTransFilter = vtkTransformPolyDataFilter::New();
	tmpTransFilter->SetTransform(sawTrans);
	tmpTransFilter->SetInputData(sawpolyData);
	tmpTransFilter->Update();

	auto sawMovedPolyData = tmpTransFilter->GetOutput();

	clock_t start = clock();

	auto bf = vtkSmartPointer<vtkPolyDataBooleanFilter>::New();
	bf->SetInputData(0, bonePolyData);
	bf->SetInputData(1, sawMovedPolyData);
	bf->SetOperModeToDifference();
	bf->Update();

	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputData(bf->GetOutput());
	normals->ComputePointNormalsOn();
	normals->ComputeCellNormalsOn();
	normals->SetFeatureAngle(20);
	normals->SplittingOn();
	normals->Update();

	if(normals->GetOutput()->GetNumberOfCells() > 0)
	{
		GetDataStorage()->GetNamedObject<mitk::Surface>("bone")->SetVtkPolyData(normals->GetOutput());
	}

	MITK_WARN << "Test.Interface.time.TestClip" << (clock() - start);
}


void MoveData::on_pushButton_testClip_clicked()
{
	if(GetDataStorage()->GetNamedNode("bone") == nullptr || GetDataStorage()->GetNamedNode("saw") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("bone or saw is missing");
		return; 
	}

	// get the bone polyData
	auto bonePolyData = GetDataStorage()->GetNamedObject<mitk::Surface>("bone")->GetVtkPolyData();

	// get the saw polyData
	auto sawpolyData = GetDataStorage()->GetNamedObject<mitk::Surface>("saw")->GetVtkPolyData();
	auto sawMatrix = GetDataStorage()->GetNamedObject<mitk::Surface>("saw")->GetGeometry()->GetVtkMatrix();
	auto sawTrans = vtkTransform::New();
	sawTrans->SetMatrix(sawMatrix);

	// apply the transform
	auto tmpTransFilter = vtkTransformPolyDataFilter::New();
	tmpTransFilter->SetTransform(sawTrans);
	tmpTransFilter->SetInputData(sawpolyData);
	tmpTransFilter->Update();

	auto sawMovedPolyData = tmpTransFilter->GetOutput();

	// vtkNew<vtkClipClosedSurface> clipper;
	// clipper->SetInputData(sawMovedPolyData);
	// clipper->SetClippingPlanes()

	clock_t start = clock();
	

	// Implicit function that will be used to slice the mesh.
	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance;
	implicitPolyDataDistance->SetInput(sawMovedPolyData);

	vtkNew<vtkClipPolyData> clipper;
	clipper->SetInputData(bonePolyData);
	clipper->GenerateClippedOutputOn();
	clipper->SetClipFunction(implicitPolyDataDistance);

	clipper->Update();
	vtkNew<vtkPolyData> clippedOutput;
	clippedOutput->DeepCopy(clipper->GetOutput());
	
	GetDataStorage()->GetNamedObject<mitk::Surface>("bone")->SetVtkPolyData(clippedOutput);

	MITK_WARN << "Test.Interface.time.TestClip" << (clock() - start);
}


void MoveData::on_pushButto_interpolation_clicked()
{
	if(GetDataStorage()->GetNamedNode("pros") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("pros is missing");
	}

	mitk::VtkInterpolationProperty::Pointer interpolationProp0;
	GetDataStorage()->GetNamedNode("pros")->GetProperty(interpolationProp0, "material.interpolation");
	interpolationProp0->SetInterpolationToFlat();
	GetDataStorage()->GetNamedNode("pros")->SetProperty("material.interpolation", interpolationProp0);

	
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}


bool MoveData::GeneratePlaneWithPset(mitk::PointSet::Pointer ptsOnPlane, double planeSize, vtkSmartPointer<vtkPolyData> generatedPlane, double planeNormal[3])
{
	if(ptsOnPlane->GetSize() < 3 || planeSize <= 0)
	{
		return false;
	}
	
	// auto psetCenter = ptsOnPlane->GetGeometry()->GetCenter();
	//
	// double planeCenter[3]{ psetCenter[0],psetCenter[1],psetCenter[2] };

	double planeCenter[3]{0};

	Eigen::Vector3d l1{};

	 for(int i{0}; i < ptsOnPlane->GetSize(); i++)
	 {
	 	planeCenter[0] += ptsOnPlane->GetPoint(i)[0];
	 	planeCenter[1] += ptsOnPlane->GetPoint(i)[1];
	 	planeCenter[2] += ptsOnPlane->GetPoint(i)[2];
	 }
	
	 planeCenter[0] = planeCenter[0] / ptsOnPlane->GetSize();
	 planeCenter[1] = planeCenter[1] / ptsOnPlane->GetSize();
	 planeCenter[2] = planeCenter[2] / ptsOnPlane->GetSize();


	// Get the planeNormal
	Eigen::Vector3d v0{ ptsOnPlane->GetPoint(0)[0], ptsOnPlane->GetPoint(0)[1],ptsOnPlane->GetPoint(0)[2] };
	Eigen::Vector3d v1{ ptsOnPlane->GetPoint(1)[0], ptsOnPlane->GetPoint(1)[1],ptsOnPlane->GetPoint(1)[2] };
	Eigen::Vector3d v2{ ptsOnPlane->GetPoint(2)[0], ptsOnPlane->GetPoint(2)[1],ptsOnPlane->GetPoint(2)[2] };

	Eigen::Vector3d normal = (v0 - v1).cross(v0 - v2);
	normal.normalize();

	planeNormal[0] = normal[0];
	planeNormal[1] = normal[1];
	planeNormal[2] = normal[2];

	// Generate the plane vtkPolyData
	auto planeSource = vtkPlaneSource::New();
	planeSource->SetOrigin(0, 0, 0);
	planeSource->SetPoint1(planeSize, 0, 0);
	planeSource->SetPoint2(0, planeSize, 0);
	planeSource->SetCenter(planeCenter);
	planeSource->SetNormal(planeNormal);

	planeSource->Update();

	// Triangulate the plane if not vtk plane-plane intersection filter will fail
	vtkNew<vtkTriangleFilter> triangleFilter;
	triangleFilter->SetInputData(planeSource->GetOutput());
	triangleFilter->Update();

	generatedPlane->DeepCopy(triangleFilter->GetOutput());

	return true;
}


void MoveData::on_pushButton_testIntersect_clicked()
{
	if(GetDataStorage()->GetNamedNode("PointSet") == nullptr || GetDataStorage()->GetNamedNode("pros") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("PointSet or pros is missing!");
		return;
	}

	
	auto inputPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("PointSet");
	auto pset0 = mitk::PointSet::New();
	auto pset1 = mitk::PointSet::New();
	auto pset2 = mitk::PointSet::New();
	auto pset3 = mitk::PointSet::New();
	auto pset4 = mitk::PointSet::New();

	if(inputPset->GetSize() != 20)
	{
		m_Controls.textBrowser_moveData->append("PointSet is wrong!");
		return;
	}

	for (int i{0}; i < 4; i++)
	{
		pset0->InsertPoint(inputPset->GetPoint(i));
		pset1->InsertPoint(inputPset->GetPoint(i+4));
		pset2->InsertPoint(inputPset->GetPoint(i+8));
		pset3->InsertPoint(inputPset->GetPoint(i+12));
		pset4->InsertPoint(inputPset->GetPoint(i+16));
	}

	auto node0 = mitk::DataNode::New(); node0->SetData(pset0); node0->SetName("anterior");
	auto node1 = mitk::DataNode::New(); node1->SetData(pset1); node1->SetName("anteriorChamfer");
	auto node2 = mitk::DataNode::New(); node2->SetData(pset2); node2->SetName("distal");
	auto node3 = mitk::DataNode::New(); node3->SetData(pset3); node3->SetName("posteriorChamfer");
	auto node4 = mitk::DataNode::New(); node4->SetData(pset4); node4->SetName("posterior");

	GetDataStorage()->Add(node0);
	GetDataStorage()->Add(node1);
	GetDataStorage()->Add(node2);
	GetDataStorage()->Add(node3);
	GetDataStorage()->Add(node4);

	// Create the anterior plane surface
	auto anteriorPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("anterior");
	auto anteriorPlane = vtkPolyData::New();
	double anteriorNormal[3];
	
	if(GeneratePlaneWithPset(anteriorPset, 100, anteriorPlane, anteriorNormal) == false)
	{
		MITK_ERROR << "anterior Pset is problematic";
		return;
	}

	auto anteriorSurface = mitk::Surface::New();
	anteriorSurface->SetVtkPolyData(anteriorPlane);
	
	auto anteriorNode = mitk::DataNode::New();
	anteriorNode->SetData(anteriorSurface);
	anteriorNode->SetName("AnteriorCutPlane");
	GetDataStorage()->Add(anteriorNode);

	// Create the anteriorChamfer plane surface
	auto anteriorChamferPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("anteriorChamfer");
	auto anteriorChamferPlane = vtkPolyData::New();
	double anteriorChamferNormal[3];

	if (GeneratePlaneWithPset(anteriorChamferPset, 100, anteriorChamferPlane, anteriorChamferNormal) == false)
	{
		MITK_ERROR << "anteriorChamfer Pset is problematic";
		return;
	}

	auto anteriorChamferSurface = mitk::Surface::New();
	anteriorChamferSurface->SetVtkPolyData(anteriorChamferPlane);

	auto anteriorChamferNode = mitk::DataNode::New();
	anteriorChamferNode->SetData(anteriorChamferSurface);
	anteriorChamferNode->SetName("AnteriorChamferCutPlane");
	GetDataStorage()->Add(anteriorChamferNode);

	// Create the posterior plane surface
	auto posteriorPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("posterior");
	auto posteriorPlane = vtkPolyData::New();
	double posteriorNormal[3];

	if (GeneratePlaneWithPset(posteriorPset, 100, posteriorPlane, posteriorNormal) == false)
	{
		MITK_ERROR << "posterior Pset is problematic";
		return;
	}

	auto posteriorSurface = mitk::Surface::New();
	posteriorSurface->SetVtkPolyData(posteriorPlane);

	auto posteriorNode = mitk::DataNode::New();
	posteriorNode->SetData(posteriorSurface);
	posteriorNode->SetName("PosteriorCutPlane");
	GetDataStorage()->Add(posteriorNode);

	// Create the posteriorChamfer plane surface
	auto posteriorChamferPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("posteriorChamfer");
	auto posteriorChamferPlane = vtkPolyData::New();
	double posteriorChamferNormal[3];

	if (GeneratePlaneWithPset(posteriorChamferPset, 100, posteriorChamferPlane, posteriorChamferNormal) == false)
	{
		MITK_ERROR << "posteriorChamfer Pset is problematic";
		return;
	}

	auto posteriorChamferSurface = mitk::Surface::New();
	posteriorChamferSurface->SetVtkPolyData(posteriorChamferPlane);

	auto posteriorChamferNode = mitk::DataNode::New();
	posteriorChamferNode->SetData(posteriorChamferSurface);
	posteriorChamferNode->SetName("PosteriorChamferCutPlane");
	GetDataStorage()->Add(posteriorChamferNode);

	// Create the distal plane surface
	auto distalPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("distal");
	auto distalPlane = vtkPolyData::New();
	double distalNormal[3];

	if (GeneratePlaneWithPset(distalPset, 100, distalPlane, distalNormal) == false)
	{
		MITK_ERROR << "distal Pset is problematic";
		return;
	}

	auto distalSurface = mitk::Surface::New();
	distalSurface->SetVtkPolyData(distalPlane);

	auto distalNode = mitk::DataNode::New();
	distalNode->SetData(distalSurface);
	distalNode->SetName("DistalCutPlane");
	GetDataStorage()->Add(distalNode);

	vtkNew<vtkPlane> plane_anterior_0;
	plane_anterior_0->SetOrigin(GetDataStorage()->GetNamedObject<mitk::Surface>("AnteriorCutPlane")->GetVtkPolyData()->GetCenter());
	plane_anterior_0->SetNormal(anteriorNormal);

	vtkNew<vtkPlane> plane_anteriorChamfer_0;
	plane_anteriorChamfer_0->SetOrigin(GetDataStorage()->GetNamedObject<mitk::Surface>("AnteriorChamferCutPlane")->GetVtkPolyData()->GetCenter());
	plane_anteriorChamfer_0->SetNormal(anteriorChamferNormal);

	vtkNew<vtkPlane> plane_distal_0;
	plane_distal_0->SetOrigin(GetDataStorage()->GetNamedObject<mitk::Surface>("DistalCutPlane")->GetVtkPolyData()->GetCenter());
	plane_distal_0->SetNormal(distalNormal);

	vtkNew<vtkPlane> plane_posterior_0;
	plane_posterior_0->SetOrigin(GetDataStorage()->GetNamedObject<mitk::Surface>("PosteriorCutPlane")->GetVtkPolyData()->GetCenter());
	plane_posterior_0->SetNormal(posteriorNormal);

	vtkNew<vtkPlane> plane_posteriorChamfer_0;
	plane_posteriorChamfer_0->SetOrigin(GetDataStorage()->GetNamedObject<mitk::Surface>("PosteriorChamferCutPlane")->GetVtkPolyData()->GetCenter());
	plane_posteriorChamfer_0->SetNormal(posteriorChamferNormal);

	// posteriorChamfer & distal intersection line
	// vtkNew<vtkIntersectionPolyDataFilter> intersectionFilter1;
	// intersectionFilter1->SetInputData(0,distalPlane);
	// intersectionFilter1->SetInputData(1,posteriorChamferPlane);

	vtkNew<vtkCutter> intersectionFilter1;
	intersectionFilter1->SetCutFunction(plane_distal_0);
	intersectionFilter1->SetInputData(posteriorChamferPlane);

	intersectionFilter1->Update();

	// vtkNew<vtkCutter> cutter_anterior_0;
	// cutter_anterior_0->SetCutFunction(plane_anterior);
	// cutter_anterior_0->SetInputData(warp_0->GetPolyDataOutput());
	// cutter_anterior_0->Update();


	auto intersect_po = mitk::Surface::New();
	intersect_po->SetVtkPolyData(intersectionFilter1->GetOutput());

	auto intersectNode_po = mitk::DataNode::New();
	intersectNode_po->SetData(intersect_po);
	intersectNode_po->SetName("intersect_po");
	intersectNode_po->SetColor(1, 0, 0);
	GetDataStorage()->Add(intersectNode_po);

	auto intersect_po_data = intersectionFilter1->GetOutput();
	int pnum = intersect_po_data->GetNumberOfPoints();
	auto intersectPoints_po = mitk::PointSet::New();
	for(int i{0}; i < pnum; i++)
	{
		mitk::Point3D a;
		a[0] = intersect_po_data->GetPoint(i)[0];
		a[1] = intersect_po_data->GetPoint(i)[1];
		a[2] = intersect_po_data->GetPoint(i)[2];

		intersectPoints_po->InsertPoint(a);
	}

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetData(intersectPoints_po);
	tmpNode->SetName("intersectPoints_po");
	tmpNode->SetColor(1, 0, 0);
	GetDataStorage()->Add(tmpNode);

	//return;

	auto distalPlaneCenter = distalSurface->GetGeometry()->GetCenter();
	auto posteriorPlaneCenter = posteriorSurface->GetGeometry()->GetCenter();

	Eigen::Vector3d helperVector{distalPlaneCenter[0]-posteriorPlaneCenter[0],
	distalPlaneCenter[1] - posteriorPlaneCenter[1],
	distalPlaneCenter[2] - posteriorPlaneCenter[2]};

	Eigen::Vector3d distalNormalVec{distalNormal[0],
	distalNormal[1],
	distalNormal[2]};

	if(distalNormalVec.dot(helperVector) < 0)
	{
		distalNormalVec = -distalNormalVec;
	}

	// Get 2 points, the line between which definitely crosses the implant
	Eigen::Vector3d tmpVec{
		intersectPoints_po->GetPoint(0)[0] - intersectPoints_po->GetPoint(1)[0],
		intersectPoints_po->GetPoint(0)[1] - intersectPoints_po->GetPoint(1)[1],
		intersectPoints_po->GetPoint(0)[2] - intersectPoints_po->GetPoint(1)[2],
	};

	tmpVec.normalize();

	double lineP1[3]{ intersectPoints_po->GetPoint(0)[0]-distalNormalVec[0]*2 + tmpVec[0]*20000
		, intersectPoints_po->GetPoint(0)[1] - distalNormalVec[1] * 2 + tmpVec[1] * 20000,
		intersectPoints_po->GetPoint(0)[2] - distalNormalVec[2] * 2 + tmpVec[2] * 20000 };
	double lineP2[3]{ intersectPoints_po->GetPoint(1)[0] - distalNormalVec[0] * 2 - tmpVec[0] * 20000
		, intersectPoints_po->GetPoint(1)[1] - distalNormalVec[1] * 2 - tmpVec[1] * 20000,
		intersectPoints_po->GetPoint(1)[2] - distalNormalVec[2] * 2 - tmpVec[2] * 20000 };
	//
	// Create the locator

	if(GetDataStorage()->GetNamedNode("pros") == nullptr)
	{
		MITK_ERROR << "pros is missing";
		return;
	}

	vtkSmartPointer<vtkOBBTree> tree =
		vtkSmartPointer<vtkOBBTree>::New();
	tree->SetDataSet(GetDataStorage()->GetNamedObject<mitk::Surface>("pros")->GetVtkPolyData());
	tree->BuildLocator();
	
	vtkSmartPointer<vtkPoints> intersectPoints =
		vtkSmartPointer<vtkPoints>::New();
	
	tree->IntersectWithLine(lineP1, lineP2, intersectPoints, NULL);
	
	auto Pset = mitk::PointSet::New();
	
	int intersectNum = intersectPoints->GetNumberOfPoints();
	
	for(int i{0}; i < intersectNum; i++)
	{
		mitk::Point3D tmpPoint;
		tmpPoint[0] = intersectPoints->GetPoint(i)[0] + distalNormalVec[0] * 2;
		tmpPoint[1] = intersectPoints->GetPoint(i)[1] + distalNormalVec[1] * 2;
		tmpPoint[2] = intersectPoints->GetPoint(i)[2] + distalNormalVec[2] * 2;
	
		Pset->InsertPoint(tmpPoint);
	}
	Pset->Update();
	
	auto intersectNode = mitk::DataNode::New();
	intersectNode->SetData(Pset);
	intersectNode->SetName("intersect_implants_po");
	GetDataStorage()->Add(intersectNode);

	//return;

	// anteriorChamfer & distal intersection line
	vtkSmartPointer<vtkIntersectionPolyDataFilter> intersectionFilter = vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
	intersectionFilter->SetInputData(0, distalPlane);
	intersectionFilter->SetInputData(1, anteriorChamferPlane);

	intersectionFilter->Update();

	auto intersect_an = mitk::Surface::New();
	intersect_an->SetVtkPolyData(intersectionFilter->GetOutput());

	auto intersectNode_an = mitk::DataNode::New();
	intersectNode_an->SetData(intersect_an);
	intersectNode_an->SetName("intersect_an");
	intersectNode_an->SetColor(1, 0, 0);
	GetDataStorage()->Add(intersectNode_an);

	auto intersect_an_data = intersectionFilter->GetOutput();
	int pnum_an = intersect_an_data->GetNumberOfPoints();
	auto intersectPoints_an = mitk::PointSet::New();
	for (int i{ 0 }; i < pnum; i++)
	{
		mitk::Point3D a;
		a[0] = intersect_an_data->GetPoint(i)[0];
		a[1] = intersect_an_data->GetPoint(i)[1];
		a[2] = intersect_an_data->GetPoint(i)[2];

		intersectPoints_an->InsertPoint(a);
	}

	auto tmpNode1 = mitk::DataNode::New();
	tmpNode1->SetData(intersectPoints_an);
	tmpNode1->SetName("intersectPoints_an");
	tmpNode1->SetColor(1, 0, 0);
	GetDataStorage()->Add(tmpNode1);
	
	// Get 2 points, the line between which definitely crosses the implant
	Eigen::Vector3d tmpVec1{
		intersectPoints_an->GetPoint(0)[0] - intersectPoints_an->GetPoint(1)[0],
		intersectPoints_an->GetPoint(0)[1] - intersectPoints_an->GetPoint(1)[1],
		intersectPoints_an->GetPoint(0)[2] - intersectPoints_an->GetPoint(1)[2],
	};

	tmpVec1.normalize();

	double lineP3[3]{ intersectPoints_an->GetPoint(0)[0] - distalNormalVec[0] * 2 + tmpVec1[0] * 20000
		, intersectPoints_an->GetPoint(0)[1] - distalNormalVec[1] * 2 + tmpVec1[1] * 20000,
		intersectPoints_an->GetPoint(0)[2] - distalNormalVec[2] * 2 + tmpVec1[2] * 20000 };
	double lineP4[3]{ intersectPoints_an->GetPoint(1)[0] - distalNormalVec[0] * 2 - tmpVec1[0] * 20000
		, intersectPoints_an->GetPoint(1)[1] - distalNormalVec[1] * 2 - tmpVec1[1] * 20000,
		intersectPoints_an->GetPoint(1)[2] - distalNormalVec[2] * 2 - tmpVec1[2] * 20000 };
	//
	// Create the locator
	vtkSmartPointer<vtkOBBTree> tree1 =
		vtkSmartPointer<vtkOBBTree>::New();
	tree1->SetDataSet(GetDataStorage()->GetNamedObject<mitk::Surface>("pros")->GetVtkPolyData());
	tree1->BuildLocator();

	vtkSmartPointer<vtkPoints> intersectPoints_tmp =
		vtkSmartPointer<vtkPoints>::New();

	tree->IntersectWithLine(lineP3, lineP4, intersectPoints_tmp, NULL);

	auto Pset1 = mitk::PointSet::New();

	int intersectNum1 = intersectPoints_tmp->GetNumberOfPoints();

	for (int i{ 0 }; i < intersectNum1; i++)
	{
		mitk::Point3D tmpPoint;
		tmpPoint[0] = intersectPoints_tmp->GetPoint(i)[0] + distalNormalVec[0] * 2;
		tmpPoint[1] = intersectPoints_tmp->GetPoint(i)[1] + distalNormalVec[1] * 2;
		tmpPoint[2] = intersectPoints_tmp->GetPoint(i)[2] + distalNormalVec[2] * 2;

		Pset1->InsertPoint(tmpPoint);
	}
	Pset1->Update();

	auto intersectNode1 = mitk::DataNode::New();
	intersectNode1->SetData(Pset1);
	intersectNode1->SetName("intersect_implants_an");
	GetDataStorage()->Add(intersectNode1);

	// Calculate the femur prosthesis frame 
	auto anCenter = Pset1->GetGeometry()->GetCenter();
	auto poCenter = Pset->GetGeometry()->GetCenter();


	// frame origin
	Eigen::Vector3d frameOrigin{ (anCenter[0] + poCenter[0])/2.0,
	(anCenter[1] + poCenter[1]) / 2.0,
	(anCenter[2] + poCenter[2]) / 2.0 };

	Eigen::Vector3d frame_y{ (anCenter[0] - poCenter[0]),
	(anCenter[1] - poCenter[1]),
	(anCenter[2] - poCenter[2])};
	frame_y.normalize();

	Eigen::Vector3d frame_z = -distalNormalVec;

	Eigen::Vector3d frame_x = frame_y.cross(frame_z);

	auto offsetMatrix = vtkMatrix4x4::New();
	offsetMatrix->Identity();
	for(int i{0}; i<3; i++)
	{
		offsetMatrix->SetElement(i, 0, frame_x[i]);
		offsetMatrix->SetElement(i, 1, frame_y[i]);
		offsetMatrix->SetElement(i, 2, frame_z[i]);
		offsetMatrix->SetElement(i, 3, frameOrigin[i]);
	}

	offsetMatrix->Invert(offsetMatrix, offsetMatrix);
	// GetDataStorage()->GetNamedNode("pros")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(offsetMatrix);
	// GetDataStorage()->GetNamedNode("pros")->GetData()->Update();

	auto polyTrans = vtkTransform::New();
	auto polyTransFilter = vtkTransformPolyDataFilter::New();
	polyTrans->Identity();
	polyTrans->SetMatrix(offsetMatrix);
	polyTrans->Update();
	polyTransFilter->SetTransform(polyTrans);
	polyTransFilter->SetInputData(GetDataStorage()->GetNamedObject<mitk::Surface>("pros")->GetVtkPolyData());
	polyTransFilter->Update();
	GetDataStorage()->GetNamedObject<mitk::Surface>("pros")->SetVtkPolyData(polyTransFilter->GetOutput());

	GetDataStorage()->GetNamedNode("anteriorChamfer")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(offsetMatrix);
	GetDataStorage()->GetNamedNode("anteriorChamfer")->Update();
	GetDataStorage()->GetNamedNode("posterior")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(offsetMatrix);
	GetDataStorage()->GetNamedNode("posterior")->GetData()->Update();
	GetDataStorage()->GetNamedNode("posteriorChamfer")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(offsetMatrix);
	GetDataStorage()->GetNamedNode("posteriorChamfer")->GetData()->Update();
	GetDataStorage()->GetNamedNode("distal")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(offsetMatrix);
	GetDataStorage()->GetNamedNode("distal")->GetData()->Update();
	GetDataStorage()->GetNamedNode("anterior")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(offsetMatrix);
	GetDataStorage()->GetNamedNode("anterior")->GetData()->Update();

	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("intersect_implants_an"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("intersectPoints_an"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("intersect_an"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("intersect_implants_po"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("intersectPoints_po"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("intersect_po"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("DistalCutPlane"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("PosteriorChamferCutPlane"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("PosteriorCutPlane"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("AnteriorChamferCutPlane"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("PosteriorCutPlane"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("AnteriorCutPlane"));


	// Generate yz plane
	auto planeSource = vtkPlaneSource::New();
	planeSource->SetOrigin(0, 0, 0);
	planeSource->SetPoint1(200, 0, 0);
	planeSource->SetPoint2(0, 200, 0);
	planeSource->SetCenter(0,0,0);
	planeSource->SetNormal(1,0,0);

	planeSource->Update();

	// Triangulate the plane if not vtk plane-plane intersection filter will fail
	vtkNew<vtkTriangleFilter> triangleFilter;
	triangleFilter->SetInputData(planeSource->GetOutput());
	triangleFilter->Update();

	auto yzPlane = triangleFilter->GetOutput();

	// Create the anterior plane surface
	
	if (GeneratePlaneWithPset(anteriorPset, 100, anteriorPlane, anteriorNormal) == false)
	{
		MITK_ERROR << "anterior Pset is problematic";
		return;
	}

	anteriorSurface->SetVtkPolyData(anteriorPlane);

	anteriorNode->SetData(anteriorSurface);
	anteriorNode->SetName("AnteriorCutPlane");
	GetDataStorage()->Add(anteriorNode,GetDataStorage()->GetNamedNode("pros"));

	// one point on the anterior plane and the plane's normal
	vtkSmartPointer<vtkIntersectionPolyDataFilter> intersectionFilter_ant = vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
	intersectionFilter_ant->SetInputData(0, yzPlane);
	intersectionFilter_ant->SetInputData(1, anteriorPlane);

	intersectionFilter_ant->Update();

	// auto intersect_ant = mitk::Surface::New();
	// intersect_ant->SetVtkPolyData(intersectionFilter_ant->GetOutput());
	//
	// auto intersectNode_ant = mitk::DataNode::New();
	// intersectNode_ant->SetData(intersect_ant);
	// intersectNode_ant->SetName("intersect_ant");
	// intersectNode_ant->SetColor(1, 0, 0);
	// GetDataStorage()->Add(intersectNode_ant);

	auto intersect_ant = intersectionFilter_ant->GetOutput();
	Eigen::Vector3d antPoint0{ intersect_ant ->GetPoints()->GetPoint(0)[0],
	intersect_ant->GetPoints()->GetPoint(0)[1] ,intersect_ant->GetPoints()->GetPoint(0)[2]};
	Eigen::Vector3d antPoint1{ intersect_ant->GetPoints()->GetPoint(1)[0],
	intersect_ant->GetPoints()->GetPoint(1)[1] ,intersect_ant->GetPoints()->GetPoint(1)[2] };
	Eigen::Vector3d antVec = (antPoint0 - antPoint1).normalized();

	auto antCenter = anteriorSurface->GetGeometry()->GetCenter();
	auto antIntersectCenter = intersect_ant->GetCenter();

	mitk::Point3D antSurfacePoint, antNormalVec;

	if((antCenter[0] - antIntersectCenter[0]) < 0.01 && (antCenter[1] - antIntersectCenter[1]) < 0.01 && (antCenter[2] - antIntersectCenter[2]) < 0.01)
	{
		antSurfacePoint[0]= antIntersectCenter[0] + antVec[0];
		antSurfacePoint[1] = antIntersectCenter[1] + antVec[1];
		antSurfacePoint[2] = antIntersectCenter[2] + antVec[2];
	}else
	{
		antSurfacePoint[0] = antIntersectCenter[0] ;
		antSurfacePoint[1] = antIntersectCenter[1] ;
		antSurfacePoint[2] = antIntersectCenter[2] ;
	}

	if(anteriorNormal[1] < 0)
	{
		antNormalVec[0] = -anteriorNormal[0];
		antNormalVec[1] = -anteriorNormal[1];
		antNormalVec[2] = -anteriorNormal[2];
	}else
	{
		antNormalVec[0] = anteriorNormal[0];
		antNormalVec[1] = anteriorNormal[1];
		antNormalVec[2] = anteriorNormal[2];
	}


	auto anteriorCut = mitk::PointSet::New();
	anteriorCut->InsertPoint(antSurfacePoint);
	anteriorCut->InsertPoint(antNormalVec);

	auto anteriorCutNode = mitk::DataNode::New();
	anteriorCutNode->SetData(anteriorCut);
	anteriorCutNode->SetName("AnteriorCut");
	GetDataStorage()->Add(anteriorCutNode, GetDataStorage()->GetNamedNode("AnteriorCutPlane"));

	// Create the anteriorChamfer plane surface


	if (GeneratePlaneWithPset(anteriorChamferPset, 100, anteriorChamferPlane, anteriorChamferNormal) == false)
	{
		MITK_ERROR << "anteriorChamfer Pset is problematic";
		return;
	}


	anteriorChamferSurface->SetVtkPolyData(anteriorChamferPlane);


	anteriorChamferNode->SetData(anteriorChamferSurface);
	anteriorChamferNode->SetName("AnteriorChamferCutPlane");
	GetDataStorage()->Add(anteriorChamferNode,GetDataStorage()->GetNamedNode("pros") );

	// one point on the anteriorChamfer and the plane's normal
	vtkSmartPointer<vtkIntersectionPolyDataFilter> intersectionFilter_antCh = vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
	intersectionFilter_antCh->SetInputData(0, yzPlane);
	intersectionFilter_antCh->SetInputData(1, anteriorChamferPlane);

	intersectionFilter_antCh->Update();

	// auto intersect_ant = mitk::Surface::New();
	// intersect_ant->SetVtkPolyData(intersectionFilter_ant->GetOutput());
	//
	// auto intersectNode_ant = mitk::DataNode::New();
	// intersectNode_ant->SetData(intersect_ant);
	// intersectNode_ant->SetName("intersect_ant");
	// intersectNode_ant->SetColor(1, 0, 0);
	// GetDataStorage()->Add(intersectNode_ant);

	auto intersect_antCh = intersectionFilter_antCh->GetOutput();
	Eigen::Vector3d antChPoint0{ intersect_antCh->GetPoints()->GetPoint(0)[0],
	intersect_antCh->GetPoints()->GetPoint(0)[1] ,intersect_antCh->GetPoints()->GetPoint(0)[2] };
	Eigen::Vector3d antChPoint1{ intersect_antCh->GetPoints()->GetPoint(1)[0],
	intersect_antCh->GetPoints()->GetPoint(1)[1] ,intersect_antCh->GetPoints()->GetPoint(1)[2] };
	Eigen::Vector3d antChVec = (antChPoint0 - antChPoint1).normalized();

	auto antChCenter = anteriorChamferSurface->GetGeometry()->GetCenter();
	auto antChIntersectCenter = intersect_antCh->GetCenter();

	mitk::Point3D antChSurfacePoint, antChNormalVec;

	if ((antChCenter[0] - antChIntersectCenter[0]) < 0.01 && (antChCenter[1] - antChIntersectCenter[1]) < 0.01 && (antChCenter[2] - antChIntersectCenter[2]) < 0.01)
	{
		antChSurfacePoint[0] = antChIntersectCenter[0] + antChVec[0];
		antChSurfacePoint[1] = antChIntersectCenter[1] + antChVec[1];
		antChSurfacePoint[2] = antChIntersectCenter[2] + antChVec[2];
	}
	else
	{
		antChSurfacePoint[0] = antChIntersectCenter[0];
		antChSurfacePoint[1] = antChIntersectCenter[1];
		antChSurfacePoint[2] = antChIntersectCenter[2];
	}

	if (anteriorChamferNormal[1] < 0)
	{
		antChNormalVec[0] = -anteriorChamferNormal[0];
		antChNormalVec[1] = -anteriorChamferNormal[1];
		antChNormalVec[2] = -anteriorChamferNormal[2];
	}
	else
	{
		antChNormalVec[0] = anteriorChamferNormal[0];
		antChNormalVec[1] = anteriorChamferNormal[1];
		antChNormalVec[2] = anteriorChamferNormal[2];
	}


	auto anteriorChCut = mitk::PointSet::New();
	anteriorChCut->InsertPoint(antChSurfacePoint);
	anteriorChCut->InsertPoint(antChNormalVec);

	auto anteriorChamferCutNode = mitk::DataNode::New();
	anteriorChamferCutNode->SetData(anteriorChCut);
	anteriorChamferCutNode->SetName("AnteriorChmferCut");
	GetDataStorage()->Add(anteriorChamferCutNode, GetDataStorage()->GetNamedNode("AnteriorChamferCutPlane"));

	// Create the posterior plane surface


	if (GeneratePlaneWithPset(posteriorPset, 100, posteriorPlane, posteriorNormal) == false)
	{
		MITK_ERROR << "posterior Pset is problematic";
		return;
	}


	posteriorSurface->SetVtkPolyData(posteriorPlane);


	posteriorNode->SetData(posteriorSurface);
	posteriorNode->SetName("PosteriorCutPlane");
	GetDataStorage()->Add(posteriorNode,GetDataStorage()->GetNamedNode("pros"));

	// one point on the posterior plane and the plane's normal
	vtkSmartPointer<vtkIntersectionPolyDataFilter> intersectionFilter_pos = vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
	intersectionFilter_pos->SetInputData(0, yzPlane);
	intersectionFilter_pos->SetInputData(1, posteriorPlane);

	intersectionFilter_pos->Update();

	// auto intersect_ant = mitk::Surface::New();
	// intersect_ant->SetVtkPolyData(intersectionFilter_ant->GetOutput());
	//
	// auto intersectNode_ant = mitk::DataNode::New();
	// intersectNode_ant->SetData(intersect_ant);
	// intersectNode_ant->SetName("intersect_ant");
	// intersectNode_ant->SetColor(1, 0, 0);
	// GetDataStorage()->Add(intersectNode_ant);

	auto intersect_pos = intersectionFilter_pos->GetOutput();
	Eigen::Vector3d posPoint0{ intersect_pos->GetPoints()->GetPoint(0)[0],
	intersect_pos->GetPoints()->GetPoint(0)[1] ,intersect_pos->GetPoints()->GetPoint(0)[2] };
	Eigen::Vector3d posPoint1{ intersect_pos->GetPoints()->GetPoint(1)[0],
	intersect_pos->GetPoints()->GetPoint(1)[1] ,intersect_pos->GetPoints()->GetPoint(1)[2] };
	Eigen::Vector3d posVec = (posPoint0 - posPoint1).normalized();

	auto posCenter = posteriorSurface->GetGeometry()->GetCenter();
	auto posIntersectCenter = intersect_pos->GetCenter();

	mitk::Point3D posSurfacePoint, posNormalVec;

	if ((posCenter[0] - posIntersectCenter[0]) < 0.01 && (posCenter[1] - posIntersectCenter[1]) < 0.01 && (posCenter[2] - posIntersectCenter[2]) < 0.01)
	{
		posSurfacePoint[0] = posIntersectCenter[0] + posVec[0];
		posSurfacePoint[1] = posIntersectCenter[1] + posVec[1];
		posSurfacePoint[2] = posIntersectCenter[2] + posVec[2];
	}
	else
	{
		posSurfacePoint[0] = posIntersectCenter[0];
		posSurfacePoint[1] = posIntersectCenter[1];
		posSurfacePoint[2] = posIntersectCenter[2];
	}

	if (posteriorNormal[1] > 0)
	{
		posNormalVec[0] = -posteriorNormal[0];
		posNormalVec[1] = -posteriorNormal[1];
		posNormalVec[2] = -posteriorNormal[2];
	}
	else
	{
		posNormalVec[0] = posteriorNormal[0];
		posNormalVec[1] = posteriorNormal[1];
		posNormalVec[2] = posteriorNormal[2];
	}


	auto posteriorCut = mitk::PointSet::New();
	posteriorCut->InsertPoint(posSurfacePoint);
	posteriorCut->InsertPoint(posNormalVec);

	auto posteriorCutNode = mitk::DataNode::New();
	posteriorCutNode->SetData(posteriorCut);
	posteriorCutNode->SetName("PosteriorCut");
	GetDataStorage()->Add(posteriorCutNode, GetDataStorage()->GetNamedNode("PosteriorCutPlane"));



	// Create the posteriorChamfer plane surface


	if (GeneratePlaneWithPset(posteriorChamferPset, 100, posteriorChamferPlane, posteriorChamferNormal) == false)
	{
		MITK_ERROR << "posteriorChamfer Pset is problematic";
		return;
	}


	posteriorChamferSurface->SetVtkPolyData(posteriorChamferPlane);


	posteriorChamferNode->SetData(posteriorChamferSurface);
	posteriorChamferNode->SetName("PosteriorChamferCutPlane");
	GetDataStorage()->Add(posteriorChamferNode,GetDataStorage()->GetNamedNode("pros") );


	// one point on the posterior plane and the plane's normal
	vtkSmartPointer<vtkIntersectionPolyDataFilter> intersectionFilter_posCh = vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
	intersectionFilter_posCh->SetInputData(0, yzPlane);
	intersectionFilter_posCh->SetInputData(1, posteriorChamferPlane);

	intersectionFilter_posCh->Update();

	// auto intersect_ant = mitk::Surface::New();
	// intersect_ant->SetVtkPolyData(intersectionFilter_ant->GetOutput());
	//
	// auto intersectNode_ant = mitk::DataNode::New();
	// intersectNode_ant->SetData(intersect_ant);
	// intersectNode_ant->SetName("intersect_ant");
	// intersectNode_ant->SetColor(1, 0, 0);
	// GetDataStorage()->Add(intersectNode_ant);

	auto intersect_posCh = intersectionFilter_posCh->GetOutput();
	Eigen::Vector3d posChPoint0{ intersect_posCh->GetPoints()->GetPoint(0)[0],
	intersect_posCh->GetPoints()->GetPoint(0)[1] ,intersect_posCh->GetPoints()->GetPoint(0)[2] };
	Eigen::Vector3d posChPoint1{ intersect_posCh->GetPoints()->GetPoint(1)[0],
	intersect_posCh->GetPoints()->GetPoint(1)[1] ,intersect_posCh->GetPoints()->GetPoint(1)[2] };
	Eigen::Vector3d posChVec = (posChPoint0 - posChPoint1).normalized();

	auto posChCenter = posteriorChamferSurface->GetGeometry()->GetCenter();
	auto posChIntersectCenter = intersect_posCh->GetCenter();

	mitk::Point3D posChSurfacePoint, posChNormalVec;

	if ((posChCenter[0] - posChIntersectCenter[0]) < 0.01 && (posChCenter[1] - posChIntersectCenter[1]) < 0.01 && (posChCenter[2] - posChIntersectCenter[2]) < 0.01)
	{
		posChSurfacePoint[0] = posChIntersectCenter[0] + posChVec[0];
		posChSurfacePoint[1] = posChIntersectCenter[1] + posChVec[1];
		posChSurfacePoint[2] = posChIntersectCenter[2] + posChVec[2];
	}
	else
	{
		posChSurfacePoint[0] = posChIntersectCenter[0];
		posChSurfacePoint[1] = posChIntersectCenter[1];
		posChSurfacePoint[2] = posChIntersectCenter[2];
	}

	if (posteriorChamferNormal[1] > 0)
	{
		posChNormalVec[0] = -posteriorChamferNormal[0];
		posChNormalVec[1] = -posteriorChamferNormal[1];
		posChNormalVec[2] = -posteriorChamferNormal[2];
	}
	else
	{
		posChNormalVec[0] = posteriorChamferNormal[0];
		posChNormalVec[1] = posteriorChamferNormal[1];
		posChNormalVec[2] = posteriorChamferNormal[2];
	}


	auto posteriorChamferCut = mitk::PointSet::New();
	posteriorChamferCut->InsertPoint(posChSurfacePoint);
	posteriorChamferCut->InsertPoint(posChNormalVec);

	auto posteriorChamferCutNode = mitk::DataNode::New();
	posteriorChamferCutNode->SetData(posteriorChamferCut);
	posteriorChamferCutNode->SetName("PosteriorChamferCut");
	GetDataStorage()->Add(posteriorChamferCutNode, GetDataStorage()->GetNamedNode("PosteriorChamferCutPlane"));



	// Create the distal plane surface
	

	if (GeneratePlaneWithPset(distalPset, 100, distalPlane, distalNormal) == false)
	{
		MITK_ERROR << "distal Pset is problematic";
		return;
	}

	mitk::Point3D disSurfacePoint, disNormalVec;
	disSurfacePoint[0] = 0;
	disSurfacePoint[1] = 0;
	disSurfacePoint[2] = 0;

	disNormalVec[0] = 0;
	disNormalVec[1] = 0;
	disNormalVec[2] = -1;

	distalSurface->SetVtkPolyData(distalPlane);

	if(abs(distalPlane->GetCenter()[0]) < 0.01 && abs(distalPlane->GetCenter()[1]) < 0.01 && abs(distalPlane->GetCenter()[2]) < 0.01)
	{
		auto tmpTrans = vtkTransform::New();
		auto tmpTransFilter = vtkTransformPolyDataFilter::New();
		tmpTrans->Identity();
		tmpTrans->Translate(1, 0, 0);
		tmpTrans->Update();
		tmpTransFilter->SetTransform(tmpTrans);
		tmpTransFilter->SetInputData(distalPlane);
		tmpTransFilter->Update();
		distalSurface->SetVtkPolyData(tmpTransFilter->GetOutput());
	}

	distalNode->SetData(distalSurface);
	distalNode->SetName("DistalCutPlane");
	GetDataStorage()->Add(distalNode, GetDataStorage()->GetNamedNode("pros"));

	auto distalCut = mitk::PointSet::New();
	distalCut->InsertPoint(disSurfacePoint);
	distalCut->InsertPoint(disNormalVec);

	auto disCutNode = mitk::DataNode::New();
	disCutNode->SetData(distalCut);
	disCutNode->SetName("DistalCut");
	GetDataStorage()->Add(disCutNode, GetDataStorage()->GetNamedNode("DistalCutPlane"));



	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("anteriorChamfer"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("anterior"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("distal"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("posteriorChamfer"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("posterior"));

	// Enlarge the prosthesis surface
	auto prosSurface = GetDataStorage()->GetNamedObject<mitk::Surface>("pros")->GetVtkPolyData();
	// Generate normals
	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputData(prosSurface);
	normals->SplittingOff();

	if(m_Controls.radioButton_invertNormal->isChecked())
	{
		normals->SetFlipNormals(true);
	}
	
	//normals->SplittingOn();

	normals->Update();

	// Warp using the normals
	vtkNew<vtkWarpVector> warp_0;
	warp_0->SetInputData(normals->GetOutput());
	warp_0->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS,
		vtkDataSetAttributes::NORMALS);
	warp_0->SetScaleFactor(3);
	warp_0->Update();

	auto warpedSurface_0 = mitk::Surface::New();
	warpedSurface_0->SetVtkPolyData(warp_0->GetPolyDataOutput());
	auto warpedNode_0 = mitk::DataNode::New();
	warpedNode_0->SetData(warpedSurface_0);
	warpedNode_0->SetColor(1, 1, 0);
	warpedNode_0->SetName("warped_3");
	//GetDataStorage()->Add(warpedNode_0);

	// Warp using the normals
	vtkNew<vtkWarpVector> warp_1;
	warp_1->SetInputData(normals->GetOutput());
	warp_1->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS,
		vtkDataSetAttributes::NORMALS);
	warp_1->SetScaleFactor(6);
	warp_1->Update();

	auto warpedSurface_1 = mitk::Surface::New();
	warpedSurface_1->SetVtkPolyData(warp_1->GetPolyDataOutput());
	auto warpedNode_1 = mitk::DataNode::New();
	warpedNode_1->SetData(warpedSurface_1);
	warpedNode_1->SetColor(1, 0, 0);
	warpedNode_1->SetName("warped_6");
	//GetDataStorage()->Add(warpedNode_1);

	vtkNew<vtkPlane> plane_anterior;
	plane_anterior->SetOrigin(GetDataStorage()->GetNamedObject<mitk::Surface>("AnteriorCutPlane")->GetVtkPolyData()->GetCenter());
	plane_anterior->SetNormal(anteriorNormal);

	vtkNew<vtkPlane> plane_anteriorChamfer;
	plane_anteriorChamfer->SetOrigin(GetDataStorage()->GetNamedObject<mitk::Surface>("AnteriorChamferCutPlane")->GetVtkPolyData()->GetCenter());
	plane_anteriorChamfer->SetNormal(anteriorChamferNormal);

	vtkNew<vtkPlane> plane_distal;
	plane_distal->SetOrigin(GetDataStorage()->GetNamedObject<mitk::Surface>("DistalCutPlane")->GetVtkPolyData()->GetCenter());
	plane_distal->SetNormal(distalNormal);

	vtkNew<vtkPlane> plane_posterior;
	plane_posterior->SetOrigin(GetDataStorage()->GetNamedObject<mitk::Surface>("PosteriorCutPlane")->GetVtkPolyData()->GetCenter());
	plane_posterior->SetNormal(posteriorNormal);

	vtkNew<vtkPlane> plane_posteriorChamfer;
	plane_posteriorChamfer->SetOrigin(GetDataStorage()->GetNamedObject<mitk::Surface>("PosteriorChamferCutPlane")->GetVtkPolyData()->GetCenter());
	plane_posteriorChamfer->SetNormal(posteriorChamferNormal);

	GetDataStorage()->GetNamedNode("AnteriorCutPlane")->SetColor(0, 0, 0.5);
	GetDataStorage()->GetNamedNode("AnteriorChamferCutPlane")->SetColor(0, 0, 0.5);
	GetDataStorage()->GetNamedNode("DistalCutPlane")->SetColor(0, 0, 0.5);
	GetDataStorage()->GetNamedNode("PosteriorCutPlane")->SetColor(0, 0, 0.5);
	GetDataStorage()->GetNamedNode("PosteriorChamferCutPlane")->SetColor(0, 0, 0.5);


	vtkNew<vtkCutter> cutter_anterior_0;
	cutter_anterior_0->SetCutFunction(plane_anterior);
	cutter_anterior_0->SetInputData(warp_0->GetPolyDataOutput());
	cutter_anterior_0->Update();

	auto warpAnteriorCut_0 = mitk::Surface::New();
    warpAnteriorCut_0->SetVtkPolyData(cutter_anterior_0->GetOutput());
    auto warpAnteriorCutNode_0 = mitk::DataNode::New();
    warpAnteriorCutNode_0->SetData(warpAnteriorCut_0);
    warpAnteriorCutNode_0->SetName("Anterior_b1");
    warpAnteriorCutNode_0->SetColor(0, 1, 0);
    GetDataStorage()->Add(warpAnteriorCutNode_0,GetDataStorage()->GetNamedNode("AnteriorCutPlane"));

	vtkNew<vtkCutter> cutter_anterior_1;
	cutter_anterior_1->SetCutFunction(plane_anterior);
	cutter_anterior_1->SetInputData(warp_1->GetPolyDataOutput());
	cutter_anterior_1->Update();

	auto warpAnteriorCut_1 = mitk::Surface::New();
	warpAnteriorCut_1->SetVtkPolyData(cutter_anterior_1->GetOutput());
	auto warpAnteriorCutNode_1 = mitk::DataNode::New();
	warpAnteriorCutNode_1->SetData(warpAnteriorCut_1);
	warpAnteriorCutNode_1->SetName("Anterior_b2");
	warpAnteriorCutNode_1->SetColor(1, 1, 1);
	GetDataStorage()->Add(warpAnteriorCutNode_1, GetDataStorage()->GetNamedNode("AnteriorCutPlane"));

	vtkNew<vtkCutter> cutter_posterior_0;
	cutter_posterior_0->SetCutFunction(plane_posterior);
	cutter_posterior_0->SetInputData(warp_0->GetPolyDataOutput());
	cutter_posterior_0->Update();

	auto warpPosteriorCut_0 = mitk::Surface::New();
	warpPosteriorCut_0->SetVtkPolyData(cutter_posterior_0->GetOutput());
	auto warpPosteriorCutNode_0 = mitk::DataNode::New();
	warpPosteriorCutNode_0->SetData(warpPosteriorCut_0);
	warpPosteriorCutNode_0->SetName("Posterior_b1");
	warpPosteriorCutNode_0->SetColor(0, 1, 0);
	GetDataStorage()->Add(warpPosteriorCutNode_0, GetDataStorage()->GetNamedNode("PosteriorCutPlane"));

	vtkNew<vtkCutter> cutter_posterior_1;
	cutter_posterior_1->SetCutFunction(plane_posterior);
	cutter_posterior_1->SetInputData(warp_1->GetPolyDataOutput());
	cutter_posterior_1->Update();

	auto warpPosteriorCut_1 = mitk::Surface::New();
	warpPosteriorCut_1->SetVtkPolyData(cutter_posterior_1->GetOutput());
	auto warpPosteriorCutNode_1 = mitk::DataNode::New();
	warpPosteriorCutNode_1->SetData(warpPosteriorCut_1);
	warpPosteriorCutNode_1->SetName("Posterior_b2");
	warpPosteriorCutNode_1->SetColor(1, 1, 1);
	GetDataStorage()->Add(warpPosteriorCutNode_1, GetDataStorage()->GetNamedNode("PosteriorCutPlane"));

	vtkNew<vtkCutter> cutter_posteriorChamfer_0;
	cutter_posteriorChamfer_0->SetCutFunction(plane_posteriorChamfer);
	cutter_posteriorChamfer_0->SetInputData(warp_0->GetPolyDataOutput());
	cutter_posteriorChamfer_0->Update();

	auto warpPosteriorChamferCut_0 = mitk::Surface::New();
	warpPosteriorChamferCut_0->SetVtkPolyData(cutter_posteriorChamfer_0->GetOutput());
	auto warpPosteriorChamferCutNode_0 = mitk::DataNode::New();
	warpPosteriorChamferCutNode_0->SetData(warpPosteriorChamferCut_0);
	warpPosteriorChamferCutNode_0->SetName("PosteriorChamfer_b1");
	warpPosteriorChamferCutNode_0->SetColor(0, 1, 0);
	GetDataStorage()->Add(warpPosteriorChamferCutNode_0, GetDataStorage()->GetNamedNode("PosteriorChamferCutPlane"));

	vtkNew<vtkCutter> cutter_posteriorChamfer_1;
	cutter_posteriorChamfer_1->SetCutFunction(plane_posteriorChamfer);
	cutter_posteriorChamfer_1->SetInputData(warp_1->GetPolyDataOutput());
	cutter_posteriorChamfer_1->Update();

	auto warpPosteriorChamferCut_1 = mitk::Surface::New();
	warpPosteriorChamferCut_1->SetVtkPolyData(cutter_posteriorChamfer_1->GetOutput());
	auto warpPosteriorChamferCutNode_1 = mitk::DataNode::New();
	warpPosteriorChamferCutNode_1->SetData(warpPosteriorChamferCut_1);
	warpPosteriorChamferCutNode_1->SetName("PosteriorChamfer_b2");
	warpPosteriorChamferCutNode_1->SetColor(1, 1, 1);
	GetDataStorage()->Add(warpPosteriorChamferCutNode_1, GetDataStorage()->GetNamedNode("PosteriorChamferCutPlane"));

	vtkNew<vtkCutter> cutter_anteriorChamfer_0;
	cutter_anteriorChamfer_0->SetCutFunction(plane_anteriorChamfer);
	cutter_anteriorChamfer_0->SetInputData(warp_0->GetPolyDataOutput());
	cutter_anteriorChamfer_0->Update();

	auto warpAnteriorChamferCut_0 = mitk::Surface::New();
	warpAnteriorChamferCut_0->SetVtkPolyData(cutter_anteriorChamfer_0->GetOutput());
	auto warpAnteriorChamferCutNode_0 = mitk::DataNode::New();
	warpAnteriorChamferCutNode_0->SetData(warpAnteriorChamferCut_0);
	warpAnteriorChamferCutNode_0->SetName("AnteriorChamfer_b1");
	warpAnteriorChamferCutNode_0->SetColor(0, 1, 0);
	GetDataStorage()->Add(warpAnteriorChamferCutNode_0, GetDataStorage()->GetNamedNode("AnteriorChamferCutPlane"));

	vtkNew<vtkCutter> cutter_anteriorChamfer_1;
	cutter_anteriorChamfer_1->SetCutFunction(plane_anteriorChamfer);
	cutter_anteriorChamfer_1->SetInputData(warp_1->GetPolyDataOutput());
	cutter_anteriorChamfer_1->Update();

	auto warpAnteriorChamferCut_1 = mitk::Surface::New();
	warpAnteriorChamferCut_1->SetVtkPolyData(cutter_anteriorChamfer_1->GetOutput());
	auto warpAnteriorChamferCutNode_1 = mitk::DataNode::New();
	warpAnteriorChamferCutNode_1->SetData(warpAnteriorChamferCut_1);
	warpAnteriorChamferCutNode_1->SetName("AnteriorChamfer_b2");
	warpAnteriorChamferCutNode_1->SetColor(1, 1, 1);
	GetDataStorage()->Add(warpAnteriorChamferCutNode_1, GetDataStorage()->GetNamedNode("AnteriorChamferCutPlane"));


	vtkNew<vtkCutter> cutter_distal_0;
	cutter_distal_0->SetCutFunction(plane_distal);
	cutter_distal_0->SetInputData(warp_0->GetPolyDataOutput());
	cutter_distal_0->Update();

	auto warpDistalCut_0 = mitk::Surface::New();
	warpDistalCut_0->SetVtkPolyData(cutter_distal_0->GetOutput());
	auto warpDistalCutNode_0 = mitk::DataNode::New();
	warpDistalCutNode_0->SetData(warpDistalCut_0);
	warpDistalCutNode_0->SetName("Distal_b1");
	warpDistalCutNode_0->SetColor(0, 1, 0);
	GetDataStorage()->Add(warpDistalCutNode_0, GetDataStorage()->GetNamedNode("DistalCutPlane"));

	vtkNew<vtkCutter> cutter_distal_1;
	cutter_distal_1->SetCutFunction(plane_distal);
	cutter_distal_1->SetInputData(warp_1->GetPolyDataOutput());
	cutter_distal_1->Update();

	auto warpDistalCut_1 = mitk::Surface::New();
	warpDistalCut_1->SetVtkPolyData(cutter_distal_1->GetOutput());
	auto warpDistalCutNode_1 = mitk::DataNode::New();
	warpDistalCutNode_1->SetData(warpDistalCut_1);
	warpDistalCutNode_1->SetName("Distal_b2");
	warpDistalCutNode_1->SetColor(1, 1, 1);
	GetDataStorage()->Add(warpDistalCutNode_1, GetDataStorage()->GetNamedNode("DistalCutPlane"));


	
	

}




void MoveData::on_pushButton_gen5planes_clicked()
{
	auto tmpPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("PointSet");

	auto p1 = tmpPset->GetPoint(0);
	auto p2 = tmpPset->GetPoint(1);
	auto p3 = tmpPset->GetPoint(2);

	Eigen::Vector3d v1;
	v1[0] = p1[0] - p2[0];
	v1[1] = p1[1] - p2[1];
	v1[2] = p1[2] - p2[2];

	Eigen::Vector3d v2; 
	v2[0] = p3[0] - p2[0];
	v2[1] = p3[1] - p2[1];
	v2[2] = p3[2] - p2[2];

	auto v3 = v1.cross(v2);
	v3.normalize();
	m_Controls.textBrowser_moveData->append("/X: " + QString::number(v3[0]) + "/Y: " + QString::number(v3[1]) + "/Z: " + QString::number(v3[2]));

	// Anterior
	double anteriorNormal[3]{-0.01, -0.5, -0.87};
	double anteriorCenter[3]{3034.19, 1804.66, -427.50};
	auto anteriorPlaneSource = vtkSmartPointer<vtkPlaneSource>::New();
	
	anteriorPlaneSource->SetOrigin(0, 0, 0);
	
	anteriorPlaneSource->SetPoint1(0, 70, 0);
	anteriorPlaneSource->SetPoint2(70, 0, 0);
	
	anteriorPlaneSource->SetNormal(anteriorNormal);
	
	anteriorPlaneSource->SetCenter(anteriorCenter);

	anteriorPlaneSource->Update();

	auto anterirorPlaneSurface = mitk::Surface::New();
	anterirorPlaneSurface->SetVtkPolyData(anteriorPlaneSource->GetOutput());

	auto anterirorPlaneNode = mitk::DataNode::New();
	anterirorPlaneNode->SetData(anterirorPlaneSurface);
	anterirorPlaneNode->SetName("anterirorPlane");
	GetDataStorage()->Add(anterirorPlaneNode);

	// Posterior
	double posteriorNormal[3]{ 0.01, 0.61, 0.79 };
	double posteriorCenter[3]{ 3025.34, 1766.80, -447.32 };
	auto posteriorPlaneSource = vtkSmartPointer<vtkPlaneSource>::New();

	posteriorPlaneSource->SetOrigin(0, 0, 0);

	posteriorPlaneSource->SetPoint1(0, 70, 0);
	posteriorPlaneSource->SetPoint2(70, 0, 0);

	posteriorPlaneSource->SetNormal(posteriorNormal);

	posteriorPlaneSource->SetCenter(posteriorCenter);

	posteriorPlaneSource->Update();

	auto posteriorPlaneSurface = mitk::Surface::New();
	posteriorPlaneSurface->SetVtkPolyData(posteriorPlaneSource->GetOutput());

	auto posteriorPlaneNode = mitk::DataNode::New();
	posteriorPlaneNode->SetData(posteriorPlaneSurface);
	posteriorPlaneNode->SetName("posteriorPlane");
	GetDataStorage()->Add(posteriorPlaneNode);

	// anteriorChamfer
	double anteriorChamferNormal[3]{ -0.09, 0.13, -0.99 };
	double anteriorChamferCenter[3]{ 3035.53, 1787.15, -461.69 };
	auto anteriorChamferPlaneSource = vtkSmartPointer<vtkPlaneSource>::New();

	anteriorChamferPlaneSource->SetOrigin(0, 0, 0);

	anteriorChamferPlaneSource->SetPoint1(0, 70, 0);
	anteriorChamferPlaneSource->SetPoint2(70, 0, 0);

	anteriorChamferPlaneSource->SetNormal(anteriorChamferNormal);

	anteriorChamferPlaneSource->SetCenter(anteriorChamferCenter);

	anteriorChamferPlaneSource->Update();

	auto anteriorChamferPlaneSurface = mitk::Surface::New();
	anteriorChamferPlaneSurface->SetVtkPolyData(anteriorChamferPlaneSource->GetOutput());

	auto anteriorChamferPlaneNode = mitk::DataNode::New();
	anteriorChamferPlaneNode->SetData(anteriorChamferPlaneSurface);
	anteriorChamferPlaneNode->SetName("anteriorChamferPlane");
	GetDataStorage()->Add(anteriorChamferPlaneNode);

	// PosteriorChamfer
	double posteriorChamferNormal[3]{ -0.07, 0.99, 0.13 };
	double posteriorChamferCenter[3]{ 3042.12, 1806.93, -429.48 };
	auto posteriorChamferPlaneSource = vtkSmartPointer<vtkPlaneSource>::New();

	posteriorChamferPlaneSource->SetOrigin(0, 0, 0);

	posteriorChamferPlaneSource->SetPoint1(0, 70, 0);
	posteriorChamferPlaneSource->SetPoint2(70, 0, 0);

	posteriorChamferPlaneSource->SetNormal(posteriorChamferNormal);

	posteriorChamferPlaneSource->SetCenter(posteriorChamferCenter);

	posteriorChamferPlaneSource->Update();

	auto posteriorChamferPlaneSurface = mitk::Surface::New();
	posteriorChamferPlaneSurface->SetVtkPolyData(posteriorChamferPlaneSource->GetOutput());

	auto posteriorChamferPlaneNode = mitk::DataNode::New();
	posteriorChamferPlaneNode->SetData(posteriorChamferPlaneSurface);
	posteriorChamferPlaneNode->SetName("posteriorChamferPlane");
	GetDataStorage()->Add(posteriorChamferPlaneNode);

	// Distal
	double distalNormal[3]{ -0.12, 0.79, -0.61 };
	double distalCenter[3]{ 3013.36, 1847.36, -421.03 };
	auto distalPlaneSource = vtkSmartPointer<vtkPlaneSource>::New();

	distalPlaneSource->SetOrigin(0, 0, 0);

	distalPlaneSource->SetPoint1(0, 70, 0);
	distalPlaneSource->SetPoint2(70, 0, 0);

	distalPlaneSource->SetNormal(distalNormal);

	distalPlaneSource->SetCenter(distalCenter);

	distalPlaneSource->Update();

	auto distalPlaneSurface = mitk::Surface::New();
	distalPlaneSurface->SetVtkPolyData(distalPlaneSource->GetOutput());

	auto distalPlaneNode = mitk::DataNode::New();
	distalPlaneNode->SetData(distalPlaneSurface);
	distalPlaneNode->SetName("distalPlane");
	GetDataStorage()->Add(distalPlaneNode);

}


void MoveData::on_pushButton_testStencil_clicked()
{
	auto imageToStencil = GetDataStorage()->GetNamedObject<mitk::Image>("image");
	auto stencil = GetDataStorage()->GetNamedObject<mitk::Surface>("implant");

	mitk::SurfaceToImageFilter::Pointer surfaceToImageFilter = mitk::SurfaceToImageFilter::New();
	surfaceToImageFilter->SetBackgroundValue(-600);
	surfaceToImageFilter->SetImage(imageToStencil);
	surfaceToImageFilter->SetInput(stencil);
	surfaceToImageFilter->SetReverseStencil(true);

	mitk::Image::Pointer convertedImage = mitk::Image::New();
	surfaceToImageFilter->Update();
	convertedImage = surfaceToImageFilter->GetOutput();

	auto newNode = mitk::DataNode::New();
	newNode->SetName("newNode");
	newNode->SetData(convertedImage);
	GetDataStorage()->Add(newNode);
}


void MoveData::on_pushButton_getCameraAngle_clicked()
{
	auto iRenderWindowPart = GetRenderWindowPart();

	QmitkRenderWindow* renderWindow = iRenderWindowPart->GetActiveQmitkRenderWindow();

	auto camera = renderWindow->GetVtkRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();

	auto viewTrans = camera->GetViewTransformMatrix();

	vtkNew<vtkMatrix4x4> worldToCameraTrans;
	worldToCameraTrans->DeepCopy(viewTrans);
	worldToCameraTrans->Invert();

	vtkNew<vtkMatrix4x4> rotationMatrix;
	rotationMatrix->DeepCopy(worldToCameraTrans);
	rotationMatrix->SetElement(0, 3, 0);
	rotationMatrix->SetElement(1, 3, 0);
	rotationMatrix->SetElement(2, 3, 0);


	// Visualize the camera matrix
	//GetDataStorage()->GetNamedNode("cameraCoord")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(worldToCameraTrans);

	m_Controls.textBrowser_moveData->append(QString::number(viewTrans->GetElement(0,0))+" / "+
		QString::number(viewTrans->GetElement(0, 1)) + " / "+
		QString::number(viewTrans->GetElement(0, 2)) + " / "+
		QString::number(viewTrans->GetElement(0, 3)) + " / ");

	m_Controls.textBrowser_moveData->append(QString::number(viewTrans->GetElement(1, 0)) + " / " +
		QString::number(viewTrans->GetElement(1, 1)) + " / " +
		QString::number(viewTrans->GetElement(1, 2)) + " / " +
		QString::number(viewTrans->GetElement(1, 3)) + " / ");

	m_Controls.textBrowser_moveData->append(QString::number(viewTrans->GetElement(2, 0)) + " / " +
		QString::number(viewTrans->GetElement(2, 1)) + " / " +
		QString::number(viewTrans->GetElement(2, 2)) + " / " +
		QString::number(viewTrans->GetElement(2, 3)) + " / ");

	m_Controls.textBrowser_moveData->append(QString::number(viewTrans->GetElement(3, 0)) + " / " +
		QString::number(viewTrans->GetElement(3, 1)) + " / " +
		QString::number(viewTrans->GetElement(3, 2)) + " / " +
		QString::number(viewTrans->GetElement(3, 3)) + " / ");

	m_Controls.textBrowser_moveData->append("---------------------");

	// Get the OBB center of the object
	auto objectSurface = GetDataStorage()->GetNamedObject<mitk::Surface>("object");

	auto objectCenter = objectSurface->GetGeometry()->GetCenter();

	double scaleFactor = sqrt(pow(worldToCameraTrans->GetElement(0, 3)- objectCenter[0], 2) +
		pow(worldToCameraTrans->GetElement(1, 3) - objectCenter[1], 2) +
		pow(worldToCameraTrans->GetElement(2, 3) - objectCenter[2], 2)) / 600;

	double lengthFactor = scaleFactor * 40;

	// the up cone transform
	auto upTrans = vtkTransform::New();
	upTrans->PostMultiply();
	upTrans->Scale(scaleFactor, scaleFactor, scaleFactor);
	upTrans->Concatenate(rotationMatrix);
	upTrans->Translate(objectCenter[0], objectCenter[1], objectCenter[2]);
	upTrans->Translate(lengthFactor*rotationMatrix->GetElement(0, 1), 
		lengthFactor * rotationMatrix->GetElement(1, 1), 
		lengthFactor * rotationMatrix->GetElement(2, 1));
	upTrans->Update();

	GetDataStorage()->GetNamedNode("upcone")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(upTrans->GetMatrix());

	// the down cone transform
	auto downTrans = vtkTransform::New();
	downTrans->PostMultiply();
	downTrans->Scale(scaleFactor, scaleFactor, scaleFactor);
	downTrans->Concatenate(rotationMatrix);
	downTrans->Translate(objectCenter[0], objectCenter[1], objectCenter[2]);
	downTrans->Translate(-lengthFactor * rotationMatrix->GetElement(0, 1),
		-lengthFactor * rotationMatrix->GetElement(1, 1),
		-lengthFactor * rotationMatrix->GetElement(2, 1));
	downTrans->Update();

	GetDataStorage()->GetNamedNode("downcone")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(downTrans->GetMatrix());

	// the right cone transform
	auto rightTrans = vtkTransform::New();
	rightTrans->PostMultiply();
	rightTrans->Scale(scaleFactor, scaleFactor, scaleFactor);
	rightTrans->Concatenate(rotationMatrix);
	rightTrans->Translate(objectCenter[0], objectCenter[1], objectCenter[2]);
	rightTrans->Translate(lengthFactor * rotationMatrix->GetElement(0, 0),
		lengthFactor * rotationMatrix->GetElement(1, 0),
		lengthFactor * rotationMatrix->GetElement(2, 0));
	rightTrans->Update();

	GetDataStorage()->GetNamedNode("rightcone")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(rightTrans->GetMatrix());

	// the left cone transform
	auto leftTrans = vtkTransform::New();
	leftTrans->PostMultiply();
	leftTrans->Scale(scaleFactor, scaleFactor, scaleFactor);
	leftTrans->Concatenate(rotationMatrix);
	leftTrans->Translate(objectCenter[0], objectCenter[1], objectCenter[2]);
	leftTrans->Translate(-lengthFactor * rotationMatrix->GetElement(0, 0),
		-lengthFactor * rotationMatrix->GetElement(1, 0),
		-lengthFactor * rotationMatrix->GetElement(2, 0));
	leftTrans->Update();

	GetDataStorage()->GetNamedNode("leftcone")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(leftTrans->GetMatrix());


	this->RequestRenderWindowUpdate();
}


void MoveData::InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget)
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

void MoveData::InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::Surface>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a surface"));
	widget->SetPopUpTitel(QString("Select surface"));
}

void MoveData::InitNodeSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(
		mitk::NodePredicateProperty::New("helper object"), mitk::NodePredicateProperty::New("hidden object"))));
	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a node"));
	widget->SetPopUpTitel(QString("Select node"));
}

void MoveData::PointSetDirectionChanged(QmitkSingleNodeSelectionWidget::NodeList)
{
	m_DirectionPointset = m_Controls.mitkSelectWidget_directionPointSet->GetSelectedNode();
}
void MoveData::PointSetLandmarkSourceChanged(QmitkSingleNodeSelectionWidget::NodeList)
{
	m_LandmarkSourcePointset = m_Controls.mitkNodeSelectWidget_LandmarkSrcPointset->GetSelectedNode();
}
void MoveData::PointSetLandmarkTargetChanged(QmitkSingleNodeSelectionWidget::NodeList)
{
	m_LandmarkTargetPointset = m_Controls.mitkNodeSelectWidget_LandmarkTargetPointset->GetSelectedNode();
}
void MoveData::PointSetIcpTargetChanged(QmitkSingleNodeSelectionWidget::NodeList)
{
	m_IcpTargetPointset = m_Controls.mitkNodeSelectWidget_IcpTargetPointset->GetSelectedNode();
}
void MoveData::SurfaceIcpSourceChanged(QmitkSingleNodeSelectionWidget::NodeList)
{
	m_IcpSourceSurface = m_Controls.mitkNodeSelectWidget_IcpSrcSurface->GetSelectedNode();
}
void MoveData::MovingObjectChanged(QmitkSingleNodeSelectionWidget::NodeList)
{
	m_MovingObject = m_Controls.mitkNodeSelectWidget_MovingObject->GetSelectedNode();
}




void MoveData::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/, const QList<mitk::DataNode::Pointer>& nodes)
{
	std::string nodeName;
	// iterate all selected objects, adjust warning visibility
	foreach(mitk::DataNode::Pointer node, nodes)
	{
		if (node.IsNull())
		{
			return;
		}

		node->GetName(nodeName);

		if (node != nullptr)
		{
			m_currentSelectedNode = node;
			m_baseDataToMove = node->GetData();
		}

	}

}

void MoveData::Translate(double direction[3], double length, mitk::BaseData* data)
{
	if (data != nullptr)
	{
		// mitk::Point3D normalizedDirection;
		double directionLength = sqrt((pow(direction[0], 2) + pow(direction[1], 2) + pow(direction[2], 2)));
		// normalizedDirection[0] = direction[0] / directionLength;
		// normalizedDirection[1] = direction[1] / directionLength;
		// normalizedDirection[2] = direction[2] / directionLength;

		mitk::Point3D movementVector;
		movementVector[0] = length * direction[0] / directionLength;
		movementVector[1] = length * direction[1] / directionLength;
		movementVector[2] = length * direction[2] / directionLength;

		auto* doOp = new mitk::PointOperation(mitk::OpMOVE, 0, movementVector, 0);
		// execute the Operation
		// here no undo is stored, because the movement-steps aren't interesting.
		// only the start and the end is interesting to store for undo.
		data->GetGeometry()->ExecuteOperation(doOp);
		delete doOp;

		mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	}else
	{
		m_Controls.textBrowser_moveData->append("Empty input. Please select a node ~");
	}
	clock_t start = clock();
	TestCut2();
	MITK_WARN << "Test.Interface.time(TestCut2): " << (clock() - start);
	
	// TestCut3();

}

void MoveData::Rotate(double center[3], double direction[3], double counterclockwiseDegree, mitk::BaseData* data)
{
	if (data != nullptr)
	{
		double normalizedDirection[3];
		double directionLength = sqrt((pow(direction[0], 2) + pow(direction[1], 2) + pow(direction[2], 2)));
		normalizedDirection[0] = direction[0] / directionLength;
		normalizedDirection[1] = direction[1] / directionLength;
		normalizedDirection[2] = direction[2] / directionLength;

	

		mitk::Point3D rotateCenter{ center };
		mitk::Vector3D rotateAxis{ normalizedDirection };
		auto* doOp = new mitk::RotationOperation(mitk::OpROTATE, rotateCenter, rotateAxis, counterclockwiseDegree);
		// execute the Operation
		// here no undo is stored, because the movement-steps aren't interesting.
		// only the start and the end is interesting to store for undo.
		data->GetGeometry()->ExecuteOperation(doOp);
		delete doOp;
		
		mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	}
	else
	{
		m_Controls.textBrowser_moveData->append("Empty input. Please select a node ~");
	}
	clock_t start = clock();
	TestCut2();
	MITK_WARN << "Test.Interface.time(TestCut2): " << (clock() - start);
	// TestCut3();
}

void MoveData::TranslateMinusX()
{
	double direction[3]{ -1,0,0 };

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for(int i = 0; i < (parentRowCount+1); i++)
	{
		if(i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();
			
		}else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}
		
		Translate(direction, m_Controls.lineEdit_intuitiveValue_1->text().toDouble(), m_baseDataToMove);
	}
}
void MoveData::TranslateMinusY()
{
	double direction[3]{ 0,-1,0 };

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		Translate(direction, m_Controls.lineEdit_intuitiveValue_1->text().toDouble(), m_baseDataToMove);
	}
}
void MoveData::TranslateMinusZ()
{
	double direction[3]{ 0,0,-1 };

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		Translate(direction, m_Controls.lineEdit_intuitiveValue_1->text().toDouble(), m_baseDataToMove);
	}
}
void MoveData::TranslatePlusX()
{
	double direction[3]{ 1,0,0 };

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		Translate(direction, m_Controls.lineEdit_intuitiveValue_1->text().toDouble(), m_baseDataToMove);
	}
}
void MoveData::TranslatePlusY()
{
	double direction[3]{ 0,1,0 };

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		Translate(direction, m_Controls.lineEdit_intuitiveValue_1->text().toDouble(), m_baseDataToMove);
	}
}
void MoveData::TranslatePlusZ()
{
	double direction[3]{ 0,0,1 };

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		Translate(direction, m_Controls.lineEdit_intuitiveValue_1->text().toDouble(), m_baseDataToMove);
	}
}
void MoveData::RotatePlusX()
{
	if (m_baseDataToMove==nullptr)
	{
		m_Controls.textBrowser_moveData->append("Empty input. Please select a node ~");
		return;
	}

	double direction[3]{ 1,0,0 };
	double angle = m_Controls.lineEdit_intuitiveValue_1->text().toDouble();
	mitk::Point<double, 3>::ValueType* center = m_currentSelectedNode->GetData()->GetGeometry()->GetCenter().GetDataPointer();

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}
		
		Rotate(center, direction, angle, m_baseDataToMove);
	}

}
void MoveData::RotatePlusY()
{
	if (m_baseDataToMove == nullptr)
	{
		m_Controls.textBrowser_moveData->append("Empty input. Please select a node ~");
		return;
	}

	double direction[3]{ 0,1,0 };
	double angle = m_Controls.lineEdit_intuitiveValue_1->text().toDouble();
	mitk::Point<double, 3>::ValueType* center = m_currentSelectedNode->GetData()->GetGeometry()->GetCenter().GetDataPointer();

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		Rotate(center, direction, angle, m_baseDataToMove);
	}
}
void MoveData::RotatePlusZ()
{
	if (m_baseDataToMove == nullptr)
	{
		m_Controls.textBrowser_moveData->append("Empty input. Please select a node ~");
		return;
	}

	double direction[3]{ 0,0,1 };
	double angle = m_Controls.lineEdit_intuitiveValue_1->text().toDouble();
	mitk::Point<double, 3>::ValueType* center = m_currentSelectedNode->GetData()->GetGeometry()->GetCenter().GetDataPointer();

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		Rotate(center, direction, angle, m_baseDataToMove);
	}
}
void MoveData::RotateMinusX()
{
	if (m_baseDataToMove == nullptr)
	{
		m_Controls.textBrowser_moveData->append("Empty input. Please select a node ~");
		return;
	}

	double direction[3]{ 1,0,0 };
	double angle = - m_Controls.lineEdit_intuitiveValue_1->text().toDouble();
	mitk::Point<double, 3>::ValueType* center = m_currentSelectedNode->GetData()->GetGeometry()->GetCenter().GetDataPointer();

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		Rotate(center, direction, angle, m_baseDataToMove);
	}
}
void MoveData::RotateMinusY()
{
	if (m_baseDataToMove == nullptr)
	{
		m_Controls.textBrowser_moveData->append("Empty input. Please select a node ~");
		return;
	}

	double direction[3]{ 0,1,0 };
	double angle = - m_Controls.lineEdit_intuitiveValue_1->text().toDouble();
	mitk::Point<double, 3>::ValueType* center = m_currentSelectedNode->GetData()->GetGeometry()->GetCenter().GetDataPointer();

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		Rotate(center, direction, angle, m_baseDataToMove);
	}
}
void MoveData::RotateMinusZ()
{
	if (m_baseDataToMove == nullptr)
	{
		m_Controls.textBrowser_moveData->append("Empty input. Please select a node ~");
		return;
	}
	
	double direction[3]{ 0,0,1 };
	double angle = - m_Controls.lineEdit_intuitiveValue_1->text().toDouble();
	mitk::Point<double, 3>::ValueType* center = m_currentSelectedNode->GetData()->GetGeometry()->GetCenter().GetDataPointer();
	
	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);
	
	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);
	
	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();
	
		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}
	
		Rotate(center, direction, angle, m_baseDataToMove);
	}

}

void MoveData::OverwriteOffsetMatrix()
{
	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		if (m_baseDataToMove != nullptr)
		{
			m_baseDataToMove->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(ObtainVtkMatrixFromUi());
			mitk::RenderingManager::GetInstance()->RequestUpdateAll();
		}
		else
		{
			m_Controls.textBrowser_moveData->append("Empty input. Please select a node ~");
		}

	}

	
	
}

void MoveData::AppendOffsetMatrix()
{
	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		if (m_baseDataToMove != nullptr)
		{
			auto tmpVtkTransform = vtkTransform::New();
			tmpVtkTransform->PostMultiply();
			tmpVtkTransform->Identity();
			tmpVtkTransform->SetMatrix(m_baseDataToMove->GetGeometry()->GetVtkMatrix());
			tmpVtkTransform->Concatenate(ObtainVtkMatrixFromUi());

			m_baseDataToMove->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpVtkTransform->GetMatrix());
			m_baseDataToMove->GetGeometry()->Modified();
			mitk::RenderingManager::GetInstance()->RequestUpdateAll();

		}
		else
		{
			m_Controls.textBrowser_moveData->append("Empty input. Please select a node ~");
		}

	}
}

void MoveData::TranslatePlus()
{
	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		if (m_DirectionPointset != nullptr)
		{
			auto tmpPointset = dynamic_cast<mitk::PointSet*> (m_DirectionPointset->GetData());
			double direction[3]
			{
				tmpPointset->GetPoint(1,0)[0] - tmpPointset->GetPoint(0,0)[0],
				tmpPointset->GetPoint(1,0)[1] - tmpPointset->GetPoint(0,0)[1],
				tmpPointset->GetPoint(1,0)[2] - tmpPointset->GetPoint(0,0)[2]
			};

			Translate(direction, m_Controls.lineEdit_intuitiveValue_2->text().toDouble(), m_baseDataToMove);

		}
		else
		{
			m_Controls.textBrowser_moveData->append("Please reselect a pointset representing the direction ~");
		}

	}

}

void MoveData::TranslateMinus()
{
	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		if (m_DirectionPointset != nullptr)
		{
			auto tmpPointset = dynamic_cast<mitk::PointSet*> (m_DirectionPointset->GetData());
			double direction[3]
			{
				- tmpPointset->GetPoint(1,0)[0] + tmpPointset->GetPoint(0,0)[0],
				- tmpPointset->GetPoint(1,0)[1] + tmpPointset->GetPoint(0,0)[1],
				- tmpPointset->GetPoint(1,0)[2] + tmpPointset->GetPoint(0,0)[2]
			};

			Translate(direction, m_Controls.lineEdit_intuitiveValue_2->text().toDouble(), m_baseDataToMove);

		}
		else
		{
			m_Controls.textBrowser_moveData->append("Please reselect a pointset representing the direction ~");
		}

	}
}

void MoveData::RotatePlus()
{
	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}
		if (m_DirectionPointset != nullptr)
		{
			if (m_baseDataToMove == nullptr)
			{
				m_Controls.textBrowser_moveData->append("Empty input. Please select a node ~");
				return;
			}
			auto tmpPointset = dynamic_cast<mitk::PointSet*> (m_DirectionPointset->GetData());
			double direction[3]
			{
				tmpPointset->GetPoint(1,0)[0] - tmpPointset->GetPoint(0,0)[0],
				tmpPointset->GetPoint(1,0)[1] - tmpPointset->GetPoint(0,0)[1],
				tmpPointset->GetPoint(1,0)[2] - tmpPointset->GetPoint(0,0)[2]
			};

			double center[3]
			{
				tmpPointset->GetPoint(1,0)[0],
				tmpPointset->GetPoint(1,0)[1],
				tmpPointset->GetPoint(1,0)[2],

			};

			Rotate(center, direction, m_Controls.lineEdit_intuitiveValue_2->text().toDouble(), m_baseDataToMove);

		}
		else
		{
			m_Controls.textBrowser_moveData->append("Please reselect a pointset representing the direction ~");
		}

	}
	
}

void MoveData::RotateMinus()
{
	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}
		if (m_DirectionPointset != nullptr)
		{
			if (m_baseDataToMove == nullptr)
			{
				m_Controls.textBrowser_moveData->append("Empty input. Please select a node ~");
				return;
			}
			auto tmpPointset = dynamic_cast<mitk::PointSet*> (m_DirectionPointset->GetData());
			double direction[3]
			{
				- tmpPointset->GetPoint(1,0)[0] + tmpPointset->GetPoint(0,0)[0],
				- tmpPointset->GetPoint(1,0)[1] + tmpPointset->GetPoint(0,0)[1],
				- tmpPointset->GetPoint(1,0)[2] + tmpPointset->GetPoint(0,0)[2]
			};

			double center[3]
			{
				tmpPointset->GetPoint(1,0)[0],
				tmpPointset->GetPoint(1,0)[1],
				tmpPointset->GetPoint(1,0)[2],

			};

			Rotate(center, direction, m_Controls.lineEdit_intuitiveValue_2->text().toDouble(), m_baseDataToMove);

		}
		else
		{
			m_Controls.textBrowser_moveData->append("Please reselect a pointset representing the direction ~");
		}

	}
}


void MoveData::RealignImage()
{

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		if (m_baseDataToMove != nullptr)
		{

			// Align the image's axes to the standard xyz axes
			auto tmpVtkTransform = vtkSmartPointer<vtkTransform>::New();
			vtkSmartPointer<vtkMatrix4x4> tmpVtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
			tmpVtkTransform->Identity();
			tmpVtkTransform->GetMatrix(tmpVtkMatrix);

			m_baseDataToMove->GetGeometry(0)->SetIndexToWorldTransformByVtkMatrixWithoutChangingSpacing(tmpVtkMatrix);
			// SetIndexToWorldTransformByVtkMatrix(tmpVtkMatrix) will set the spacing as (1, 1, 1),
			// because the spacing is determined by the matrix diagonal.
			  // So SetIndexToWorldTransformByVtkMatrixWithoutChangingSpacing(tmpVtkMatrix) which keep the spacing regardless of the
			  // input matrix

			mitk::RenderingManager::GetInstance()->RequestUpdateAll();

		}
		else
		{
			m_Controls.textBrowser_moveData->append("Empty input. Please select a node ~");
		}
	}

}


void MoveData::ClearMatrixContent(){
	m_Controls.lineEdit_offsetMatrix_0->setText(QString::number(1));
	m_Controls.lineEdit_offsetMatrix_1->setText(QString::number(0));
	m_Controls.lineEdit_offsetMatrix_2->setText(QString::number(0));
	m_Controls.lineEdit_offsetMatrix_3->setText(QString::number(0));
	m_Controls.lineEdit_offsetMatrix_4->setText(QString::number(0));
	m_Controls.lineEdit_offsetMatrix_5->setText(QString::number(1));
	m_Controls.lineEdit_offsetMatrix_6->setText(QString::number(0));
	m_Controls.lineEdit_offsetMatrix_7->setText(QString::number(0));
	m_Controls.lineEdit_offsetMatrix_8->setText(QString::number(0));
	m_Controls.lineEdit_offsetMatrix_9->setText(QString::number(0));
	m_Controls.lineEdit_offsetMatrix_10->setText(QString::number(1));
	m_Controls.lineEdit_offsetMatrix_11->setText(QString::number(0));
	m_Controls.lineEdit_offsetMatrix_12->setText(QString::number(0));
	m_Controls.lineEdit_offsetMatrix_13->setText(QString::number(0));
	m_Controls.lineEdit_offsetMatrix_14->setText(QString::number(0));
	m_Controls.lineEdit_offsetMatrix_15->setText(QString::number(1));
}



vtkMatrix4x4* MoveData::ObtainVtkMatrixFromUi()
	{
	auto tmpMatrix = vtkMatrix4x4::New();

	tmpMatrix->SetElement(0, 0, m_Controls.lineEdit_offsetMatrix_0->text().toDouble());
	tmpMatrix->SetElement(1, 0, m_Controls.lineEdit_offsetMatrix_1->text().toDouble());
	tmpMatrix->SetElement(2, 0, m_Controls.lineEdit_offsetMatrix_2->text().toDouble());
	tmpMatrix->SetElement(3, 0, m_Controls.lineEdit_offsetMatrix_3->text().toDouble());
	tmpMatrix->SetElement(0, 1, m_Controls.lineEdit_offsetMatrix_4->text().toDouble());
	tmpMatrix->SetElement(1, 1, m_Controls.lineEdit_offsetMatrix_5->text().toDouble());
	tmpMatrix->SetElement(2, 1, m_Controls.lineEdit_offsetMatrix_6->text().toDouble());
	tmpMatrix->SetElement(3, 1, m_Controls.lineEdit_offsetMatrix_7->text().toDouble());
	tmpMatrix->SetElement(0, 2, m_Controls.lineEdit_offsetMatrix_8->text().toDouble());
	tmpMatrix->SetElement(1, 2, m_Controls.lineEdit_offsetMatrix_9->text().toDouble());
	tmpMatrix->SetElement(2, 2, m_Controls.lineEdit_offsetMatrix_10->text().toDouble());
	tmpMatrix->SetElement(3, 2, m_Controls.lineEdit_offsetMatrix_11->text().toDouble());
	tmpMatrix->SetElement(0, 3, m_Controls.lineEdit_offsetMatrix_12->text().toDouble());
	tmpMatrix->SetElement(1, 3, m_Controls.lineEdit_offsetMatrix_13->text().toDouble());
	tmpMatrix->SetElement(2, 3, m_Controls.lineEdit_offsetMatrix_14->text().toDouble());
	tmpMatrix->SetElement(3, 3, m_Controls.lineEdit_offsetMatrix_15->text().toDouble());

	return tmpMatrix;
}


void MoveData::UpdateUiRegistrationMatrix()
{
	m_Controls.lineEdit_RegistrationMatrix_0->setText(QString::number(m_eigenMatrixTmpRegistrationResult(0)));
	m_Controls.lineEdit_RegistrationMatrix_1->setText(QString::number(m_eigenMatrixTmpRegistrationResult(1)));
	m_Controls.lineEdit_RegistrationMatrix_2->setText(QString::number(m_eigenMatrixTmpRegistrationResult(2)));
	m_Controls.lineEdit_RegistrationMatrix_3->setText(QString::number(m_eigenMatrixTmpRegistrationResult(3)));
	m_Controls.lineEdit_RegistrationMatrix_4->setText(QString::number(m_eigenMatrixTmpRegistrationResult(4)));
	m_Controls.lineEdit_RegistrationMatrix_5->setText(QString::number(m_eigenMatrixTmpRegistrationResult(5)));
	m_Controls.lineEdit_RegistrationMatrix_6->setText(QString::number(m_eigenMatrixTmpRegistrationResult(6)));
	m_Controls.lineEdit_RegistrationMatrix_7->setText(QString::number(m_eigenMatrixTmpRegistrationResult(7)));
	m_Controls.lineEdit_RegistrationMatrix_8->setText(QString::number(m_eigenMatrixTmpRegistrationResult(8)));
	m_Controls.lineEdit_RegistrationMatrix_9->setText(QString::number(m_eigenMatrixTmpRegistrationResult(9)));
	m_Controls.lineEdit_RegistrationMatrix_10->setText(QString::number(m_eigenMatrixTmpRegistrationResult(10)));
	m_Controls.lineEdit_RegistrationMatrix_11->setText(QString::number(m_eigenMatrixTmpRegistrationResult(11)));
	m_Controls.lineEdit_RegistrationMatrix_12->setText(QString::number(m_eigenMatrixTmpRegistrationResult(12)));
	m_Controls.lineEdit_RegistrationMatrix_13->setText(QString::number(m_eigenMatrixTmpRegistrationResult(13)));
	m_Controls.lineEdit_RegistrationMatrix_14->setText(QString::number(m_eigenMatrixTmpRegistrationResult(14)));
	m_Controls.lineEdit_RegistrationMatrix_15->setText(QString::number(m_eigenMatrixTmpRegistrationResult(15)));
}

void MoveData::LandmarkRegistration()
{
	auto landmarkRegistrator = mitk::SurfaceRegistration::New();
	m_LandmarkSourcePointset = m_Controls.mitkNodeSelectWidget_LandmarkSrcPointset->GetSelectedNode();
	m_LandmarkTargetPointset = m_Controls.mitkNodeSelectWidget_LandmarkTargetPointset->GetSelectedNode();


	MITK_INFO << "Proceeding Landmark registration";
	if (m_LandmarkSourcePointset != nullptr && m_LandmarkTargetPointset != nullptr)
	{
		auto sourcePointset = dynamic_cast<mitk::PointSet*>(m_LandmarkSourcePointset->GetData());
		auto targetPointset = dynamic_cast<mitk::PointSet*>(m_LandmarkTargetPointset->GetData());
		landmarkRegistrator->SetLandmarksSrc(sourcePointset);
		landmarkRegistrator->SetLandmarksTarget(targetPointset);
		landmarkRegistrator->ComputeLandMarkResult();

		

		Eigen::Matrix4d tmpRegistrationResult{ landmarkRegistrator->GetResult()->GetData() };
		tmpRegistrationResult.transposeInPlace();
		m_eigenMatrixTmpRegistrationResult = tmpRegistrationResult;
		MITK_INFO << m_eigenMatrixTmpRegistrationResult;
		UpdateUiRegistrationMatrix();
	}else
	{
		m_Controls.textBrowser_moveData->append("Landmark source or target are is not ready");
	}

	//std::ostringstream os;
	//landmarkRegistrator->GetResult()->Print(os);

	m_Controls.textBrowser_moveData->append("-------------Start landmark registration----------");
	// m_Controls.textBrowser_moveData->append(QString::fromStdString(os.str()));

	m_Controls.textBrowser_moveData->append("Maximal landmark registration error: " + QString::number(landmarkRegistrator->GetmaxLandmarkError()));
	m_Controls.textBrowser_moveData->append("Average landmark registration error: " + QString::number(landmarkRegistrator->GetavgLandmarkError()));
	m_Controls.textBrowser_moveData->append("-------------Landmark registration succeeded----------");
}

vtkMatrix4x4* MoveData::ObtainVtkMatrixFromRegistrationUi()
{
	auto tmpMatrix = vtkMatrix4x4::New();

	tmpMatrix->SetElement(0, 0, m_Controls.lineEdit_RegistrationMatrix_0->text().toDouble());
	tmpMatrix->SetElement(1, 0, m_Controls.lineEdit_RegistrationMatrix_1->text().toDouble());
	tmpMatrix->SetElement(2, 0, m_Controls.lineEdit_RegistrationMatrix_2->text().toDouble());
	tmpMatrix->SetElement(3, 0, m_Controls.lineEdit_RegistrationMatrix_3->text().toDouble());
	tmpMatrix->SetElement(0, 1, m_Controls.lineEdit_RegistrationMatrix_4->text().toDouble());
	tmpMatrix->SetElement(1, 1, m_Controls.lineEdit_RegistrationMatrix_5->text().toDouble());
	tmpMatrix->SetElement(2, 1, m_Controls.lineEdit_RegistrationMatrix_6->text().toDouble());
	tmpMatrix->SetElement(3, 1, m_Controls.lineEdit_RegistrationMatrix_7->text().toDouble());
	tmpMatrix->SetElement(0, 2, m_Controls.lineEdit_RegistrationMatrix_8->text().toDouble());
	tmpMatrix->SetElement(1, 2, m_Controls.lineEdit_RegistrationMatrix_9->text().toDouble());
	tmpMatrix->SetElement(2, 2, m_Controls.lineEdit_RegistrationMatrix_10->text().toDouble());
	tmpMatrix->SetElement(3, 2, m_Controls.lineEdit_RegistrationMatrix_11->text().toDouble());
	tmpMatrix->SetElement(0, 3, m_Controls.lineEdit_RegistrationMatrix_12->text().toDouble());
	tmpMatrix->SetElement(1, 3, m_Controls.lineEdit_RegistrationMatrix_13->text().toDouble());
	tmpMatrix->SetElement(2, 3, m_Controls.lineEdit_RegistrationMatrix_14->text().toDouble());
	tmpMatrix->SetElement(3, 3, m_Controls.lineEdit_RegistrationMatrix_15->text().toDouble());

	return tmpMatrix;
}

void MoveData::AppendRegistrationMatrix()
{
	m_currentSelectedNode = m_Controls.mitkNodeSelectWidget_MovingObject->GetSelectedNode();

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		if (m_baseDataToMove != nullptr)
		{
			auto tmpVtkTransform = vtkTransform::New();
			tmpVtkTransform->PostMultiply();
			tmpVtkTransform->Identity();
			tmpVtkTransform->SetMatrix(m_baseDataToMove->GetGeometry()->GetVtkMatrix());
			tmpVtkTransform->Concatenate(ObtainVtkMatrixFromRegistrationUi());

			m_baseDataToMove->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpVtkTransform->GetMatrix());
			mitk::RenderingManager::GetInstance()->RequestUpdateAll();


		}
		else
		{
			m_Controls.textBrowser_moveData->append("Moving object is empty ~~");
		}

		// if the m_baseDataToMove is pointSet, rewrite the pointSet members and keep geometry matrix as identity
		if(dynamic_cast<mitk::PointSet*>(m_baseDataToMove) != nullptr)
		{
			auto tmpPointSet = dynamic_cast<mitk::PointSet*>(m_baseDataToMove);
			int size = tmpPointSet->GetSize();

			for(int i{0}; i < size; i++)
			{
				auto tmpPoint = tmpPointSet->GetPoint(i);
				vtkNew<vtkTransform> tmpTrans;
				tmpTrans->Identity();
				tmpTrans->PostMultiply();
				tmpTrans->Translate(tmpPoint[0], tmpPoint[1], tmpPoint[2]);
				tmpTrans->Concatenate(tmpPointSet->GetGeometry()->GetVtkMatrix());
				tmpTrans->Update();
				auto tmpMatrix = tmpTrans->GetMatrix();

				mitk::Point3D newPoint;
				newPoint[0] = tmpMatrix->GetElement(0, 3);
				newPoint[1] = tmpMatrix->GetElement(1, 3);
				newPoint[2] = tmpMatrix->GetElement(2, 3);

				tmpPointSet->SetPoint(i, newPoint);

			}
			vtkNew<vtkMatrix4x4> identityMatrix;
			identityMatrix->Identity();
			tmpPointSet->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);
		}


	}
	
}

void MoveData::IcpRegistration()
{
	auto icpRegistrator = mitk::SurfaceRegistration::New();

	m_IcpSourceSurface = m_Controls.mitkNodeSelectWidget_IcpSrcSurface->GetSelectedNode();
	m_IcpTargetPointset = m_Controls.mitkNodeSelectWidget_IcpTargetPointset->GetSelectedNode();

	MITK_INFO << "Proceeding ICP registration";

	if (m_IcpSourceSurface != nullptr && m_IcpTargetPointset != nullptr)
	{
		auto icpTargetPointset = dynamic_cast<mitk::PointSet*>(m_IcpTargetPointset->GetData());
		auto icpSrcSurface = dynamic_cast<mitk::Surface*>(m_IcpSourceSurface->GetData());
		icpRegistrator->SetIcpPoints(icpTargetPointset);
		icpRegistrator->SetSurfaceSrc(icpSrcSurface);
		icpRegistrator->ComputeIcpResult();

		Eigen::Matrix4d tmpRegistrationResult{ icpRegistrator->GetResult()->GetData() };
		tmpRegistrationResult.transposeInPlace();

		m_eigenMatrixTmpRegistrationResult = tmpRegistrationResult;
		MITK_INFO << m_eigenMatrixTmpRegistrationResult;

		UpdateUiRegistrationMatrix();
	}
	else
	{
		m_Controls.textBrowser_moveData->append("Icp source or target are is not ready");
	}

	//std::ostringstream os;
	//icpRegistrator->GetResult()->Print(os);
	m_Controls.textBrowser_moveData->append("-------------Started ICP registration----------");
	//m_Controls.textBrowser_moveData->append(QString::fromStdString(os.str()));

	m_Controls.textBrowser_moveData->append("Maximal ICP registration error: " + QString::number(icpRegistrator->GetmaxIcpError()));
	m_Controls.textBrowser_moveData->append("Average ICP registration error: " + QString::number(icpRegistrator->GetavgIcpError()));
	m_Controls.textBrowser_moveData->append("-------------ICP registration succeeded----------");
};

void MoveData::SurfaceToSurfaceIcp()
{
	auto icpRegistrator = mitk::SurfaceRegistration::New();

	
	MITK_INFO << "Proceeding Surface-to-surface ICP registration";


	icpRegistrator->SetSurfaceSrc(dynamic_cast<mitk::Surface*>(m_Controls.mitkNodeSelectWidget_srcSurface->GetSelectedNode()->GetData()));
	icpRegistrator->SetSurfaceTarget(dynamic_cast<mitk::Surface*>(m_Controls.mitkNodeSelectWidget_targetSurface->GetSelectedNode()->GetData()));
	icpRegistrator->ComputeSurfaceIcpResult();

	Eigen::Matrix4d tmpRegistrationResult{ icpRegistrator->GetResult()->GetData() };
	tmpRegistrationResult.transposeInPlace();
	
	m_eigenMatrixTmpRegistrationResult = tmpRegistrationResult;
	MITK_INFO << m_eigenMatrixTmpRegistrationResult;
	
	UpdateUiRegistrationMatrix();
	
	
	std::ostringstream os;
	icpRegistrator->GetResult()->Print(os);
	m_Controls.textBrowser_moveData->append("-------------Start ICP registration----------");
	m_Controls.textBrowser_moveData->append(QString::fromStdString(os.str()));

	m_Controls.textBrowser_moveData->append("Maximal ICP registration error: Surface-to-surface ICP is too time-consuming");
	m_Controls.textBrowser_moveData->append("Average ICP registration error: Surface-to-surface ICP is too time-consuming");
	m_Controls.textBrowser_moveData->append("-------------ICP registration succeeded----------");
}


void MoveData::GetObjectGeometryWithSpacing()
{
	if(m_baseDataToMove != nullptr)
	{
		auto tmpvtkMatrix = m_baseDataToMove->GetGeometry()->GetVtkMatrix();

		m_Controls.lineEdit_offsetMatrix_0->setText(QString::number(tmpvtkMatrix->GetElement(0, 0)));
		m_Controls.lineEdit_offsetMatrix_1->setText(QString::number(tmpvtkMatrix->GetElement(1, 0)));
		m_Controls.lineEdit_offsetMatrix_2->setText(QString::number(tmpvtkMatrix->GetElement(2, 0)));
		m_Controls.lineEdit_offsetMatrix_3->setText(QString::number(tmpvtkMatrix->GetElement(3, 0)));
		m_Controls.lineEdit_offsetMatrix_4->setText(QString::number(tmpvtkMatrix->GetElement(0, 1)));
		m_Controls.lineEdit_offsetMatrix_5->setText(QString::number(tmpvtkMatrix->GetElement(1, 1)));
		m_Controls.lineEdit_offsetMatrix_6->setText(QString::number(tmpvtkMatrix->GetElement(2, 1)));
		m_Controls.lineEdit_offsetMatrix_7->setText(QString::number(tmpvtkMatrix->GetElement(3, 1)));
		m_Controls.lineEdit_offsetMatrix_8->setText(QString::number(tmpvtkMatrix->GetElement(0, 2)));
		m_Controls.lineEdit_offsetMatrix_9->setText(QString::number(tmpvtkMatrix->GetElement(1, 2)));
		m_Controls.lineEdit_offsetMatrix_10->setText(QString::number(tmpvtkMatrix->GetElement(2, 2)));
		m_Controls.lineEdit_offsetMatrix_11->setText(QString::number(tmpvtkMatrix->GetElement(3, 2)));
		m_Controls.lineEdit_offsetMatrix_12->setText(QString::number(tmpvtkMatrix->GetElement(0, 3)));
		m_Controls.lineEdit_offsetMatrix_13->setText(QString::number(tmpvtkMatrix->GetElement(1, 3)));
		m_Controls.lineEdit_offsetMatrix_14->setText(QString::number(tmpvtkMatrix->GetElement(2, 3)));
		m_Controls.lineEdit_offsetMatrix_15->setText(QString::number(tmpvtkMatrix->GetElement(3, 3)));
	}
	

}

void MoveData::GetObjectGeometryWithoutSpacing()
{
	if (m_currentSelectedNode != nullptr)
	{
		auto initialMatrix = m_currentSelectedNode->GetData()->GetGeometry()->GetVtkMatrix();
		auto spacing = m_currentSelectedNode->GetData()->GetGeometry()->GetSpacing();
		auto spacingOffsetMatrix = vtkMatrix4x4::New();
		spacingOffsetMatrix->Identity();
		spacingOffsetMatrix->SetElement(0, 0, 1 / spacing[0]);
		spacingOffsetMatrix->SetElement(1, 1, 1 / spacing[1]);
		spacingOffsetMatrix->SetElement(2, 2, 1 / spacing[2]);

		auto tmpTransform = vtkTransform::New();
		tmpTransform->PreMultiply();
		tmpTransform->Identity();
		tmpTransform->SetMatrix(initialMatrix);
		tmpTransform->Concatenate(spacingOffsetMatrix);

		auto tmpvtkMatrix = tmpTransform->GetMatrix();


		m_Controls.lineEdit_offsetMatrix_0->setText(QString::number(tmpvtkMatrix->GetElement(0, 0)));
		m_Controls.lineEdit_offsetMatrix_1->setText(QString::number(tmpvtkMatrix->GetElement(1, 0)));
		m_Controls.lineEdit_offsetMatrix_2->setText(QString::number(tmpvtkMatrix->GetElement(2, 0)));
		m_Controls.lineEdit_offsetMatrix_3->setText(QString::number(tmpvtkMatrix->GetElement(3, 0)));
		m_Controls.lineEdit_offsetMatrix_4->setText(QString::number(tmpvtkMatrix->GetElement(0, 1)));
		m_Controls.lineEdit_offsetMatrix_5->setText(QString::number(tmpvtkMatrix->GetElement(1, 1)));
		m_Controls.lineEdit_offsetMatrix_6->setText(QString::number(tmpvtkMatrix->GetElement(2, 1)));
		m_Controls.lineEdit_offsetMatrix_7->setText(QString::number(tmpvtkMatrix->GetElement(3, 1)));
		m_Controls.lineEdit_offsetMatrix_8->setText(QString::number(tmpvtkMatrix->GetElement(0, 2)));
		m_Controls.lineEdit_offsetMatrix_9->setText(QString::number(tmpvtkMatrix->GetElement(1, 2)));
		m_Controls.lineEdit_offsetMatrix_10->setText(QString::number(tmpvtkMatrix->GetElement(2, 2)));
		m_Controls.lineEdit_offsetMatrix_11->setText(QString::number(tmpvtkMatrix->GetElement(3, 2)));
		m_Controls.lineEdit_offsetMatrix_12->setText(QString::number(tmpvtkMatrix->GetElement(0, 3)));
		m_Controls.lineEdit_offsetMatrix_13->setText(QString::number(tmpvtkMatrix->GetElement(1, 3)));
		m_Controls.lineEdit_offsetMatrix_14->setText(QString::number(tmpvtkMatrix->GetElement(2, 3)));
		m_Controls.lineEdit_offsetMatrix_15->setText(QString::number(tmpvtkMatrix->GetElement(3, 3)));
	}

	GetRenderWindowPart()->GetRenderingManager();

}

void MoveData::TestCut() 
{
	if (m_Controls.radioButton_testCutting->isChecked() == false) 
	{
		return;
	}

	if (m_growingCutterNode == nullptr)
	{
		m_growingCutterNode = mitk::DataNode::New();
		auto initSurface = dynamic_cast<mitk::Surface*>(m_currentSelectedNode->GetData());
		auto initPolyData = initSurface->GetVtkPolyData();

		vtkNew<vtkPolyData> movedPolyData;

		vtkNew<vtkTransformFilter> transformFilter;
		vtkNew<vtkTransform> tmpTransform;
		tmpTransform->SetMatrix(initSurface->GetGeometry()->GetVtkMatrix());
		transformFilter->SetTransform(tmpTransform);
		transformFilter->SetInputData(initPolyData);
		transformFilter->Update();

		movedPolyData->DeepCopy(transformFilter->GetPolyDataOutput());

		auto tmpSurface = mitk::Surface::New();
		tmpSurface->SetVtkPolyData(movedPolyData);
		m_growingCutterNode->SetData(tmpSurface);
		m_growingCutterNode->SetName("growingCutter");
		GetDataStorage()->Add(m_growingCutterNode);
	}

	// Append the current cutter
	auto initSurface = dynamic_cast<mitk::Surface*>(m_currentSelectedNode->GetData());
	auto initPolyData = initSurface->GetVtkPolyData();

	vtkNew<vtkPolyData> movedPolyData;

	vtkNew<vtkTransformFilter> transformFilter;
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->SetMatrix(initSurface->GetGeometry()->GetVtkMatrix());
	transformFilter->SetTransform(tmpTransform);
	transformFilter->SetInputData(initPolyData);
	transformFilter->Update();

	movedPolyData->DeepCopy(transformFilter->GetPolyDataOutput());

	auto initGrowingCutter = dynamic_cast<mitk::Surface*>(m_growingCutterNode->GetData())->GetVtkPolyData();

	vtkNew<vtkAppendPolyData> appendFilter;
	appendFilter->AddInputData(initGrowingCutter);
	appendFilter->AddInputData(movedPolyData);
	appendFilter->Update();

	auto tmpSurface = mitk::Surface::New();
	tmpSurface->SetVtkPolyData(appendFilter->GetOutput());
	m_growingCutterNode->SetData(tmpSurface);

}

void MoveData::TestCut3()
{
	if (m_Controls.radioButton_testCutting->isChecked() == false)
	{
		return;
	}


	auto dataToCut = GetDataStorage()->GetNamedObject<mitk::Surface>("surfaceToCut")->GetVtkPolyData();

	auto initSurface = dynamic_cast<mitk::Surface*>(m_currentSelectedNode->GetData());
	auto initPolyData = initSurface->GetVtkPolyData();

	vtkNew<vtkPolyData> movedPolyData;

	vtkNew<vtkTransformFilter> transformFilter;
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->SetMatrix(initSurface->GetGeometry()->GetVtkMatrix());
	transformFilter->SetTransform(tmpTransform);
	transformFilter->SetInputData(initPolyData);
	transformFilter->Update();

	movedPolyData->DeepCopy(transformFilter->GetPolyDataOutput());

	vtkNew<vtkClipPolyData> clipper;
	clipper->SetInputData(dataToCut);
	clipper->GenerateClippedOutputOn();

	vtkNew<vtkImplicitPolyDataDistance> implicitDistance;

	implicitDistance->SetInput(movedPolyData);

	clipper->SetClipFunction(implicitDistance);

	clipper->Update();

	// // use the bone to clip the saw
	// vtkNew<vtkClipPolyData> clipper2;
	// clipper2->SetInputData(movedPolyData);
	// clipper2->GenerateClippedOutputOn();
	//
	// vtkNew<vtkImplicitPolyDataDistance> implicitDistance2;
	//
	// implicitDistance2->SetInput(GetDataStorage()->GetNamedObject<mitk::Surface>("backup")->GetVtkPolyData());
	//
	//
	// clipper2->SetClipFunction(implicitDistance2);
	//
	// clipper2->Update();


	// vtkNew<vtkAppendPolyData> appenderFilter;
	// appenderFilter->SetInputData(clipper2->GetOutput());
	// appenderFilter->AddInputData(clipper->GetOutput());
	// appenderFilter->Update();

	GetDataStorage()->GetNamedObject<mitk::Surface>("surfaceToCut")->SetVtkPolyData(clipper->GetOutput());

	
}

void MoveData::TestCut2()
{
	if (m_Controls.radioButton_testCutting->isChecked() == false)
	{
		return;
	}

	auto initSurface = dynamic_cast<mitk::Surface*>(m_currentSelectedNode->GetData());

	if(initSurface == nullptr)
	{
		return;
	}

	auto initPolyData = initSurface->GetVtkPolyData();

	vtkNew<vtkPolyData> movedPolyData;

	vtkNew<vtkTransformFilter> transformFilter;
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->SetMatrix(initSurface->GetGeometry()->GetVtkMatrix());
	transformFilter->SetTransform(tmpTransform);
	transformFilter->SetInputData(initPolyData);
	transformFilter->Update();

	movedPolyData->DeepCopy(transformFilter->GetPolyDataOutput());

	auto movedSurface = mitk::Surface::New();
	movedSurface->SetVtkPolyData(movedPolyData);

	if(GetDataStorage()->GetNamedNode("imageToCut") == nullptr)
	{
		return;
	}

	auto imageToCut = GetDataStorage()->GetNamedObject<mitk::Image>("imageToCut");

	auto surfaceToImageFilter = mitk::SurfaceToImageFilter::New();
	//surfaceToImageFilter->SetMakeOutputBinary(false);
	surfaceToImageFilter->SetBackgroundValue(2150);
	//surfaceToImageFilter->SetUShortBinaryPixelType(false);
	surfaceToImageFilter->SetImage(imageToCut);
	surfaceToImageFilter->SetInput(movedSurface);
	surfaceToImageFilter->SetReverseStencil(true);
	surfaceToImageFilter->Update();

	GetDataStorage()->GetNamedNode("imageToCut")->SetData(surfaceToImageFilter->GetOutput());
}


void MoveData::on_pushButton_ApplyStencil_clicked()
{
	// Generate a white image to apply the polydata stencil
	auto nodeSurface = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("surfaceToCut")->GetData());

	vtkNew<vtkPolyData> surfacePolyData;
	surfacePolyData->DeepCopy(nodeSurface->GetVtkPolyData());

	vtkNew<vtkTransformFilter> tmpTransFilter;
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->SetMatrix(nodeSurface->GetGeometry()->GetVtkMatrix());
	tmpTransFilter->SetTransform(tmpTransform);
	tmpTransFilter->SetInputData(surfacePolyData);
	tmpTransFilter->Update();

	auto objectSurface = mitk::Surface::New();
	objectSurface->SetVtkPolyData(tmpTransFilter->GetPolyDataOutput());

	vtkNew<vtkImageData> whiteImage;
	double imageBounds[6]{ 0 };
	double imageSpacing[3]{ 0.4, 0.4, 0.4 };
	whiteImage->SetSpacing(imageSpacing);

	auto geometry = objectSurface->GetGeometry();
	auto surfaceBounds = geometry->GetBounds();
	for (int n = 0; n < 6; n++)
	{
		imageBounds[n] = surfaceBounds[n];
	}

	int dim[3];
	for (int i = 0; i < 3; i++)
	{
		dim[i] = static_cast<int>(ceil((imageBounds[i * 2 + 1] - imageBounds[i * 2]) / imageSpacing[i]));
	}
	whiteImage->SetDimensions(dim);
	whiteImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

	double origin[3];
	origin[0] = imageBounds[0] + imageSpacing[0] / 2;
	origin[1] = imageBounds[2] + imageSpacing[1] / 2;
	origin[2] = imageBounds[4] + imageSpacing[2] / 2;
	whiteImage->SetOrigin(origin);
	whiteImage->AllocateScalars(VTK_SHORT, 1);

	// fill the image with foreground voxels:
	short insideValue = 2000;
	// short outsideValue = 0;
	vtkIdType count = whiteImage->GetNumberOfPoints();
	for (vtkIdType i = 0; i < count; ++i)
	{
		whiteImage->GetPointData()->GetScalars()->SetTuple1(i, insideValue);
	}

	auto imageToCrop = mitk::Image::New();
	imageToCrop->Initialize(whiteImage);
	imageToCrop->SetVolume(whiteImage->GetScalarPointer());


	// Apply the stencil
	mitk::SurfaceToImageFilter::Pointer surfaceToImageFilter = mitk::SurfaceToImageFilter::New();
	surfaceToImageFilter->SetBackgroundValue(0);
	surfaceToImageFilter->SetImage(imageToCrop);
	surfaceToImageFilter->SetInput(objectSurface);
	surfaceToImageFilter->SetReverseStencil(false);

	mitk::Image::Pointer convertedImage = mitk::Image::New();
	surfaceToImageFilter->Update();
	convertedImage = surfaceToImageFilter->GetOutput();

	// Test: set the boundary voxel of the image to 
	auto inputVtkImage = convertedImage->GetVtkImageData();
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	int tmpRegion[6]{ 0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1 };

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToInt();
	caster->Update();

	auto castVtkImage = caster->GetOutput();

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int x_p = ((x + 1) > (dims[0] - 1)) ? (dims[0] - 1) : (x + 1);
				int x_m = ((x - 1) < 0) ? 0 : (x - 1);

				int y_p = ((y + 1) > (dims[1] - 1)) ? (dims[1] - 1) : (y + 1);
				int y_m = ((y - 1) < 0) ? 0 : (y - 1);

				int z_p = ((z + 1) > (dims[2] - 1)) ? (dims[2] - 1) : (z + 1);
				int z_m = ((z - 1) < 0) ? z : (z - 1);

				int* n = static_cast<int*>(castVtkImage->GetScalarPointer(x, y, z));
				int* n1 = static_cast<int*>(castVtkImage->GetScalarPointer(x_m, y, z));
				int* n2 = static_cast<int*>(castVtkImage->GetScalarPointer(x_p, y, z));
				int* n3 = static_cast<int*>(castVtkImage->GetScalarPointer(x, y_m, z));
				int* n4 = static_cast<int*>(castVtkImage->GetScalarPointer(x, y_p, z));
				int* n5 = static_cast<int*>(castVtkImage->GetScalarPointer(x, y, z_m));
				int* n6 = static_cast<int*>(castVtkImage->GetScalarPointer(x, y, z_p));

				if (n[0] == 2000)
				{
					if (n1[0] == 0 || n2[0] == 0 || n3[0] == 0 || n4[0] == 0 || n5[0] == 0 || n6[0] == 0)
					{
						n[0] = 3000;
					}

					if (x == dims[0]-1 || x == 0 || y == dims[1]-1 || y == 0 || z == dims[2]-1 || z == 0)
					{
						n[0] = 3000;
					}
				}

			}
		}
	}

	auto resultMitkImage = mitk::Image::New();
	resultMitkImage->Initialize(castVtkImage);
	resultMitkImage->SetVolume(castVtkImage->GetScalarPointer());
	resultMitkImage->SetGeometry(convertedImage->GetGeometry());

	//-----------------------------------------------

	auto newNode = mitk::DataNode::New();

	newNode->SetName("proximalTibialImage");

	// add new node
	newNode->SetData(resultMitkImage);
	GetDataStorage()->Add(newNode);

}


void MoveData::on_pushButton_implantStencil_clicked()
{
	auto nodeSurface = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("implant")->GetData());

	vtkNew<vtkPolyData> surfacePolyData;
	surfacePolyData->DeepCopy(nodeSurface->GetVtkPolyData());

	vtkNew<vtkTransformFilter> tmpTransFilter;
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->SetMatrix(nodeSurface->GetGeometry()->GetVtkMatrix());
	tmpTransFilter->SetTransform(tmpTransform);
	tmpTransFilter->SetInputData(surfacePolyData);
	tmpTransFilter->Update();

	auto objectSurface = mitk::Surface::New();
	objectSurface->SetVtkPolyData(tmpTransFilter->GetPolyDataOutput());

	auto imageToCrop = GetDataStorage()->GetNamedObject<mitk::Image>("proximalTibialImage");

	mitk::SurfaceToImageFilter::Pointer surfaceToImageFilter = mitk::SurfaceToImageFilter::New();
	surfaceToImageFilter->SetBackgroundValue(0);
	surfaceToImageFilter->SetImage(imageToCrop);
	surfaceToImageFilter->SetInput(objectSurface);
	surfaceToImageFilter->SetReverseStencil(false);

	mitk::Image::Pointer convertedImage = mitk::Image::New();
	surfaceToImageFilter->Update();
	convertedImage = surfaceToImageFilter->GetOutput();


	// Test: set the boundary voxel of the image to 
	auto inputVtkImage = convertedImage->GetVtkImageData();
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	int tmpRegion[6]{ 0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1 };

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToInt();
	caster->Update();

	auto implantStencilImage = caster->GetOutput();

	auto boneVtkImage = imageToCrop->GetVtkImageData();
	

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int* n = static_cast<int*>(implantStencilImage->GetScalarPointer(x, y, z));
				int* m = static_cast<int*>(boneVtkImage->GetScalarPointer(x, y, z));

				if(m[0] > 0 && n[0] != 0)
				{
					m[0] = 1000;
				}

			}
		}
	}


	// 1-voxel layer white boundary
	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int x_p = ((x + 1) > (dims[0] - 1)) ? (dims[0] - 1) : (x + 1);
				int x_m = ((x - 1) < 0) ? 0 : (x - 1);

				int y_p = ((y + 1) > (dims[1] - 1)) ? (dims[1] - 1) : (y + 1);
				int y_m = ((y - 1) < 0) ? 0 : (y - 1);

				int z_p = ((z + 1) > (dims[2] - 1)) ? (dims[2] - 1) : (z + 1);
				int z_m = ((z - 1) < 0) ? z : (z - 1);

				int* n = static_cast<int*>(boneVtkImage->GetScalarPointer(x, y, z));
				int* n1 = static_cast<int*>(boneVtkImage->GetScalarPointer(x_m, y, z));
				int* n2 = static_cast<int*>(boneVtkImage->GetScalarPointer(x_p, y, z));
				int* n3 = static_cast<int*>(boneVtkImage->GetScalarPointer(x, y_m, z));
				int* n4 = static_cast<int*>(boneVtkImage->GetScalarPointer(x, y_p, z));
				int* n5 = static_cast<int*>(boneVtkImage->GetScalarPointer(x, y, z_m));
				int* n6 = static_cast<int*>(boneVtkImage->GetScalarPointer(x, y, z_p));

				if (n[0] == 2000)
				{
					if (n1[0] == 1000 || n2[0] == 1000 || n3[0] == 1000 || n4[0] == 1000 || n5[0] == 1000 || n6[0] == 1000)
					{
						n[0] = 3000;
					}

				}

			}
		}
	}

	auto resultMitkImage = mitk::Image::New();
	resultMitkImage->Initialize(boneVtkImage);
	resultMitkImage->SetVolume(boneVtkImage->GetScalarPointer());
	resultMitkImage->SetGeometry(convertedImage->GetGeometry());

	auto newNode = mitk::DataNode::New();

	newNode->SetName("implantStencilImage");

	// add new node
	newNode->SetData(resultMitkImage);
	GetDataStorage()->Add(newNode);

}


void MoveData::on_pushButton_level_clicked()
{
	auto image = GetDataStorage()->GetNamedObject<mitk::Image>("implantStencilImage");
	auto imageNode = GetDataStorage()->GetNamedNode("implantStencilImage");

	auto caster = vtkImageCast::New();
	caster->SetInputData(image->GetVtkImageData());
	caster->SetOutputScalarTypeToInt();
	caster->Update();


	auto vtkImage = caster->GetOutput();

	int dims[3];

	vtkImage->GetDimensions(dims);

	vtkNew<vtkImageData> whiteLayer_image;
	whiteLayer_image->DeepCopy(vtkImage);

	vtkNew<vtkImageData> redPart_image;
	redPart_image->DeepCopy(vtkImage);

	vtkNew<vtkImageData> greenPart_image;
	greenPart_image->DeepCopy(vtkImage);

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int* n = static_cast<int*>(vtkImage->GetScalarPointer(x, y, z));
				int* w = static_cast<int*>(whiteLayer_image->GetScalarPointer(x, y, z));
				int* r = static_cast<int*>(redPart_image->GetScalarPointer(x, y, z));
				int* g = static_cast<int*>(greenPart_image->GetScalarPointer(x, y, z));

				if(n[0] == 3000)
				{
					r[0] = 0;
					g[0] = 0;

					g[0] = 1700;
				}

				if(n[0] == 2000)
				{
					w[0] = 0;
					g[0] = 0;

					w[0] = 3000;
					g[0] = 1700;
				}

				if(n[0] == 1000)
				{
					w[0] = 0;
					r[0] = 0;

					w[0] = 2600;
				}

				if(n[0] == 0)
				{
					w[0] = 2600;
					g[0] = 1700;
				}
			
			}
		}
	}

	

	auto whiteMitkImage = mitk::Image::New();
	whiteMitkImage->Initialize(whiteLayer_image);
	whiteMitkImage->SetVolume(whiteLayer_image->GetScalarPointer());
	whiteMitkImage->SetGeometry(image->GetGeometry());

	auto whiteMitkImage_smoothed = mitk::Image::New();
	mitk::CLUtil::GaussianFilter(whiteMitkImage, whiteMitkImage_smoothed, 0.1);

	auto whiteNode = mitk::DataNode::New();

	whiteNode->SetName("white");

	whiteNode->SetData(whiteMitkImage_smoothed);
	GetDataStorage()->Add(whiteNode, imageNode);

	auto redMitkImage = mitk::Image::New();
	redMitkImage->Initialize(redPart_image);
	redMitkImage->SetVolume(redPart_image->GetScalarPointer());
	redMitkImage->SetGeometry(image->GetGeometry());

	auto redNode = mitk::DataNode::New();

	redNode->SetName("red");

	redNode->SetData(redMitkImage);
	GetDataStorage()->Add(redNode,imageNode);

	auto greenMitkImage = mitk::Image::New();
	greenMitkImage->Initialize(greenPart_image);
	greenMitkImage->SetVolume(greenPart_image->GetScalarPointer());
	greenMitkImage->SetGeometry(image->GetGeometry());

	auto greenMitkImage_smoothed = mitk::Image::New();
	mitk::CLUtil::GaussianFilter(greenMitkImage, greenMitkImage_smoothed, 0.1);
	
	auto greenNode = mitk::DataNode::New();

	greenNode->SetName("green");

	greenNode->SetData(greenMitkImage_smoothed);
	GetDataStorage()->Add(greenNode, imageNode);
}


void MoveData::on_pushButton_combine_clicked()
{
	auto redImage = GetDataStorage()->GetNamedObject<mitk::Image>("red");

	auto caster_red = vtkImageCast::New();
	caster_red->SetInputData(redImage->GetVtkImageData());
	caster_red->SetOutputScalarTypeToInt();
	caster_red->Update();

	auto redVtkImage = caster_red->GetOutput();

	int dims[3];

	redVtkImage->GetDimensions(dims);

	//----------
	auto whiteImage = GetDataStorage()->GetNamedObject<mitk::Image>("white");

	auto caster_white = vtkImageCast::New();
	caster_white->SetInputData(whiteImage->GetVtkImageData());
	caster_white->SetOutputScalarTypeToInt();
	caster_white->Update();

	auto whiteVtkImage = caster_white->GetOutput();

	// ------------------
	auto greenImage = GetDataStorage()->GetNamedObject<mitk::Image>("green");

	auto caster_green = vtkImageCast::New();
	caster_green->SetInputData(greenImage->GetVtkImageData());
	caster_green->SetOutputScalarTypeToInt();
	caster_green->Update();

	auto greenVtkImage = caster_green->GetOutput();

	vtkNew<vtkImageData> combinedVtkImage;
	combinedVtkImage->DeepCopy(redVtkImage);

	// --------------
	auto image = GetDataStorage()->GetNamedObject<mitk::Image>("implantStencilImage");

	auto caster = vtkImageCast::New();
	caster->SetInputData(image->GetVtkImageData());
	caster->SetOutputScalarTypeToInt();
	caster->Update();


	auto vtkImage = caster->GetOutput();

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int* n = static_cast<int*>(vtkImage->GetScalarPointer(x, y, z));
				int* m = static_cast<int*>(combinedVtkImage->GetScalarPointer(x, y, z));
				int* w = static_cast<int*>(whiteVtkImage->GetScalarPointer(x, y, z));
				int* r = static_cast<int*>(redVtkImage->GetScalarPointer(x, y, z));
				int* g = static_cast<int*>(greenVtkImage->GetScalarPointer(x, y, z));

				m[0] = n[0];

				// green part
				if(g[0] < 1695)
				{
					if(n[0] == 0 || n[0] == 1000)
					{
						m[0] = g[0];
					}					
				}

				// white part
				if(/*n[0] != 2000 &&*/ n[0] != 1000 && w[0] > 2600)
				{
					m[0] = w[0];
				}

				if(n[0] == 0 && m[0] == 0)
				{
					m[0] = 2150;
				}

				if(n[0] == 2000 && m[0] >= 2995)
				{
					m[0] = 1964;
				}

			}
		}
	}


	auto combinedMitkImage = mitk::Image::New();
	combinedMitkImage->Initialize(combinedVtkImage);
	combinedMitkImage->SetVolume(combinedVtkImage->GetScalarPointer());
	combinedMitkImage->SetGeometry(image->GetGeometry());

	auto newNode = mitk::DataNode::New();

	newNode->SetName("imageToCut");

	newNode->SetData(combinedMitkImage);
	GetDataStorage()->Add(newNode);

}


void MoveData::on_pushButton_hardenData_clicked()
{
	if(dynamic_cast<mitk::Surface*>(m_currentSelectedNode->GetData()) != nullptr)
	{
		vtkNew<vtkTransformFilter> tmpTransFilter;
		vtkNew<vtkTransform> tmpTransform;
		tmpTransform->SetMatrix(m_currentSelectedNode->GetData()->GetGeometry()->GetVtkMatrix());
		tmpTransFilter->SetTransform(tmpTransform);
		tmpTransFilter->SetInputData(dynamic_cast<mitk::Surface*>(m_currentSelectedNode->GetData())->GetVtkPolyData());
		tmpTransFilter->Update();

		auto tmpSurface = mitk::Surface::New();
		tmpSurface->SetVtkPolyData(tmpTransFilter->GetPolyDataOutput());
		m_currentSelectedNode->SetData(tmpSurface);
	}

	if (dynamic_cast<mitk::PointSet*>(m_currentSelectedNode->GetData()) != nullptr)
	{
		auto tmpPset = dynamic_cast<mitk::PointSet*>(m_currentSelectedNode->GetData());
		auto newPset = mitk::PointSet::New();

		for(int i{0}; i < tmpPset->GetSize(); i ++)
		{
			mitk::Point3D tmpPoint;
			tmpPoint = tmpPset->GetPoint(i);
			newPset->InsertPoint(tmpPoint);
		}

		m_currentSelectedNode->SetData(newPset);
	}
}


void MoveData::on_pushButton_addGizmo_clicked()
{

	if(mitk::Gizmo::HasGizmoAttached(m_currentSelectedNode, GetDataStorage()) == 1)
	{
		mitk::Gizmo::RemoveGizmoFromNode(m_currentSelectedNode, GetDataStorage());
	}else
	{
		mitk::Gizmo::AddGizmoToNode(m_currentSelectedNode, GetDataStorage());
	}
	
}


void MoveData::on_pushButton_testCrosshair_clicked()
{
	
	// Test vtkSplineFilter

	// vtkPoints
	if(GetDataStorage()->GetNamedNode("Dental curve") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("Dental curve missing");
		return;
	}

	auto mitkPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("Dental curve");

	vtkNew<vtkPoints> points;
	for (int i{0}; i < mitkPset->GetSize(); i++)
	{
		auto mitkPoint = mitkPset->GetPoint(i);
		points->InsertNextPoint(mitkPoint[0], mitkPoint[1], mitkPoint[2]);
	}

	// vtkCellArrays
	vtkNew<vtkCellArray> lines;
	lines->InsertNextCell(mitkPset->GetSize());
	for (unsigned int i = 0; i < mitkPset->GetSize(); ++i)
	{
		lines->InsertCellPoint(i);
	}

	// vtkPolyData
	auto polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);
	polyData->SetLines(lines);


	auto spline = vtkCardinalSpline::New();
	spline->SetLeftConstraint(2);
	spline->SetLeftValue(0.0);
	spline->SetRightConstraint(2);
	spline->SetRightValue(0.0);
	
	vtkNew<vtkSplineFilter> splineFilter;
	splineFilter->SetInputData(polyData);
	// splineFilter->SetNumberOfSubdivisions(polyData->GetNumberOfPoints() * 10);
	splineFilter->SetSubdivideToLength();
	splineFilter->SetLength(0.3);
	splineFilter->SetSpline(spline);
	splineFilter->Update();

	auto output = splineFilter->GetOutput();

	int ptNum = output->GetNumberOfPoints();
	auto outputPts = output->GetPoints();

	auto mitkPsetNew = mitk::PointSet::New();

	for(int i{0}; i < ptNum; i++)
	{
		mitk::Point3D a;
		a[0] = outputPts->GetPoint(i)[0];
		a[1] = outputPts->GetPoint(i)[1];
		a[2] = outputPts->GetPoint(i)[2];

		mitkPsetNew->InsertPoint(a);
	}



	// auto tmpSurface = mitk::Surface::New();
	// tmpSurface->SetVtkPolyData(output);

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetData(mitkPsetNew);
	tmpNode->SetName("Dental spline");
	tmpNode->SetFloatProperty("point 2D size", 0.2);

	GetDataStorage()->Add(tmpNode);
}


void MoveData::horizontalSlider_testCrosshair_value_changed()
{

	if(GetDataStorage()->GetNamedNode("Dental spline") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("Dental spline missing");
		return;
	}

	auto mitkPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("Dental spline");

	double sliderValue = m_Controls.horizontalSlider_testCrosshair->value();

	int currentIndex = floor(sliderValue*(mitkPset->GetSize()-1) / m_Controls.horizontalSlider_testCrosshair->maximum());

	if(currentIndex == (mitkPset->GetSize() - 1))
	{
		currentIndex -= 1;
	}

	auto currentPoint = mitkPset->GetPoint(currentIndex);
	auto nextPoint = mitkPset->GetPoint(currentIndex + 1);

		
	auto iRenderWindowPart = GetRenderWindowPart();

    QmitkRenderWindow* renderWindow = iRenderWindowPart->GetQmitkRenderWindow("coronal");
	if (renderWindow)
	{
		// m_Controls.textBrowser_moveData->append("Coronal exists");
	
		mitk::Vector3D a;
		a[0] = nextPoint[0]-currentPoint[0];
		a[1] = nextPoint[1] - currentPoint[1];
		a[2] = nextPoint[2] - currentPoint[2];
		a.Normalize();

		mitk::Vector3D b;
		b[0] = 0;
		b[1] = 0;
		b[2] = 1;

		renderWindow->ResetView();

		renderWindow->GetSliceNavigationController()->ReorientSlices(currentPoint,a,b);
		
	}


	
	renderWindow = iRenderWindowPart->GetQmitkRenderWindow("sagittal");
	if (renderWindow)
	{
		mitk::Vector3D a;
		a[1] = (nextPoint[0] - currentPoint[0]);
		a[0] = -(nextPoint[1] - currentPoint[1]);
		a[2] = nextPoint[2] - currentPoint[2];
		a.Normalize();

		mitk::Vector3D b;
		b[0] = 0;
		b[1] = 0;
		b[2] = 1;
		// m_Controls.textBrowser_moveData->append("Sagittal exists");

		
		renderWindow->GetSliceNavigationController()->ReorientSlices(currentPoint, a, b);
	}
	
	
	renderWindow = iRenderWindowPart->GetQmitkRenderWindow("axial");
	if (renderWindow)
	{
		mitk::Vector3D a;
		a[0] = 1;
		a[1] = 0;
		a[2] = 0;
		a.Normalize();

		mitk::Vector3D b;
		b[0] = 0;
		b[1] = -1;
		b[2] = 0;
		// m_Controls.textBrowser_moveData->append("axial exists");

		
		renderWindow->GetSliceNavigationController()->ReorientSlices(currentPoint, a, b);
	}

}


vtkSmartPointer<vtkPolyData> SweepLine_2Sides(vtkPolyData* line, double direction[3],
	double distance, unsigned int cols)
{
	unsigned int rows = line->GetNumberOfPoints();
	double spacing = distance / cols;
	vtkNew<vtkPolyData> surface;

	// Generate the points.
	cols++;
	unsigned int numberOfPoints = rows * cols;
	unsigned int numberOfPolys = (rows - 1) * (cols - 1);
	vtkNew<vtkPoints> points;
	points->Allocate(numberOfPoints);
	vtkNew<vtkCellArray> polys;
	polys->Allocate(numberOfPolys * 4);

	double x[3];
	unsigned int cnt = 0;
	for (unsigned int row = 0; row < rows; row++)
	{
		for (unsigned int col = 0; col < cols; col++)
		{
			double p[3];
			line->GetPoint(row, p);
			x[0] = p[0] - distance * direction[0] / 2 + direction[0] * col * spacing;
			x[1] = p[1] - distance * direction[1] / 2 + direction[1] * col * spacing;
			x[2] = p[2] - distance * direction[2] / 2 + direction[2] * col * spacing;
			points->InsertPoint(cnt++, x);
		}
	}
	// Generate the quads.
	vtkIdType pts[4];
	for (unsigned int row = 0; row < rows - 1; row++)
	{
		for (unsigned int col = 0; col < cols - 1; col++)
		{
			pts[0] = col + row * (cols);
			pts[1] = pts[0] + 1;
			pts[2] = pts[0] + cols + 1;
			pts[3] = pts[0] + cols;
			polys->InsertNextCell(4, pts);
		}
	}
	surface->SetPoints(points);
	surface->SetPolys(polys);

	// double x[3];
	// unsigned int cnt = 0;
	// for (unsigned int col = 0; col < cols; col++)
	// {
	// 	for (unsigned int row = 0; row < rows; row++)
	// 	{
	// 		double p[3];
	// 		line->GetPoint(row, p);
	// 		x[0] = p[0] - distance * direction[0] / 2 + direction[0] * col * spacing;
	// 		x[1] = p[1] - distance * direction[1] / 2 + direction[1] * col * spacing;
	// 		x[2] = p[2] - distance * direction[2] / 2 + direction[2] * col * spacing;
	// 		points->InsertPoint(cnt++, x);
	// 	}
	// }
	// // Generate the quads.
	// vtkIdType pts[4];
	// for (unsigned int col = 0; col < cols - 1; col++)
	// {
	// 	for (unsigned int row = 0; row < rows - 1; row++)
	// 	{
	// 		pts[0] = col + row * (cols);
	// 		pts[1] = pts[0] + 1;
	// 		pts[2] = pts[0] + cols + 1;
	// 		pts[3] = pts[0] + cols;
	// 		polys->InsertNextCell(4, pts);
	// 	}
	// }
	// surface->SetPoints(points);
	// surface->SetPolys(polys);

	return surface;
}

vtkSmartPointer<vtkPolyData> SweepLine(vtkPolyData* line, double direction[3],
	double distance, unsigned int cols)
{
	unsigned int rows = line->GetNumberOfPoints();
	double spacing = distance / cols;
	vtkNew<vtkPolyData> surface;

	// Generate the points.
	cols++;
	unsigned int numberOfPoints = rows * cols;
	unsigned int numberOfPolys = (rows - 1) * (cols - 1);
	vtkNew<vtkPoints> points;
	points->Allocate(numberOfPoints);
	vtkNew<vtkCellArray> polys;
	polys->Allocate(numberOfPolys * 4);

	double x[3];
	unsigned int cnt = 0;
	for (unsigned int row = 0; row < rows; row++)
	{
		for (unsigned int col = 0; col < cols; col++)
		{
			double p[3];
			line->GetPoint(row, p);
			x[0] = p[0] + direction[0] * col * spacing;
			x[1] = p[1] + direction[1] * col * spacing;
			x[2] = p[2] + direction[2] * col * spacing;
			points->InsertPoint(cnt++, x);
		}
	}
	// Generate the quads.
	vtkIdType pts[4];
	for (unsigned int row = 0; row < rows - 1; row++)
	{
		for (unsigned int col = 0; col < cols - 1; col++)
		{
			pts[0] = col + row * (cols);
			pts[1] = pts[0] + 1;
			pts[2] = pts[0] + cols + 1;
			pts[3] = pts[0] + cols;
			polys->InsertNextCell(4, pts);
		}
	}
	surface->SetPoints(points);
	surface->SetPolys(polys);

	return surface;
}

vtkSmartPointer<vtkPolyData> ExpandSpline(vtkPolyData* line, int divisionNum,
	double stepSize)
{

	vtkNew<vtkPoints> points;
	for (int i{ 0 }; i < line->GetNumberOfPoints(); i++)
	{
		Eigen::Vector3d currentPoint;
		currentPoint[0] = line->GetPoint(i)[0];
		currentPoint[1] = line->GetPoint(i)[1];
		currentPoint[2] = line->GetPoint(i)[2];

		Eigen::Vector3d z_axis;
		z_axis[0] = 0;
		z_axis[1] = 0;
		z_axis[2] = 1;

		Eigen::Vector3d ptpVector;

		if (i == (line->GetNumberOfPoints() - 1))
		{
			ptpVector[0] = -line->GetPoint(i-1)[0] + currentPoint[0];
			ptpVector[1] = -line->GetPoint(i-1)[1] + currentPoint[1];
			ptpVector[2] = -line->GetPoint(i-1)[2] + currentPoint[2];
		}else
		{
			ptpVector[0] = line->GetPoint(i + 1)[0] - currentPoint[0];
			ptpVector[1] = line->GetPoint(i + 1)[1] - currentPoint[1];
			ptpVector[2] = line->GetPoint(i + 1)[2] - currentPoint[2];
		}

		Eigen::Vector3d tmpVector;

		tmpVector = z_axis.cross(ptpVector);

		tmpVector.normalize();

		points->InsertNextPoint(currentPoint[0] + tmpVector[0] * stepSize,
			currentPoint[1] + tmpVector[1] * stepSize, 
			currentPoint[2] + tmpVector[2] * stepSize);

	}

	// vtkCellArrays
	vtkNew<vtkCellArray> lines;
	lines->InsertNextCell(points->GetNumberOfPoints());
	for (unsigned int i = 0; i < points->GetNumberOfPoints(); ++i)
	{
		lines->InsertCellPoint(i);
	}

	// vtkPolyData
	auto polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);
	polyData->SetLines(lines);

	auto spline = vtkCardinalSpline::New();
	spline->SetLeftConstraint(2);
	spline->SetLeftValue(0.0);
	spline->SetRightConstraint(2);
	spline->SetRightValue(0.0);

	vtkNew<vtkSplineFilter> splineFilter;
	splineFilter->SetInputData(polyData);
	splineFilter->SetSubdivideToSpecified();
	splineFilter->SetNumberOfSubdivisions(divisionNum);
	splineFilter->SetSpline(spline);
	splineFilter->Update();

	auto spline_PolyData = splineFilter->GetOutput();

	return spline_PolyData;
}

// void MoveData::on_pushButton_testCPR_clicked()
// {
// 	if (GetDataStorage()->GetNamedNode("Dental curve") == nullptr)
// 	{
// 		m_Controls.textBrowser_moveData->append("Dental curve missing");
// 		return;
// 	}
//
// 	auto mitkPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("Dental curve");
//
// 	vtkNew<vtkPoints> points;
// 	for (int i{ 0 }; i < mitkPset->GetSize(); i++)
// 	{
// 		auto mitkPoint = mitkPset->GetPoint(i);
// 		points->InsertNextPoint(mitkPoint[0], mitkPoint[1], mitkPoint[2]);
// 	}
//
// 	// vtkCellArrays
// 	vtkNew<vtkCellArray> lines;
// 	lines->InsertNextCell(mitkPset->GetSize());
// 	for (unsigned int i = 0; i < mitkPset->GetSize(); ++i)
// 	{
// 		lines->InsertCellPoint(i);
// 	}
//
// 	// vtkPolyData
// 	auto polyData = vtkSmartPointer<vtkPolyData>::New();
// 	polyData->SetPoints(points);
// 	polyData->SetLines(lines);
//
//
// 	auto spline = vtkCardinalSpline::New();
// 	spline->SetLeftConstraint(2);
// 	spline->SetLeftValue(0.0);
// 	spline->SetRightConstraint(2);
// 	spline->SetRightValue(0.0);
//
// 	double segLength{ 0.3 };
//
// 	vtkNew<vtkSplineFilter> splineFilter;
// 	splineFilter->SetInputData(polyData);
// 	splineFilter->SetLength(segLength);
// 	splineFilter->SetSubdivideToLength();
// 	splineFilter->SetSpline(spline);
// 	splineFilter->Update();
// 	
// 	auto spline_PolyData = splineFilter->GetOutput();
//
// 	auto expandedSpline = ExpandSpline(spline_PolyData, spline_PolyData->GetNumberOfPoints()-1, 3);
//
// 	auto mitkSurface = mitk::Surface::New();
// 	mitkSurface->SetVtkPolyData(expandedSpline);
//
// 	auto mitkNode = mitk::DataNode::New();
//
// 	mitkNode->SetData(mitkSurface);
// 	mitkNode->SetName("test Expand");
// 	GetDataStorage()->Add(mitkNode);
//
// 	// Sweep the line to form a surface.
// 	double direction[3];
// 	direction[0] = 0.0;
// 	direction[1] = 0.0;
// 	direction[2] = 1.0;
// 	unsigned cols = 400;
//
// 	double distance = cols * segLength;
// 	auto surface =
// 		SweepLine(spline_PolyData, direction, distance, cols);
//
// 	auto tmpSurface = mitk::Surface::New();
// 	tmpSurface->SetVtkPolyData(surface);
// 	
// 	auto tmpNode1 = mitk::DataNode::New();
// 	tmpNode1->SetData(tmpSurface);
// 	tmpNode1->SetName("probeSurface");
// 	GetDataStorage()->Add(tmpNode1);
//
//
// 	// Probe the volume with the extruded surface.
//
// 	auto vtkImage = GetDataStorage()->GetNamedObject<mitk::Image>("Image")->GetVtkImageData();
// 	
// 	vtkNew<vtkProbeFilter> sampleVolume;
// 	sampleVolume->SetSourceData(vtkImage);
// 	sampleVolume->SetInputData(surface);
// 	
// 	sampleVolume->Update();
//
// 	auto probeData = sampleVolume->GetOutput();
//
// 	auto probePointData = probeData->GetPointData();
//
// 	auto tmpArray = probePointData->GetScalars();
// 	
// 	auto testimageData = vtkImageData::New();
// 	testimageData->SetDimensions(cols+1, spline_PolyData->GetNumberOfPoints(), 1);
//
// 	testimageData->SetSpacing(segLength, segLength, 1);
// 	testimageData->SetOrigin(0,0,0);
// 	testimageData->AllocateScalars(VTK_INT, 1);
// 	testimageData->GetPointData()->SetScalars(tmpArray);
//
// 	auto tmpImage = mitk::Image::New();
//
// 	if (testimageData == nullptr)
// 	{
// 		m_Controls.textBrowser_moveData->append("no imageData");
// 		return;
// 	}
//
// 	tmpImage->Initialize(testimageData);
// 	tmpImage->SetVolume(testimageData->GetScalarPointer());
//
// 	auto tmpNode = mitk::DataNode::New();
// 	tmpNode->SetData(tmpImage);
// 	tmpNode->SetName("Oral Panorama");
// 	
// 	GetDataStorage()->Add(tmpNode);
// }

void MoveData::on_pushButton_testCPR_clicked()
{
	if (GetDataStorage()->GetNamedNode("Dental curve") == nullptr)
	{
		m_Controls.textBrowser_moveData->append("Dental curve missing");
		return;
	}

	auto mitkPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("Dental curve");

	vtkNew<vtkPoints> points;
	for (int i{ 0 }; i < mitkPset->GetSize(); i++)
	{
		auto mitkPoint = mitkPset->GetPoint(i);
		points->InsertNextPoint(mitkPoint[0], mitkPoint[1], mitkPoint[2]);
	}

	// vtkCellArrays
	vtkNew<vtkCellArray> lines;
	lines->InsertNextCell(mitkPset->GetSize());
	for (unsigned int i = 0; i < mitkPset->GetSize(); ++i)
	{
		lines->InsertCellPoint(i);
	}

	// vtkPolyData
	auto polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);
	polyData->SetLines(lines);


	auto spline = vtkCardinalSpline::New();
	spline->SetLeftConstraint(2);
	spline->SetLeftValue(0.0);
	spline->SetRightConstraint(2);
	spline->SetRightValue(0.0);

	double segLength{ 0.3 };

	vtkNew<vtkSplineFilter> splineFilter;
	splineFilter->SetInputData(polyData);
	splineFilter->SetLength(segLength);
	splineFilter->SetSubdivideToLength();
	splineFilter->SetSpline(spline);
	splineFilter->Update();
	
	auto spline_PolyData = splineFilter->GetOutput();


	vtkSmartPointer<vtkImageAppend> append = vtkSmartPointer<vtkImageAppend >::New();

	append->SetAppendAxis(2);

	int thickness{ 30 };

	auto vtkImage = GetDataStorage()->GetNamedObject<mitk::Image>("Image")->GetVtkImageData();

	// Note: the vtkImage above may not occupy the same space as the mitk Image because "GetVtkImageData()" loses the MITK geometry transform
	// therefore we have to apply the inverse transform to the probe polydata instead

	// auto mitkAppendedImage_ = mitk::Image::New();
	//
	// mitkAppendedImage_->Initialize(vtkImage);
	// mitkAppendedImage_->SetVolume(vtkImage->GetScalarPointer());
	//
	// auto tmpNode__ = mitk::DataNode::New();
	// tmpNode__->SetData(mitkAppendedImage_);
	// tmpNode__->SetName("vtkImage");
	// GetDataStorage()->Add(tmpNode__);

	for (int i{0}; i < 2* thickness; i++)
	{
		double stepSize = segLength * (-thickness + i);

		auto expandedSpline = ExpandSpline(spline_PolyData, spline_PolyData->GetNumberOfPoints() - 1, stepSize);

		

		// Sweep the line to form a surface.
		double direction[3];
		direction[0] = 0.0;
		direction[1] = 0.0;
		direction[2] = 1.0;
		unsigned cols = 250;

		double distance = cols * segLength;
		auto surface =
			SweepLine_2Sides(expandedSpline, direction, distance, cols);
			   
		// Apply the inverse geometry of the MITK image to the probe surface
		auto geometryMatrix = vtkMatrix4x4::New();
		geometryMatrix->DeepCopy(GetDataStorage()->GetNamedObject<mitk::Image>("Image")->GetGeometry()->GetVtkMatrix());
		auto spacing = GetDataStorage()->GetNamedObject<mitk::Image>("Image")->GetGeometry()->GetSpacing();

		for(int j{0}; j < 3; j++)
		{
			geometryMatrix->SetElement(j, 0, geometryMatrix->GetElement(j, 0) / spacing[0]);
			geometryMatrix->SetElement(j, 1, geometryMatrix->GetElement(j, 1) / spacing[1]);
			geometryMatrix->SetElement(j, 2, geometryMatrix->GetElement(j, 2) / spacing[2]);
		}

		geometryMatrix->Invert();

		vtkNew<vtkTransformFilter> tmpTransFilter;
		vtkNew<vtkTransform> tmpTransform;
		tmpTransform->SetMatrix(geometryMatrix);
		tmpTransFilter->SetTransform(tmpTransform);
		tmpTransFilter->SetInputData(surface);
		tmpTransFilter->Update();

		// auto testSpline = mitk::Surface::New();
		// testSpline->SetVtkPolyData(tmpTransFilter->GetPolyDataOutput());
		// auto testNode = mitk::DataNode::New();
		// testNode->SetData(testSpline);
		// testNode->SetName("surface");
		// GetDataStorage()->Add(testNode);


		vtkNew<vtkProbeFilter> sampleVolume;
		sampleVolume->SetSourceData(vtkImage);
		sampleVolume->SetInputData(tmpTransFilter->GetPolyDataOutput());

		sampleVolume->Update();

		auto probeData = sampleVolume->GetOutput();

		// probeData test show below:

		if( i == thickness)
		{
			vtkNew<vtkProbeFilter> sampleVolume_;
			sampleVolume_->SetSourceData(vtkImage);
			tmpTransFilter->SetTransform(tmpTransform);
			tmpTransFilter->SetInputData(GetDataStorage()->GetNamedObject<mitk::Surface>("probe surface")->GetVtkPolyData());
			tmpTransFilter->Update();
			sampleVolume_->SetInputData(tmpTransFilter->GetOutput());

			sampleVolume_->Update();

			auto probeData_ = sampleVolume_->GetOutput();

			vtkNew<vtkWindowLevelLookupTable> wlLut;
			// double range = 5837;
			// double level = 1919;
			double range = 1000;
			double level = 500;
			wlLut->SetWindow(range);
			wlLut->SetLevel(level);
		
			// Create a mapper and actor.
			vtkNew<vtkDataSetMapper> mapper;
			mapper->SetInputData(probeData_);
			mapper->SetLookupTable(wlLut);
			//mapper->SetScalarRange(-500, 1500);
			mapper->SetScalarRange(-700, 2000);
		
			vtkNew<vtkActor> actor;
			actor->SetMapper(mapper);
		
			auto iRenderWindowPart = GetRenderWindowPart();
		
			QmitkRenderWindow* mitkRenderWindow = iRenderWindowPart->GetQmitkRenderWindow("3d");
		
			auto renderWindow = mitkRenderWindow->GetVtkRenderWindow();
		
			vtkNew<vtkRenderer> renderer;
		
			renderWindow->AddRenderer(renderer);
			
			renderer->AddActor(actor);
			renderer->SetBackground(255,255,255);
		
			// // Create a renderer, render window, and interactor.
			// vtkNew<vtkRenderer> renderer;
			// vtkNew<vtkRenderWindow> renderWindow;
			// renderWindow->AddRenderer(renderer);
			// renderWindow->SetWindowName("CurvedReformation");
			//
			// // Add the actors to the scene.
			// renderer->AddActor(actor);
			// renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
			//
			// // Set the camera for viewing medical images.
			// renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
			// renderer->GetActiveCamera()->SetPosition(0, 0, 0);
			// renderer->GetActiveCamera()->SetFocalPoint(0, 1, 0);
			// renderer->ResetCamera();
			//
			// // Render and interact
			// renderWindow->Render();
			// renderWindowInteractor->Start();
		
			// probeData test show above
		}
		



		auto probePointData = probeData->GetPointData();

		auto tmpArray = probePointData->GetScalars();

		auto testimageData = vtkImageData::New();
		testimageData->SetDimensions(cols + 1, spline_PolyData->GetNumberOfPoints(), 1);
		//testimageData->SetDimensions( spline_PolyData->GetNumberOfPoints(), cols + 1, 1);


		testimageData->SetSpacing(segLength, segLength, 1);
		testimageData->SetOrigin(0, 0, 0);
		testimageData->AllocateScalars(VTK_INT, 1);
		testimageData->GetPointData()->SetScalars(tmpArray);

		// auto tmpImage = vtkImageData::New();
		// tmpImage->DeepCopy(testimageData);

		append->AddInputData(testimageData);
		
	}

	append->Update();
	auto appenedImage = append->GetOutput();
	appenedImage->SetSpacing(segLength, segLength, segLength);
	
	auto mitkAppendedImage = mitk::Image::New();

	mitkAppendedImage->Initialize(appenedImage);
	mitkAppendedImage->SetVolume(appenedImage->GetScalarPointer());

	// Rotate the image by -90 degree along the +z axis
	auto rotateTrans = vtkTransform::New();
	rotateTrans->PostMultiply();
	rotateTrans->SetMatrix(mitkAppendedImage->GetGeometry()->GetVtkMatrix());
	rotateTrans->RotateZ(-90);
	rotateTrans->Update();

	mitkAppendedImage->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(rotateTrans->GetMatrix());

	auto tmpNode_ = mitk::DataNode::New();
	tmpNode_->SetData(mitkAppendedImage);
	tmpNode_->SetName("Panorama");

	

	GetDataStorage()->Add(tmpNode_);

	
}
