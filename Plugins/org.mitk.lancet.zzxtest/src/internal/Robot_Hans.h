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
	// ����������
	virtual ~Robot() = default;

	/**
	 * @brief ���ӵ�������
	 * @param error ������Ϣ���
	 * @return �Ƿ�ɹ�
	 */
	virtual bool connect(std::string& error) = 0;
	/**
	 * @brief ����Ƿ����ӵ�������
	 * @param error ������Ϣ���
	 * @return �Ƿ�ɹ�
	 */
	virtual bool Isconnect(std::string& error) = 0;
	/**
	 * @brief �Ͽ����ӵ�������
	 * @param error ������Ϣ���
	 * @return �Ƿ�ɹ�
	 */
	virtual bool Disconnect(std::string& error) = 0;

	/**
	 * @brief ���������˵�Դ
	 * @param error ������Ϣ���
	 * @return �Ƿ�ɹ�
	 */
	virtual bool PowerOn(std::string& error) = 0;

	/**
	 * @brief �رջ����˵�Դ
	 * @param error ������Ϣ���
	 * @return �Ƿ�ɹ�
	 */
	virtual bool PowerOff(std::string& error) = 0;

	/**
	 * @brief ����TCP
	 * @param TCP 6ά��TCP��������
	 * @param error ������Ϣ���
	 * @return �Ƿ�ɹ�
	 */
	virtual bool setTCP(const std::array<double, 6>& TCP, std::string& error) = 0;

	/**
	 * @brief �˶���ָ���ؽڽ�
	 * @param joint_value 6ά�ؽڽǶ�
	 * @param error ������Ϣ���
	 * @return �Ƿ�ɹ�
	 */
	virtual bool movej(const std::array<double, 6>& joint_value, std::string& error) = 0;

	/**
	 * @brief �˶���ָ��TCPλ��
	 * @param TCP_value 6άTCPλ��
	 * @param error ������Ϣ���
	 * @return �Ƿ�ɹ�
	 */
	virtual bool moveP(const std::array<double, 6>& TCP_value, std::string& error) = 0;

	/**
	 * @brief ��ӡ������Ϣ
	 * @param nRet �������
	 * @param error ������Ϣ
	 * @param behavior ��ǰ��Ϊ
	 */
	virtual void printError(int nRet, std::string& error, std::string behavior) = 0;

	/**
	 * @brief ��ȡ�����˵�ǰλ������
	 * @param error ������Ϣ���
	 * @return �Ƿ�ɹ�
	 */
	virtual bool readPositionData(std::string& error) = 0;

	/**
	 * @brief �����ٶȺͼ��ٶ�
	 * @param Velocity �ٶ�
	 * @param dAcc ���ٶ�
	 * @return �Ƿ�ɹ�
	 */
	virtual bool setVelocity_dAcc_dRadius(const double& Velocity, const double& dAcc,const double &dRadius) = 0;

	/**
	 * @brief ���������Ƿ����˶�
	 * @param statue �Ƿ����˶�
	 * @return �Ƿ�ɹ�
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
	 * @brief  ���캯��
	 * @param  1��hostname ip��ַ  2��nPort �˿ں�
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



//һЩĬ�ϲ������޸�
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
	//�ٶ���ҪС�ڼ��ٶ� 
	double dAcc = 30;
	double dRadius = 40;
	bool bDone;
	std::map<int, std::string> poseMap = {
	   {0, "x"}, {1, "y"}, {2, "z"}, {3, "rx"}, {4, "ry"}, {5, "rz"}
	};
	QTimer* update_date{ nullptr };//��������ݵ�QT��ʱ��
	
private:
	std::array<double, 6> end_Position = { 0 };
	std::array<double, 6> joint_value = { 0 };
	std::array<double, 6> tcp = { 0 };
	std::array<double, 6> ucs = { 0 };
	std::array<double, 6>set_tcp = { 0 };
	


signals:
	//void positionDataUpdated(std::array<double, 6> jointValue, std::array<double, 6> endPosition);  // ����һ���ź����ڸ���λ����Ϣ

public slots:
	void update();
};

