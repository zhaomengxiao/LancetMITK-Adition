#pragma once
#include"HR_Pro.h"
#include<vtkMatrix4x4.h>
#include<vector>
#include<iostream>
class DZ_Robot
{
private:
	const double M_PI = 3.1415926;
	string sTcpName;
	string sUcsName;
	double dVelocity;
	double dAcc;
	double dRadius;
	int nIsUseJoint;
	int nIsSeek;
	double nIOBit;
	int nIOState;
	string strCmdID;
public:
	DZ_Robot();

	void RobotMove(vtkMatrix4x4* T_robot);
	std::vector<double> Dazu_matrix2angle(const double R[3][3]);
	void set_external_IO(int nIsSeek,double nIOBit,int nIOState);
};
