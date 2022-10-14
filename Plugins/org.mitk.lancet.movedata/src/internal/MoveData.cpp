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
#include <mitkImage.h>

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
#include "QmitkSingleNodeSelectionWidget.h"
#include "QmitkDataStorageTreeModel.h"
#include "QmitkDataStorageTreeModelInternalItem.h"
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








