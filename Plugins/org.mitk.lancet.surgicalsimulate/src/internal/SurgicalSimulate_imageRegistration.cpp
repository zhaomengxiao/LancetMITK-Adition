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
#include "SurgicalSimulate.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <mitkAffineTransform3D.h>
#include <mitkMatrixConvert.h>

//igt
#include <lancetVegaTrackingDevice.h>
#include <kukaRobotDevice.h>
#include <lancetApplyDeviceRegistratioinFilter.h>

#include "lancetTrackingDeviceSourceConfigurator.h"
#include "mitkNavigationToolStorageDeserializer.h"
#include <QtWidgets\qfiledialog.h>

#include "mitkNodePredicateAnd.h"
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include "QmitkDataStorageTreeModel.h"
#include <QmitkSingleNodeSelectionWidget.h>

void SurgicalSimulate::InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget)
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

void SurgicalSimulate::InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget)
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



bool SurgicalSimulate::AssembleNavigationObjectFromDataNode(mitk::DataNode* parentNode, lancet::NavigationObject* assembledObject)
{
	auto nodeTreeModel = new QmitkDataStorageTreeModel(this->GetDataStorage());

	assembledObject->SetDataNode(parentNode);

	QModelIndex parentIndex = nodeTreeModel->GetIndex(parentNode);

	int parentRowCount = nodeTreeModel->rowCount(parentIndex);

	// Iteratively check the subNode content and add them to the NavigationObject
	// Do not add missing subNode at this step otherwise the nodeTreeModel will get messed up,
	// Only record which subNodes are missing
	int flag_hasMatrixFlag{ 0 };
	int flag_hasSurfaceBackUp{ 0 };
	int flag_hasLandmarkSurface{ 0 };
	int flag_hasLandmarkRf{ 0 };
	int flag_hasIcpSurface{ 0 };
	int flag_hasIcpRf{ 0 };

	for (int i = 0; i < parentRowCount; i++)
	{
		QModelIndex tmpIndex = nodeTreeModel->index(i, 0, parentIndex);
		auto tmpNodeName = nodeTreeModel->GetNode(tmpIndex)->GetName();
		auto tmpNode = nodeTreeModel->GetNode(tmpIndex);

		if (tmpNodeName.compare("surfaceToRf_matrix") == 0)
		{
			if (dynamic_cast<mitk::PointSet*>(tmpNode->GetData()) !=nullptr)
			{
				auto pointSet_matrix = dynamic_cast<mitk::PointSet*>(tmpNode->GetData());
				if (pointSet_matrix->GetSize() == 4)
				{
					flag_hasMatrixFlag = 1;

					// Write the matrix into NavigationObject
					vtkNew<vtkMatrix4x4> T_Object2ReferenceFrame;
					T_Object2ReferenceFrame->Identity();

					for(int m{0}; m < 3; m++)
					{
						for(int n{0}; n < 4; n++)
						{
							auto tmpPoint = pointSet_matrix->GetPoint(n);
							T_Object2ReferenceFrame->SetElement(m, n, tmpPoint[m]);
						}
					}

					assembledObject->SetT_Object2ReferenceFrame(T_Object2ReferenceFrame);
				}
			}
		}

		if (tmpNodeName.compare("surface_backup") == 0)
		{
			if(dynamic_cast<mitk::Surface*>(tmpNode->GetData()) != nullptr)
			{
				flag_hasSurfaceBackUp = 1;
			}
		}

		if (tmpNodeName.compare("landmark_surface") == 0)
		{
			if (dynamic_cast<mitk::PointSet*>(tmpNode->GetData()) != nullptr)
			{
				auto pointSet_landmark_surface = dynamic_cast<mitk::PointSet*>(tmpNode->GetData());
				if (pointSet_landmark_surface->GetSize() > 2)
				{
					flag_hasLandmarkSurface = 1;
					assembledObject->SetLandmarks(pointSet_landmark_surface);
				}
			}
		}

		if (tmpNodeName.compare("landmark_rf") == 0)
		{
			if (dynamic_cast<mitk::PointSet*>(tmpNode->GetData()) != nullptr)
			{
				auto pointSet_landmark_rf = dynamic_cast<mitk::PointSet*>(tmpNode->GetData());
				if (pointSet_landmark_rf->GetSize() > 2)
				{
					flag_hasLandmarkRf = 1;
					assembledObject->SetLandmarks_probe(pointSet_landmark_rf);
				}
			}
		}

		if (tmpNodeName.compare("icp_surface") == 0)
		{
			if (dynamic_cast<mitk::PointSet*>(tmpNode->GetData()) != nullptr)
			{
				auto pointSet_icp_surface = dynamic_cast<mitk::PointSet*>(tmpNode->GetData());
				if (pointSet_icp_surface->GetSize() > 2)
				{
					flag_hasIcpSurface = 1;
					assembledObject->SetIcpPoints(pointSet_icp_surface);
				}
			}
		}

		if (tmpNodeName.compare("icp_rf") == 0)
		{
			if (dynamic_cast<mitk::PointSet*>(tmpNode->GetData()) != nullptr)
			{
				auto pointSet_icp_rf = dynamic_cast<mitk::PointSet*>(tmpNode->GetData());
				if (pointSet_icp_rf->GetSize() > 2)
				{
					flag_hasIcpRf = 1;
					assembledObject->SetIcpPoints_probe(pointSet_icp_rf);
				}
			}
		}
	}




	return false;
}


bool SurgicalSimulate::SetupNavigatedImage()
{
	// The surface node should have no offset, i.e., should have an identity matrix!
	auto surfaceNode = m_Controls.mitkNodeSelectWidget_surface_regis->GetSelectedNode();
	auto landmarkSrcNode = m_Controls.mitkNodeSelectWidget_landmark_src->GetSelectedNode();

	if (surfaceNode == nullptr || landmarkSrcNode == nullptr)
	{
		m_Controls.textBrowser->append("Source surface or source landmarks is not ready!");
		return false;
	}

	navigatedImage = lancet::NavigationObject::New();

	auto matrix = dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->GetVtkMatrix();

	if(matrix ->IsIdentity() ==false)
	{
		vtkNew<vtkMatrix4x4> identityMatrix;
		identityMatrix->Identity();
		dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);

		m_Controls.textBrowser->append("Warning: the initial surface has a non-identity offset matrix; the matrix has been reset to identity!");
	}

	navigatedImage->SetDataNode(surfaceNode);

	navigatedImage->SetLandmarks(dynamic_cast<mitk::PointSet*>(landmarkSrcNode->GetData()));

	navigatedImage->SetReferencFrameName(surfaceNode->GetName());

	m_Controls.textBrowser->append("--- navigatedImage has been set up ---");

	return true;
}

bool SurgicalSimulate::CollectLanmarkProbe()
{
	if(navigatedImage == nullptr)
	{
		m_Controls.textBrowser->append("Please setup the navigationObject first!");
		return false;
	}

	int landmark_surfaceNum = navigatedImage->GetLandmarks()->GetSize();
	int landmark_rmNum = navigatedImage->GetLandmarks_probe()->GetSize();

	if(landmark_surfaceNum == landmark_rmNum)
	{
		m_Controls.textBrowser->append("--- Enough landmarks have been collected ----");
		return true;
	}

	auto pointSet_probeLandmark = navigatedImage->GetLandmarks_probe();

	//get navigation data of RobotEndRF in ndi coords,
	auto probeIndex = m_VegaToolStorage->GetToolIndexByName("Probe");
	auto objectRfIndex = m_VegaToolStorage->GetToolIndexByName("ObjectRf");
	if(probeIndex == -1 || objectRfIndex == -1)
	{
		m_Controls.textBrowser->append("There is no 'Probe' or 'ObjectRf' in the toolStorage!");
	}

	mitk::NavigationData::Pointer nd_ndiToProbe = m_VegaSource->GetOutput(probeIndex);
	mitk::NavigationData::Pointer nd_ndiToObjectRf = m_VegaSource->GetOutput(objectRfIndex);

	mitk::NavigationData::Pointer nd_rfToProbe = GetNavigationDataInRef(nd_ndiToProbe, nd_ndiToObjectRf);


	mitk::Point3D probeTipPointUnderRf = nd_rfToProbe->GetPosition();

	pointSet_probeLandmark->InsertPoint(probeTipPointUnderRf);

	m_Controls.textBrowser->append("Added landmark: " + QString::number(probeTipPointUnderRf[0]) +
	"/ " + QString::number(probeTipPointUnderRf[1]) + "/ " + QString::number(probeTipPointUnderRf[2]));

	return true;
}


bool SurgicalSimulate::ApplySurfaceRegistration()
{
	//build ApplyDeviceRegistrationFilter
	m_surfaceRegistrationFilter = lancet::ApplySurfaceRegistratioinFilter::New();
	m_surfaceRegistrationFilter->ConnectTo(m_VegaSource);
	m_surfaceRegistrationFilter->SetnavigationImage(navigatedImage);
	auto indexOfObjectRF = m_VegaToolStorage->GetToolIndexByName("ObjectRf");
	m_surfaceRegistrationFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput(indexOfObjectRF));


	m_VegaVisualizeTimer->stop();
	m_VegaVisualizer->ConnectTo(m_KukaApplyRegistrationFilter);
	m_VegaVisualizeTimer->start();

	return true;
}



