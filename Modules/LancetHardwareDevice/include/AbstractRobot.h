/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef ABSTRACTROBOT_h
#define ABSTRACTROBOT_h
#include <QObject>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkSmartPointer.h>
#include <QFileDialog>
#include <vector>
#include <qstring.h>
#include <eigen3/Eigen/Dense>
#include <QApplication>
#include <QThread>
//#include <MitkLancetHardwareDeviceExports.h>
class  AbstractRobot : public QObject
{
public:
	//AbstractRobot()// : m_InitialPos(vtkSmartPointer<vtkMatrix4x4>::New())
	//{
	//}
	virtual void Connect() = 0;
	virtual void PowerOn() = 0;
	virtual void PowerOff() = 0;
	void SetRobotIpAddress(const char* aIP)
	{
		m_IpAddress = aIP;
	}
	virtual void Translate(double x, double y, double z) = 0;
	virtual void Translate(double* aDirection, double aLength) = 0;
	virtual void Rotate(double* aDirection, double aAngle) = 0;

	virtual void RecordInitialPos() = 0;
	virtual void GoToInitialPos() = 0;
	virtual void SetTCPToFlange() = 0;
	virtual bool SetTCP(vtkMatrix4x4* aMatrix) = 0;

	virtual std::vector<double> GetJointAngles() = 0;
	virtual void SetJointAngles(double* aJointAngles) = 0;

	virtual vtkSmartPointer<vtkMatrix4x4> GetBaseToTCP() = 0;
	virtual vtkSmartPointer<vtkMatrix4x4> GetFlangeToTCP() = 0;
	virtual vtkSmartPointer<vtkMatrix4x4> GetBaseToFlange() = 0;

	virtual void RobotTransformInBase(double* aMatrix) = 0;
	virtual void RobotTransformInTCP(double* aMatrix) = 0;

	virtual std::vector<double> GetCartImpeda() = 0;
	virtual bool SetCartImpeda(double* aImpeda) = 0;

	virtual bool SetVelocity(double aVelocity) = 0;

	virtual std::vector<std::vector<double>> GetJointAngleLimits() = 0;

	virtual void WaitMove() = 0;

protected:
	//vtkSmartPointer<vtkMatrix4x4> m_InitialPos;
	const char* m_IpAddress;
};

#endif
