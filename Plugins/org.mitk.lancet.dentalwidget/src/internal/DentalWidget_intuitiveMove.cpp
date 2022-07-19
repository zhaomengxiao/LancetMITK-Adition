


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "DentalWidget.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include "mitkInteractionConst.h"
#include "mitkRotationOperation.h"

void DentalWidget::Translate(double direction[3], double length, mitk::BaseData* data)
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
	}
	else
	{
		m_Controls.textBrowser->append("Empty input. Please select a node ~");
	}

}

void DentalWidget::Rotate(double center[3], double direction[3], double counterclockwiseDegree, mitk::BaseData* data)
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
		m_Controls.textBrowser->append("Empty input. Please select a node ~");
	}
}
void DentalWidget::TranslateMinusX()
{
	double direction[3]{ -1,0,0 };

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
void DentalWidget::TranslateMinusY()
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
void DentalWidget::TranslateMinusZ()
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
void DentalWidget::TranslatePlusX()
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
void DentalWidget::TranslatePlusY()
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
void DentalWidget::TranslatePlusZ()
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
void DentalWidget::RotatePlusX()
{
	if (m_baseDataToMove == nullptr)
	{
		m_Controls.textBrowser->append("Empty input. Please select a node ~");
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
void DentalWidget::RotatePlusY()
{
	if (m_baseDataToMove == nullptr)
	{
		m_Controls.textBrowser->append("Empty input. Please select a node ~");
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
void DentalWidget::RotatePlusZ()
{
	if (m_baseDataToMove == nullptr)
	{
		m_Controls.textBrowser->append("Empty input. Please select a node ~");
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
void DentalWidget::RotateMinusX()
{
	if (m_baseDataToMove == nullptr)
	{
		m_Controls.textBrowser->append("Empty input. Please select a node ~");
		return;
	}

	double direction[3]{ 1,0,0 };
	double angle = -m_Controls.lineEdit_intuitiveValue_1->text().toDouble();
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
void DentalWidget::RotateMinusY()
{
	if (m_baseDataToMove == nullptr)
	{
		m_Controls.textBrowser->append("Empty input. Please select a node ~");
		return;
	}

	double direction[3]{ 0,1,0 };
	double angle = -m_Controls.lineEdit_intuitiveValue_1->text().toDouble();
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
void DentalWidget::RotateMinusZ()
{
	if (m_baseDataToMove == nullptr)
	{
		m_Controls.textBrowser->append("Empty input. Please select a node ~");
		return;
	}

	double direction[3]{ 0,0,1 };
	double angle = -m_Controls.lineEdit_intuitiveValue_1->text().toDouble();
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





