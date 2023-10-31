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
#include <vtkCardinalSpline.h>
#include <vtkSplineFilter.h>
#include <vtkProbeFilter.h>
#include <vtkImageSlabReslice.h>
#include <vtkImageAppend.h>

#include "mitkSurfaceToImageFilter.h"
#include <ep/include/vtk-9.1/vtkTransformFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkImageCast.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkPointData.h>

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

	int thickness{ 20 };

	auto vtkImage = GetDataStorage()->GetNamedObject<mitk::Image>("Image")->GetVtkImageData();

	for (int i{0}; i < 2* thickness; i++)
	{
		double stepSize = segLength * (-thickness + i);

		auto expandedSpline = ExpandSpline(spline_PolyData, spline_PolyData->GetNumberOfPoints() - 1, stepSize);
		
		// Sweep the line to form a surface.
		double direction[3];
		direction[0] = 0.0;
		direction[1] = 0.0;
		direction[2] = 1.0;
		unsigned cols = 300;

		double distance = cols * segLength;
		auto surface =
			SweepLine(expandedSpline, direction, distance, cols);

		vtkNew<vtkProbeFilter> sampleVolume;
		sampleVolume->SetSourceData(vtkImage);
		sampleVolume->SetInputData(surface);

		sampleVolume->Update();

		auto probeData = sampleVolume->GetOutput();

		auto probePointData = probeData->GetPointData();

		auto tmpArray = probePointData->GetScalars();

		auto testimageData = vtkImageData::New();
		testimageData->SetDimensions(cols + 1, spline_PolyData->GetNumberOfPoints(), 1);

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

	auto tmpNode_ = mitk::DataNode::New();
	tmpNode_->SetData(mitkAppendedImage);
	tmpNode_->SetName("Appended Image");

	GetDataStorage()->Add(tmpNode_);

	// auto expandedSpline = ExpandSpline(spline_PolyData, spline_PolyData->GetNumberOfPoints()-1, 3);


	// Sweep the line to form a surface.
	double direction[3];
	direction[0] = 0.0;
	direction[1] = 0.0;
	direction[2] = 1.0;
	unsigned cols = 300;

	double distance = cols * segLength;
	auto surface =
		SweepLine(spline_PolyData, direction, distance, cols);

	auto tmpSurface = mitk::Surface::New();
	tmpSurface->SetVtkPolyData(surface);
	
	auto tmpNode1 = mitk::DataNode::New();
	tmpNode1->SetData(tmpSurface);
	tmpNode1->SetName("probeSurface");
	GetDataStorage()->Add(tmpNode1);


	// Probe the volume with the extruded surface.

	
	vtkNew<vtkProbeFilter> sampleVolume;
	sampleVolume->SetSourceData(vtkImage);
	sampleVolume->SetInputData(surface);
	
	sampleVolume->Update();

	auto probeData = sampleVolume->GetOutput();

	auto probePointData = probeData->GetPointData();

	auto tmpArray = probePointData->GetScalars();
	
	auto testimageData = vtkImageData::New();
	testimageData->SetDimensions(cols+1, spline_PolyData->GetNumberOfPoints(), 1);

	testimageData->SetSpacing(segLength, segLength, 1);
	testimageData->SetOrigin(0,0,0);
	testimageData->AllocateScalars(VTK_INT, 1);
	testimageData->GetPointData()->SetScalars(tmpArray);

	auto tmpImage = mitk::Image::New();

	if (testimageData == nullptr)
	{
		m_Controls.textBrowser_moveData->append("no imageData");
		return;
	}

	tmpImage->Initialize(testimageData);
	tmpImage->SetVolume(testimageData->GetScalarPointer());

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetData(tmpImage);
	tmpNode->SetName("Oral Panorama");
	
	GetDataStorage()->Add(tmpNode);
}
