#pragma once 
#include <HR_Pro.h>
#include<iostream>
#include<vector>
#include <array>
#include <atomic>
#include <mutex>
#include <thread>
#include<map>
#include <QLabel>
#include <QTimer>
#include <QObject>
#include <QThread>
#include <QWidget>
class Robot
{
public:
	// 虚析构函数
	virtual ~Robot() = default;

	/**
	 * @brief 连接到机器人
	 * @param error 错误信息输出
	 * @return 是否成功
	 */
	virtual bool connect(std::string& error) = 0;
	/**
	 * @brief 检查是否连接到机器人
	 * @param error 错误信息输出
	 * @return 是否成功
	 */
	virtual bool Isconnect(std::string& error) = 0;
	/**
	 * @brief 断开连接到机器人
	 * @param error 错误信息输出
	 * @return 是否成功
	 */
	virtual bool Disconnect(std::string& error) = 0;

	/**
	 * @brief 开启机器人电源
	 * @param error 错误信息输出
	 * @return 是否成功
	 */
	virtual bool PowerOn(std::string& error) = 0;

	/**
	 * @brief 关闭机器人电源
	 * @param error 错误信息输出
	 * @return 是否成功
	 */
	virtual bool PowerOff(std::string& error) = 0;

	/**
	 * @brief 设置TCP
	 * @param TCP 6维的TCP参数数组
	 * @param error 错误信息输出
	 * @return 是否成功
	 */
	virtual bool setTCP(const std::array<double, 6>& TCP, std::string& error) = 0;

	/**
	 * @brief 运动到指定关节角
	 * @param joint_value 6维关节角度
	 * @param error 错误信息输出
	 * @return 是否成功
	 */
	virtual bool movej(const std::array<double, 6>& joint_value, std::string& error) = 0;

	/**
	 * @brief 运动到指定TCP位置
	 * @param TCP_value 6维TCP位置
	 * @param error 错误信息输出
	 * @return 是否成功
	 */
	virtual bool moveP(const std::array<double, 6>& TCP_value, std::string& error) = 0;

	/**
	 * @brief 打印错误信息
	 * @param nRet 错误代码
	 * @param error 错误信息
	 * @param behavior 当前行为
	 */
	virtual void printError(int nRet, std::string& error, std::string behavior) = 0;

	/**
	 * @brief 读取机器人当前位置数据
	 * @param error 错误信息输出
	 * @return 是否成功
	 */
	virtual bool readPositionData(std::string& error) = 0;

	/**
	 * @brief 设置速度和加速度
	 * @param Velocity 速度
	 * @param dAcc 加速度
	 * @return 是否成功
	 */
	virtual bool setVelocity_dAcc_dRadius(const double& Velocity, const double& dAcc,const double &dRadius) = 0;

	/**
	 * @brief 检查机器人是否在运动
	 * @param statue 是否在运动
	 * @return 是否成功
	 */
	virtual bool isMoving(bool& statue) = 0;

	virtual std::array<double, 6>getEndPosition() = 0;
	virtual std::array<double, 6>getJointValue() = 0;
	virtual std::array<double, 6>getFlange2Tcp() = 0;
	virtual std::array<double, 6>getCurrentUcs() = 0;
	//
};


class Robot_Hans :public QObject
{
	Q_OBJECT
public:

	/**
	 * @brief  构造函数
	 * @param  1：hostname ip地址  2：nPort 端口号
	 * @return [return type]
	 * @author zzx
	 * @date   2024/9/2
	 */
	Robot_Hans(std::string,int nPort);
	~Robot_Hans();
	
public:
	bool connect(string& error);
	bool PowerOn(string& error);
	bool PowerOff(string& error);
	bool setTCP(const std::array<double, 6>& TCP,string &error);

	bool configTcp(const std::array<double,6>& TCP,string TCP_Name,string &error);

	bool movej(const std::array<double, 6>& joint_value,string &error);
	bool moveP(const std::array<double, 6>& TCP_value, string& error);
	void printError(int nRet, string &error,string behavior);
	bool readPositionData(string &error);
	bool setVelocity_dAcc_dRadius(const double &Velocity, const double &dAcc, const double& dRadius);
	bool isMoving(bool& statue);
	bool Stop(string& error);
	std::array<double, 6>getEndPosition();
	std::array<double, 6>getJointValue();
	std::array<double, 6>getFlange2Tcp();
	std::array<double, 6>getCurrentUcs();

	bool RelMove(std::string& error,const int &Axis, const int& Direction, const double& Distance,const double& toolMotion);
	
private:



//一些默认参数勿修改
private:
	unsigned int boxID = 0;
	unsigned int rbtID = 0;
	unsigned short int nPort = 10003;
	unsigned int nMoveType = 0;
	std::string hostname = "192.168.0.10";
	int nIsSeek = 0;
	int nIOBit = 0;
	int nIOState = 0;
	string sTcpName = "TCP_1";
	string sUcsName = "Base";
	double dVelocity = 10;
	//速度需要小于加速度 
	double dAcc = 30;
	double dRadius = 40;
	bool bDone;
	std::map<int, std::string> poseMap = {
	   {0, "x"}, {1, "y"}, {2, "z"}, {3, "rx"}, {4, "ry"}, {5, "rz"}
	};
	QTimer* update_date{ nullptr };//相机拿数据的QT定时器
	
private:
	std::array<double, 6> end_Position = { 0 };
	std::array<double, 6> joint_value = { 0 };
	std::array<double, 6> tcp = { 0 };
	std::array<double, 6> ucs = { 0 };
	std::array<double, 6>set_tcp = { 0 };
	


signals:
	//void positionDataUpdated(std::array<double, 6> jointValue, std::array<double, 6> endPosition);  // 定义一个信号用于更新位置信息

public slots:
	void update();
};

