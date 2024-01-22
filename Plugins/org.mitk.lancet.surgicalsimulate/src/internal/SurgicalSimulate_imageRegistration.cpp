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
#include <vtkImplicitPolyDataDistance.h>

#include "lancetTreeCoords.h"

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

bool SurgicalSimulate::CollectLandmarkProbe()
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
	m_imageRegistrationMatrix = mitk::AffineTransform3D::New();
	navigatedImage->UpdateObjectToRfMatrix();
	mitk::TransferVtkMatrixToItkTransform(navigatedImage->GetT_Object2ReferenceFrame(), m_imageRegistrationMatrix.GetPointer());

	m_VegaToolStorage->GetToolByName("ObjectRf")->SetToolRegistrationMatrix(m_imageRegistrationMatrix);

	m_surfaceRegistrationFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix());

	auto indexOfObjectRF = m_VegaToolStorage->GetToolIndexByName("ObjectRf");
	m_surfaceRegistrationFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput(indexOfObjectRF));
	
	m_VegaVisualizeTimer->stop();
	m_VegaVisualizer->ConnectTo(m_surfaceRegistrationFilter);
	m_VegaVisualizeTimer->start();

	return true;
}

bool SurgicalSimulate::ApplySurfaceRegistration_staticImage()
{
	m_surfaceRegistrationStaticImageFilter = lancet::ApplySurfaceRegistratioinStaticImageFilter::New();
	m_surfaceRegistrationStaticImageFilter->ConnectTo(m_VegaSource);
	m_imageRegistrationMatrix = mitk::AffineTransform3D::New();
	navigatedImage->UpdateObjectToRfMatrix();
	m_Controls.textBrowser->append("Avg landmark error:" + QString::number(navigatedImage->GetlandmarkRegis_avgError()));
	m_Controls.textBrowser->append("Max landmark error:" + QString::number(navigatedImage->GetlandmarkRegis_maxError()));

	m_Controls.textBrowser->append("Avg ICP error:" + QString::number(navigatedImage->GetIcpRegis_avgError()));
	m_Controls.textBrowser->append("Max ICP error:" + QString::number(navigatedImage->GetIcpRegis_maxError()));


	mitk::TransferVtkMatrixToItkTransform(navigatedImage->GetT_Object2ReferenceFrame(), m_imageRegistrationMatrix.GetPointer());

	m_VegaToolStorage->GetToolByName("ObjectRf")->SetToolRegistrationMatrix(m_imageRegistrationMatrix);

	m_surfaceRegistrationStaticImageFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix());

	m_surfaceRegistrationStaticImageFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("ObjectRf"));

	m_VegaVisualizeTimer->stop();
	m_VegaVisualizer->ConnectTo(m_surfaceRegistrationStaticImageFilter);
	m_VegaVisualizeTimer->start();


	// Reconnect the robot pipeline so that its tcp can be visualized in the image frame as well
	// Only takes place when the robot is registered or the previous robot registration is applied
	if (m_RobotRegistrationMatrix.IsNull())
	{
		return true;
	}

	m_KukaApplyRegistrationFilter = lancet::ApplyDeviceRegistratioinFilter::New();
	m_KukaApplyRegistrationFilter->ConnectTo(m_KukaSource);
	m_KukaApplyRegistrationFilter->SetRegistrationMatrix(m_RobotRegistrationMatrix);
	
	auto robotBaseRFIndex = m_VegaToolStorage->GetToolIndexByName("RobotBaseRF");
	m_KukaApplyRegistrationFilter->SetNavigationDataOfRF(m_surfaceRegistrationStaticImageFilter->GetOutput(robotBaseRFIndex));//must make sure NavigationDataOfRF update somewhere else.

	m_KukaVisualizeTimer->stop();
	m_KukaVisualizer->ConnectTo(m_KukaApplyRegistrationFilter);
	m_KukaVisualizeTimer->start();

	// // set TCP for precision test
	// // For Test Use, regard ball 2 as the TCP, the pose can be seen on
 //    // https://gn1phhht53.feishu.cn/wiki/wikcnAYrihLnKdt5kqGYIwmZACh
	// //--------------------------------------------------
	// Eigen::Vector3d x_tcp;
	// x_tcp[0] = 51.82;
	// x_tcp[1] = -55.49;
	// x_tcp[2] = 0.21;
	// x_tcp.normalize();
 //
	// Eigen::Vector3d z_flange;
	// z_flange[0] = 0.0;
	// z_flange[1] = 0.0;
	// z_flange[2] = 1;
 //
	// Eigen::Vector3d y_tcp;
	// y_tcp = z_flange.cross(x_tcp);
	// y_tcp.normalize();
 //
	// Eigen::Vector3d z_tcp;
	// z_tcp = x_tcp.cross(y_tcp);
 //
	// Eigen::Matrix3d Re;
 //
	// Re << x_tcp[0], y_tcp[0], z_tcp[0],
	// 	x_tcp[1], y_tcp[1], z_tcp[1],
	// 	x_tcp[2], y_tcp[2], z_tcp[2];
 //
 //
	// Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);
 //
	// //------------------------------------------------
	// double tcp[6];
	// tcp[0] = 0.05; // tx
	// tcp[1] = 100.37; // ty
	// tcp[2] = 137.85; // tz
	// tcp[3] = eulerAngle(0);//-0.81;// -0.813428203; // rz
	// tcp[4] = eulerAngle(1); // ry
	// tcp[5] = eulerAngle(2); // rx
	// MITK_INFO << "TCP:" << tcp[0] << "," << tcp[1] << "," << tcp[2] << "," << tcp[3] << "," << tcp[4] << "," << tcp[5];
	// //set tcp to robot
	//   //set tcp
	// QThread::msleep(1000);
	// m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
	// QThread::msleep(1000);
	// m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
	// QThread::msleep(1000);
	// m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });
 //
	// return true;

	// set TCP for precision test
	// For plane Test Use, regard ball 1 as the TCP, the pose can be seen on
	// https://gn1phhht53.feishu.cn/wiki/wikcnAYrihLnKdt5kqGYIwmZACh
	//--------------------------------------------------
	Eigen::Vector3d x_tcp;
	x_tcp[0] = 90.33;
	x_tcp[1] = -0.16;
	x_tcp[2] = 0.03;
	x_tcp.normalize();

	Eigen::Vector3d z_flange;
	z_flange[0] = 0.0;
	z_flange[1] = 0.0;
	z_flange[2] = 1;

	Eigen::Vector3d y_tcp;
	y_tcp = z_flange.cross(x_tcp);
	y_tcp.normalize();

	Eigen::Vector3d z_tcp;
	z_tcp = x_tcp.cross(y_tcp);

	Eigen::Matrix3d Re;

	Re << x_tcp[0], y_tcp[0], z_tcp[0],
		x_tcp[1], y_tcp[1], z_tcp[1],
		x_tcp[2], y_tcp[2], z_tcp[2];

	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);

	//------------------------------------------------
	double tcp[6];
	tcp[0] = -37.93; // tx
	tcp[1] = 45.31; // ty
	tcp[2] = 137.99; // tz
	tcp[3] = eulerAngle(0); //-0.81;// -0.813428203; // rz
	tcp[4] = eulerAngle(1); // ry
	tcp[5] = eulerAngle(2); // rx
	MITK_INFO << "TCP:" << tcp[0] << "," << tcp[1] << "," << tcp[2] << "," << tcp[3] << "," << tcp[4] << "," << tcp[5];
	//set tcp to robot
	  //set tcp
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });
	return true;
}

bool SurgicalSimulate::ApplyPreexistingImageSurfaceRegistration_staticImage()
{
	// Apply preexisting surface registration result to all the NDI tools
	m_surfaceRegistrationStaticImageFilter = lancet::ApplySurfaceRegistratioinStaticImageFilter::New();
	m_surfaceRegistrationStaticImageFilter->ConnectTo(m_VegaSource);
	
	m_surfaceRegistrationStaticImageFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix());
	m_surfaceRegistrationStaticImageFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("ObjectRf"));


	m_VegaVisualizeTimer->stop();
	m_VegaVisualizer->ConnectTo(m_surfaceRegistrationStaticImageFilter);
	m_VegaVisualizeTimer->start();

	// Reconnect the robot pipeline so that its tcp can be visualized in the image frame as well
	// Only takes place when the robot is registered or the previous robot registration is applied
	if (m_RobotRegistrationMatrix.IsNull())
	{
		return true;
	}

	m_KukaApplyRegistrationFilter = lancet::ApplyDeviceRegistratioinFilter::New();
	m_KukaApplyRegistrationFilter->ConnectTo(m_KukaSource);
	m_KukaApplyRegistrationFilter->SetRegistrationMatrix(m_RobotRegistrationMatrix);
	auto robotBaseRFIndex = m_VegaToolStorage->GetToolIndexByName("RobotBaseRF");
	m_KukaApplyRegistrationFilter->SetNavigationDataOfRF(m_surfaceRegistrationStaticImageFilter->GetOutput(robotBaseRFIndex));//must make sure NavigationDataOfRF update somewhere else.

	m_KukaVisualizeTimer->stop();
	m_KukaVisualizer->ConnectTo(m_KukaApplyRegistrationFilter);
	m_KukaVisualizeTimer->start();

	// // set TCP for precision test
	// // For Test Use, regard ball 2 as the TCP, the pose can be seen on
	// // https://gn1phhht53.feishu.cn/wiki/wikcnAYrihLnKdt5kqGYIwmZACh
	// //--------------------------------------------------
	// Eigen::Vector3d x_tcp;
	// x_tcp[0] = 51.82;
	// x_tcp[1] = -55.49;
	// x_tcp[2] = 0.21;
	// x_tcp.normalize();
	//
	// Eigen::Vector3d z_flange;
	// z_flange[0] = 0.0;
	// z_flange[1] = 0.0;
	// z_flange[2] = 1;
	//
	// Eigen::Vector3d y_tcp;
	// y_tcp = z_flange.cross(x_tcp);
	// y_tcp.normalize();
	//
	// Eigen::Vector3d z_tcp;
	// z_tcp = x_tcp.cross(y_tcp);
	//
	// Eigen::Matrix3d Re;
	//
	// Re << x_tcp[0], y_tcp[0], z_tcp[0],
	// 	x_tcp[1], y_tcp[1], z_tcp[1],
	// 	x_tcp[2], y_tcp[2], z_tcp[2];
	//
	// Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);
	//
	// //------------------------------------------------
	// double tcp[6];
	// tcp[0] = 0.05; // tx
	// tcp[1] = 100.37; // ty
	// tcp[2] = 137.85; // tz
	// tcp[3] = eulerAngle(0); //-0.81;// -0.813428203; // rz
	// tcp[4] = eulerAngle(1); // ry
	// tcp[5] = eulerAngle(2); // rx
	// MITK_INFO << "TCP:" << tcp[0] << "," << tcp[1] << "," << tcp[2] << "," << tcp[3] << "," << tcp[4] << "," << tcp[5];
	// //set tcp to robot
	//   //set tcp
	// QThread::msleep(1000);
	// m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
	// QThread::msleep(1000);
	// m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
	// QThread::msleep(1000);
	// m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });

	// set TCP for precision test
	// For plane Test Use, regard ball 1 as the TCP, the pose can be seen on
	// https://gn1phhht53.feishu.cn/wiki/wikcnAYrihLnKdt5kqGYIwmZACh
	//--------------------------------------------------
	Eigen::Vector3d x_tcp;
	x_tcp[0] = 90.33;
	x_tcp[1] = -0.16;
	x_tcp[2] = 0.03;
	x_tcp.normalize();

	Eigen::Vector3d z_flange;
	z_flange[0] = 0.0;
	z_flange[1] = 0.0;
	z_flange[2] = 1;

	Eigen::Vector3d y_tcp;
	y_tcp = z_flange.cross(x_tcp);
	y_tcp.normalize();

	Eigen::Vector3d z_tcp;
	z_tcp = x_tcp.cross(y_tcp);

	Eigen::Matrix3d Re;

	Re << x_tcp[0], y_tcp[0], z_tcp[0],
		x_tcp[1], y_tcp[1], z_tcp[1],
		x_tcp[2], y_tcp[2], z_tcp[2];

	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);

	//------------------------------------------------
	double tcp[6];
	tcp[0] = -37.93; // tx
	tcp[1] = 45.31; // ty
	tcp[2] = 137.99; // tz
	tcp[3] = eulerAngle(0); //-0.81;// -0.813428203; // rz
	tcp[4] = eulerAngle(1); // ry
	tcp[5] = eulerAngle(2); // rx
	MITK_INFO << "TCP:" << tcp[0] << "," << tcp[1] << "," << tcp[2] << "," << tcp[3] << "," << tcp[4] << "," << tcp[5];
	//set tcp to robot
	  //set tcp
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });
	return true;
}


bool SurgicalSimulate::ApplyPreexistingImageSurfaceRegistration()
{
	// Apply preexisting surface registration result
	m_surfaceRegistrationFilter = lancet::ApplySurfaceRegistratioinFilter::New();
	m_surfaceRegistrationFilter->ConnectTo(m_VegaSource);
	/*vtkNew<vtkMatrix4x4> surfaceToRfMatrix;
	mitk::TransferItkTransformToVtkMatrix(m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix().GetPointer(), surfaceToRfMatrix);
	navigatedImage->SetT_Object2ReferenceFrame(surfaceToRfMatrix);*/

	m_surfaceRegistrationFilter->SetnavigationImage(navigatedImage);
	m_surfaceRegistrationFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix());
	m_surfaceRegistrationFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("ObjectRf"));

	// save image(surface) registration matrix into its corresponding RF tool
	// m_imageRegistrationMatrix = mitk::AffineTransform3D::New();
	// mitk::TransferVtkMatrixToItkTransform(navigatedImage->GetT_Object2ReferenceFrame(), m_imageRegistrationMatrix.GetPointer());
	// m_VegaToolStorage->GetToolByName("ObjectRf")->SetToolRegistrationMatrix(m_imageRegistrationMatrix);

	m_VegaVisualizeTimer->stop();
	m_VegaVisualizer->ConnectTo(m_surfaceRegistrationFilter);
	m_VegaVisualizeTimer->start();

	return true;
}


bool SurgicalSimulate::CollectIcpProbe()
{
	if (navigatedImage == nullptr)
	{
		m_Controls.textBrowser->append("Please setup the navigationObject first!");
		return false;
	}

	

	auto pointSet_probeIcp = navigatedImage->GetIcpPoints_probe();

	//get navigation data of RobotEndRF in ndi coords,
	auto probeIndex = m_VegaToolStorage->GetToolIndexByName("Probe");
	auto objectRfIndex = m_VegaToolStorage->GetToolIndexByName("ObjectRf");
	if (probeIndex == -1 || objectRfIndex == -1)
	{
		m_Controls.textBrowser->append("There is no 'Probe' or 'ObjectRf' in the toolStorage!");
	}

	mitk::NavigationData::Pointer nd_ndiToProbe = m_VegaSource->GetOutput(probeIndex);
	mitk::NavigationData::Pointer nd_ndiToObjectRf = m_VegaSource->GetOutput(objectRfIndex);

	mitk::NavigationData::Pointer nd_rfToProbe = GetNavigationDataInRef(nd_ndiToProbe, nd_ndiToObjectRf);


	mitk::Point3D probeTipPointUnderRf = nd_rfToProbe->GetPosition();

	pointSet_probeIcp->InsertPoint(probeTipPointUnderRf);

	m_Controls.textBrowser->append("Added icp point: " + QString::number(probeTipPointUnderRf[0]) +
		"/ " + QString::number(probeTipPointUnderRf[1]) + "/ " + QString::number(probeTipPointUnderRf[2]));

	return true;
}




bool SurgicalSimulate::TouchProbeCalibrationPoint1()
{
	NavigationTree::Pointer tree = NavigationTree::New();

  NavigationNode::Pointer ndi = NavigationNode::New();
  ndi->SetNavigationData(mitk::NavigationData::New());
  ndi->SetNodeName("ndi");

  tree->Init(ndi);

  NavigationNode::Pointer probe = NavigationNode::New();
  probe->SetNodeName("Probe");
  probe->SetNavigationData(m_VegaSource->GetOutput("Probe"));

  tree->AddChild(probe, ndi);

  NavigationNode::Pointer probeCalibrator = NavigationNode::New();
  probeCalibrator->SetNodeName("ProbeCalibrator");
  probeCalibrator->SetNavigationData(m_VegaSource->GetOutput("ProbeCalibrator"));

  tree->AddChild(probeCalibrator, ndi);

  //use tree
  mitk::NavigationData::Pointer input = mitk::NavigationData::New();
  double p[3]{ 52.73,51.56,23 };
  input->SetPosition(p);
  mitk::NavigationData::Pointer treeRes =  tree->GetNavigationData(input, "ProbeCalibrator", "Probe");
  //mitk::AffineTransform3D::Pointer treeResMatrix = treeRes->GetAffineTransform3D();
  MITK_INFO << "tree res:";
  MITK_INFO << treeRes->GetPosition();




	//===================================================
	m_probeOffset[0] = 0;
	m_probeOffset[1] = 0;
	m_probeOffset[2] = 0;


	//get navigation data of RobotEndRF in ndi coords,
	auto probeIndex = m_VegaToolStorage->GetToolIndexByName("Probe");
	auto probeCalibratorIndex = m_VegaToolStorage->GetToolIndexByName("ProbeCalibrator");
	if (probeIndex == -1 || probeCalibratorIndex == -1)
	{
		m_Controls.textBrowser->append("There is no 'Probe' or 'ProbeCalibrator' in the toolStorage!");
	}
	mitk::NavigationData::Pointer nd_ndiToProbe = m_VegaSource->GetOutput(probeIndex);
	mitk::NavigationData::Pointer nd_ndiToProbeCalibrator = m_VegaSource->GetOutput(probeCalibratorIndex);

	mitk::NavigationData::Pointer nd_ProbeToProbeCalibrator = GetNavigationDataInRef(nd_ndiToProbeCalibrator,nd_ndiToProbe);

	auto probeToProbeCalibratorMatrix = nd_ProbeToProbeCalibrator->GetAffineTransform3D();

	auto checkPointInCalibrator = mitk::AffineTransform3D::New();
	double tmpPoint[3];
	tmpPoint[0] = 52.73;
	tmpPoint[1] = 51.56;
	tmpPoint[2] = 23;
	
	checkPointInCalibrator->SetOffset(tmpPoint);

	checkPointInCalibrator->Compose(probeToProbeCalibratorMatrix);

	auto newOffset = checkPointInCalibrator->GetOffset();

	m_probeOffset[0] = m_probeOffset[0] + newOffset[0];
	m_probeOffset[1] = m_probeOffset[1] + newOffset[1];
	m_probeOffset[2] = m_probeOffset[2] + newOffset[2];

	m_Controls.textBrowser->append("Probe offset: " + QString::number(m_probeOffset[0]) + "/"
		+ QString::number(m_probeOffset[1]) + "/"
		+ QString::number(m_probeOffset[2]));

	m_Controls.textBrowser->append("offset: " + QString::number(newOffset[0]) + "/"
		+ QString::number(newOffset[1]) + "/"
		+ QString::number(newOffset[2]));

	return true;
}

bool SurgicalSimulate::TouchProbeCalibrationPoint2()
{
	//get navigation data of RobotEndRF in ndi coords,
	auto probeIndex = m_VegaToolStorage->GetToolIndexByName("Probe");
	auto probeCalibratorIndex = m_VegaToolStorage->GetToolIndexByName("ProbeCalibrator");
	if (probeIndex == -1 || probeCalibratorIndex == -1)
	{
		m_Controls.textBrowser->append("There is no 'Probe' or 'ProbeCalibrator' in the toolStorage!");
	}
	mitk::NavigationData::Pointer nd_ndiToProbe = m_VegaSource->GetOutput(probeIndex);
	mitk::NavigationData::Pointer nd_ndiToProbeCalibrator = m_VegaSource->GetOutput(probeCalibratorIndex);

	mitk::NavigationData::Pointer nd_ProbeToProbeCalibrator = GetNavigationDataInRef(nd_ndiToProbeCalibrator, nd_ndiToProbe);

	auto probeToProbeCalibratorMatrix = nd_ProbeToProbeCalibrator->GetAffineTransform3D();

	auto checkPointInCalibrator = mitk::AffineTransform3D::New();
	double tmpPoint[3];
	tmpPoint[0] = 62.69;
	tmpPoint[1] = 52.44;
	tmpPoint[2] = 5.5;
	checkPointInCalibrator->SetOffset(tmpPoint);

	checkPointInCalibrator->Compose(probeToProbeCalibratorMatrix);

	auto newOffset = checkPointInCalibrator->GetOffset();

	m_probeOffset[0] = (m_probeOffset[0] + newOffset[0])/2;
	m_probeOffset[1] = (m_probeOffset[1] + newOffset[1]) / 2;
	m_probeOffset[2] = (m_probeOffset[2] + newOffset[2]) / 2;

	m_Controls.textBrowser->append("Probe offset: " + QString::number(m_probeOffset[0]) + "/"
		+ QString::number(m_probeOffset[1]) + "/"
		+ QString::number(m_probeOffset[2]));

	m_Controls.textBrowser->append("offset: " + QString::number(newOffset[0]) + "/"
		+ QString::number(newOffset[1]) + "/"
		+ QString::number(newOffset[2]));

	return true;
}

bool SurgicalSimulate::TouchProbeCalibrationPoint3()
{
	//get navigation data of RobotEndRF in ndi coords,
	auto probeIndex = m_VegaToolStorage->GetToolIndexByName("Probe");
	auto probeCalibratorIndex = m_VegaToolStorage->GetToolIndexByName("ProbeCalibrator");
	if (probeIndex == -1 || probeCalibratorIndex == -1)
	{
		m_Controls.textBrowser->append("There is no 'Probe' or 'ProbeCalibrator' in the toolStorage!");
	}
	mitk::NavigationData::Pointer nd_ndiToProbe = m_VegaSource->GetOutput(probeIndex);
	mitk::NavigationData::Pointer nd_ndiToProbeCalibrator = m_VegaSource->GetOutput(probeCalibratorIndex);

	mitk::NavigationData::Pointer nd_ProbeToProbeCalibrator = GetNavigationDataInRef(nd_ndiToProbeCalibrator, nd_ndiToProbe);

	auto probeToProbeCalibratorMatrix = nd_ProbeToProbeCalibrator->GetAffineTransform3D();

	auto checkPointInCalibrator = mitk::AffineTransform3D::New();
	double tmpPoint[3];
	tmpPoint[0] = 72.65;
	tmpPoint[1] = 53.33;
	tmpPoint[2] = 15;
	checkPointInCalibrator->SetOffset(tmpPoint);

	checkPointInCalibrator->Compose(probeToProbeCalibratorMatrix);

	auto newOffset = checkPointInCalibrator->GetOffset();

	m_probeOffset[0] = (2 * m_probeOffset[0] + newOffset[0]) / 3;
	m_probeOffset[1] =(2 * m_probeOffset[1] + newOffset[1]) / 3;
	m_probeOffset[2] = (2 * m_probeOffset[2] + newOffset[2]) / 3;

	m_Controls.textBrowser->append("Probe offset: " + QString::number(m_probeOffset[0]) + "/"
		+ QString::number(m_probeOffset[1]) + "/"
		+ QString::number(m_probeOffset[2]));

	m_Controls.textBrowser->append("offset: " + QString::number(newOffset[0]) + "/"
		+ QString::number(newOffset[1]) + "/"
		+ QString::number(newOffset[2]));

	return true;
}

bool SurgicalSimulate::ProbeImageCheckPoint()
{
	auto imageRfRegistrationMatrix = m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix();
	vtkNew<vtkMatrix4x4> vtkImageRfRegistrationMatrix; // image to RF matrix
	mitk::TransferItkTransformToVtkMatrix(imageRfRegistrationMatrix.GetPointer(),vtkImageRfRegistrationMatrix);

	if (vtkImageRfRegistrationMatrix->IsIdentity())
	{
		m_Controls.textBrowser->append("No image registration available");
		return false;
	}

	auto tmpPointSet = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_ImageCheckPoint->GetSelectedNode()->GetData());
	auto imageCheckPoint = tmpPointSet->GetPoint(0);

	mitk::NavigationData::Pointer nd_ndiToProbe = m_VegaSource->GetOutput("Probe");
	mitk::NavigationData::Pointer nd_ndiToObjectRf = m_VegaSource->GetOutput("ObjectRf");

	mitk::NavigationData::Pointer nd_rfToProbe = GetNavigationDataInRef(nd_ndiToProbe, nd_ndiToObjectRf);

	mitk::Point3D probeTipPointUnderRf = nd_rfToProbe->GetPosition();

	vtkNew<vtkMatrix4x4> tmpMatrix;
	tmpMatrix->Identity();
	tmpMatrix->SetElement(0, 3, probeTipPointUnderRf[0]);
	tmpMatrix->SetElement(1, 3, probeTipPointUnderRf[1]);
	tmpMatrix->SetElement(2, 3, probeTipPointUnderRf[2]);

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->SetMatrix(tmpMatrix);
	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(vtkImageRfRegistrationMatrix);
	tmpTransform->Update();

	auto transMatrix = tmpTransform->GetMatrix();

	double probeTipInImage[3]
	{
		transMatrix->GetElement(0,3),
		transMatrix->GetElement(1,3),
		transMatrix->GetElement(2,3),
	};

	double distance = sqrt(pow(probeTipInImage[0] - imageCheckPoint[0], 2)+ 
		pow(probeTipInImage[1] - imageCheckPoint[1], 2) + 
		pow(probeTipInImage[2] - imageCheckPoint[2], 2));

	m_Controls.textBrowser->append("Distance to the checkpoint:" + QString::number(distance));

	return true;
}


bool SurgicalSimulate::ProbeSurface()
{
	auto imageRfRegistrationMatrix = m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix();
	vtkNew<vtkMatrix4x4> vtkImageRfRegistrationMatrix; // image to RF matrix
	mitk::TransferItkTransformToVtkMatrix(imageRfRegistrationMatrix.GetPointer(), vtkImageRfRegistrationMatrix);

	if (vtkImageRfRegistrationMatrix->IsIdentity())
	{
		m_Controls.textBrowser->append("No image registration available");
		return false;
	}

	// auto tmpPointSet = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_ImageCheckPoint->GetSelectedNode()->GetData());
	// auto imageCheckPoint = tmpPointSet->GetPoint(0);

	mitk::NavigationData::Pointer nd_ndiToProbe = m_VegaSource->GetOutput("Probe");
	mitk::NavigationData::Pointer nd_ndiToObjectRf = m_VegaSource->GetOutput("ObjectRf");

	mitk::NavigationData::Pointer nd_rfToProbe = GetNavigationDataInRef(nd_ndiToProbe, nd_ndiToObjectRf);

	mitk::Point3D probeTipPointUnderRf = nd_rfToProbe->GetPosition();

	vtkNew<vtkMatrix4x4> tmpMatrix;
	tmpMatrix->Identity();
	tmpMatrix->SetElement(0, 3, probeTipPointUnderRf[0]);
	tmpMatrix->SetElement(1, 3, probeTipPointUnderRf[1]);
	tmpMatrix->SetElement(2, 3, probeTipPointUnderRf[2]);

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->SetMatrix(tmpMatrix);
	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(vtkImageRfRegistrationMatrix);
	tmpTransform->Update();

	auto transMatrix = tmpTransform->GetMatrix();

	double probeTipInImage[3]
	{
		transMatrix->GetElement(0,3),
		transMatrix->GetElement(1,3),
		transMatrix->GetElement(2,3),
	};


	auto surfacePolyData = dynamic_cast<mitk::Surface*>(m_Controls.mitkNodeSelectWidget_surface_regis->GetSelectedNode()->GetData())->GetVtkPolyData();
	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance;

	implicitPolyDataDistance->SetInput(surfacePolyData);

	double currentError = implicitPolyDataDistance->EvaluateFunction(probeTipInImage);

	// double distance = sqrt(pow(probeTipInImage[0] - imageCheckPoint[0], 2) +
	// 	pow(probeTipInImage[1] - imageCheckPoint[1], 2) +
	// 	pow(probeTipInImage[2] - imageCheckPoint[2], 2));

	m_Controls.textBrowser->append("Distance to the surface: " + QString::number(currentError));

	return true;
}


