#include"DZ_Robot.h"

DZ_Robot::DZ_Robot()
{

}
std::vector<double>  DZ_Robot::Dazu_matrix2angle(const double R[3][3])
{
	std::vector<double> angles(3);

	if (R[2][0] < 1)
	{
		if (R[2][0] > -1)
		{
			angles[1] = asin(R[2][0]);           // Y angle
			angles[0] = atan2(-R[2][1], R[2][2]); // X angle
			angles[2] = atan2(-R[1][0], R[0][0]); // Z angle
		}
		else
		{
			// R[2][0] == -1
			angles[1] = -M_PI / 2;
			angles[0] = -atan2(R[1][2], R[1][1]);
			angles[2] = 0;
		}
	}
	else
	{
		// R[2][0] == 1
		angles[1] = M_PI / 2;
		angles[0] = atan2(R[1][2], R[1][1]);
		angles[2] = 0;
	}

	return angles;
}


void DZ_Robot::RobotMove(vtkMatrix4x4* T_robot)
{
	const double R[3][3] = { {T_robot->GetElement(0, 0), T_robot->GetElement(0, 1), T_robot->GetElement(0, 2)},
	 {T_robot->GetElement(1, 0), T_robot->GetElement(1, 1), T_robot->GetElement(1, 2)},
	 {T_robot->GetElement(2, 0), T_robot->GetElement(2, 1), T_robot->GetElement(2, 2) }
	};
	//rotation2abc
	std::vector<double> xyz = Dazu_matrix2angle(R);


	//电箱号 机器人id号  空间目标位置
	int nRet = HRIF_MoveJ(0, 0, T_robot->GetElement(0,3), T_robot->GetElement(1, 3), T_robot->GetElement(2, 3), xyz[0], xyz[1], xyz[2],0, 0, 0, 0, 0, 0, sTcpName, sUcsName,
		dVelocity, dAcc, dRadius, nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
}
//是否检测di停止 电箱di索引0—7 di状态10
void DZ_Robot::set_external_IO(int nIsSeek, double nIOBit, int nIOState)
{
	//用做外部的io检测初始化这个类的时候需要使用
	this->nIsSeek = nIsSeek;
	this->nIOBit = nIOBit;
	this->nIOState = nIOState;
}