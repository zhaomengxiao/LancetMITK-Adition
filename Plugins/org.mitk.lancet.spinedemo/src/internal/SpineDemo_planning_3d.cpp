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
#include "SpineDemo.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <vtkCamera.h>
#include <vtkRendererCollection.h>

#include "mitkGizmo.h"
#include "QmitkRenderWindow.h"

void SpineDemo::on_pushButton_startPlan_3d_clicked()
{
	// Step 1: move the implant to the trajectory
	MoveImplantToTrajectory();

	// Step 2: set up a gizmo for the implant
	mitk::Gizmo::RemoveGizmoFromNode(GetDataStorage()->GetNamedNode("implant"), GetDataStorage()); // in case there's already a gizmo
	mitk::Gizmo::AddGizmoToNode(GetDataStorage()->GetNamedNode("implant"), GetDataStorage());

	// Step 3: set the MPR to focus on the implant
	FocusOnImplant();

}

void SpineDemo::FocusOnImplant()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("roi_implantMPR") == nullptr)
	{
		m_Controls.textBrowser->append("roi_implantMPR is missing");
		return;
	}

	auto iRenderWindowPart = GetRenderWindowPart();

	// Turn off all
	auto dataNodes = GetDataStorage()->GetAll();
	for (auto item = dataNodes->begin(); item != dataNodes->end(); ++item)
	{
		(*item)->SetVisibility(false);
	}

	GetDataStorage()->GetNamedNode("stdmulti.widget0.plane")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("stdmulti.widget1.plane")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("stdmulti.widget2.plane")->SetVisibility(true);


	GetDataStorage()->GetNamedNode("roi_implantMPR")->SetVisibility(true);

	auto implantMatrix = GetDataStorage()->GetNamedNode("implant")->GetData()->GetGeometry()->GetVtkMatrix();

	mitk::Point3D implantPoint = GetDataStorage()->GetNamedNode("implant")->GetData()->GetGeometry()->GetCenter();

	auto tmpMatrix = vtkMatrix4x4::New();
	tmpMatrix->Identity();
	tmpMatrix->SetElement(0, 3, implantPoint[0]);
	tmpMatrix->SetElement(1, 3, implantPoint[1]);
	tmpMatrix->SetElement(2, 3, implantPoint[2]);


	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };


	Eigen::Vector3d x_implant{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	x_implant.normalize();

	if (x_implant.dot(x_std) < 0)
	{
		x_implant = -x_implant;
	}

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}


	// Eigen::Vector3d y_std{ implantMatrix->GetElement(1,0),implantMatrix->GetElement(1,1),0 };
	// y_std.normalize();

	Eigen::Vector3d y_mpr = z_mpr.cross(x_implant);
	y_mpr.normalize();

	Eigen::Vector3d x_mpr = y_mpr.cross(z_mpr);
	x_mpr.normalize();

	GetDataStorage()->GetNamedNode("roi_implantMPR")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpMatrix);

	iRenderWindowPart->SetSelectedPosition(implantPoint);

	QmitkRenderWindow* renderWindow = iRenderWindowPart->GetQmitkRenderWindow("3d");

	if (renderWindow)
	{

		auto camera = renderWindow->GetVtkRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();

		auto camera_copy = vtkCamera::New();

		camera_copy->DeepCopy(camera);

		renderWindow->ResetView();

		renderWindow->GetVtkRenderWindow()->GetRenderers()->GetFirstRenderer()->SetActiveCamera(camera_copy);

	}

	renderWindow = iRenderWindowPart->GetQmitkRenderWindow("coronal");
	if (renderWindow)
	{
		// Eigen::Vector3d x_projection = x_std - y_mpr * (x_std.dot(y_mpr));
		// x_projection.normalize();

		Eigen::Vector3d x_projection = y_mpr.cross(z_std);

		Eigen::Vector3d z_projection = x_projection.cross(y_mpr);
		z_projection.normalize();

		mitk::Vector3D a;
		a[0] = x_projection[0];
		a[1] = x_projection[1];
		a[2] = x_projection[2];


		mitk::Vector3D b;
		b[0] = z_projection[0];
		b[1] = z_projection[1];
		b[2] = z_projection[2];

		// renderWindow->ResetView();
		// renderWindow->CrosshairVisibilityChanged(false);

		renderWindow->GetSliceNavigationController()->ReorientSlices(implantPoint, a, b);

		// mitk::Point3D origin;
		// FillVector3D(origin, 0.0, 0.0, 0.0);
		//renderWindow->GetSliceNavigationController()->ReorientSlices(origin, a, b);
		//renderWindow->GetSliceNavigationController()->SelectSliceByPoint(worldPoint);


	}


	renderWindow = iRenderWindowPart->GetQmitkRenderWindow("sagittal");
	if (renderWindow)
	{
		// Eigen::Vector3d y_projection = y_std - x_mpr * (y_std.dot(x_mpr));
		// y_projection.normalize();

		Eigen::Vector3d y_projection = z_std.cross(x_mpr);

		Eigen::Vector3d z_projection = x_mpr.cross(y_projection);
		y_projection.normalize();

		mitk::Vector3D a;
		a[0] = y_projection[0];
		a[1] = y_projection[1];
		a[2] = y_projection[2];


		mitk::Vector3D b;
		b[0] = z_projection[0];
		b[1] = z_projection[1];
		b[2] = z_projection[2];

		renderWindow->GetSliceNavigationController()->ReorientSlices(implantPoint, a, b);


		// mitk::Point3D origin;
		// FillVector3D(origin, 0.0, 0.0, 0.0);
		//renderWindow->GetSliceNavigationController()->ReorientSlices(origin, a, b);
		//renderWindow->GetSliceNavigationController()->SelectSliceByPoint(worldPoint);
	}


	renderWindow = iRenderWindowPart->GetQmitkRenderWindow("axial");
	if (renderWindow)
	{
		// Eigen::Vector3d x_projection = x_std - z_mpr * (x_std.dot(z_mpr));
		// x_projection.normalize();

		Eigen::Vector3d x_projection = y_std.cross(z_mpr);

		Eigen::Vector3d y_projection = z_mpr.cross(x_projection);
		y_projection.normalize();

		mitk::Vector3D a;
		a[0] = x_projection[0];
		a[1] = x_projection[1];
		a[2] = x_projection[2];


		mitk::Vector3D b;
		b[0] = -y_projection[0];
		b[1] = -y_projection[1];
		b[2] = -y_projection[2];

		renderWindow->GetSliceNavigationController()->ReorientSlices(implantPoint, a, b);

	}

	GetDataStorage()->GetNamedNode("roi_implantMPR")->SetVisibility(false);
	GetDataStorage()->GetNamedNode("implant")->SetVisibility(true);

	if (GetDataStorage()->GetNamedNode("Gizmo") != nullptr)
	{
		GetDataStorage()->GetNamedNode("Gizmo")->SetVisibility(true);
	}

	GetDataStorage()->GetNamedNode("CT")->SetVisibility(true);
}


void SpineDemo::on_pushButton_U_ax_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	Eigen::Vector3d x_projection = x_std - z_mpr * (x_std.dot(z_mpr));
	x_projection.normalize();

	Eigen::Vector3d y_projection = z_mpr.cross(x_projection);
	y_projection.normalize();


	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(-y_mpr[0] * stepSize,
		-y_mpr[1] * stepSize,
		-y_mpr[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_D_ax_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();


	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(y_mpr[0] * stepSize,
		y_mpr[1] * stepSize,
		y_mpr[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_R_ax_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(x_mpr[0] * stepSize,
		x_mpr[1] * stepSize,
		x_mpr[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_L_ax_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(-x_mpr[0] * stepSize,
		-x_mpr[1] * stepSize,
		-x_mpr[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_U_sag_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(z_mpr[0] * stepSize,
		z_mpr[1] * stepSize,
		z_mpr[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_U_cor_clicked()
{
	on_pushButton_U_sag_clicked();
}
void SpineDemo::on_pushButton_D_sag_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}


	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(-z_mpr[0] * stepSize,
		-z_mpr[1] * stepSize,
		-z_mpr[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_D_cor_clicked()
{
	on_pushButton_D_sag_clicked();
}
void SpineDemo::on_pushButton_L_cor_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	Eigen::Vector3d leftSide = z_mpr.cross(y_mpr);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(leftSide[0] * stepSize,
		leftSide[1] * stepSize,
		leftSide[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_R_cor_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	Eigen::Vector3d rightSide = -z_mpr.cross(y_mpr);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(rightSide[0] * stepSize,
		rightSide[1] * stepSize,
		rightSide[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_R_sag_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	Eigen::Vector3d rightSide = z_mpr.cross(x_mpr);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(rightSide[0] * stepSize,
		rightSide[1] * stepSize,
		rightSide[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_L_sag_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	Eigen::Vector3d leftSide = -z_mpr.cross(x_mpr);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(leftSide[0] * stepSize,
		leftSide[1] * stepSize,
		leftSide[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_clock_cor_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	auto maxBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantEndPoint = tmpPset->GetPoint(0);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	// tmpTrans->Translate(leftSide[0] * stepSize,
	// 	leftSide[1] * stepSize,
	// 	leftSide[2] * stepSize);
	tmpTrans->Translate(-implantEndPoint[0], -implantEndPoint[1], -implantEndPoint[2]);
	tmpTrans->RotateWXYZ(stepSize, y_mpr[0], y_mpr[1], y_mpr[2]);
	tmpTrans->Translate(implantEndPoint[0], implantEndPoint[1], implantEndPoint[2]);

	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_counter_cor_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	auto maxBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantEndPoint = tmpPset->GetPoint(0);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	// tmpTrans->Translate(leftSide[0] * stepSize,
	// 	leftSide[1] * stepSize,
	// 	leftSide[2] * stepSize);
	tmpTrans->Translate(-implantEndPoint[0], -implantEndPoint[1], -implantEndPoint[2]);
	tmpTrans->RotateWXYZ(-stepSize, y_mpr[0], y_mpr[1], y_mpr[2]);
	tmpTrans->Translate(implantEndPoint[0], implantEndPoint[1], implantEndPoint[2]);

	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_clock_sag_clicked()
{

	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	Eigen::Vector3d x_std{ 1,0,0 };

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	auto maxBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantEndPoint = tmpPset->GetPoint(0);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	// tmpTrans->Translate(leftSide[0] * stepSize,
	// 	leftSide[1] * stepSize,
	// 	leftSide[2] * stepSize);
	tmpTrans->Translate(-implantEndPoint[0], -implantEndPoint[1], -implantEndPoint[2]);
	tmpTrans->RotateWXYZ(-stepSize, x_mpr[0], x_mpr[1], x_mpr[2]);
	tmpTrans->Translate(implantEndPoint[0], implantEndPoint[1], implantEndPoint[2]);

	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();

}
void SpineDemo::on_pushButton_counter_sag_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	Eigen::Vector3d x_std{ 1,0,0 };

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	auto maxBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantEndPoint = tmpPset->GetPoint(0);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	// tmpTrans->Translate(leftSide[0] * stepSize,
	// 	leftSide[1] * stepSize,
	// 	leftSide[2] * stepSize);
	tmpTrans->Translate(-implantEndPoint[0], -implantEndPoint[1], -implantEndPoint[2]);
	tmpTrans->RotateWXYZ(stepSize, x_mpr[0], x_mpr[1], x_mpr[2]);
	tmpTrans->Translate(implantEndPoint[0], implantEndPoint[1], implantEndPoint[2]);

	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_clock_ax_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	auto maxBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantEndPoint = tmpPset->GetPoint(0);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	// tmpTrans->Translate(leftSide[0] * stepSize,
	// 	leftSide[1] * stepSize,
	// 	leftSide[2] * stepSize);
	tmpTrans->Translate(-implantEndPoint[0], -implantEndPoint[1], -implantEndPoint[2]);
	tmpTrans->RotateWXYZ(stepSize, z_mpr[0], z_mpr[1], z_mpr[2]);
	tmpTrans->Translate(implantEndPoint[0], implantEndPoint[1], implantEndPoint[2]);

	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}
void SpineDemo::on_pushButton_counter_ax_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}
	// Eigen::Vector3d leftSide = z_mpr.cross(y_mpr);

	auto maxBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantEndPoint = tmpPset->GetPoint(0);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	// tmpTrans->Translate(leftSide[0] * stepSize,
	// 	leftSide[1] * stepSize,
	// 	leftSide[2] * stepSize);
	tmpTrans->Translate(-implantEndPoint[0], -implantEndPoint[1], -implantEndPoint[2]);
	tmpTrans->RotateWXYZ(-stepSize, z_mpr[0], z_mpr[1], z_mpr[2]);
	tmpTrans->Translate(implantEndPoint[0], implantEndPoint[1], implantEndPoint[2]);

	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	FocusOnImplant();
}


void SpineDemo::MoveImplantToTrajectory()
{
	if(GetDataStorage()->GetNamedNode("trajectory") == nullptr)
	{
		m_Controls.textBrowser->append("trajectory is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto twoPointNode = GetDataStorage()->GetNamedNode("trajectory"); // Data node containing the entry and exit point
	auto implantNode = GetDataStorage()->GetNamedNode("implant"); // Data node containing the implant surface

	auto entryPoint = dynamic_cast<mitk::PointSet*>(twoPointNode->GetData())->GetPoint(0);
	auto exitPoint = dynamic_cast<mitk::PointSet*>(twoPointNode->GetData())->GetPoint(1);

	Eigen::Vector3d z_axis_implant;
	z_axis_implant[0] = exitPoint[0] - entryPoint[0];
	z_axis_implant[1] = exitPoint[1] - entryPoint[1];
	z_axis_implant[2] = exitPoint[2] - entryPoint[2];

	z_axis_implant.normalize();

	Eigen::Vector3d z_axis_std;
	z_axis_std[0] = 0;
	z_axis_std[1] = 0;
	z_axis_std[2] = 1;

	double rotAngle = 180 * acos(z_axis_implant.dot(z_axis_std))/ 3.1415926;

	Eigen::Vector3d rotAxis = z_axis_std.cross(z_axis_implant);
	rotAxis.normalize();

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->Identity();
	tmpTrans->RotateWXYZ(rotAngle, rotAxis[0], rotAxis[1], rotAxis[2]);
	tmpTrans->Translate(exitPoint[0], exitPoint[1], exitPoint[2]);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());
}




