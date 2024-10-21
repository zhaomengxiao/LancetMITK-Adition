#include "Robot_Hans.h"
bool Robot_Hans::connect(string& error)
{
	int nRet = HRIF_Connect(boxID, hostname.c_str(), nPort);
	printError(nRet, error, "connect");

	//
	//if (update_date == nullptr)
	//{
	//	update_date = new QTimer(this);
	//}

	//QObject::connect(update_date, SIGNAL(timeout()), this, SLOT(update())); //拿数据
	//update_date->start(100);
	//
	if (HRIF_IsConnected(0))
	{
		
		return true;
	}
	else
	{
		
		return false;
	}
}

void Robot_Hans::update()
{
	std::string error;
	readPositionData(error);
}
bool Robot_Hans::PowerOn(string& error)
{
	int nRet = HRIF_GrpEnable(boxID, rbtID);
	printError(nRet, error, "power ON");
	if (!nRet)
	{
		
		return true;
	}
	else
	{
		
		return false;
	}
}
bool Robot_Hans::PowerOff(string& error)
{
	int nRet = HRIF_GrpDisable(boxID, rbtID);
	printError(nRet, error, "power off");
	if (!nRet)
	{
		
		return true;
	}
	else
	{
		
		return false;
	}
}
bool Robot_Hans::setTCP(const std::array<double, 6>& TCP, string& error)
{
	int nRet = HRIF_SetTCP(boxID, rbtID, TCP[0], TCP[1], TCP[2], TCP[3], TCP[4], TCP[5]);

	printError(nRet, error, "set tcp");
	if (!nRet)
	{
		set_tcp = TCP;
		
		return true;
	}
	else
	{
		
		return false;
	}
}

bool Robot_Hans::configTcp(const std::array<double, 6>& TCP, string TCP_Name, string& error)
{
	int nRet = HRIF_ConfigTCP(boxID, rbtID, TCP_Name, TCP[0], TCP[1], TCP[2], TCP[3], TCP[4], TCP[5]);
	nRet = HRIF_SetTCPByName(boxID, rbtID, TCP_Name);
	nRet = HRIF_SetToolMotion(boxID, rbtID, 1);
	nRet = HRIF_SetToolMotion(boxID, rbtID, 0);
	nRet = HRIF_SetToolMotion(boxID, rbtID, 1);
	nRet = HRIF_SetTCPByName(boxID, rbtID, TCP_Name);
	nRet = HRIF_SetTCP(boxID, rbtID, TCP[0], TCP[1], TCP[2], TCP[3], TCP[4], TCP[5]);
	printError(nRet, error, "config tcp");
	if (!nRet)
	{
		set_tcp = TCP;
		return true;
	}
	else
	{
		return false;
	}
}




/**
 * @brief  [报错打印]
 * @param  
 * @return [return type]
 * @author zzx
 * @date   
 */

void Robot_Hans::printError(int nRet, string& error, string behavior)
{
	if (nRet == 0)
	{
		error = "succeed";
		std::cout << behavior << " Succeed" << std::endl;
		std::cout << "---------------------------------------------------" << std::endl;
		
	}
	else
	{
		
		std::cout << behavior << " Failed" << std::endl;

		std::cout << "ErrorCode: " << nRet << std::endl;
		string tmpstr;
		int Ret = HRIF_GetErrorCodeStr(0, nRet, tmpstr);
		std::cout << "Ret " << Ret << std::endl;
		error = tmpstr;

		//针对一些常见错误做的一些解释
		if (nRet == 39500)
		{
			std::cout << "Please check the connection of the robot arm" << std::endl;
		

		}
		if (nRet == 40025)
		{
			std::cout << "The robot is not in a state of readiness" << std::endl;
		}

		if (nRet >= 40000 && nRet <= 40500)
		{
			std::cout << "CDS executed command with error: " << nRet << std::endl;
			
		}
		else if (nRet >= 10000 && nRet <= 10015)
		{
			std::cout << "Robot servo drive reported fault code: " << nRet << std::endl;
			
		}
		else if (nRet >= 10016 && nRet <= 11000)
		{
			std::cout << "Robot collaboration algorithm detected fault: " << nRet << std::endl;
			
		}
		else if (nRet >= 15000 && nRet <= 16000)
		{
			std::cout << "Robot control module detected fault: " << nRet << std::endl;
		
		}
		else if (nRet >= 30001 && nRet <= 30016)
		{
			std::cout << "Modbus module error during command execution: " << nRet << std::endl;
		
		}
		else if (nRet >= 20000 && nRet <= 39999)
		{
			if (nRet == 20031)
				std::cout << "the target cant arrived" << std::endl;
			std::cout << "CPS executed command with error: " << nRet << std::endl;
			
		}

		std::cout << "---------------------------------------------------" << std::endl;
		
	}
}

/**
 * @brief  执行关节运动
 * @param  传入值为joint_value end_position 做逆解参考从机械臂读取，其他为默认值。
 * @return [return type]
 * @author zzx
 * @date   2024/9/2
 */
bool Robot_Hans::movej(const std::array<double, 6>& joint_value, string& error)
{
	int nIsUseJoint = 1;
	readPositionData(error);
	int nRet = HRIF_WayPointEx(boxID, rbtID, nMoveType, end_Position[0], end_Position[1], end_Position[2], end_Position[3], end_Position[4], end_Position[5],
		joint_value[0], joint_value[1], joint_value[2], joint_value[3], joint_value[4], joint_value[5],
		set_tcp[0], set_tcp[1], set_tcp[2], set_tcp[3], set_tcp[4], set_tcp[5], ucs[0], ucs[1], ucs[2], ucs[3], ucs[4], ucs[5], dVelocity, dAcc, dRadius, nIsUseJoint, nIsSeek, nIOBit, nIOState, "ID0");
	printError(nRet, error, "move j");
	if (!nRet)
	{
		
		return true;
	}
	else
	{
		
		return false;
	}

}
/**
 * @brief  执行末端运动
 * @param  传入值为TCP_value joint_value 做逆解参考实时从机械臂读取，其他为默认值。
 * @return [return type]
 * @author zzx
 * @date   2024/9/2
 */
bool Robot_Hans::moveP(const std::array<double, 6>& TCP_value, string& error)
{
	int nIsUseJoint = 0;
	readPositionData(error);
	int nRet = HRIF_WayPointEx(boxID, rbtID, nMoveType, TCP_value[0], TCP_value[1], TCP_value[2], TCP_value[3], TCP_value[4], TCP_value[5],
		joint_value[0], joint_value[1], joint_value[2], joint_value[3], joint_value[4], joint_value[5],
		set_tcp[0], set_tcp[1], set_tcp[2], set_tcp[3], set_tcp[4], set_tcp[5], ucs[0], ucs[1], ucs[2], ucs[3], ucs[4], ucs[5], dVelocity, dAcc, dRadius, nIsUseJoint, nIsSeek, nIOBit, nIOState, "ID0");
	printError(nRet, error, "move p");
	if (!nRet)
	{
		
		return true;
	}
	else
	{
		
		return false;
	}

}
/**
 * @brief  读取机械臂数据
 * @param  
 * @return [return type]
 * @author zzx
 * @date   2024/9/2
 */
bool  Robot_Hans::readPositionData(string& error)
{
	
		double dX, dY, dZ, dRx, dRy, dRz;
		double dJ1, dJ2, dJ3, dJ4, dJ5, dJ6;
		double dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz;
		double dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz;

		int nRet = HRIF_ReadActPos(
			boxID, rbtID, dX, dY, dZ, dRx, dRy, dRz,
			dJ1, dJ2, dJ3, dJ4, dJ5, dJ6,
			dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz,
			dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz
		);
		//printError(nRet, error, "read robot data");
		if (nRet == 0)
		{  // 假设0表示成功
			end_Position = { dX, dY, dZ, dRx, dRy, dRz };
			joint_value = { dJ1, dJ2, dJ3, dJ4, dJ5, dJ6 };
			tcp = { dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz };
			ucs = { dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz };
			
			return true;
		}
		else
		{
			std::cout << "read robot data error" << std::endl;
			return false;
		}

	
}
bool Robot_Hans::setVelocity_dAcc_dRadius(const double& Velocity, const double& dAcc, const double& dRadius)
{
	if (Velocity > dAcc)
	{
		std::cout << "input is error Velocity must be less than dAcc" << std::endl;
		return false;
	}
	else
	{
		std::cout << "set succeed" << std::endl;
		this->dAcc = dAcc;
		this->dVelocity = Velocity;
		this->dRadius = dRadius;
		return true;
	}
	
}
bool Robot_Hans::isMoving(bool& statue)
{
	
	int nRet =HRIF_IsMotionDone(boxID, rbtID, statue);
	if (nRet == 0)
	{
		//运动完成
		if (statue == true)
		{
			return false;
		}
		else//运动未完成
		{
			return true;
		}
	}
	else
	{
		std::cout << "ismoving model is error please check " << std::endl;
		return false;
	}
}
std::array<double, 6>Robot_Hans::getEndPosition()
{
	std::string error;
	readPositionData(error);
	std::cout << "EndPosition is----------" << std::endl;
	for (auto& j : end_Position)
	{
		std::cout << "  " << j << " ";
	}
	auto t = end_Position;

	std::cout << std::endl;
	return end_Position;
}
std::array<double, 6>Robot_Hans::getJointValue()
{
	std::string error;
	readPositionData(error);
	std::cout << "getJointValue is----------" << std::endl;

	for (auto& j : joint_value)
	{
		std::cout << "  " << j << " ";
	}
	std::cout << std::endl;
	return joint_value;
}
std::array<double, 6>Robot_Hans::getFlange2Tcp()
{
	std::string error;
	readPositionData(error);
	std::cout << "getFlange2Tcp is----------" << std::endl;
	for (auto& j : tcp)
	{
		std::cout << "  " << j << " ";
	}
	std::cout << std::endl;
	return tcp;
}
std::array<double, 6>Robot_Hans::getCurrentUcs()
{
	std::string error;
	readPositionData(error);
	std::cout << "getCurrentUcs is----------" << std::endl;
	for (auto& j : ucs)
	{
		std::cout << "  " << j << " ";
	}
	std::cout << std::endl;
	return ucs;
}
bool Robot_Hans::Stop(string& error)
{
	int nRet = HRIF_GrpStop(boxID,rbtID);
	printError(nRet, error, "stop Robot");
	if (!nRet)
	{
		return true;
	}
	else
	{
		return false;
	}

}
bool Robot_Hans::RelMove(std::string &error,const int& Axis, const int& Direction, const double& Distance,const double &toolmotion)
{
	int nRet = HRIF_MoveRelL(boxID, rbtID, Axis, Direction, Distance, toolmotion);
	printError(nRet, error, "Rel Move :" +poseMap[Axis]);
	if (!nRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Robot_Hans::Robot_Hans(std::string hostname, int nPort):hostname(hostname),nPort(nPort)
{
	std::cout << "RObot IP is:" << this->hostname << "  " << "Robot port is" << this->nPort << std::endl;
}

Robot_Hans::~Robot_Hans()
{

}
