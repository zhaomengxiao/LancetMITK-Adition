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
#include "AccuracyTest.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <QTimer>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
const std::string AccuracyTest::VIEW_ID = "org.mitk.views.accuracytest";

void AccuracyTest::SetFocus()
{
	m_Controls.m_pushButtonSetProbe->setFocus();
}

void AccuracyTest::CreateQtPartControl(QWidget* parent)
{
	//init members
	m_IDofProbe = -1;
	m_IDofRF = -1;
	m_TrackingTimer = new QTimer(this);

	m_PointSetPivoting = mitk::PointSet::New();
	m_PointSetPivotingNode = mitk::DataNode::New();
	m_PointSetPivotingNode->SetData(m_PointSetPivoting);

	m_PointSetTopple = mitk::PointSet::New();
	m_PointSetToppleNode = mitk::DataNode::New();
	m_PointSetToppleNode->SetData(m_PointSetTopple);

	m_PointSetTilt = mitk::PointSet::New();
	m_PointSetTiltNode = mitk::DataNode::New();
	m_PointSetTiltNode->SetData(m_PointSetTilt);

	m_distancePointSet = mitk::PointSet::New();
	m_distancePointSetNode = mitk::DataNode::New();
	m_distancePointSetNode->SetData(m_distancePointSet);
	// create GUI widgets from the Qt Designer's .ui file
	m_Controls.setupUi(parent);

	//init widget

	m_Controls.m_PointListPivoting->SetPointSetNode(m_PointSetPivotingNode);
	m_Controls.m_PointListTopple->SetPointSetNode(m_PointSetToppleNode);
	m_Controls.m_PointListTilt->SetPointSetNode(m_PointSetTiltNode);
	m_Controls.m_distancePointList->SetPointSetNode(m_distancePointSetNode);
	//connect
	connect(m_TrackingTimer, SIGNAL(timeout()), this, SLOT(UpdateTrackingTimer()));
	connect(m_Controls.m_pushButtonSetProbe, &QPushButton::clicked, this, &AccuracyTest::SetProbe);
	connect(m_Controls.m_pushButtonSetRF, &QPushButton::clicked, this, &AccuracyTest::SetReferenceFrame);
	connect(m_Controls.m_AddPoint, &QPushButton::clicked, this, &AccuracyTest::AddPivotPoint);
	connect(m_Controls.m_compute, &QPushButton::clicked, this, &AccuracyTest::compute);
	connect(m_Controls.m_AddPoint1, &QPushButton::clicked, this, &AccuracyTest::AddTopplePoint);
	connect(m_Controls.m_compute1, &QPushButton::clicked, this, &AccuracyTest::computeTopple);
	connect(m_Controls.m_AddPoint_2, &QPushButton::clicked, this, &AccuracyTest::AddTiltPoint);
	connect(m_Controls.m_compute_2, &QPushButton::clicked, this, &AccuracyTest::computeTilt);
	connect(m_Controls.m_AddPoint_3, &QPushButton::clicked, this, &AccuracyTest::AddDistancePoint);
	connect(m_Controls.m_compute_3, &QPushButton::clicked, this, &AccuracyTest::computeDistance);
}

void AccuracyTest::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
	const QList<mitk::DataNode::Pointer>& nodes)
{
	// iterate all selected objects, adjust warning visibility
	// foreach (mitk::DataNode::Pointer node, nodes)
	// {
	//   if (node.IsNotNull() && dynamic_cast<mitk::Image *>(node->GetData()))
	//   {
	//     m_Controls.labelWarning->setVisible(false);
	//     m_Controls.buttonPerformImageProcessing->setEnabled(true);
	//     return;
	//   }
	// }
	//
	// m_Controls.labelWarning->setVisible(true);
	// m_Controls.buttonPerformImageProcessing->setEnabled(false);
}


bool AccuracyTest::CheckInitialization(bool requireRF)
{
	if ((m_IDofProbe == -1) ||
		((requireRF) &&
		(m_IDofRF == -1)
			)
		)
	{
		QMessageBox msgBox;
		msgBox.setText("Tool to calibrate and/or calibration pointer not initialized, cannot proceed!");
		msgBox.exec();
		return false;
	}
	return true;
}

void AccuracyTest::SetProbe()
{
	m_IDofProbe = m_Controls.m_SelectionWidget->GetSelectedToolID();
	if (m_IDofProbe == -1)
	{
		m_Controls.m_Label_Probe->setText("<none>");
		m_Controls.m_StatusWidgetProbe->RemoveStatusLabels();
		m_TrackingTimer->stop();
	}
	else
	{
		m_NavigationDataSourceOfProbe = m_Controls.m_SelectionWidget->GetSelectedNavigationDataSource();
		m_Controls.m_Label_Probe->setText(m_NavigationDataSourceOfProbe->GetOutput(m_IDofProbe)->GetName());
		//initialize widget
		m_Controls.m_StatusWidgetProbe->RemoveStatusLabels();
		m_Controls.m_StatusWidgetProbe->SetShowPositions(true);
		m_Controls.m_StatusWidgetProbe->SetTextAlignment(Qt::AlignLeft);
		m_Controls.m_StatusWidgetProbe->AddNavigationData(m_NavigationDataSourceOfProbe->GetOutput(m_IDofProbe));
		m_Controls.m_StatusWidgetProbe->ShowStatusLabels();
		if (!m_TrackingTimer->isActive()) m_TrackingTimer->start(100);
	}
}

void AccuracyTest::SetReferenceFrame()
{
	m_IDofRF = m_Controls.m_SelectionWidget->GetSelectedToolID();
	if (m_IDofRF == -1)
	{
		m_Controls.m_Label_RF->setText("<none>");
		m_Controls.m_StatusWidgetRF->RemoveStatusLabels();
		m_TrackingTimer->stop();
	}
	else
	{
		m_NavigationDataSourceOfRF = m_Controls.m_SelectionWidget->GetSelectedNavigationDataSource();
		m_Controls.m_Label_RF->setText(m_NavigationDataSourceOfRF->GetOutput(m_IDofRF)->GetName());
		//initialize widget
		m_Controls.m_StatusWidgetRF->RemoveStatusLabels();
		m_Controls.m_StatusWidgetRF->SetShowPositions(true);
		m_Controls.m_StatusWidgetRF->SetTextAlignment(Qt::AlignLeft);
		m_Controls.m_StatusWidgetRF->AddNavigationData(m_NavigationDataSourceOfRF->GetOutput(m_IDofRF));
		m_Controls.m_StatusWidgetRF->ShowStatusLabels();
		if (!m_TrackingTimer->isActive()) m_TrackingTimer->start(100);
	}
}

void AccuracyTest::UpdateTrackingTimer()
{
	m_Controls.m_StatusWidgetProbe->Refresh();
	m_Controls.m_StatusWidgetRF->Refresh();
}

void AccuracyTest::AddPivotPoint()
{
	if (!CheckInitialization()) { return; }
	mitk::NavigationData::Pointer navDataTool = m_NavigationDataSourceOfRF->GetOutput(m_IDofRF);
	mitk::Point3D point = m_NavigationDataSourceOfProbe->GetOutput(m_IDofProbe)->GetPosition();

	//convert to itk transform
	itk::Vector<double, 3> translation;
	for (int k = 0; k < 3; k++) translation[k] = navDataTool->GetPosition()[k];
	itk::Matrix<double, 3, 3> rotation;
	for (int k = 0; k < 3; k++) for (int l = 0; l < 3; l++) rotation[k][l] = navDataTool->GetOrientation().rotation_matrix_transpose()[k][l];
	rotation = rotation.GetTranspose();
	itk::Vector<double> point_itk;
	point_itk[0] = point[0];
	point_itk[1] = point[1];
	point_itk[2] = point[2];

	//compute point in reference frame coordinates
	itk::Matrix<double, 3, 3> rotationInverse;
	for (int k = 0; k < 3; k++) for (int l = 0; l < 3; l++) rotationInverse[k][l] = rotation.GetInverse()[k][l];
	point_itk = rotationInverse * (point_itk - translation);

	//convert back and add landmark to pointset
	point[0] = point_itk[0];
	point[1] = point_itk[1];
	point[2] = point_itk[2];

	m_PointSetPivoting->InsertPoint(m_PointSetPivoting->GetSize(), point);
}

void AccuracyTest::AddTopplePoint()
{
	if (!CheckInitialization()) { return; }
	mitk::NavigationData::Pointer navDataTool = m_NavigationDataSourceOfRF->GetOutput(m_IDofRF);
	mitk::Point3D point = m_NavigationDataSourceOfProbe->GetOutput(m_IDofProbe)->GetPosition();

	//convert to itk transform
	itk::Vector<double, 3> translation;
	for (int k = 0; k < 3; k++) translation[k] = navDataTool->GetPosition()[k];
	itk::Matrix<double, 3, 3> rotation;
	for (int k = 0; k < 3; k++) for (int l = 0; l < 3; l++) rotation[k][l] = navDataTool->GetOrientation().rotation_matrix_transpose()[k][l];
	rotation = rotation.GetTranspose();
	itk::Vector<double> point_itk;
	point_itk[0] = point[0];
	point_itk[1] = point[1];
	point_itk[2] = point[2];

	//compute point in reference frame coordinates
	itk::Matrix<double, 3, 3> rotationInverse;
	for (int k = 0; k < 3; k++) for (int l = 0; l < 3; l++) rotationInverse[k][l] = rotation.GetInverse()[k][l];
	point_itk = rotationInverse * (point_itk - translation);

	//convert back and add landmark to pointset
	point[0] = point_itk[0];
	point[1] = point_itk[1];
	point[2] = point_itk[2];

	m_PointSetTopple->InsertPoint(m_PointSetTopple->GetSize(), point);
}

void AccuracyTest::AddTiltPoint()
{
	if (!CheckInitialization()) { return; }
	mitk::NavigationData::Pointer navDataTool = m_NavigationDataSourceOfRF->GetOutput(m_IDofRF);
	mitk::Point3D point = m_NavigationDataSourceOfProbe->GetOutput(m_IDofProbe)->GetPosition();

	//convert to itk transform
	itk::Vector<double, 3> translation;
	for (int k = 0; k < 3; k++) translation[k] = navDataTool->GetPosition()[k];
	itk::Matrix<double, 3, 3> rotation;
	for (int k = 0; k < 3; k++) for (int l = 0; l < 3; l++) rotation[k][l] = navDataTool->GetOrientation().rotation_matrix_transpose()[k][l];
	rotation = rotation.GetTranspose();
	itk::Vector<double> point_itk;
	point_itk[0] = point[0];
	point_itk[1] = point[1];
	point_itk[2] = point[2];

	//compute point in reference frame coordinates
	itk::Matrix<double, 3, 3> rotationInverse;
	for (int k = 0; k < 3; k++) for (int l = 0; l < 3; l++) rotationInverse[k][l] = rotation.GetInverse()[k][l];
	point_itk = rotationInverse * (point_itk - translation);

	//convert back and add landmark to pointset
	point[0] = point_itk[0];
	point[1] = point_itk[1];
	point[2] = point_itk[2];

	m_PointSetTilt->InsertPoint(m_PointSetTilt->GetSize(), point);
}

void AccuracyTest::AddDistancePoint()
{
	if (!CheckInitialization()) { return; }
	mitk::NavigationData::Pointer navDataTool = m_NavigationDataSourceOfRF->GetOutput(m_IDofRF);
	mitk::Point3D point = m_NavigationDataSourceOfProbe->GetOutput(m_IDofProbe)->GetPosition();

	//convert to itk transform
	itk::Vector<double, 3> translation;
	for (int k = 0; k < 3; k++) translation[k] = navDataTool->GetPosition()[k];
	itk::Matrix<double, 3, 3> rotation;
	for (int k = 0; k < 3; k++) for (int l = 0; l < 3; l++) rotation[k][l] = navDataTool->GetOrientation().rotation_matrix_transpose()[k][l];
	rotation = rotation.GetTranspose();
	itk::Vector<double> point_itk;
	point_itk[0] = point[0];
	point_itk[1] = point[1];
	point_itk[2] = point[2];

	//compute point in reference frame coordinates
	itk::Matrix<double, 3, 3> rotationInverse;
	for (int k = 0; k < 3; k++) for (int l = 0; l < 3; l++) rotationInverse[k][l] = rotation.GetInverse()[k][l];
	point_itk = rotationInverse * (point_itk - translation);

	//convert back and add landmark to pointset
	point[0] = point_itk[0];
	point[1] = point_itk[1];
	point[2] = point_itk[2];

	m_distancePointSet->InsertPoint(m_distancePointSet->GetSize(), point);
}

//Calculate the average tip position
mitk::Point3D AccuracyTest::computeAverageOfPosition(mitk::PointSet::Pointer pointGroup)
{
	mitk::Point3D ret;
	for (int index = 0; index < pointGroup->GetSize(); ++index)
	{
		mitk::Point3D item = pointGroup->GetPoint(index);
		ret[0] += item[0];
		ret[1] += item[1];
		ret[2] += item[2];
	}
	ret[0] /= pointGroup->GetSize();
	ret[1] /= pointGroup->GetSize();
	ret[2] /= pointGroup->GetSize();

	return ret;
}
//Difference between each measurement and the average needle tip position
double AccuracyTest::eachDifference(const mitk::Point3D& point, const mitk::Point3D& positon)
{
	return sqrt((point[0] - positon[0]) * (point[0] - positon[0]) + (point[1] - positon[1]) * (point[1] - positon[1]) + (point[2] - positon[2]) * (point[2] - positon[2]));
}
//standardError
double AccuracyTest::standardError(const std::vector<double>& pointset)
{
	double average_value = 0.0;
	for (auto item : pointset)
	{
		average_value += item;
	}
	average_value /= pointset.size();
	double standard_error = 0.0;
	for (auto item : pointset)
	{
		standard_error += (item - average_value) * (item - average_value);
	}
	standard_error /= pointset.size();

	return sqrt(standard_error);
}


// distance Compare
std::vector<double> AccuracyTest::distanceCompare(const std::vector<double>& dist_vec)
{
	std::vector<double> KnownDistance = { 45.95376083,37.18980932,37.20449293,32.6917076,32.7427404,34.18480307,34.25938296,41.03174218,
	41.10534716,51.12603051,51.17078386,62.96128963,62.97015749 ,75.6389134 ,75.63974443 ,88.82109178 ,88.89362005 ,102.3783352 ,102.424229 ,
	82.92142976 ,65.04598861 ,51.43260343 ,46.03481776 ,51.42804595 ,65.02629601 ,82.91179408 ,98.12478232 ,88.32098422 ,74.44223663 ,79.49015128 ,
	75.30410736 ,88.98374322 ,98.74590333 ,111.3207773 ,101.5571322 ,111.1444976 ,112.055758 ,111.9833086 ,123.3458558 ,118.9614379 ,123.9522882 ,
	132.2928106 ,132.3394648 ,136.2839058 ,141.9433759,141.92486 };
	std::vector<double> computeDistance;
	for (int i = 0; i < dist_vec.size(); ++i)
	{
		computeDistance.push_back(dist_vec[i] - KnownDistance[i]);
	}
	return computeDistance;
}
//Calculate average
double AccuracyTest::averageValueCompute(const std::vector<double>& dist)
{
	double averageValue = 0.0;
	for (int i = 0; i < dist.size(); ++i)
	{
		averageValue += dist[i];
	}
	return averageValue /= dist.size();
}
void AccuracyTest::compute()
{
	double mistake = 0.0;
	double average_value = 0.0;
	double average_error = 0.0;
	double maximum_error = 0.0;
	double standard_error = 0.0;
	std::vector<double> error_vector;

	//平均针尖位置
	auto position = computeAverageOfPosition(m_PointSetPivoting);
	MITK_INFO << "position" << position;
	string csvPath = "D:\\绕轴旋转.csv";
	ofstream csvfile;
	csvfile.open(csvPath,ios::out |ios::trunc);
	csvfile << "工具绕轴旋转" << "," << "X" << "," << "Y" << "," << "Z" << "\n";
	
	mitk::Point3D error;
	for (int index = 0; index < m_PointSetPivoting->GetSize(); ++index)
	{
		mitk::Point3D point = m_PointSetPivoting->GetPoint(index);

		mistake = eachDifference(position, point);
		error_vector.push_back(mistake);
		
		average_error += mistake;
		if (maximum_error < mistake)
		{
			maximum_error = mistake;
		}
		
		csvfile << "坐标(mm)" << "," << point[0] << "," << point[1] << "," << point[2] << "\n";
		csvfile << "偏差(mm)" << "," << mistake << "\n";
	}
	
	
	//平均误差
	average_error /= m_PointSetPivoting->GetSize();
	//标准差
	standard_error = standardError(error_vector);
	csvfile << "maximum_error(mm)" << "," << maximum_error << "\n";
	csvfile << "average_error(mm)" << "," << average_error << "\n";
	csvfile << "standard_error(mm)" << "," << standard_error << "\n";
	csvfile.close();

	for (int i = 0; i < error_vector.size(); ++i)
	{
		MITK_INFO << "每次测量与平均针尖位置的差" << error_vector[i];
	}
	MITK_INFO << "average_error" << average_error;
	MITK_INFO << "maximum_error" << maximum_error;
	MITK_INFO << "standard_error" << standard_error;
}
void AccuracyTest::computeTopple()
{
	double mistake = 0.0;
	double average_value = 0.0;
	double average_error = 0.0;
	double maximum_error = 0.0;
	double standard_error = 0.0;
	std::vector<double> error_vector;
	
	auto position = computeAverageOfPosition(m_PointSetTopple);
	MITK_INFO << "position" << position;
	string csvPath = "D:\\前后倾倒.csv";
	ofstream csvfile;
	csvfile.open(csvPath, ios::out | ios::trunc);
	csvfile << "工具前后倾倒" << "," << "X" << "," << "Y" << "," << "Z" << "\n";
	mitk::Point3D error;
	for (int index = 0; index < m_PointSetTopple->GetSize(); ++index)
	{
		mitk::Point3D point = m_PointSetTopple->GetPoint(index);
		mistake = eachDifference(position, point);
		error_vector.push_back(mistake);
		average_error += mistake;
		if (maximum_error < mistake)
		{
			maximum_error = mistake;
		}
		csvfile << "坐标(mm)" << "," << point[0] << "," << point[1] << "," << point[2] << "\n";
		csvfile << "偏差(mm)" << "," << mistake << "\n";
	}

	average_error /= m_PointSetTopple->GetSize();
	
	standard_error = standardError(error_vector);
	csvfile << "maximum_error(mm)" << "," << maximum_error << "\n";
	csvfile << "average_error(mm)" << "," << average_error << "\n";
	csvfile << "standard_error(mm)" << "," << standard_error << "\n";
	csvfile.close();
	MITK_INFO << "mistake" << mistake;

	MITK_INFO << "average_error" << average_error;
	MITK_INFO << "maximum_error" << maximum_error;
	MITK_INFO << "standard_error" << standard_error;
}
void AccuracyTest::computeTilt()
{
	double mistake = 0.0;
	double average_value = 0.0;
	double average_error = 0.0;
	double maximum_error = 0.0;
	double standard_error = 0.0;
	std::vector<double> error_vector;
	//Average needle tip position
	auto position = computeAverageOfPosition(m_PointSetTilt);
	MITK_INFO << "position" << position;
	string csvPath = "D:\\左右倾倒.csv";
	ofstream csvfile;
	csvfile.open(csvPath, ios::out | ios::trunc);
	csvfile << "工具左右倾倒" << "," << "X" << "," << "Y" << "," << "Z" << "\n";
	mitk::Point3D error;
	for (int index = 0; index < m_PointSetTilt->GetSize(); ++index)
	{
		mitk::Point3D point = m_PointSetTilt->GetPoint(index);
		mistake = eachDifference(position, point);
		error_vector.push_back(mistake);
		average_error += mistake;
		if (maximum_error < mistake)
		{
			maximum_error = mistake;
		}
		csvfile << "坐标(mm)" << "," << point[0] << "," << point[1] << "," << point[2] << "\n";
		csvfile << "偏差(mm)" << "," << mistake << "\n";
	}
	average_error /= m_PointSetTilt->GetSize();
	standard_error = standardError(error_vector);
	csvfile << "maximum_error(mm)" << "," << maximum_error << "\n";
	csvfile << "average_error(mm)" << "," << average_error << "\n";
	csvfile << "standard_error(mm)" << "," << standard_error << "\n";
	csvfile.close();
	MITK_INFO << "mistake" << mistake;
	MITK_INFO << "average_error" << average_error;
	MITK_INFO << "maximum_error" << maximum_error;
	MITK_INFO << "standard_error" << standard_error;
}

void AccuracyTest::computeDistance()
{
	double distance = 0.0;
	std::vector<double> distance_vector;
	double distanceDifference = 0.0;
	std::vector<double> distCompare;
	mitk::Point3D point20;
	point20 = m_distancePointSet->GetPoint(0);
	string csvPath = "D:\\点与点距离测量.csv";
	ofstream csvfile;
	csvfile.open(csvPath, ios::out | ios::trunc);
	csvfile << "距离测量" <<","<<"X"<<","<<"Y"<<","<<"Z"<<"\n";
	csvfile << "20点坐标(mm)" << "," << point20[0] << "," << point20[1] << "," << point20[2] << "\n";
	for (int index = 1; index < m_distancePointSet->GetSize(); ++index)
	{
		mitk::Point3D point = m_distancePointSet->GetPoint(index);
		//点与点之间的距离
		distance = m_distancePointSet->GetPoint(0).EuclideanDistanceTo(point);
		distance_vector.push_back(distance);

		csvfile << "其余点坐标(mm)" << "," << point[0] << "," << point[1] << "," << point[2] << "\n";
		csvfile << "其余点与20点的距离测量值(mm)" << "," << distance << "\n";
	}
	for (int i = 0; i < distance_vector.size(); ++i)
	{
	MITK_INFO << "序号:"<<i 
		<<"---距离值："<<distance_vector[i];
	}
	distCompare = distanceCompare(distance_vector);
	distCompare.push_back(distanceDifference);
	for (int i = 0; i < distCompare.size()-1; ++i)
	{
		//csvfile << "其余点与20点的距离测量值(mm)" << "," << distance_vector[i] << "\n";
		csvfile << "长度偏差(mm)" << "," << distCompare[i] << "\n";
		MITK_INFO << "distCompare序号:" << i
			<< "---距离值：" << distCompare[i];
	}
	double maxdisterror = distCompare[0];
	double mindisterror = distCompare[0];
	double averageValue = 0.0;
	for (int i = 1; i < distCompare.size(); ++i)
	{
		if (distCompare[i] > maxdisterror)
		{
			//距离差值最大值
			maxdisterror = distCompare[i];
		}
		if (distCompare[i] < mindisterror)
		{
			//距离差值最小值
			mindisterror = distCompare[i];
		}
	}
	//距离差值平均值
	averageValue = averageValueCompute(distCompare);
	//距离差值标准差
	double standard = standardError(distCompare);
	csvfile << "最大偏差(mm)" << "," << maxdisterror << "\n";
	csvfile << "平均偏差(mm)" << "," << averageValue << "\n";
	csvfile << "标准偏差(mm)" << "," << standard << "\n";
	csvfile.close();
	MITK_INFO << "距离差值最大值" << maxdisterror;
	MITK_INFO << "距离差值最小值" << mindisterror;
	MITK_INFO << "距离差值平均值" << averageValue;
	MITK_INFO << "距离差值标准差" << standard;
}