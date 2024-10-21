#pragma once
/*******************************************************************
 *	Copyright(c) 2020-2022 Company Name
 *  All rights reserved.
 *	
 *	FileName:HR_Pro.h
 *	Descriptio:C++SDK
 *  version:1.0.5.0
 *	modification records:
 *	Date         Author          Description
 *  2022-09-16   ZhangJiansheng  add English comments for interfaces
 *  2022-08-29   Chenpeng        add comments for interfaces
 ******************************************************************/

#ifndef _HR_PRO_H_
#define _HR_PRO_H_


#include <string>
#include <vector>
using namespace std;

#define MaxBox 5
//#ifdef ONEDLL_EXPORTS
//#define ONEDLL_API __declspec(dllexport)
//#else
//#define ONEDLL_API __declspec(dllimport)
//#endif
#ifdef HANSROBOT_PRO_EXPORTS
#define HANSROBOT_PRO_API __declspec(dllexport)
#else
#define HANSROBOT_PRO_API __declspec(dllimport)
#endif
#ifdef __cplusplus
extern "C"
{
#endif
//************************************************************************/
//**    Interfaces for callback / 回调类函数接口
//**    Parameter boxID in all the interfaces below indicates the number of control box. Maximum 5 boxes could be connected. / 以下的接口，boxID是代表哪个电箱，最多可以连接5个电箱
//**    Euler angles are used to indicate orientation by default, with unit of degree. / 默认使用欧拉角表示，单位是度                                                                  
//************************************************************************/

/**
 *	@brief: Register callback function to get log / 注册日志回调函数
  *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param logLevel : Log level  / 日志等级
 *	@param strLog : Log information / 日志信息
 *	@param arg:
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
typedef void (*LogDbgCallback)(int logLevel, const string& strLog, void* arg);
HANSROBOT_PRO_API int HRIF_SetLogDbgCB(unsigned int boxID, LogDbgCallback ptr, void* arg);

/**
 *	@brief: Register callback function to get event / 事件回调函数
  *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nErrorCode : Error code / 错误码
 *	@param nState : State / 状态
 *	@param strState: State information / 状态信息
 *	@param arg:
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
typedef void (*EventCallback)(int nErrorCode, int nState, const string& strState, void* arg);
HANSROBOT_PRO_API int HRIF_SetEventCB(unsigned int boxID, EventCallback ptr, void* arg);


//************************************************************************/
//**   Interfaces for initialization / 初始化函数接口                                                             
//************************************************************************/
/**
 *	@brief: Connect to port 10003 of CPS / 连接控制器10003端口
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param hostName : IP address of CPS / 控制器IP地址，根据实际设置的IP地址定义
 *	@param nPort : Port number, 10003 as default / 控制器端口号，默认值=10003
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_Connect(unsigned int boxID, const char* hostName, unsigned short nPort);

/**
 *	@brief: Disconnet from port 10003 of CPS / 断开连接控制器10003端口
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_DisConnect(unsigned int boxID);

/**
 *	@brief: Check the connection status to CPS / 检查控制器连接状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@return : false : Disconnected / 控制器未连接
 *            true : Connected / 控制器已连接
 */
HANSROBOT_PRO_API bool HRIF_IsConnected(unsigned int boxID);

/**
 *	@brief: Power off the robot and shut down the system / 控制器断电(断开机器人供电，系统关机)
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ShutdownRobot(unsigned int boxID);

/**
 *	@brief: Connect to control box / 连接控制器电箱
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_Connect2Box(unsigned int boxID);

/**
 *	@brief: Power on the robot / 机器人上电
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
  *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_Electrify(unsigned int boxID);

/**
 *	@brief: Power off the robot / 机器人断电
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded  / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_Blackout(unsigned int boxID);

/**
 *	@brief: Connect to the controller, start master, initialize slave, configure and check paramters, finally switch to DISABLE state / 连接过程中会启动主站，初始化从站，配置参数，检查配置，完成后跳转到去使能状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_Connect2Controller(unsigned int boxID);

/**
 *	@brief: Check the simulation status / 判断是否为模拟机器人
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nSimulateRobot ：Simulation status/ 是否为模拟机器人
 *	                           0 : real mode / 真实机器人
 *                             1 : simulated mode / 模拟机器人
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_IsSimulateRobot(unsigned int boxID, int& nSimulateRobot);

/**
 *	@brief: Check the simulation status / 设置机器人的模拟状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nSimulation ：Simulation status/ 设置机器人的状态
 *	                           0 : real mode / 真实机器人
 *                             1 : simulated mode / 模拟机器人
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetSimulation(unsigned int boxID, unsigned int rbtID, int nSimulation);

/**
 *	@brief: Check the controller's state   / 控制器是否启动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nStarted ：Start status/ 是否启动
 *	                      0 : not started / 未启动
 *                        1 : started / 启动
 * 
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_IsControllerStarted(unsigned int boxID, int& nStarted);

/**
 *	@brief: Read the version of robot / 读取机器人版本号
 *          Version consists of digits separated by '.'  / 版本号的格式以.分隔，数字的意义如下
 *  CPSVer.controlVer.BoxVerMajor.BoxVerMid.BoxVerMin.AlgorithmVer.ElfinFirmwareVer

 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param strVer : Whole version / 整体版本号
 *	@param nCPSVersion : CPS version / CPS版本
 *	@param nCodesysVersion : Controller version / 控制器版本
 *	@param nBoxVerMajor : Control box version, 0 for simulated, 1~4 for real / 电箱版本号:0：模拟电箱 1-4：电箱版本号
 *	@param nBoxVerMid : Firmware version of control board / 控制板固件版本
 *	@param nBoxVerMin : Firmware version of control board / 控制板固件版本
 *	@param nAlgorithmVer : Algorithm version / 算法版本
 *	@param nElfinFirmwareVer : Firmware version / 固件版本
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadVersion(unsigned int boxID, unsigned int rbtID, string& strVer, int& nCPSVersion, int& nCodesysVersion,
                     int &nBoxVerMajor, int &nBoxVerMid, int &nBoxVerMin,
                     int &nAlgorithmVer, int &nElfinFirmwareVer);

/**
 *	@brief: Get explanation of error code / 获取错误码的解释
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nErrorCode : Error code / 错误码
 *  @param strErrorMsg : Error code explanation / 错误码解释
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GetErrorCodeStr(unsigned int boxID, int nErrorCode, string &strErrorMsg);
/**
 *	@brief: Read the robot model / 读取机器人类型
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param strModel : Robot model / 机器人类型
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadRobotModel(unsigned int boxID, string& strModel);

/**
 *	@brief: Enable the robot / 机器人使能命令
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpEnable(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief: Disable the robot / 机器人去使能命令
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpDisable(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief: Reset the robot / 机器人复位命令
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpReset(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief: Stop robot moving / 停止运动命令
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpStop(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief: Pause robot moving / 暂停运动命令
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpInterrupt(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief: Continue robot moving after pause / 继续运动命令
 *	@param boxID : Control box number, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpContinue(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief: Open free driver / 开启零力示教
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpOpenFreeDriver(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief: Close free driver / 关闭零力示教
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpCloseFreeDriver(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief: Open the brake / 松闸
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nAxisID : Robot Axis ID，0-5 / 轴ID
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_OpenBrake(unsigned int boxID, unsigned int rbtID, int nAxisID);

/**
 *	@brief: Close the brake / 抱闸
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nAxisID : Robot Axis ID，0-5 / 轴ID
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_CloseBrake(unsigned int boxID, unsigned int rbtID, int nAxisID);

/**
 *	@brief: Read the brake status of each Axis / 读取各关节松/抱闸状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param stateJ1~J6 : Robot Axis status / 各关节轴状态，1：松闸，0：抱闸
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBrakeStatus(unsigned int boxID,unsigned int rbtID,int& stateJ1,int& stateJ2,int& stateJ3,int& stateJ4,int& stateJ5,int& stateJ6);

//************************************************************************/
//**    Interfaces for script / 脚本函数接口                                                             
//************************************************************************/
/**
 *	@brief: Run the specified function / 运行指定脚本函数
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param strFuncName : The specified function name composed in the teaching pendant / 指定脚本函数名称，对应示教器界面的函数
 *	@param param :  Parameters / 参数
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_RunFunc(unsigned int boxID, string strFuncName, vector<string> param);

/**
 *	@brief: Run the main function to start the script execution composed and compiled in the teaching pendant / 执行脚本Main函数，调用后执行示教器页面编译好的脚本文件
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_StartScript(unsigned int boxID);

/**
 *	@brief: Stop the script execution running in teaching pendant / 停止脚本，调用后停止示教器页面正在执行脚本文件
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_StopScript(unsigned int boxID);

/**
 *	@brief: Pause the script execution running in teaching pendant / 暂停脚本，调用后暂停示教器页面正在执行脚本文件
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PauseScript(unsigned int boxID);

/**
 *	@brief:Continue the script execution paused int the teaching pendant / 继续脚本，调用后继续运行示教器页面正在暂停的脚本文件，不处于暂停状态则返回20018错误
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ContinueScript(unsigned int boxID);


//************************************************************************/
//**    Interfaces for control box / 电箱控制函数接口                                                             
//************************************************************************/
/**
 *	@brief:读取电箱信息
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nConnected : Control box connection status / 电箱连接状态
 *	@param n48V_ON : Voltage value with 48V / 48V电压值
 *	@param d48OUT_Voltag : 48V output voltage / 48V输出电压值
 *	@param d48OUT_Current : 48V output current / 48V输出电流值
 *	@param nRemoteBTN : Remote emergency stop status / 远程急停状态
 *	@param nThreeStageBTN : Three stage status / 三段按钮状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxInfo(unsigned int boxID, unsigned int rbtID,
                    int &nConnected, 
                    int &n48V_ON, 
                    double &d48OUT_Voltag, 
                    double &d48OUT_Current, 
                    int &nRemoteBTN, 
                    int &nThreeStageBTN);

/**
 *	@brief: Read box control input / 读取电箱控制数字输入状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nBit : CI bit / 控制数字输入位
 *	@param nVal : CI value / 控制数字输入对应位状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxCI(unsigned int boxID, int nBit, int& nVal);

/**
 *	@brief: Read box digital input / 读取电箱通用数字输入状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nBit : DI bit / 通用数字输入位
 *	@param nVal : DI value /通用数字输入对应位状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxDI(unsigned int boxID, int nBit, int& nVal);

/**
 *	@brief: Read box control output / 读取电箱控制器数字输出状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nBit : CO bit / 控制器数字输出位
 *	@param nVal : CO value /控制数字输出对应位状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxCO(unsigned int boxID, int nBit, int& nVal);

/**
 *	@brief: Read box digital output / 读取电箱通用数字输出状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nBit : DO bit / 通用数字输出位
 *	@param nVal : DO value /通用数字输出对应位状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxDO(unsigned int boxID, int nBit, int& nVal);

/**
 *	@brief: Read box analog input / 读取电箱模拟量输入值
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nBit : AI bit / 模拟量输入通道
 *	@param dbVal : Current (4~20mA) or voltage (0~10V) / 电流模式:4-20mA,电压模式:0-10V
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxAI(unsigned int boxID, int nBit, double& dbVal);

/**
 *	@brief: Read box analog output / 读取电箱模拟量输出模式和值
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nBit : AO bit / 模拟量输出通道
 *	@param nMode : Current or voltage mode / 对应位模拟量通道模式
 *	@param dbVal : Current value (4~20mA) or voltage value (0~10V) / 电流模式:4-20mA,电压模式:0-10V
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxAO(unsigned int boxID, int nBit, int& nMode, double& dbVal);

/**
 *	@brief: Set Box control output / 设置控制数字输出状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nBit : CO bit / 控制数字输出位
 *	@param nVal : CO value / 控制器数字输出目标状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetBoxCO(unsigned int boxID, int nBit, int nVal);

/**
 *	@brief: Set Box digital output / 设置电箱通用数字输出状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nBit : DO bit / 通用数字输出位
 *	@param nVal : DO value / 通用数字输出目标状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetBoxDO(unsigned int boxID, int nBit, int nVal);

/**
 *	@brief: Set Box analog output mode / 设置电箱模拟量输出模式
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nBit : AO bit / 模拟量输出模式通道
 *	@param nMode : AO mode / 模拟量输出模式目标状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetBoxAOMode(unsigned int boxID, int nBit, int nMode);

/**
 *	@brief: Set Box analog output / 设置电箱模拟量输出值
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param nBit : AO bit / 模拟量输出通道
  *	@param dbVal : AO value / 模拟量输出目标值
 *	@param nMode : AO mode / 模拟量输出模式目标状态

 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetBoxAOVal(unsigned int boxID, int nBit, double dbVal, int nMode);

/**
 *	@brief: Read End digital input / 读取末端数字输入状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nBit : End DI bit / 末端数字输入索引
 *	@param nVal : End DI value / 末端数字输入状态值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadEndDI(unsigned int boxID, unsigned int rbtID, int& nBit, int& nVal);

/**
 *	@brief: Read End digital output / 读取末端数字输出状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nBit : End DO bit / 末端数字输出索引
 *	@param nVal : End DO value / 末端数字输出状态值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadEndDO(unsigned int boxID, unsigned int rbtID, int& nBit, int& nVal);

/**
 *	@brief: Set End digital output / 设置末端数字输出状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nBit : End DO bit / 末端数字输出索引
 *	@param nVal : End DO value / 末端数字输出状态值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetEndDO(unsigned int boxID, unsigned int rbtID, int nBit, int nVal);

/**
 *	@brief: Read End analog input / 读取末端模拟量输入状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nBit :  End AI bit / 末端模拟量输入索引
 *	@param nVal : End AI value / 末端模拟量输入状态值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadEndAI(unsigned int boxID, unsigned int rbtID, int& nBit, double& dVal);

/**
 *	@brief: Read End Button / 读取末端按键状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nBit1 : End button 1 / 末端按键1状态
 *	@param nBit2 : End button 2 / 末端按键2状态
 *	@param nBit3 : End button 3 / 末端按键3状态
 *	@param nBit4 : End button 4 / 末端按键4状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadEndBTN(unsigned int boxID, unsigned int rbtID, int& nBit1, int& nBit2, int& nBit3, int& nBit4);

//////////////////////////////////////////////////////////////////////////////////////
/**
 *	@brief: Set Override / 设置速度比
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dbOverride : Override value (0.01~1.00) / 需要设置的速度比(0.01-1)
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetOverride(unsigned int boxID, unsigned int rbtID, double dOverride);

/**
 *	@brief: Read Override / 读取当前速度比
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dbOverride : Override value  / 读取到的速度比
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadOverride(unsigned int boxID, unsigned int rbtID, double &dOverride);

/**
 *	@brief: Set Tool motion / 开启或关闭Tool坐标系运动模式
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nState : 0 for closed, 1 for open / 0(关闭)/1(开启)
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetToolMotion(unsigned int boxID, unsigned int rbtID, int nState);

/**
 *	@brief: Read the maximum payload / 读取末端最大负载
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dbMaxPayload : maximum payload at the end (kg) / 末端最大负载，单位：kg
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadMaxPayload(unsigned int boxID, unsigned int rbtID, double& dbMaxPayload);

/**
 *	@brief: Set payload / 设置当前负载参数
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param Mass：Mass / 质量
 *	@param dX：Gravity-CX / 质心X方向偏移
 *	@param dY：Gravity-CY / 质心Y方向偏移
 *	@param dZ：Gravity-CZ / 质心Z方向偏移
 *	@param bUpdateDB：可选参数，默认为1
 *                    1：修改负载，修改重心，写入数据库
 *                    3：修改负载，不修改重心，写入数据库
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetPayload(unsigned int boxID, unsigned int rbtID, double dMass, double dX, double dY, double dZ, unsigned char byOption=1);

/**
 *	@brief: Read payload / 读取当前负载参数
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param Mass：Mass / 质量
 *	@param dX：Gravity-CX / 质心X方向偏移
 *	@param dY：Gravity-CY / 质心Y方向偏移
 *	@param dZ：Gravity-CZ / 质心Z方向偏移
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadPayload(unsigned int boxID, unsigned int rbtID, double& dMass, double& dX, double& dY, double& dZ);

// 速度单位是毫米每秒，度每秒，加速度毫米每秒平方，度每秒平方
/**
 *	@brief: Set joint max velocity / 设置关节最大速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1~6 : Joint1~6 max velocity / J1-J6轴最大速度，单位[°/ s]
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetJointMaxVel(unsigned int boxID, unsigned int rbtID, double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6);

/**
 *	@brief: Set joint max acceleration / 设置关节最大加速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1~6 : Joint1~6 max acceleration /J1-J6轴轴最大加速度，单位[°/ s^2]
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetJointMaxAcc(unsigned int boxID, unsigned int rbtID, double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6);

/**
 *	@brief: Set linear max velocity / 设置直线运动最大速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dMaxVel : Linear max velocity / 直线运动最大速度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetLinearMaxVel(unsigned int boxID, unsigned int rbtID, double dMaxVel);

/**
 *	@brief: Set linear max acceleration / 设置直线运动最大加速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dMaxAcc : Linear max acceleration / 直线运动最大加速度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetLinearMaxAcc(unsigned int boxID, unsigned int rbtID, double dMaxAcc);

/**
 *	@brief: Set max range of joint motion / 设置最大关节运动范围
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dMaxJ1-dMaxJ6 : Max joint angle / 最大关节角度
 *	@param dMinJ1-dMinJ6 : Min joint angle /最小关节角度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetMaxAcsRange(unsigned int boxID, unsigned int rbtID,
                        double dMaxJ1, double dMaxJ2, double dMaxJ3, double dMaxJ4, double dMaxJ5, double dMaxJ6,
                        double dMinJ1, double dMinJ2, double dMinJ3, double dMinJ4, double dMinJ5, double dMinJ6);

/**
 *	@brief: Set max range of linear motion / 设置空间最大运动范围
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dMaxX-dMaxZ : Max range of XYZ / XYZ最大范围
 *	@param dMinX-dMinZ : Min range of XYZ / XYZ最小范围
 *	@param dUcs_X~Rz : UCS pose / 基于用户坐标
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetMaxPcsRange(unsigned int boxID, unsigned int rbtID,
                        double dMaxX, double dMaxY, double dMaxZ, double dMinX, double dMinY, double dMinZ,
                        double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz);

/**
 *	@brief: Set welding analog output voltage / 设置焊接模拟量输出电压
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nState : Channel switch status / 通道开关状态 0：关闭 1：开启
 *	@param nIndex : Voltage analog channel / 电压模拟量通道，AO0和AO1
 *	@param dInitAO : Initial voltage / 初始化电压
 *	@param dWeldingAO : Welding voltage / 焊接电压，单位：V
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetMoveParamsAO(unsigned int boxID, unsigned int rbtID,
                        int nState, int nIndex, double dInitAO, double dWeldingAO);

/**
 *	@brief: Read joint max velocity / 读取关节最大运动速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1~6 : Max velocity of J1~J6 / J1-J6轴最大运动速度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadJointMaxVel(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief: Read joint max acceleration / 读取关节最大加速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1~6 : Max acceleration of J1~J6 / J1-J6轴最大加速度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadJointMaxAcc(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief: Read joint max jerk / 读取关节最大加加速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1~6 : Max jerk of J1~J6 / J1-J6轴最大加加速度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadJointMaxJerk(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief:Read linear motion parameters /读取直线运动最大速度、加速度和加加速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dMaxVel : Linear motion velocity / 直线运动速度
 *	@param dMaxAcc : Linear motion acceleration / 直线运动加速度
 *	@param dMaxJerk : Linear motion jerk / 直线运动加加速度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadLinearMaxSpeed(unsigned int boxID, unsigned int rbtID, double& dMaxVel, double& dMaxAcc, double& dMaxJerk);

//////////////////////////////////////////////////////////////////////////////////////
/**
 *	@brief: Read Emergency stop information / 读取急停信息
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nESTO_Error: Error occurs when the two emergency signals are different / 急停回路有两路，当两路信号不相同时，则认为急停回路有错误，这个则为1
 *	@param nESTO： Cut off 48V output to the robot when an emergency stop occurs / 急停信号，发生急停时，会断48V输出到本体的供电，但是不会断220V到48V的供电
 *	@param nSafetyGuard_Error: Error occurs when the two safety guard signals are different / 安全光幕回路有两路，当两路信号不相同时，则认为安全光幕回路有错误，这个则为1
 *	@param nSafetyGuard：Stop the motion but never cutt off power supply when safety guard occurs / 安全光幕信号，发生安全光幕时，会停止机器人运动，并且不再接受运动指令，不会断本体供电
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadEmergencyInfo(unsigned int boxID, unsigned int rbtID,
                            int &nESTO_Error, 
                            int &nESTO, 
                            int &nSafetyGuard_Error, 
                            int &nSafetyGuard);

/**
 *	@brief: Enter or exit safety guard status by force to realize soft EStop(emergency stop). Move will be stopped. System can be reset after EStop is canceled. / 强制进入或者取消进入安全光幕状态，以此方式实现软件急停，以及软急停后的恢复。强制进入安全光幕状态后，当前的运动会被停止掉。需要先取消强制，然后才能复位系统。
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nFlag：1 to enter, 0 to cancel / 1: 强制进入安全光幕状态；0: 取消强制进入安全光幕状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_EnterSafetyGuard(unsigned int boxID, unsigned int rbtID, int nFlag);

/**
 *	@brief: Read robot state / 读取当前机器人状态标志
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nMovingState : Moving state / 运动状态
 *	@param nEnableState : Enable state / 使能状态
 *	@param nErrorState : Error state / 错误状态
 *	@param nErrorCode : Error code / 错误码
 *	@param nErrorAxis : Error axis ID / 错误轴ID
 *	@param nBreaking : Breaking status / 抱闸是否打开状态
 *	@param nPause : Pause state /暂停状态
 *	@param nEmergencyStop : Emergency stop state /急停状态
 *	@param nSaftyGuard : Safty guard state / 安全光幕状态
 *	@param nElectrify : Electrify state / 上电状态
 *	@param nIsConnectToBox : Connection of control box state / 连接电箱状态
 *	@param nBlendingDone : Moving blending done state / WayPoint运动完成状态
 *	@param nInPos : In actual pose state / 运动命令位置与实际位置是否到位
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadRobotState(unsigned int boxID, unsigned int rbtID,
						int &nMovingState, 
						int &nEnableState, 
						int &nErrorState, 
						int &nErrorCode, 
						int &nErrorAxis,
						int &nBreaking, 
						int &nPause,
						int &nEmergencyStop, 
						int &nSaftyGuard, 
						int &nElectrify, 
						int &nIsConnectToBox,
						int &nBlendingDone, 
						int &nInpos);

/**
 *	@brief: Read robot state / 读取当前机器人状态标志
 *	@param boxID : control box number, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nMovingState : Moving state / 运动状态
 *	@param nEnableState : Enable state / 使能状态
 *	@param nErrorState : Error state / 错误状态
 *	@param nErrorCode : Error code / 错误码
 *	@param nErrorAxis : Error axis ID / 错误轴ID
 *	@param nBreaking : Breaking status / 抱闸是否打开状态
 *	@param nPause : Pause state / 暂停状态
 *	@param nBlendingDone : Moving blending done state / WayPoint运动完成状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadRobotFlags(unsigned int boxID, unsigned int rbtID,
						int &nMovingState, 
						int &nEnableState, 
						int &nErrorState, 
						int &nErrorCode, 
						int &nErrorAxis,
						int &nBreaking, 
						int &nPause,
						int &nBlendingDone);
/**
 *	@brief: Read current waypoint ID / 读取WayPoint当前运动ID号
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param strCurWaypointID : Current ID / 当前ID号
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCurWaypointID(unsigned int boxID, unsigned int rbtID, string& strCurWaypointID);

/**
 *	@brief: Read axis error code / 读取错误码
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nErrorCode : Error code / 错误码
 *	@param Params : The error code of J1~J6, 0 if no error / 每个轴(J1~J6)的错误码，如果没有错误则为0
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadAxisErrorCode(unsigned int boxID, unsigned int rbtID, int& nErrorCode, int& nJ1, int& nJ2, int& nJ3, int& nJ4, int& nJ5, int& nJ6);

/**
 *	@brief: Read current FSM / 读取当前状态机状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nCurFSM : Current FSM / 当前状态机状态，具体描述见接口说明文档
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCurFSM(unsigned int boxID, unsigned int rbtID, int& nCurFSM, string& strCurFSM);

/**
 *	@brief:Read FSM from CPS / 读取状态机状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nCurFSM : Current FSM当前状态机状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCurFSMFromCPS(unsigned int boxID, unsigned int rbtID, int& nCurFSM);

/**
 *	@brief: Read actual pose and joint positions / 读取当前位置信息
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Rz : pose / 笛卡尔坐标
 *	@param dJ1~6 : joint positions / 关节坐标
 *	@param dTcp_X~Rz : TCP pose / TCP坐标
 *	@param dUcs_X~Rz : UCS pose / 用户坐标
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadActPos(unsigned int boxID, unsigned int rbtID,
					 double &dX, double &dY, double &dZ, double &dRx, double &dRy, double &dRz,
					 double &dJ1, double &dJ2, double &dJ3, double &dJ4, double &dJ5, double &dJ6,
					 double &dTcp_X, double &dTcp_Y, double &dTcp_Z, double &dTcp_Rx, double &dTcp_Ry, double &dTcp_Rz,
                    double &dUcs_X, double &dUcs_Y, double &dUcs_Z, double &dUcs_Rx, double &dUcs_Ry, double &dUcs_Rz);

/**
 *	@brief: Read point pose, joint positions, UCS and TCP by point name / 根据点位名称读取其位置信息
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1~6 : joint positions / 关节坐标
 *	@param dX~Rz : pose / 笛卡尔坐标
 *	@param dTcp_X~Rz : TCP pose / TCP坐标
 *	@param dUcs_X~Rz : UCS pose / 用户坐标
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadPointByName(unsigned int boxID, unsigned int rbtID, string strPointName,
    double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6,
    double& dX, double& dY, double& dZ, double& dRx, double& dRy, double& dRz,
    double& dTcp_X, double& dTcp_Y, double& dTcp_Z, double& dTcp_Rx, double& dTcp_Ry, double& dTcp_Rz,
    double& dUcs_X, double& dUcs_Y, double& dUcs_Z, double& dUcs_Rx, double& dUcs_Ry, double& dUcs_Rz);

/**
 *	@brief: Read command joint positions / 读取命令关节位置
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1~6 : Joint command positions / 命令关节位置
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCmdJointPos(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief: Read actual joint positions / 读取实际关节位置
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1~6 : Joint actual positions / 实际关节位置
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadActJointPos(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief: Read command pose of TCP / 读取命令TCP位置
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Rz : TCP command pose / 命令TCP位置
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCmdTcpPos(unsigned int boxID, unsigned int rbtID, double& dX, double& dY, double& dZ, double& dRx, double& dRy, double& dRz);

/**
 *	@brief: Read actual pose of TCP / 读取实际TCP位置
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Rz : TCP actual pose / 实际TCP位置
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadActTcpPos(unsigned int boxID, unsigned int rbtID, double& dX, double& dY, double& dZ, double& dRx, double& dRy, double& dRz);

/**
 *	@brief: Read command velocity of joint / 读取命令关节速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1~6 : Joint command velocity / 命令关节速度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCmdJointVel(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief: Read actual velocity of joint / 读取实际关节速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1~6 : Joint actual velocity / 实际关节速度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadActJointVel(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief: Read command velocity of TCP / 读取命令TCP速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Rz : TCP command velocity / 命令TCP速度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCmdTcpVel(unsigned int boxID, unsigned int rbtID, double& dX, double& dY, double& dZ, double& dRx, double& dRy, double& dRz);

/**
 *	@brief: Read actual velocity of TCP / 读取实际TCP速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Rz : TCP actual velocity / 实际TCP速度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadActTcpVel(unsigned int boxID, unsigned int rbtID, double& dX, double& dY, double& dZ, double& dRx, double& dRy, double& dRz);

/**
 *	@brief: Read command current of joint / 读取命令关节电流
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1~6 : Joint command current / 命令关节电流
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCmdJointCur(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief: Read actual current of joint / 读取实际关节电流
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1~6 : Joint actual current / 实际关节电流
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadActJointCur(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief: Read End velocity of TCP / 读取末端TCP速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dCmdVel : TCP command velocity / 命令速度mm/s
 *	@param dActVel : TCP actual velocity / 实际速度mm/s
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadTcpVelocity(unsigned int boxID, unsigned int rbtID, double& dCmdVel, double& dActVel);

//************************************************************************/
//**    Interfaces for calculation / 计算类函数接口                                                             
//************************************************************************/
/**
 *	@brief: Quaternion to Euler angle / 四元素转欧拉角，欧拉角的单位是度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dQuaW : Quaternion W / 四元素W
 *	@param dQuaX : Quaternion X / 四元素X
 *	@param dQuaY : Quaternion Y / 四元素Y
 *	@param dQuaZ : Quaternion Z / 四元素Z
 *	@param dRx: Euler angle RX / 欧拉角rx
 *	@param rRy: Euler angle RY / 欧拉角ry
 *	@param dRz: Euler angle RZ /欧拉角rz
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_Quaternion2RPY(unsigned int boxID, unsigned int rbtID, double dQuaW, double dQuaX, double dQuaY, double dQuaZ, double& dRx, double& dRy, double& dRz);

/**
 *	@brief: Euler angle to Quaternion / 欧拉角转四元素，欧拉角的单位是度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dRx: Euler angle RX / 欧拉角rx
 *	@param dRy: Euler angle RY / 欧拉角ry
 *	@param dRz: Euler angle RZ / 欧拉角rz
 *	@param dQuaW : Quaternion W / 四元素W
 *	@param dQuaX : Quaternion X / 四元素X
 *	@param dQuaY : Quaternion Y / 四元素Y
 *	@param dQuaZ : Quaternion Z / 四元素Z
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_RPY2Quaternion(unsigned int boxID, unsigned int rbtID, double dRx, double dRy, double dRz, double& dQuaW, double& dQuaX, double& dQuaY, double& dQuaZ);
/**
 *	@brief: Inverse kinematics transformation from pose to joint positions / 逆解，由指定用户坐标系位置和工具坐标系下的迪卡尔坐标计算对应的关节坐标位置
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dCoord_X~Rz : pose in specified UCS and TCP / 指定用户坐标系和工具坐标系下的迪卡尔坐标位置(需要逆解的迪卡尔坐标位置)
 *	@param dTcp_X~Rz : TCP pose / dCoord所在的工具坐标
 *	@param dUcs_X~Rz : UCS pose / dCoord所在的用户坐标
 *	@param dJ1~6 : Reference joint positions used for the selection in multiple solutions / 参考关节坐标，对于多个解用于选取接近的关节坐标位置
 *	@param dTargetJ1~6 : Inverse solution of J1~J6 / 逆解
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GetInverseKin(unsigned int boxID, unsigned int rbtID, double dCoord_X, double dCoord_Y, double dCoord_Z, double dCoord_Rx, double dCoord_Ry, double dCoord_Rz,
                 double dTcp_X, double dTcp_Y, double dTcp_Z, double dTcp_Rx, double dTcp_Ry, double dTcp_Rz,
                 double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz,
                 double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,
                 double &dTargetJ1, double &dTargetJ2, double &dTargetJ3, double &dTargetJ4, double &dTargetJ5, double &dTargetJ6);

/**
 *	@brief: Forward kinematics transformation from joint positions to pose / 正解，由关节坐标位置计算指定用户坐标系和工具坐标系下的迪卡尔坐标位置
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1~6 : joint positions / 需要计算的关节坐标位置
 *	@param dTcp_X~Rz : TCP pose for the target / Target所对应的工具坐标
 *	@param dUcs_X~Rz : UCS pose for the target / Target所对应的用户坐标
 *	@param dTarget_X~Rz : pose in specified UCS and TCP / 指定用户坐标系和工具坐标系下的迪卡尔坐标位置
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */				 
HANSROBOT_PRO_API int HRIF_GetForwardKin(unsigned int boxID, unsigned int rbtID, double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,
                 double dTcp_X, double dTcp_Y, double dTcp_Z, double dTcp_Rx, double dTcp_Ry, double dTcp_Rz,
                 double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz,
                 double &dTarget_X, double &dTarget_Y, double &dTarget_Z, double &dTarget_Rx, double &dTarget_Ry, double &dTarget_Rz);

/**
 *	@brief: Pose transformation from Base to UCS&TCP / 由基座坐标系下的坐标位置计算指定用户坐标系和工具坐标系下的迪卡尔坐标位置
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dCoord_X~Rz : pose in Base coordinate system / 基座坐标系下的迪卡尔坐标位置
 *	@param dTcp_X~Rz : TCP pose for the target / dTarget所对应的工具坐标
 *	@param dUcs_X~Rz : UCS pose for the target / dTarget所对应的用户坐标
 *	@param dTarget_X~Rz : pose in the specified UCS and TCP / 指定用户坐标系和工具坐标系下的迪卡尔坐标
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */				
HANSROBOT_PRO_API int HRIF_Base2UcsTcp(unsigned int boxID, unsigned int rbtID, double dCoord_X, double dCoord_Y, double dCoord_Z, double dCoord_Rx, double dCoord_Ry, double dCoord_Rz,
                     double dTcp_X, double dTcp_Y, double dTcp_Z, double dTcp_Rx, double dTcp_Ry, double dTcp_Rz,
                     double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz,
                     double &dTarget_X, double &dTarget_Y, double &dTarget_Z, double &dTarget_Rx, double &dTarget_Ry, double &dTarget_Rz);
					 
/**
 *	@brief: Pose transformation from UCS&TCP to Base / 由指定用户坐标系和工具坐标系下的迪卡尔坐标位置计算基座坐标系下的坐标位置
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dCoord_X~Rz : pose in speicified UCS and TCP / 指定用户坐标系和工具坐标系下的迪卡尔坐标
 *	@param dTcp_X~Rz : TCP pose for dCoord / dCoord所对应的工具坐标
 *	@param dUcs_X~Rz : UCS pose for dCoord / dCoord所对应的用户坐标
 *	@param dTarget_X~Rz : pose in Base coordinate system / 基座坐标系下的迪卡尔坐标位置
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */		
HANSROBOT_PRO_API int HRIF_UcsTcp2Base(unsigned int boxID, unsigned int rbtID, double dCoord_X, double dCoord_Y, double dCoord_Z, double dCoord_Rx, double dCoord_Ry, double dCoord_Rz,
                     double dTcp_X, double dTcp_Y, double dTcp_Z, double dTcp_Rx, double dTcp_Ry, double dTcp_Rz,
                     double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz,
                     double &dTarget_X, double &dTarget_Y, double &dTarget_Z, double &dTarget_Rx, double &dTarget_Ry, double &dTarget_Rz);

/**
 *	@brief: Pose addition calculation / 点位加法计算
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPose1_X~Rz : Pose of point 1 / 坐标1
 *	@param dPose2_X~Rz : Pose of point 2 / 坐标2
 *	@param dPose3_X~Rz : Calculation result / 计算结果
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_PoseAdd(unsigned int boxID, unsigned int rbtID, double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,
                    double &dPose3_X,double &dPose3_Y,double &dPose3_Z,double &dPose3_Rx,double &dPose3_Ry,double &dPose3_Rz);

/**
 *	@brief: Pose subtraction calculation / 点位减法计算
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPose1_X~Rz : Pose of point 1 / 坐标1
 *	@param dPose2_X~Rz : Pose of point 2 / 坐标2
 *	@param dPose3_X~Rz : Calculation result / 计算结果
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	                  
HANSROBOT_PRO_API int HRIF_PoseSub(unsigned int boxID, unsigned int rbtID, double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,
                    double &dPose3_X,double &dPose3_Y,double &dPose3_Z,double &dPose3_Rx,double &dPose3_Ry,double &dPose3_Rz);

/**
 *	@brief: Pose transformation through combined caculations, from P1 based on Base coordinate system to P3 based on UCS with P2 / 坐标变换,组合运算P3=HRIF_PoseTrans(P1,HRIF_PoseInverse(P2))，得到的就是基坐标系下的P1,在用户坐标系P2下的位置P3
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPose1_X~Rz : Pose of P1 / 坐标1
 *	@param dPose2_X~Rz : Pose of P2 / 坐标2
 *	@param dPose3_X~Rz : Calculation result, P3 / 计算结果
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	                       
HANSROBOT_PRO_API int HRIF_PoseTrans(unsigned int boxID, unsigned int rbtID, double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,
                    double &dPose3_X,double &dPose3_Y,double &dPose3_Z,double &dPose3_Rx,double &dPose3_Ry,double &dPose3_Rz);

/**
 *	@brief: Inverse kinematics transformation for pose / 坐标运动学逆变换
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPoseFrom_X~Rz : Pose of oringinal point / 源点空间坐标
 *	@param dPoseTo_X~Rz : Pose of calculation result / 逆运算后的空间坐标
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	                       
HANSROBOT_PRO_API int HRIF_PoseInverse(unsigned int boxID, unsigned int rbtID, double dPoseFrom_X, double dPoseFrom_Y, double dPoseFrom_Z, double dPoseFrom_Rx, double dPoseFrom_Ry, double dPoseFrom_Rz,
                    double &dPoseTo_X,double &dPoseTo_Y,double &dPoseTo_Z,double &dPoseTo_Rx,double &dPoseTo_Ry,double &dPoseTo_Rz);

/**
 *	@brief:  Calculate points distance / 计算点位距离
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPose1_X~Rz : Pose of point 1 / 坐标1
 *	@param dPose2_X~Rz : Pose of point 2 / 坐标2
 *	@param dDistance : Points distance (mm) / 点位距离，单位(mm)
 *	@param dAngle : Angle between poses / 姿态距离，单位(°)
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	                       
HANSROBOT_PRO_API int HRIF_PoseDist(unsigned int boxID, unsigned int rbtID, double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,
                    double &dDistance,double &dAngle);


/**
 *	@brief: Linear interpolation calculation for pose / 空间位置直线插补计算
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPose1_X~Rz : Pose of point 1 / 坐标1
 *	@param dPose2_X~Rz : Pose of point 2 / 坐标2
 *	@param dAlpha : Interpolation scale / 插补比例
 *	@param dPose3_X~Rz : Calculation result, pose of point 3 / 计算结果
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	                      
HANSROBOT_PRO_API int HRIF_PoseInterpolate(unsigned int boxID, unsigned int rbtID, double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,double dAlpha,
                    double &dPose3_X,double &dPose3_Y,double &dPose3_Z,double &dPose3_Rx,double &dPose3_Ry,double &dPose3_Rz);

/***
 *	@brief: Calculate the rotation center. P1,P2,P3 are the points before rotation and P4,P5,P6 are the points after rotation / 轨迹旋转中心计算，p1,p2,p3为旋转前轨迹的特征点，p4,p5,p6为旋转后的轨迹的特征点
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPose1_X~Z : Pose of point 1 / 坐标1
 *	@param dPose2_X~Z : Pose of point 2 / 坐标2
 *	@param dPose3_X~Z : Pose of point 3 / 坐标3
 *	@param dPose4_X~Z : Pose of point 4 / 坐标4
 *	@param dPose5_X~Z : Pose of point 5 / 坐标5
 *	@param dPose6_X~Z : Pose of point 6 / 坐标6
 *	@param dUcs_X~Rz : Calculation result, UCS / 计算结果
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	  
HANSROBOT_PRO_API int HRIF_PoseDefdFrame(unsigned int boxID, unsigned int rbtID, double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose2_X, double dPose2_Y, double dPose2_Z,
                    double dPose3_X,double dPose3_Y,double dPose3_Z,double dPose4_X,double dPose4_Y,double dPose4_Z,
                    double dPose5_X,double dPose5_Y,double dPose5_Z,double dPose6_X,double dPose6_Y,double dPose6_Z,
                    double &dUcs_X,double &dUcs_Y,double &dUcs_Z,double &dUcs_Rx,double &dUcs_Ry,double &dUcs_Rz);

/***
 *	@brief: Calculate the two passing through intermediate points in arc using three-point teaching / 三点示教圆弧计算两个中间经过点的接口
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPose1_X~Rz : Pose of start point / 起始点坐标
 *	@param dPose2_X~Rz : Pose of aux point / 中间点坐标
 *	@param dPose3_X~Rz : Pose of end point / 结束点坐标
 *  @param dRetPose1_X~Rz  : Pose of point between start point and aux point / 起始点和中间点之间的圆弧中间经过点
 *	@param dRetPose2_X~Rz  : Pose of point between start point and aux point / 中间点和结束点之间的圆弧中间经过点
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	  
HANSROBOT_PRO_API int HRIF_GetCircularViaPose(unsigned int boxID, unsigned int rbtID, 
                    double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,
                    double dPose3_X,double dPose3_Y,double dPose3_Z,double dPose3_Rx,double dPose3_Ry,double dPose3_Rz,
                    double& dRetPose1_X,double& dRetPose1_Y,double& dRetPose1_Z,double& dRetPose1_Rx,double& dRetPose1_Ry,double& dRetPose1_Rz,
                    double& dRetPose2_X,double& dRetPose2_Y,double& dRetPose2_Z,double& dRetPose2_Rx,double& dRetPose2_Ry,double& dRetPose2_Rz);

/***
 *	@brief: Calculate the two passing through intermediate points in arc using three-point teaching / 三点示教圆弧计算两个中间经过点的接口
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPose1_X~Rz : Pose of start point / 起始点坐标
 *	@param dPose2_X~Rz : Pose of aux point / 中间点坐标
 *	@param dPose3_X~Rz : Pose of end point / 结束点坐标
 *  @param dRetPose1_X~Rz  : Pose of point between start point and aux point / 起始点和中间点之间的圆弧中间经过点
 *	@param dRetPose2_X~Rz  : Pose of point between start point and aux point / 中间点和结束点之间的圆弧中间经过点
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	  
HANSROBOT_PRO_API int HRIF_GetCircularViaPose(unsigned int boxID, unsigned int rbtID, 
                    double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,
                    double dPose3_X,double dPose3_Y,double dPose3_Z,double dPose3_Rx,double dPose3_Ry,double dPose3_Rz,
                    double& dRetPose1_X,double& dRetPose1_Y,double& dRetPose1_Z,double& dRetPose1_Rx,double& dRetPose1_Ry,double& dRetPose1_Rz,
                    double& dRetPose2_X,double& dRetPose2_Y,double& dRetPose2_Z,double& dRetPose2_Rx,double& dRetPose2_Ry,double& dRetPose2_Rz);

/***
 *	@brief: Calculate UCS through 3-point plane / 通过三点平面法计算UCS
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPose1_X~Z : Position of point 1 in Base and specified TCP / 点1在Base坐标系下指定TCP的位置
 *	@param dPose2_X~Z : Position of point 2 in Base and specified TCP / 点2在Base坐标系下指定TCP的位置
 *	@param dPose3_X~Z : Position of point 3 in Base and specified TCP / 点3在Base坐标系下指定TCP的位置
 *  @param dRetPose_X~Rz  : Pose of UCS / 计算得出的UCS位姿
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	  
HANSROBOT_PRO_API int HRIF_CalUcsPlane(unsigned int boxID, unsigned int rbtID, 
                    double dPose1_X, double dPose1_Y, double dPose1_Z,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,
                    double dPose3_X,double dPose3_Y,double dPose3_Z,
                    double& dRetPose_X,double& dRetPose_Y,double& dRetPose_Z,double& dRetPose_Rx,double& dRetPose_Ry,double& dRetPose_Rz);

/***
 *	@brief: Calculate UCS through 2-point linee / 通过两点直线法计算UCS
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPose1_X~Rz : Pose of point 1 in Base and specified TCP / 点1在Base坐标系下指定TCP的位姿
 *	@param dPose2_X~Rz : Pose of point 2 in Base and specified TCP / 点2在Base坐标系下指定TCP的位姿
 *  @param dRetPose_X~Rz  : Pose of UCS / 计算得出的UCS
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	  
HANSROBOT_PRO_API int HRIF_CalUcsLine(unsigned int boxID, unsigned int rbtID, 
                    double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,
                    double& dRetPose_X,double& dRetPose_Y,double& dRetPose_Z,double& dRetPose_Rx,double& dRetPose_Ry,double& dRetPose_Rz);

/***
 *	@brief: Calculate TCP through 3-point / 通过三点法计算TCP
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPose1_X~Rx : Pose of point 1 in Base and system default TCP / 点1在Base坐标系下系统默认TCP的位姿
 *	@param dPose2_X~Rz : Pose of point 2 in Base and system default TCP / 点2在Base坐标系下系统默认TCP的位姿
 *	@param dPose3_X~Rz : Pose of point 3 in Base and system default TCP / 点3在Base坐标系下系统默认TCP的位姿
 *  @param dRetPose_X~Z  : Position of TCP / 计算得出的TCP的位置
 *  @param dRetPose_Rx~Rz  : Orientation of TCP, usually 0 / 计算得出的TCP的姿态, 结果一般为0
 *  @param quality : quality of the result, 0 for good, 1 for poor, 2 for abnormal / 计算结果的质量，0：良好；1：差（最好不用）；2：异常
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	  
HANSROBOT_PRO_API int HRIF_CalTcp3P(unsigned int boxID, unsigned int rbtID, 
                    double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,
                    double dPose3_X,double dPose3_Y,double dPose3_Z,double dPose3_Rx,double dPose3_Ry,double dPose3_Rz,
                    double& dRetPose_X,double& dRetPose_Y,double& dRetPose_Z,double& dRetPose_Rx,double& dRetPose_Ry,double& dRetPose_Rz,
                    int& quality);

 /***
 *	@brief: Calculate TCP through 4-point / 通过四点法计算TCP
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPose1_X~Rx : Pose of point 1 in Base and system default TCP / 点1在Base坐标系下系统默认TCP的位姿
 *	@param dPose2_X~Rz : Pose of point 2 in Base and system default TCP / 点2在Base坐标系下系统默认TCP的位姿
 *	@param dPose3_X~Rz : Pose of point 3 in Base and system default TCP / 点3在Base坐标系下系统默认TCP的位姿
 *	@param dPose4_X~Rz : Pose of point 4 in Base and system default TCP / 点4在Base坐标系下系统默认TCP的位姿
 *  @param dRetPose_X~Z  : Position of TCP / 计算得出的TCP的位置
 *  @param dRetPose_Rx~Rz  : Orientation of TCP, usually 0 / 计算得出的TCP的姿态, 结果一般为0
 *  @param quality : quality of the result, 0 for good, 1 for poor, 2 for abnormal / 计算结果的质量，0：良好；1：差（最好不用）；2：异常
 *  @param erroIndex_P1~4 : error index for the 4 source points, 0 for abnormal, 1 for normal / 各个源点的错误指示，0：异常；1：正常
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	  
HANSROBOT_PRO_API int HRIF_CalTcp4P(unsigned int boxID, unsigned int rbtID, 
                    double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,
                    double dPose3_X,double dPose3_Y,double dPose3_Z,double dPose3_Rx,double dPose3_Ry,double dPose3_Rz,
                    double dPose4_X,double dPose4_Y,double dPose4_Z,double dPose4_Rx,double dPose4_Ry,double dPose4_Rz,
                    double& dRetPose_X,double& dRetPose_Y,double& dRetPose_Z,double& dRetPose_Rx,double& dRetPose_Ry,double& dRetPose_Rz,
                    int& quality, int& erroIndex_P1,int& erroIndex_P2,int& erroIndex_P3,int& erroIndex_P4);

//////////////////////////////////////////////////////////////////////////////////////
//************************************************************************/
//**    Interfaces for TCP and UCS / TCP和UCS设置
//************************************************************************/
//////////////////////////////////////////////////////////////////////////////////////
/**
 *	@brief:Set current TCP / 设置当前工具坐标
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dTcp_X~Rz : Pose of TCP / 需设置的工具坐标值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetTCP(unsigned int boxID, unsigned int rbtID, double dTcp_X, double dTcp_Y, double dTcp_Z, double dTcp_Rx, double dTcp_Ry, double dTcp_Rz);

/**
 *	@brief: Set current UCS / 设置当前用户坐标
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dUcs_X~Rz : Pose of UCS / 需设置的用户坐标值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetUCS(unsigned int boxID, unsigned int rbtID, double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz);

/**
 *	@brief: Read current TCP / 读取当前工具坐标
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dTcp_X~Rz : Pose of TCP / 读取的工具坐标值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCurTCP(unsigned int boxID, unsigned int rbtID, double& dTcp_X, double& dTcp_Y, double& dTcp_Z, double& dTcp_Rx, double& dTcp_Ry, double& dTcp_Rz);

/**
 *	@brief: Read current UCS / 读取当前用户坐标
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID :  Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dUcs_X~Rz : Pose of UCS / 读取的用户坐标值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCurUCS(unsigned int boxID, unsigned int rbtID, double& dUcs_X, double& dUcs_Y, double& dUcs_Z, double& dUcs_Rx, double& dUcs_Ry, double& dUcs_Rz);

/**
 *	@brief: Set current TCP By Name / 通过名称设置TCP列表中的值为当前TCP坐标
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sTcpName : TCP name / TCP坐标名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetTCPByName(unsigned int boxID, unsigned int rbtID, string sTcpName);

/**
 *	@brief: Set current UCS By Name / 通过名称设置用户坐标列表中的值为当前用户坐标
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sUcsName : UCS name / UCS坐标名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetUCSByName(unsigned int boxID, unsigned int rbtID, string sUcsName);

/**
 *	@brief: Read TCP By Name / 通过名称读取指定TCP坐标
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sName : TCP name / TCP名称，和示教器页面的TCP对应
 *	@param dTcp_X~Rz : Pose of TCP / 读取到的对应TCP值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_ReadTCPByName(unsigned int boxID, unsigned int rbtID,
                        string sName, 
                        double& dTcp_X, 
                        double& dTcp_Y, 
                        double& dTcp_Z, 
                        double& dTcp_Rx, 
                        double& dTcp_Ry, 
                        double& dTcp_Rz);

/**
 *	@brief: Read UCS By Name / 通过名称读取指定UCS坐标
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sName : UCS name / UCS名称，和示教器页面的UCS对应
 *	@param dUcs_X~Rz: Pose of UCS / 读取到的对应UCS值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_ReadUCSByName(unsigned int boxID, unsigned int rbtID,
                        string sName, 
                        double& dUcs_X, 
                        double& dUcs_Y, 
                        double& dUcs_Z, 
                        double& dUcs_Rx, 
                        double& dUcs_Ry, 
                        double& dUcs_Rz);

/**
 *	@brief: Configure TCP by Name / 配置已示教的指定名称的TCP坐标值
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sTcpName : TCP name / 已示教的指定的TCP名称
 *	@param dX~Rz : Pose of TCP / 需要修改的TCP值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
 //HANSROBOT_PRO_API int HRIF_ConfigTCP(unsigned int boxID, string sTcpName,double dX, double dY, double dZ, double dRx, double dRy, double dRz);


 
//////////////////////////////////////////////////////////////////////////////////////
//************************************************************************/
//**    Interfaces for force control / 力控类函数接口
//************************************************************************/
//////////////////////////////////////////////////////////////////////////////////////
HANSROBOT_PRO_API int HRIF_GetForceParams(unsigned int boxID, unsigned int rbtID, double* dInertia, double* dDamping, double* dStiffness);

/**
 *	@brief: Set force parameters / 设置力控参数
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dInertia : Mass / 质量参数(size=6)
 *	@param dDamping : Damp / 阻尼参数(size=6)
 *	@param dStiffness : Stiffness / 刚度参数(size=6)
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceParams(unsigned int boxID, unsigned int rbtID, double* dInertia, double* dDamping, double* dStiffness);

/**
 *	@brief: Set force control parameters / 设置主动探寻参数
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dForceLimit : Force limit / 探寻最大力限制(size=6)
 *	@param dDistLimit : Distance limit / 探寻最大距离限制(size=6)
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceControlParams(unsigned int boxID, unsigned int rbtID, double* dForceLimit, double* dDistLimit);

/**
 *	@brief: Set force control switch / 设置开启或者关闭力控模式
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dEnbaleFlag : true for open, false for closed / 开启(TRUE)/关闭(FALSE)
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceMode(unsigned int boxID, unsigned int rbtID, bool dEnbaleFlag);

/**
 *	@brief: Set force control status / 设置力控状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nState : 0 for closed, 1 for open / 0:关闭力控运动/1:开启力控运动
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceControlState(unsigned int boxID, unsigned int rbtID, int nState);
/**
 *	@brief: Read force control status /读取力控状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nState : 0 for closed, 1 for seeking, 2 for seeking completed, 3 for free drive / 力控状态 0:关闭状态,1:力控探寻状态,2:力控探寻完成状态,3:力控自由驱动状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadForceControlState(unsigned int boxID, unsigned int rbtID, int& nState);
/**
 *	@brief: Set force sensor direction align with TCP / 设置力传感器方向为tool坐标方向
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nState : 0 for not align with TCP, 1 for align with TCP / 0(力传感器方向不为tool坐标方向)/1(力传感器方向为tool坐标方向)
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceToolCoordinateMotion(unsigned int boxID, unsigned int rbtID, int nMode);
/**
 *	@brief: Disable force control / 暂停力控运动，仅暂停力控功能，不暂停运动和脚本
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ForceControlInterrupt(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief: Enable force control / 继续力控运动，仅继续力控运动功能，不继续运动和脚本
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ForceControlContinue(unsigned int boxID, unsigned int rbtID);
/**
 *	@brief: Reset force sensor to zero / 清零力传感器
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceZero(unsigned int boxID, unsigned int rbtID);
/**
 *	@brief: Set max velocity of force seeking / 设置力控探寻的最大速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dMaxLinearVelocity : Max linear velocity / 直线探寻最大速度
 *	@param dMaxAngularVelocity : Max angular velocity / 姿态探寻最大速度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetMaxSearchVelocities(unsigned int boxID, unsigned int rbtID, double dMaxLinearVelocity, double dMaxAngularVelocity);

/**
 *	@brief: Set control freedom / 设置力控探寻自由度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nX~Rz : 0 for closed / 1 for open / 各方向力控自由度状态 0：关闭  1：开启
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetControlFreedom(unsigned int boxID, unsigned int rbtID, int nX, int nY, int nZ, int nRx, int nRy, int nRz);

/**
 *	@brief: Set force control strategy / 设置控制策略
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nState : 0 for Compliant force and 1 for Constant force / 控制策略 0：柔顺模式 1：恒力模式
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceControlStrategy(unsigned int boxID, unsigned int rbtID, int nState);
/**
 *	@brief: Set force sensor pose / 设置力传感器中心相对于法兰盘的安装位置和姿态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Rz : Pose / 力传感器中心相对于法兰盘的安装位置和姿态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetFreeDrivePositionAndOrientation(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz);
/**
 *	@brief: Set PID for force seeking / 设置力控探寻PID参数
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dFp~Td : PID / PID参数
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetPIDControlParams(unsigned int boxID, unsigned int rbtID, double dFp, double dFi, double dFd, double dTp, double dTi, double dTd);
/**
 *	@brief: Set mass parameters / 设置惯量控制参数
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Rz : Mass parameters / 惯量控制参数
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetMassParams(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz);
/**
 *	@brief: Set damp parameters / 设置阻尼(b)控制参数
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Rz : Damp parameters / 阻尼(b)控制参数
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetDampParams(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz);
/**
 *	@brief: Set stiffness parameters / 设置刚度(k)控制参数
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Rz : Stiffness parameters / 刚度(k)控制参数
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetStiffParams(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz);
/**
 *	@brief: Set force control goal / 设置力控控制目标值
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Rz: Goal values / 对应目标力
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceControlGoal(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRX, double dRY, double dRZ);
/**
 *	@brief: Set control seeking goal / 设置力控探寻目标力大小
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dWrench_X~Rz : Seeking force / 各方向探寻目标力
 *	@param dDistance_X~Rz : Seeking distance / 各方向探寻距离(暂未启用，可全部设置为0)
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetControlGoal(unsigned int boxID, unsigned int rbtID, double dWrench_X, double dWrench_Y, double dWrench_Z, double dWrench_Rx, double dWrench_Ry, double dWrench_Rz,
                        double dDistance_X,double dDistance_Y, double dDistance_Z,double dDistance_Rx,double dDistance_Ry, double dDistance_Rz);

/**
 *	@brief: Set force data limit / 设置力控限制范围-力传感器超过此范围后控制器断电
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dMax_X~Rz) : Max limit / 各方向传感器限制最大值
 *	@param dMin_X~Rz) : Min limit / 各方向传感器限制最小值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetForceDataLimit(unsigned int boxID, unsigned int rbtID, double dMax_X, double dMax_Y, double dMax_Z, double dMax_Rx, double dMax_Ry, double dMax_Rz,
                            double dMin_X,double dMin_Y, double dMin_Z,double dMin_Rx,double dMin_Ry, double dMin_Rz);
/**
 *	@brief: Set force distance limit / 设置力控形变范围
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dAllowDistance : Allowed seeking distance / 允许的探寻距离
 *	@param dStrengthLevel : Power term for the deviation from boundary. 2 for square, 3 for cubic / 位置与边界设置偏离距离的幂次项，设成2，就表示阻力与偏离边界的平方项成比例；设成3，就表示阻力与偏离边界的立方项成比例
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceDistanceLimit(unsigned int boxID, unsigned int rbtID, double dAllowDistance, double dStrengthLevel);

/**
 *	@brief: Set force free drive mode / 设置开启或者关闭力控自由驱动模式
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nEnableFlag : 0 for closed, 1 for open /  0(关闭)/1(开启)
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceFreeDriveMode(unsigned int boxID, unsigned int rbtID, int nEnableFlag);

/**
 *	@brief:Read force control calibration data / 读取末端力传感器值(标定后)
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Rz : Calibration data /需读取到的对应力传感器值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadFTCabData(unsigned int boxID, unsigned int rbtID, double& dX, double& dY, double& dZ, double& dRX, double& dRY, double& dRZ);

/**
 *	@brief: Set the end freedom in free drive / 设置力控自由驱动的末端自由度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nX-nRz : The available freedom parameters in free drive / 设置力控拖动下可开放的自由度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetFreeDriveMotionFreedom(unsigned int boxID, unsigned int rbtID, int nX, int nY, int nZ, int nRx, int nRy, int nRz);

/**
 *	@brief: 设置力控自由驱动平移柔顺度和旋转柔顺度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param Linear : 平移柔顺度
 *	@param Angular : 旋转柔顺度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetFTFreeFactor(unsigned int boxID, unsigned int rbtID, double Linear, double Angular);

/**
 *	@brief: 设置x/y方向切向力最大值、最小值和上抬最大速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dMax : 切向力最大值，单位：N
 *	@param dMin : 切向力最小值，单位：N
 *  @param dVel : 越障上抬最大速度，单位：mm/s
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetTangentForceBounds(unsigned int boxID, unsigned int rbtID, double dMax, double dMin, double dVel);

/**
 *	@brief: 设置FreeDrive模式下的定向补偿力大小和矢量方向[x,y,z]
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dForce : 补偿力大小，单位：N
 *	@param dX-dZ : 补偿力在基坐标系下的矢量方向
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetFreeDriveCompensateForce(unsigned int boxID, unsigned int rbtID, double dForce, double dX, double dY, double dZ);


//HANSROBOT_PRO_API bool HRIF_ReadTCPInCorrectValue(unsigned int boxID, unsigned int rbtID);

//************************************************************************/
//**    Interfaces for moving / 运动类函数接口                                                             
//************************************************************************/
/**
 *	@brief: Start joint short jog / 关节短点动 运动距离2°，最大速度<10°/s
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nAxisId : Axis ID, 0~5 / 需要运动的关节索引0-5
 *	@param nDirection : Moving direction, 0 for negative and 1 for positive / 需要运动的关节运动方向：0(负方向)/1(正方向)
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ShortJogJ(unsigned int boxID, unsigned int rbtID, int nAxisId, int nDirection);

/**
 *	@brief: Cartesian short jog / 迪卡尔坐标短点动 运动距离2mm，最大速度<10mm/s
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nAxisId : Axis ID, 0~5 / 需要运动的关节索引0-5
 *	@param nDirection :  Moving direction, 0 for negative and 1 for positive / 需要运动的关节运动方向：0(负方向)/1(正方向)
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ShortJogL(unsigned int boxID, unsigned int rbtID, int nAxisId, int nDirection);

/**
 *	@brief: Start joint long jog / 关节长点动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nAxisId : Axis ID, 0~5 / 指定运动的关节索引0-5
 *	@param nDirection : Moving direction, 0 for negative and 1 for positive / 指定运动的关节方向：0(负方向)/1(正方向)
 *	@param nState : 0 for closed and 1 for open / 0(关闭)/1(开启)
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_LongJogJ(unsigned int boxID, unsigned int rbtID, int nAxisId, int nDirection, int nState);

/**
 *	@brief: Start cartesian long jog / 迪卡尔坐标长点动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nAxisId : Axis ID, 0~5 /指定运动的迪卡尔坐标索引0-5
 *	@param nDirection : Moving direction, 0 for negative and 1 for positive / 指定运动的迪卡尔坐标方向：0(负方向)/1(正方向)
 *	@param nState : 0 for closed and 1 for open / 0(关闭)/1(开启)
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_LongJogL(unsigned int boxID, unsigned int rbtID, int nAxisId, int nDirection, int nState);

/**
 *	@brief: Continue long jog. Successively call this function after HRIF_ShortJogL or HRIF_LongJogJ with less than 500 ms interval to keep continuous moving. / 长点动继续指令，当开始长点动之后，要按 500 毫秒或更短时间为时间周期发送一次该指令，否则长点动会停止
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_LongMoveEvent(unsigned int boxID, unsigned int rbtID);

//////////////////////////////////////////////////////////////////////////////////////

/**
 *	@brief: Check if the motion is done / 判断运动是否停止
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param bDone : true for done, false for not done / true: 完成；false: 未完成
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码，此时bDone的取值无意义
 */
HANSROBOT_PRO_API int HRIF_IsMotionDone(unsigned int boxID, unsigned int rbtID, bool& bDone);

/**
 *	@brief: Check if the waypoint blending is done / 判断路点是否运行完成
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param bDone : true for done, false for not done / true: 完成；false: 未完成；
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码，此时bDone的取值无意义
 */
HANSROBOT_PRO_API int HRIF_IsBlendingDone(unsigned int boxID, unsigned int rbtID, bool& bDone);

/**
 *	@brief: Start waypoint move. / 执行路点运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nMoveType : Move type, 0 for MoveJ, 1 for MoveL / 运动类型，0：MoveJ；1：MoveL
 *	@param dX~Rz : Target pose, invalid when IsUseJoint equals 1. Target joint positions is obtained through inversely solving from this pose when IsUseJoint equals 0 / 目标迪卡尔位置-当nIsUseJoint=1时无效，nIsUseJoint=0时，用此迪卡尔坐标作为目标位置，通过逆解计算得到关节坐标为目标关节坐标
 *	@param dJ1~6 : Target joint positions when IsUseJoint equals 1; Reference joint positions for the inversely solving when IsUseJoint equals 0. / 目标关节位置-当nIsUseJoint=1时，使用此关节坐标作为目标关节坐标，nIsUseJoint=0时，此关节坐标仅作为计算逆解时选解的参考关节坐标
 *	@param dTcp_X~Rz : Target TCP, invalid when IsUseJoint equals 1 / 目标迪卡尔坐标所处的工具坐标系，当nIsUseJoint=1时无效，可设置为0
 *	@param dUcs_X~Rz : Target UCS, invalid when IsUseJoint equals 1 / 目标迪卡尔坐标所处的用户坐标系，当nIsUseJoint=1时无效，可设置为0：
 *	@param dVelocity : Motion speed, mm/s or °/s / 运动速度，速度单位是毫米每秒，度每秒
 *	@param dAcc : Motion acceleration, mm/(s^2) or °/(s^2) / 运动加速度，毫米每秒平方，度每秒平方
 *	@param dRadius : Blending radius, mm / 过渡半径，单位毫米
 *	@param nIsUseJoint : Using joint or not, usable when MoveType equals 0, 0 for not using joint, 1 for using joint / 是否使用关节角度，是否使用关节角度作为目标点，仅当nMoveType=0时本参数有效。0：不使用关节角度1：使用关节角度
 *	@param nIsSeek,nIOBit,nIOState：Seeking or not, 1 for seeking; Motion stops when DO-nIOBit equals nIoState / 探寻参数，当nIsSeek为1，则开启探寻，这时电箱的DO nIOBit位为nIOState时，就停止运动，否则运动到目标点再停止
 *	@param strCmdID：Command ID, waypoint ID also, customized or set to 1, 2, 3 in order / 当前路点ID，可以自定义，也可以按顺序设置为“1”，“2”，“3”
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_WayPointEx(unsigned int boxID, unsigned int rbtID, int nMoveType,
                    double dX, double dY, double dZ, double dRx, double dRy, double dRz,                        
                    double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,                      
                    double dTcp_X, double dTcp_Y, double dTcp_Z, double dTcp_Rx, double dTcp_Ry, double dTcp_Rz,
                    double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz,
                    double dVelocity, double dAcc, double dRadius, int nIsUseJoint, int nIsSeek, int nIOBit, int nIOState, string strCmdID);
/**
 *	@brief: Start waypoint move. / 执行路点运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nMoveType : Move type, 0 for MoveJ, 1 for MoveL / 运动类型，0：MoveJ；1：MoveL
 *	@param dX~Rz : Target pose, invalid when IsUseJoint equals 1. Target joint positions is obtained through inversely solving from this pose when IsUseJoint equals 0 / 目标迪卡尔位置-当nIsUseJoint=1时无效，nIsUseJoint=0时，用此迪卡尔坐标作为目标位置，通过逆解计算得到关节坐标为目标关节坐标
 *	@param dJ1~6 : Target joint positions when IsUseJoint equals 1; Reference joint positions for the inversely solving when IsUseJoint equals 0. / 目标关节位置-当nIsUseJoint=1时，使用此关节坐标作为目标关节坐标，nIsUseJoint=0时，此关节坐标仅作为计算逆解时选解的参考关节坐标
 *	@param sTcpName : Target TCP name, invalid when IsUseJoint equals 1, "TCP" is valid as default / 目标迪卡尔坐标所处的工具坐标系名称，与示教器页面的名称对应，当nIsUseJoint=1时无效，可使用默认名称”TCP”
 *	@param sUcsName : Target UCS name, invalid when IsUseJoint equals 1, "Base" is valid as default / 目标迪卡尔坐标所处的用户坐标系名称，与示教器页面的名称对应-当nIsUseJoint=1时无效，可使用默认名称”Base”
 *	@param dVelocity : Motion speed, mm/s or °/s / 运动速度，速度单位是毫米每秒，度每秒
 *	@param dAcc : Motion acceleration, mm/(s^2) or °/(s^2) / 运动加速度，毫米每秒平方，度每秒平方
 *	@param dRadius : Blending radius, mm / 过渡半径，单位毫米
 *	@param nIsUseJoint : Using joint or not, usable when MoveType equals 0, 0 for not using joint, 1 for using joint / 是否使用关节角度，是否使用关节角度作为目标点，仅当nMoveType=0时本参数有效。0：不使用关节角度1：使用关节角度
 *	@param nIsSeek,nIOBit,nIOState：Seeking or not, 1 for seeking; Motion stops when DO-nIOBit equals nIoState / 探寻参数，当nIsSeek为1，则开启探寻，这时电箱的DO nIOBit位为nIOState时，就停止运动，否则运动到目标点再停止
 *	@param strCmdID：Command ID, waypoint ID also, customized or set to 1, 2, 3 in order / 当前路点ID，可以自定义，也可以按顺序设置为“1”，“2”，“3”
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_WayPoint(unsigned int boxID, unsigned int rbtID, int nMoveType,
                double dX, double dY, double dZ, double dRx, double dRy, double dRz,                         
                double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,
                string sTcpName, string sUcsName, double dVelocity, double dAcc, double dRadius, 
                int nIsUseJoint,int nIsSeek, int nIOBit, int nIOState, string strCmdID);

/**
 *	@brief: Start waypoint move. / 执行路点运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nMoveType : Move type, 0 for MoveJ, 1 for MoveL, 2 for MoveC / 运动类型，0：MoveJ；1：MoveL；2：MoveC
 *	@param dEndPos_X~Rz : Target pose, invalid when IsUseJoint equals 1. Target pose is obtained through inverse kinematics from this pose when IsUseJoint equals 0 / 目标迪卡尔位置-当nIsUseJoint=1时无效，nIsUseJoint=0时，用此迪卡尔坐标作为目标位置，通过逆解计算得到关节坐标为目标关节坐标
*	@param dAuxPos_X~Rz : Target auxiliary pose invalid when IsUseJoint equals 1, used as the middle point when nMoveType equals 2. Target pose is obtained through inverse kinematics from this pose when IsUseJoint equals 0 / 目标迪卡尔位置-当nIsUseJoint=1时无效，nIsUseJoint=0时，用此迪卡尔坐标作为目标位置，通过逆解计算得到关节坐标为目标关节坐标                                
 *	@param dJ1~6 : Target joint positions when IsUseJoint equals 1; Reference joint positions for the inverse kinematics when IsUseJoint equals 0. / 目标关节位置-当nIsUseJoint=1时，使用此关节坐标作为目标关节坐标，nIsUseJoint=0时，此关节坐标仅作为计算逆解时选解的参考关节坐标
 *	@param sTcpName : Target TCP name, invalid when IsUseJoint equals 1, "TCP" is valid as default / 目标迪卡尔坐标所处的工具坐标系名称，与示教器页面的名称对应，当nIsUseJoint=1时无效，可使用默认名称”TCP”
 *	@param sUcsName : Target UCS name, invalid when IsUseJoint equals 1, "Base" is valid as default / 目标迪卡尔坐标所处的用户坐标系名称，与示教器页面的名称对应-当nIsUseJoint=1时无效，可使用默认名称”Base”
 *	@param dVelocity : Motion speed, mm/s or °/s / 运动速度，速度单位是毫米每秒，度每秒
 *	@param dAcc : Motion acceleration, mm/(s^2) or °/(s^2) / 运动加速度，毫米每秒平方，度每秒平方
 *	@param dRadius : Blending radius, mm / 过渡半径，单位毫米
 *	@param nIsUseJoint : Using joint or not, usable when MoveType equals 0, 0 for not using joint, 1 for using joint / 是否使用关节角度，是否使用关节角度作为目标点，仅当nMoveType=0时本参数有效。0：不使用关节角度1：使用关节角度
 *	@param nIsSeek,nIOBit,nIOState：Seeking or not, 1 for seeking; Motion stops when DO-nIOBit equals nIoState / 探寻参数，当nIsSeek为1，则开启探寻，这时电箱的DO nIOBit位为nIOState时，就停止运动，否则运动到目标点再停止
 *	@param strCmdID：Command ID, waypoint ID also, customized or set to 1, 2, 3 in order / 当前路点ID，可以自定义，也可以按顺序设置为“1”，“2”，“3”
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_WayPoint2(unsigned int boxID, unsigned int rbtID, int nMoveType,
                double dEndPos_X, double dEndPos_Y, double dEndPos_Z, double dEndPos_Rx, double dEndPos_Ry, double dEndPos_Rz,   
                double dAuxPos_X, double dAuxPos_Y, double dAuxPos_Z, double dAuxPos_Rx, double dAuxPos_Ry, double dAuxPos_Rz,                       
                double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,
                string sTcpName, string sUcsName, double dVelocity, double dAcc, double dRadius, 
                int nIsUseJoint,int nIsSeek, int nIOBit, int nIOState, string strCmdID);


/**
 *	@brief: Start joint move. HRIF_WayPoint is more recommended yet / 机器人运动到指定的角度坐标位置, 建议使用HRIF_WayPoint
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Rz : Target pose, invalid when IsUseJoint equals 1. Target joint positions is obtained through inversely solving from this pose when IsUseJoint equals 0 / 目标迪卡尔位置-当nIsUseJoint=1时无效，nIsUseJoint=0时，用此迪卡尔坐标作为目标位置，通过逆解计算得到关节坐标为目标关节坐标
 *	@param dJ1~6 : Target joint positions when IsUseJoint equals 1; Reference joint positions for the inversely solving when IsUseJoint equals 0. / 目标关节位置-当nIsUseJoint=1时，使用此关节坐标作为目标关节坐标，nIsUseJoint=0时，此关节坐标仅作为计算逆解时选解的参考关节坐标
 *	@param sTcpName : Target TCP name, invalid when IsUseJoint equals 1, "TCP" is valid as default / 目标迪卡尔坐标所处的工具坐标系名称，与示教器页面的名称对应，当nIsUseJoint=1时无效，可使用默认名称”TCP”
 *	@param sUcsName : Target UCS name, invalid when IsUseJoint equals 1, "Base" is valid as default / 目标迪卡尔坐标所处的用户坐标系名称，与示教器页面的名称对应-当nIsUseJoint=1时无效，可使用默认名称”Base”
 *	@param dVelocity : Motion speed, mm/s or °/s / 运动速度，速度单位是毫米每秒，度每秒
 *	@param dAcc : Motion acceleration, mm/(s^2) or °/(s^2) / 运动加速度，毫米每秒平方，度每秒平方
 *	@param dRadius : Blending radius, mm / 过渡半径，单位毫米
 *	@param nIsUseJoint : Using joint or not, usable when MoveType equals 0, 0 for not using joint, 1 for using joint / 是否使用关节角度，是否使用关节角度作为目标点，仅当nMoveType=0时本参数有效。0：不使用关节角度1：使用关节角度
 *	@param nIsSeek,nIOBit,nIOState：Seeking or not, 1 for seeking; Motion stops when DO-nIOBit equals nIoState / 探寻参数，当nIsSeek为1，则开启探寻，这时电箱的DO nIOBit位为nIOState时，就停止运动，否则运动到目标点再停止
 *	@param strCmdID：Command ID, waypoint ID also, customized or set to 1, 2, 3 in order / 当前路点ID，可以自定义，也可以按顺序设置为“1”，“2”，“3”
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_MoveJ(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz,
                double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,
                string sTcpName, string sUcsName, double dVelocity, double dAcc, double dRadius, 
                int nIsUseJoint,int nIsSeek, int nIOBit, int nIOState, string strCmdID);

/**
 *	@brief: Start linear move. HRIF_WayPoint is more recommended yet / 机器人直线运动到指定的空间坐标位置, 建议使用HRIF_WayPoint
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Rz : Target pose / 目标迪卡尔位置
 *	@param dJ1~6 : Reference joint positions near to the target pose / 关节位置-建议使用目标迪卡尔坐标附近的关节坐标值。
 *	@param sTcpName : TCP name / 目标迪卡尔坐标所处的工具坐标系名称
 *	@param sUcsName : UCS name / 目标迪卡尔坐标所处的用户坐标系名称
 *	@param dVelocity : Motion speed, mm/s or °/s / 运动速度，速度单位是毫米每秒，度每秒
 *	@param dAcc : Motion acceleration, mm/(s^2) or °/(s^2) / 运动加速度，毫米每秒平方，度每秒平方
 *	@param dRadius :Blending radius, mm / 过渡半径，单位毫米
 *	@param nIsSeek,nIOBit,nIOState：Seeking or not, 1 for seeking; Motion stops when DO-nIOBit equals nIoState / 探寻参数，当nIsSeek为1，则开启探寻，这时电箱的DO nIOBit位为nIOState时，就停止运动，否则运动到目标点再停止
 *	@param strCmdID：Command ID, waypoint ID also, customized or set to 1, 2, 3 in order / 当前路点ID，可以自定义，也可以按顺序设置为“1”，“2”，“3”
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_MoveL(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz,
                double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,
                string sTcpName, string sUcsName, double dVelocity, double dAcc, double dRadius,
                int nIsSeek, int nIOBit, int nIOState, string strCmdID);

/**
 *	@brief: Start round move / 开始圆弧轨迹运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dStartPos_X~Rz : Start pose / 圆弧开始位置
 *	@param dAuxPos_X~Rz : Via pose / 圆弧经过位置
 *	@param dEndPos_X~Rz : End pose / 圆弧结束位置
 *	@param nFixedPosure : 0 for unfixed orientation , 1 for fixed orientation  / 0:不使用固定姿态，1:使用固定姿态
 *	@param nMoveCType : 1 for arc, 0 for circle / 1:圆弧轨迹，0:整圆轨迹
 *	@param dRadLen : Invalid when nMoveCType equals 1; Circles when nMoveCType equals 0. / 当nMoveCType=1时参数无效，由三个点确定圆弧轨迹
 *			         当nMoveCType=0时，参数为整圆的圈数,通过三个点位确定圆弧路径，当使用整圆运动时表示整圆的圈数，小数部分无效。
 *	@param dVelocity : Motion speed, mm/s / 运动速度，单位是毫米每秒
 *	@param dAcc : Motion acceleration, mm/(s^2) / 运动加速度，毫米每秒平方
 *	@param dRadius : Blending radius, mm / 过渡半径，单位毫米
 *	@param sTcpName : TCP name / 刀具坐标名称
 *	@param sUcsName : UCS name / 用户坐标名称
 *	@param sCmdID : Command ID / 命令ID
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_MoveC(unsigned int boxID, unsigned int rbtID,
                double dStartPos_X,double dStartPos_Y,double dStartPos_Z,double dStartPos_Rx,double dStartPos_Ry,double dStartPos_Rz,
                double dAuxPos_X,double dAuxPos_Y,double dAuxPos_Z,double dAuxPos_Rx,double dAuxPos_Ry,double dAuxPos_Rz,
                double dEndPos_X,double dEndPos_Y,double dEndPos_Z,double dEndPos_Rx,double dEndPos_Ry,double dEndPos_Rz,
                int nFixedPosure,int nMoveCType,double dRadLen,double dVelocity, double dAcc, 
                double dRadius, string sTcpName, string sUcsName, string sCmdID);

/**
 *	@brief: Start round move / 开始运行超过整圆的轨迹运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dStartPos_X~Rz : Start pose / 开始位置
 *	@param dAux1Pos_X~Rz: Via pose 1 / 经过位置 1
 *	@param dAux2Pos_X~Rz : Via pose 2 / 经过位置 2
 *	@param dAngle : 整圆运行的角度(如 dAngle = 420°，则运行一整圈圆过 60°)
 *	@param sTcpName : TCP name / 刀具坐标名称
 *	@param sUcsName : UCS name / 用户坐标名称
 *	@param dVelocity : Motion speed, mm/s / 运动速度，单位是毫米每秒
 *	@param dAcc : Motion acceleration, mm/(s^2) / 运动加速度，毫米每秒平方
 *	@param dRadius : Blending radius, mm / 过渡半径，单位毫米
 *	@param nIsSeek,nIOBit,nIOState：Seeking or not, 1 for seeking; Motion stops when DO-nIOBit equals nIoState / 探寻参数，当nIsSeek为1，则开启探寻，这时电箱的DO nIOBit位为nIOState时，就停止运动，否则运动到目标点再停止
 *	@param sCmdID : Command ID / 命令ID
 *	@param vecLastPoint : The end pose of the trajectory / 结束点空间位置
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_MoveCAngle(unsigned int boxID, unsigned int rbtID,
                double dStartPos_X,double dStartPos_Y,double dStartPos_Z,double dStartPos_Rx,double dStartPos_Ry,double dStartPos_Rz,
                double dAux1Pos_X,double dAux1Pos_Y,double dAux1Pos_Z,double dAux1Pos_Rx,double dAux1Pos_Ry,double dAux1Pos_Rz,
                double dAux2Pos_X,double dAux2Pos_Y,double dAux2Pos_Z,double dAux2Pos_Rx,double dAux2Pos_Ry,double dAux2Pos_Rz,
                double dAngle, string sTcpName, string sUcsName, double dVelocity, double dAcc, 
                double dRadius, int nIsSeek, int nBit, int nDIType, string sCmdID, vector<string> &vecLastPoint);

/**
 *	@brief: Start zigzag move / 开始Z型轨迹运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dStartPos_X~Rz : Start pose / 开始位置
 *	@param dEndPos_X~Rz : End pose / 结束位置
 *	@param dPlanePos_X~Rz : Plane pose / 确定平面点位置
 *	@param dVelocity : Motion speed, mm/s / 运动速度，单位是毫米每秒
 *	@param dAcc : Motion acceleration, mm/(s^2) / 运动加速度，毫米每秒平方
 *	@param dWIdth : Width / 宽度
 *	@param dDensity : Density, mm / 密度，毫米
 *	@param nEnableDensity : 0 is disable density and 1 for enable density / 是否使用密度(0:不使用，1:使用)
 *	@param nEnablePlane : Use plane pose or not; UCS is automatically used if plane pose is not used / 是否使用平面点，不使用时根据选择的用户坐标确定XYZ平面
 *	@param nEnableWaiTime : 0 for disable waiting time, 1 for enable waiting time / 是否开启转折点等待时间(0:不使用，1:使用)
 *	@param nPosiTime : Waiting time (ms) for Positive turning point / 正向转折点等待时间ms
 *	@param nNegaTime : Waiting time (ms) for Negative turning point / 负向转折点等待时间ms
 *	@param dRadius : Blending radius, mm / 过渡半径，单位毫米
 *	@param sTcpName : TCP name / 刀具坐标名称
 *	@param sUcsName : UCS name / 用户坐标名称
 *	@param sCmdID : Command ID / 命令ID
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_MoveZ(unsigned int boxID, unsigned int rbtID,
                double dStartPos_X,double dStartPos_Y,double dStartPos_Z,double dStartPos_Rx,double dStartPos_Ry,double dStartPos_Rz,
                double dEndPos_X,double dEndPos_Y,double dEndPos_Z,double dEndPos_Rx,double dEndPos_Ry,double dEndPos_Rz,
                double dPlanePos_X,double dPlanePos_Y,double dPlanePos_Z,double dPlanePos_Rx,double dPlanePos_Ry,double dPlanePos_Rz,
                double dVelocity, double dAcc, double dWIdth, double dDensity, int nEnableDensity, int nEnablePlane, int nEnableWaiTime, 
                int nPosiTime, int nNegaTime, double dRadius, string sTcpName, string sUcsName, string sCmdID);

/**
 *	@brief: Start linear weave move / 开始直线摆焊运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dStartPos_X~Rz : Start pose / 开始位置
 *	@param dEndPos_X~Rz : End pose / 结束位置
 *	@param dVelocity : Motion speed, mm/s / 运动速度，单位是毫米每秒
 *	@param dAcc : Motion acceleration, mm/(s^2) / 运动加速度，毫米每秒平方
 *	@param dRadius : Blending radius, mm / 过渡半径，单位毫米
 *	@param dAmplitude : Weaving amplitude, mm / 宽度，摆焊的幅值，单位:mm
 *	@param dIntervalDistance : Referenced weaving interval, robot will rectify it slightly to ensure the StartPos and EndPos be passed through after some weaving cycles, mm / 摆焊的间距，此值仅供参考，机器人系统会自动微调该值以确保运动轨迹经过若干个完整周期的摆动后能通过起始点和结束点，单位:mm
 *	@param nWeaveFrameType : the way to determine the weaving plane and weaving direction
 *                           0: The +X of weaving plane is from StartPos to EndPos. The +Z(normal direction) of weaving plane is along TCP +Z. The +Y of weaving plane is determined by +X and +Z. The start direction of weaving move is toward the +Y side.
 *                           1: The +X of weaving plane is from StartPos to EndPos. The Z of weaving plane is parallel with TCP Z. The +Y of weaving plane is along TCP +Y. The start direction of weaving move is toward the +Y side.
 *                           / 用于决定摆焊平面和摆焊方向的选择方式
 *                           0：摆焊平面的+X方向为从StartPos到EndPos，+Z方向（即摆焊平面的法线方向）为跟刀具坐标系的+Z相同的方向，+Y方向由右手定则确定。摆焊启动时的运动方向跟+Y方向同侧。
 *                           1：摆焊平面的+X方向为从StartPos到EndPos，Z方向跟刀具坐标系的Z方向平行，+Y方向跟刀具坐标系的+Y方向同侧。摆焊启动时的运动方向跟+Y方向同侧。
 *	@param dElevation : The elevation of the weave, degree / 摆焊的仰角，单位:度
 *	@param dAzimuth : The azimuth of the weave, degree / 摆焊的方向角，即绕摆焊平面法向量的旋转角，单位:度
 *	@param dCentreRise : The bulge height of the welding torch at the weaving weld center, mm / 摆焊的中心隆起量，即摆焊中心处焊炬的隆起量，单位:mm
 *  @param nEnableWaiTime : 0 to disable waiting time, 1 to enable waiting time / 是否开启转折点等待时间(0:不使用，1:使用)
 *	@param nPosiTime : Waiting time (ms) at Positive turning point / 正向转折点等待时间ms
 *	@param nNegaTime : Waiting time (ms) at Negative turning point / 负向转折点等待时间ms
 *	@param sTcpName : TCP name / 刀具坐标名称
 *	@param sUcsName : UCS name / 用户坐标名称
 *	@param sCmdID : Command ID / 命令ID
 *  @param vecLastPoint : The end pose of the trajectory / 结束点空间位置
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_MoveLinearWeave(unsigned int boxID, unsigned int rbtID,
                double dStartPos_X,double dStartPos_Y,double dStartPos_Z,double dStartPos_Rx,double dStartPos_Ry,double dStartPos_Rz,
                double dEndPos_X,double dEndPos_Y,double dEndPos_Z,double dEndPos_Rx,double dEndPos_Ry,double dEndPos_Rz,
                double dVelocity, double dAcc, double dRadius, double dAmplitude, double dIntervalDistance, int nWeaveFrameType, double dElevation, 
                double dAzimuth, double dCentreRise, int nEnableWaiTime, int nPosiTime, int nNegaTime, string sTcpName, string sUcsName, string sCmdID, vector<string> &vecLastPoint);

/**
 *	@brief: Start circular weave move / 开始圆弧摆焊运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dStartPos_X~Rz : Start pose / 开始位置
 *	@param dAuxPos_X~Rz : via pose / 经过点位置
 *	@param dEndPos_X~Rz : End pose / 结束位置
 *	@param dVelocity : Motion speed, mm/s / 运动速度，单位是毫米每秒
 *	@param dAcc : Motion acceleration, mm/(s^2) / 运动加速度，毫米每秒平方
 *	@param dRadius : Blending radius, mm / 过渡半径，单位毫米
 *	@param nOrientMode : 0 for unfixed orientation, 1 for fixed orientation  / 0:不使用固定姿态，1:使用固定姿态
 *	@param nMoveWhole : 0 for circle, 1 for arc / 0:整圆轨迹，1:圆弧轨迹，
 *	@param dMoveWholeLen : Invalid when nMoveCType equals 1; Circles when nMoveCType equals 0. / 当使用圆弧运动时该参数无效，
                           通过计算三个点位来确定圆弧路径及弧度；当使用整圆运动时表示整圆的圈数
 *	@param dAmplitude : Amplitude, mm / 宽度，摆焊的幅值，单位:mm
 *	@param dIntervalDistance : Referenced weaving interval, robot will rectify it slightly to ensure the StartPos and EndPos be passed through after some weaving cycles, mm / 摆焊的间距，此值仅供参考，机器人系统会自动微调该值以确保运动轨迹经过若干个完整周期的摆动后能通过起始点和结束点，单位:mm
 *	@param nWeaveFrameType : the way to determine the weaving plane and weaving direction
 *                           0: The +X of weaving plane is from StartPos to EndPos. The +Z(normal direction) of weaving plane is along TCP +Z. The +Y of weaving plane is determined by +X and +Z. The start direction of weaving move is toward the +Y side.
 *                           1: The +X of weaving plane is from StartPos to EndPos. The Z of weaving plane is parallel with TCP Z. The +Y of weaving plane is along TCP +Y. The start direction of weaving move is toward the +Y side.
 *                           / 用于决定摆焊平面和摆焊方向的选择方式
 *                           0：摆焊平面的+X方向为从StartPos到EndPos，+Z方向（即摆焊平面的法线方向）为跟刀具坐标系的+Z相同的方向，+Y方向由右手定则确定。摆焊启动时的运动方向跟+Y方向同侧。
 *                           1：摆焊平面的+X方向为从StartPos到EndPos，Z方向跟刀具坐标系的Z方向平行，+Y方向跟刀具坐标系的+Y方向同侧。摆焊启动时的运动方向跟+Y方向同侧。
 *	@param dElevation : The elevation of the weave, degree / 摆焊的仰角，单位:度
 *	@param dAzimuth : The azimuth of the weave, degree / 摆焊的方向角，即绕摆焊平面法向量的旋转角，单位:度
 *	@param dCentreRise : The bulge height of the welding torch at the weaving weld center, mm / 摆焊的中心隆起量，即摆焊中心处焊炬的隆起量，单位:mm
 *  @param nEnableWaiTime : 0 to disable waiting time, 1 to enable waiting time / 是否开启转折点等待时间(0:不使用，1:使用)
 *	@param nPosiTime : Waiting time (ms) at Positive turning point / 正向转折点等待时间ms
 *	@param nNegaTime : Waiting time (ms) at Negative turning point / 负向转折点等待时间ms
 *	@param sTcpName : TCP name / 刀具坐标名称
 *	@param sUcsName : UCS name / 用户坐标名称
 *	@param sCmdID : Command ID / 命令ID
 *  @param vecLastPoint : The end pose of the trajectory / 结束点空间位置
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_MoveCircularWeave(unsigned int boxID, unsigned int rbtID,
                double dStartPos_X,double dStartPos_Y,double dStartPos_Z,double dStartPos_Rx,double dStartPos_Ry,double dStartPos_Rz,
                double dAuxPos_X,double dAuxPos_Y,double dAuxPos_Z,double dAuxPos_RX,double dAuxPos_RY,double dAuxPos_RZ,
                double dEndPos_X,double dEndPos_Y,double dEndPos_Z,double dEndPos_Rx,double dEndPos_Ry,double dEndPos_Rz,
                double dVelocity, double dAcc, double dRadius, int nOrientMode, int nMoveWhole, double dMoveWholeLen, 
                double dAmplitude, double dIntervalDistance, int nWeaveFrameType, double dElevation, double dAzimuth, 
                double dCentreRise, int nEnableWaiTime, int nPosiTime, int nNegaTime, string sTcpName, string sUcsName, string sCmdID, vector<string> &vecLastPoint);

/**
 *	@brief: Start elliptical move / 开始椭圆轨迹运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dP1_X~Rz : Teaching p1 (p1~p5 should be listed as sequence in arc)/ 示教点1,示教点位必须按照轨迹顺序排列
 *	@param dP2_X~Rz : Teaching p2 / 示教点2
 *	@param dP3_X~Rz : Teaching p3 / 示教点3
 *	@param dP4_X~Rz : Teaching p4 / 示教点4
 *	@param dP5_X~Rz : Teaching p5 / 示教点5
 *	@param nOrientMode : 0 is for arc, 1 is for circle / 弧运动类型：0：圆弧，1：整圆
 *	@param nMoveType : 0 for unfixed orientation , 1 for fixed orientation  / 0:不使用固定姿态，1:使用固定姿态
 *	@param dArcLength : Arc length (mm) / 弧长
 *	@param dVelocity : Motion speed, mm/s / 运动速度，单位是毫米每秒
 *	@param dAcc : Motion acceleration, mm/(s^2) / 运动加速度，毫米每秒平方
 *	@param dRadius : Blending radius, mm / 过渡半径，单位毫米
 *	@param sTcpName : TCP name / 刀具坐标名称
 *	@param sUcsName : UCS name / 用户坐标名称
 *	@param sCmdID : Command ID / 命令ID
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	

HANSROBOT_PRO_API int HRIF_MoveE(unsigned int boxID, unsigned int rbtID,
                double dP1_X, double dP1_Y, double dP1_Z, double dP1_Rx, double dP1_Ry, double dP1_Rz, 
                double dP2_X, double dP2_Y, double dP2_Z, double dP2_Rx, double dP2_Ry, double dP2_Rz, 
                double dP3_X, double dP3_Y, double dP3_Z, double dP3_Rx, double dP3_Ry, double dP3_Rz, 
                double dP4_X, double dP4_Y, double dP4_Z, double dP4_Rx, double dP4_Ry, double dP4_Rz, 
                double dP5_X, double dP5_Y, double dP5_Z, double dP5_Rx, double dP5_Ry, double dP5_Rz, 
                int nOrientMode, int nMoveType, double dArcLength,
                double dVelocity, double dAcc, double dRadius, string sTcpName, string sUcsName, string sCmdID);

/**
 *	@brief: Start spiral move / 开始螺旋运动轨迹运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dSpiralIncrement : Spiral Increment radius /  螺旋运动每圈增量半径
 *	@param dSpiralRadius : Spiral end radius / 螺旋运动结束半径
 *	@param dVelocity : Motion speed, mm/s / 运动速度，单位是毫米每秒
 *	@param dAcc : Motion acceleration, mm/(s^2) / 运动加速度，毫米每秒平方
 *	@param dRadius : Blending radius, mm / 过渡半径，单位毫米
 *	@param sUcsName : UCS name / 用户坐标名称
 *	@param sCmdID : Command ID / 命令ID
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_MoveS(unsigned int boxID, unsigned int rbtID, double dSpiralIncrement, double dSpiralRadius,
                double dVelocity, double dAcc, double dRadius, string sTCPName, string sUcsName, string sCmdID);

/**
 *	@brief: Start relative joint move / 关节相对运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nAxisId: Axis ID , 0~5 for jonit 1 ~6 / 关节编号
 *	@param nDirection: Move direction, 0 for negative，1 for positive / 运动方向，1为正方向，0为负方向
 *	@param dDistance: Move distance (°) / 运动距离(°)
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_MoveRelJ(unsigned int boxID, unsigned int rbtID, int nAxisId, int nDirection, double dDistance);

/**
 *	@brief: Start relative linear move / 开始空间相对运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nPoseId: Pose ID, 0~5 for X, Y, Z, Rx, Ry, Rz / 空间坐标维度编号
 *	@param nDirection: Move direction, 0 for negative, 1 for positive / 运动方向，1为正方向，0为负方向
 *	@param dDistance: Move distance (mm or °) / 运动距离(mm或者°)
 *  @param nToolMotion : Tool motion, 0 for UCS based move, 1 for TCP based move / 运动坐标类型，0为按当前UCS，1为当前TCP
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_MoveRelL(unsigned int boxID, unsigned int rbtID, int nPoseId, int nDirection, double dDistance, int nToolMotion);

/**
 *	@brief: Start relative wayPoint / 开始路点相对运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nType : Move type, 0 for joint, 1 for linear / 运动类型，0为关节运动，1为直线运动
 *  @param nPointList : Using point saved in points list or not, 0 for not using,  1 for using / 是否使用点位列表的点位，0为不使用，1为使用
 *  @param dPos_X~Rz : Target pose / 空间目标位置
 *  @param dPos_J1~6 : Target joint positions / 关节目标位置
 *  @param nrelMoveType : Relative move type, 0 for absolute value, 1 for superimposed value / 相对运动类型，0为绝对值，1为叠加值
 *  @param nAxisMask_1~6 : Is Axis/Direction Moving, 0 for not moving, 1 for moving / 各轴\各方向是否运动，0为不运动，1为运动
 *  @param dTarget_1~6 : Moving distance; invalid when nAxisMask equals 0; joint move an absolute or superimposed distance when nAxisMask equals 1 and nType equals 0; Pose move an absolute or superimposed distance when nAxisMask equals 1 and nType equals 1. / 运动距离，nType=0 并 nAxisMask=1：该关节运动绝对距离或叠加距离；nType=1 并 nAxisMask=1：该坐标运动绝对距离或叠加距离；nAxisMask=0：本参数无效
 *	@param sTcpName : Target TCP name, invalid when IsUseJoint equals 1, "TCP" is valid as default / 目标迪卡尔坐标所处的工具坐标系名称，与示教器页面的名称对应，当nIsUseJoint=1时无效，可使用默认名称”TCP”
 *	@param sUcsName : Target UCS name, invalid when IsUseJoint equals 1, "Base" is valid as default / 目标迪卡尔坐标所处的用户坐标系名称，与示教器页面的名称对应-当nIsUseJoint=1时无效，可使用默认名称”Base”
 *  @param dVelocity: Velocity (mm/s) or (°/s) / 运动速度，关节运动时单位(°/s)，空间运动时 X，Y，Z 单位(mm/s)，Rx，Ry，Rz 单位(°/s)
 *  @param dAcc: Acceleration (mm/s^2) or (°/s^2) / 运动加速度，关节运动时单位(°/s^2)，空间运动时 X，Y，Z 单位(mm/s^2)，Rx，Ry，Rz 单位(°/s^2)
 *  @param dRadius: Blending radius, mm / 过渡半径，单位毫米
 *	@param nIsUseJoint : Using joint or not, usable when MoveType equals 0, 0 for not using joint, 1 for using joint / 是否使用关节角度，是否使用关节角度作为目标点，仅当nMoveType=0时本参数有效。0：不使用关节角度1：使用关节角度
 *	@param nIsSeek,nIOBit,nIOState：Seeking or not, 1 for seeking; Motion stops when DO-nIOBit equals nIoState / 探寻参数，当nIsSeek为1，则开启探寻，这时电箱的DO nIOBit位为nIOState时，就停止运动，否则运动到目标点再停止
 *	@param strCmdID：Command ID, waypoint ID also, customized or set to 1, 2, 3 in order / 当前路点ID，可以自定义，也可以按顺序设置为“1”，“2”，“3”
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_WayPointRel(unsigned int boxID, unsigned int rbtID,
    int nType,
    int nPointList,
    double dPos_X, double dPos_Y, double dPos_Z, double dPos_Rx, double dPos_Ry, double dPos_Rz,
    double dPos_J1, double dPos_J2, double dPos_J3, double dPos_J4, double dPos_J5, double dPos_J6,
    int nrelMoveType,
    int nAxisMask_1, int nAxisMask_2, int nAxisMask_3, int nAxisMask_4, int nAxisMask_5, int nAxisMask_6,
    double dTarget_1, double dTarget_2, double dTarget_3, double dTarget_4, double dTarget_5, double dTarget_6,
    string sTcpName, string sUcsName,
    double dVelocity, double dAcc, double dRadius,
    int nIsUseJoint, int nIsSeek, int nIOBit, int nIOState, string strCmdID);

//************************************************************************/
//**    Interfaces for trajectory / 轨迹运动函数接口                                                             
//************************************************************************/
/**
 *	@brief: Initialize path / 初始化轨迹
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nRawDataType : Raw points data type, 0 for ACS, 1 for PCS / 原始点位数据类型 0：关节点位 1：空间点位
 *	@param sPathName : Path name / 轨迹名称
 *	@param dSpeedRatio : Speed override for PathJ calculation / 轨迹运动速度比(用于PathJ计算)
 *	@param dRadius : Blending radius for PathJ calculation, mm / 过渡半径(用于PathJ计算)，单位：mm
 *	@param dVelocity : Velocity for PathL calculation / 运动速度(用于PathL计算)
 *	@param dAcc : Acceleration for PathL calculation / 运动加速度(用于PathL计算)
 *	@param dJerk : Jerk for PathL calculation / 运动加加速度(用于PathL计算)
 *	@param sUcsName : UCS name of raw point for PathL calculation / 原始点位所在的用户坐标系名称
 *	@param sTcpName : TCP name of raw point for PathL calculation / 原始点位所在的工具坐标值名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_InitPath(unsigned int boxID, unsigned int rbtID, int nRawDataType, string sPathName, double dSpeedRatio,
    double dRadius, double dVelocity, double dAcc, double dJerk, string sUcsName, string sTcpName);

/**
 *	@brief: Push raw points to path / 向轨迹中批量推送原始点位（可多次调用）
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : Path name / 轨迹名称
 *	@param sPoints : Points data, separated by comma, including 6 data for each point. / 点位数据，每个数据以逗号分隔。每个点位6个坐标值，因此此字段值个数必须是6的倍数。系统自动计算出点位个数。
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PushPathPoints(unsigned int boxID, unsigned int rbtID, string sPathName, string sPoints);

/**
 *	@brief: End pushing points to the path and start calculating the pathJ/L / 结束向轨迹中推送点位，并开始计算轨迹J/L
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : Path name / 轨迹名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_EndPushPathPoints(unsigned int boxID,unsigned int rbtID, string sPathName);

/**
 *	@brief: Delete the Path with the specified name / 删除指定名称的轨迹
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : Path name / 轨迹名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_DelPath(unsigned int boxID, unsigned int rbtID, string sPathName);

/**
 *	@brief: Read all path lists / 读取轨迹列表
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param result : Path name list / 读取到的所有轨迹名称列表
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadPathList(unsigned int boxID, unsigned int rbtID, string &result);

/**
 *	@brief: Read the information of the specified name path / 读取指定名称轨迹的信息
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : Path name / 轨迹名称 
 *	@param result : Path name list / 读取到的指定名称轨迹信息
 *           返回结果参数说明，如下：
 *           nRawDataType：Raw point type, 0 for ACS, 1 for PCS / 原始点位类型 0：关节点位 1：空间点位
 *           stateJ：PathJ status: Not taught (0), Teaching in progress (1), Teaching completed (4), Calculating (2), Calculation error (5), 
 *                  Calculation completed (3), Imported (9), Post-import processing ( 10) 
 *                  / PathJ的状态, 8种状态：未示教(0)、示教中(1)、示教完(4)、计算中(2)、计算出错(5)、计算完成(3)、已导入(9)、导入后处理(10)
 *           ErrorCodeJ：PathJ error code / PathJ的错误码
 *           stateL：The state of PathL, the same as stateJ / PathL的状态，同stateJ
 *           ErrorCodeL：PathL error code / PathL的错误码
 *           dOverride：Path speed ratio / 轨迹运动速度比
 *           dRadius：过渡半径，单位：mm
 *           dLinearVel：轨迹运动速度，单位：mm/s
 *           dLinearAcc：轨迹运动加速度，单位：mm/s^2
 *           dLinearJerk：轨迹运动加加速度，单位：mm/s^3
 *           dUcs_X-dUcs_Rz：用户坐标 / User coordinates
 *           dTcp_X-dTcp_Rz：工具坐标 / Tool coordinates
 *           nRawPointCount：原始点位个数 / Raw points count
 *           HeadPoint[]：第一个原始点位坐标(J1/J2/J3/J4/J5/J6 或 X/Y/Z/RX/RY/RZ) / The first raw point coordinates (J1/J2/J3/J4/J5/J6 or X/Y/Z/RX/RY/RZ)
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadPathInfo(unsigned int boxID, unsigned int rbtID, string sPathName, string &result);

/**
 *	@brief: Update the name of the specified path / 更新轨迹名称
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : Raw path name / 轨迹原名称
 *	@param sPathNewName : New path name / 新轨迹名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_UpdatePathName(unsigned int boxID, unsigned int rbtID, string sPathName, string sPathNewName);

/**
 *	@brief: Read the status of the path / 读取轨迹的状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : Path name / 轨迹名称
 *	@param nStateJ : nStateJ, refer to HRIF_ReadPathInfo / PathJ的状态, 参见HRIF_ReadPathInfo
 *	@param nErrorCodeJ : nErrorCodeJ / PathJ的错误码
 *	@param nStateL : nStateL, refer to HRIF_ReadPathInfo / PathL的状态, 参见HRIF_ReadPathInfo
 *	@param nErrorCodeL : nErrorCodeL / PathL的错误码
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadPathState(unsigned int boxID,unsigned int rbtID,string sPathName,int &nStateJ,int &nErrorCodeJ,int &nStateL,int &nErrorCodeL);

/**
 *	@brief: Run PathJ / 运行PathJ
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : Path name / 轨迹名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_MovePathJ(unsigned int boxID, unsigned int rbtID, string sPathName);

/**
 *	@brief: Run PathL / 运行PathL
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : Path name / 轨迹名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_MovePathL(unsigned int boxID,unsigned int rbtID, string sPathName);

/**
 *	@brief: Start to push ACS points for MovePathJ / 初始化关节连续轨迹运动
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : MovePathJ name / 轨迹名称
 *	@param dSpeedRatio : Speed override / 轨迹运动速度比
 *	@param dRadius : Blending radius, mm / 过渡半径，单位毫米
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_StartPushMovePathJ(unsigned int boxID, unsigned int rbtID, string sPathName, double dSpeedRatio, double dRadius);

/**
 *	@brief: Push an ACS raw point to MovePathJ. 4 points at lease are needed. / 添加运动轨迹点，可多次调用此函数，一般情况下点位数量需要>=4
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : MovePathJ name / 轨迹名称
 *	@param J1~6 : joint positions / 关节位置
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PushMovePathJ(unsigned int boxID, unsigned int rbtID, string sPathName, double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6);

/**
 *	@brief: End pushing ACS raw point to MovePathJ and start calculating the path / 结束向轨迹中推送点位，并开始计算轨迹
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : MovePathJ name / 轨迹名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_EndPushMovePathJ(unsigned int boxID, unsigned int rbtID, string sPathName);

/**
 *	@brief: End pushing PCS raw point to MovePathL and start calculating the path / 结束向轨迹中推送点位，并开始计算轨迹
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : MovePathL name / 轨迹名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_EndPushMovePath(unsigned int boxID, unsigned int rbtID, string sPathName);

/**
 *	@brief: Read status of MovePathJ / 读取当前的轨迹状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : MovePathJ name / 轨迹名称
 *	@param nResult : MovePathJ status / 轨迹状态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadMovePathJState(unsigned int boxID, unsigned int rbtID, string sPathName, int& nResult);

/**
 *	@brief: Update MovePathJ name / 更新指定轨迹的名称
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : MovePathJ name / 轨迹名称
 *	@param sPathNewName : New MovePathJ name / 需要更新的轨迹名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_UpdateMovePathJName(unsigned int boxID, unsigned int rbtID, string PathName, string sPathNewName);

/**
 *	@brief: Delete a MovePathJ / 删除指定轨迹
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : MovePathJ name / 需要删除的轨迹名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_DelMovePathJ(unsigned int boxID, unsigned int rbtID, string sPathName);

/**
 *	@brief:  Initialize MovePathL / 初始化MovePathL
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : MovePathL name / 轨迹名称
 *	@param dVelocity : Velocity / 运动速度
 *	@param dAcceleration : Acceleration / 运动加速度
 *	@param jerk : Jerk / 运动加加速度
 *	@param sUcsName : UCS name / 指定轨迹所在的用户坐标系名称
 *	@param sTcpName : TCP name / 指定轨迹所在的工具坐标值名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_InitMovePathL(unsigned int boxID, unsigned int rbtID, string sPathName, double dVelocity, double dAceleration, double djerk, string sUcsName, string sTcpName);

/**
 *	@brief: Push an PCS point to MovePathL / 下发MovePathL点位-调用一次下发一个目标点位
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : MovePathL name / 轨迹名称
 *	@param dX~Rz : Raw point pose / 目标迪卡尔坐标位置 
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PushMovePathL(unsigned int boxID, unsigned int rbtID, string sPathName, double dX, double dY, double dZ, double dRX, double dRY, double dRZ);

/**
 *	@brief: Push an list of ACS or PCS points to MovePathJ or MovePathL / 批量下发MovePathJ/MovePathL点位-调用一次可下发多个点位数据
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param sPathName : MovePathJ or MovePathL name / 轨迹名称
 *	@param nMoveType : Move type, 0 for MovePathJ, 1 for MovePathL / 运动类型-0(MovePathJ)/1(MovePathL)
 *	@param nPointsSize : Points number / 轨迹点位数量
 *	@param sPoints : Points list, separated by comma  / 轨迹点列表，以","分隔
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PushMovePaths(unsigned int boxID, unsigned int rbtID, string sPathName, int nMoveType, int nPointsSize, string sPoints);

/**
 *	@brief: Set Override for MovePathL / 设置轨迹运动速度比，轨迹运动中设置有效。仅对MovePathL生效。
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dOverride: Override / 速度参数 [1~100]
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetMovePathOverride(unsigned int boxID, unsigned int rbtID, double dOverride);

/**
 *	@brief: Read process and point index of a path move / 读取轨迹运动进度，仅对MovePathL生效，建议读取频率保持在20ms上下。
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dProcess : Current path process, 0~1, more than 0.999999 for move done / 当前运动进度(0-1),>0.999999表示运动完成
 *	@param nIndex : Point index finished / 当前轨迹运动到的点位索引
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadTrackProcess(unsigned int boxID, unsigned int rbtID, double& dProcess, int& nIndex);

//************************************************************************/
//**    Interfaces for Servo / 伺服控制类函数接口                                                             
//************************************************************************/
/**
 *	@brief: Start Servo J/P with the specified ServoTime of update and LookaheadTime. This function is a member of the suite including HRIF_StartServo, HRIF_PushServoJ and HRIF_PushServoP. / 启动机器人在线控制（servoJ 或 servoP）时，设定位置固定更新的周期和前瞻时间。注：HRIF_StartServo/HRIF_PushServoJ/HRIF_PushServoP为一套接口，其他servo指令为一套接口，不共用。
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dServoTime : interval of update (s) / 固定更新的周期 s
 *	@param dLookaheadTime : LookaheadTime (s) / 前瞻时间 s
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_StartServo(unsigned int boxID, unsigned int rbtID, double dServoTime, double dLookaheadTime);

/**
 *	@brief: Push point of joint positions to robot with the ServoTime and LookaheadTime specified in HRIF_StartServo function, and the robot will track the joint positions in real time. This function is a member of the suite including HRIF_StartServo, HRIF_PushServoJ and HRIF_PushServoP. / 在线关节位置命令控制，以 StartServo 设定的固定更新时间发送关节位置，机器人将实时的跟踪关节位置指令。注：HRIF_StartServo/HRIF_PushServoJ/HRIF_PushServoP为一套接口，其他servo指令为一套接口，不共用。
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param J1~6 : Joint positions / 更新的关节位置
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PushServoJ(unsigned int boxID, unsigned int rbtID, double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6);

/**
 *	@brief: Push pose point to robot with the ServoTime and LookaheadTime specified in HRIF_StartServo function, and the robot will track the pose in real time. This function is a member of the suite including HRIF_StartServo, HRIF_PushServoJ and HRIF_PushServoP. / 在线末端TCP位置命令控制，以 StartServo 设定的固定更新时间发送 TCP 位置，机器人将实时的跟踪目标 TCP 位置逆运算转换后的关节位置指令。注：HRIF_StartServo/HRIF_PushServoJ/HRIF_PushServoP为一套接口，其他servo指令为一套接口，不共用。
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param vecCoord : pose list / 更新的目标迪卡尔坐标位置
 *	@param vecUcs : UCS list / 目标位置对应的UCS
 *	@param vecTcp : TCP list /目标位置对应的TCP
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PushServoP(unsigned int boxID, unsigned int rbtID, vector<double>& vecCoord, vector<double>& vecUcs, vector<double>& vecTcp);

/**
 *	@brief: 在SpeedJ模式下，下发关节命令速度指令
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dJ1CmdVel~dJ6CmdVel : Joint speed / 更新的命令关节速度，单位：°/s
 *  @param dAcc : Joint Acc / 关节设定加速度，单位：°/s^2
 *  @param dRuntime : 指令运行超过该时间，运动将会停止。单位：s
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SpeedJ(unsigned int boxID, unsigned int rbtID, double dJ1CmdVel,double dJ2CmdVel,double dJ3CmdVel,double dJ4CmdVel,
                    double dJ5CmdVel,double dJ6CmdVel,double dAcc,double dRuntime);

/**
 *	@brief: Initialize servoEsJ, truncating points / 初始化在线控制模式，清空缓存点位
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_InitServoEsJ(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief: Start move for servoEsJ / 启动在线控制模式，开始运动
 *	@param boxID : control box number, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dServoTime : interval of update (s) / 固定更新的周期 s
 *	@param dLookaheadTime : LookaheadTime (s) / 前瞻时间 s
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_StartServoEsJ(unsigned int boxID, unsigned int rbtID, double dServoTime, double dLookheadTime);

/**
 *	@brief: Push points list for servoEsJ / 批量下发在线控制点位
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nPointsSize: Point size, maximum 500 / 点位数量，最大允许下发500个点位
 *	@param sPoints : Points list, separated by ","  / 点列表，以","分隔
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PushServoEsJ(unsigned int boxID, unsigned int rbtID, int nPointsSize, string sPoints);

/**
 *	@brief: Read servoEsJ state with call interval more than 20 ms / 读取当前是否可以继续下发点位信息，循环读取间隔>20ms
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nState: Pushing point is allowed or not; 0 for allowed, 1 for not allowed /  0：允许下发点位; 1：不允许下发点位
 *	@param nIndex: Current point index reached / 当前运行点位索引
 *	@param nCount: Points number finished / 当前点位数量
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadServoEsJState(unsigned int boxID, unsigned int rbtID, int& nState, int& nIndex, int& nCount);

/**
 *	@brief: Set motion parameters of tracking motion / 设置相对跟踪运动控制参数
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nState: 0 to disable tracking, 1 to enable tracking / 0:关闭传送带跟踪，1:开启传送带跟踪
 *	@param dDistance: Relative distance of tracking / 相对跟踪运动保持的相对距离
 *	@param dAwayVelocity: Away velocity of tracking / 相对跟踪的运动的远离速度
 *	@param dGobackVelocity: Back velocity of tracking / 相对跟踪的运动的返回速度
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetMoveTraceParams(unsigned int boxID, unsigned int rbtID, int nState, double dDistance, double dAwayVelocity, double dGobackVelocity);

/**
 *	@brief: Set initial parameters of tracking motion / 设置相对跟踪运动初始化参数
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dK,dB: parameters in equation as y = dK * x + dB / 计算公式y = dK * x + dB
 *	@param dMaxLimit: Max distance detected by laser sensor / 激光传感器检测距离最大值
 *	@param dMinLinit: Min distance detected by laser sensor / 激光传感器检测距离最小值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetMoveTraceInitParams(unsigned int boxID, unsigned int rbtID, double dK, double dB, double dMaxLimit, double dMinLinit);

/**
 *	@brief: Set end orientation in tracking motion / 设置相对跟踪运动时末端的姿态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX~Z: invalid, 0 recommended / 无意义的参数，建议输入0
 *	@param dRx~Rz: End orientation in specified UCS through function HRIF_SetUCSByName / 跟踪探寻时末端在以HRIF_SetUCSByName指定的UCS中的姿态
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetMoveTraceUcs(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz);

/**
 *	@brief: Set tracking motion state / 设置传送带跟踪运动开关状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nState: 0 to disable tracking, 1 to enable tracking / 0:关闭传送带跟踪，1:开启传送带跟踪
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetTrackingState(unsigned int boxID, unsigned int rbtID, int nState);

/**
 *	@brief: Excute HRApp command / 执行插件app命令
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param sCmdName: Command name / 命令名称
 *	@param sParams: Parameters list / 参数列表
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_HRAppCmd(unsigned int boxID, string sCmdName, string sParams);

/**
 *	@brief: Write End holding registers of Modbus slave / 写末端连接的modbus从站寄存器
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nSlaveID: Modbus slave ID / Modbus从站ID
 *	@param nFunction: Function code / 功能码
 *	@param nRegAddr: Register beginning address / 起始地址
 *	@param nRegCount: Registers number, maximum 11 / 寄存器数量，最大11个
 *	@param vecData: Registers value, with the registers size of nRegCount / 需要写的寄存器值，vector的大小与寄存器数量一致
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_WriteEndHoldingRegisters(unsigned int boxID, unsigned int rbtID, int nSlaveID, int nFunction, int nRegAddr, int nRegCount, vector<int> vecData);

/**
 *	@brief: Read End holding registers of Modbus slave / 读末端连接的modbus从站寄存器
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nSlaveID: Modbus slave ID / Modbus从站ID
 *	@param nFunction: Function code / 功能码
 *	@param nRegAddr: Register beginning address / 起始地址
 *	@param nRegCount: Registers number, maximum 12 / 寄存器数量，最大12个
 *	@param vecData: obtained registers value, with the registers size of nRegCount / 返回读取的寄存器值，vector的大小与寄存器数量一致
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_ReadEndHoldingRegisters(unsigned int boxID, unsigned int rbtID, int nSlaveID, int nFunction, int nRegAddr, int nRegCount, vector<int>& vecData);

/**
 *	@brief: Set max velocity of tracking position  / 设置位置跟随的最大跟随速度
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dMaxLineVel: Max follow position velocity / 位置跟随直线最大跟随速度，单位[mm/s]
 *	@param dMaxOriVel:  Max follow angular velocity / 位置跟随姿态最大跟随速度，单位[°/s]
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetPoseTrackingMaxMotionLimit(unsigned int boxID, unsigned int rbtID,
                        double dMaxLineVel,double dMaxOriVel);

/**
 *	@brief: Set location tracking timeout stop time / 设置位置跟踪超时停止时间
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dTime: time out / 超时时间，单位：秒[s]
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetPoseTrackingStopTimeOut(unsigned int boxID, unsigned int rbtID, double dTime);

/**
 *	@brief: Set PID of tracking position / 设置PID参数
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dPosPID1-dPosPID3: Follow position PID / 位置跟随PID
 *	@param dOriPID1-dOriPID3: Follow angular PID / 姿态跟随PID
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetPoseTrackingPIDParams(unsigned int boxID, unsigned int rbtID, 
                        double dPosPID1,double dPosPID2,double dPosPID3,
                        double dOriPID1,double dOriPID2,double dOriPID3);

/**
 *	@brief: Set target of tracking position / 设置位置跟随的目标位置
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX-dRz: Target Position / 位置跟随的目标位置
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetPoseTrackingTargetPos(unsigned int boxID, unsigned int rbtID, 
                        double dX,double dY,double dZ,double dRx,double dRy,double dRz);

/**
 *	@brief: Set state of tracking position / 设置位置跟随状态
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nState: 0: Disable tracking position / 0：关闭位置跟随
 *                 1: Enable tracking position / 1：开启位置跟随
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetPoseTrackingState(unsigned int boxID, unsigned int rbtID, int nState);

/**
 *	@brief:Set real time position update of tracking position / 设置实时更新传感器位置信息
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX-dRz: Checking distance / 检测距离
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetUpdateTrackingPose(unsigned int boxID, unsigned int rbtID, 
                        double dX,double dY,double dZ,double dRx,double dRy,double dRz);

/**
 *	@brief: Confige TCP / 配置TCP
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX-dRz: TCP Position / TCP位置与方向
 *                 
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_ConfigTCP(unsigned int boxID, unsigned int rbtID,
                                     string sTcpName,double dX, double dY, double dZ, double dRx, double dRy, double dRz);

/**
 *	@brief: Confige UCS / 配置UCS
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX-dRz: UCS Position / 用户坐标系值
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_ConfigUCS(unsigned int boxID, unsigned int rbtID,
                                     string strName,double x,double y,double z,double rx,double ry,double rz);

/**
 *	@brief:Move to target pose / 运动到空间点位
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param dX-dRz: target pose / 目标点空间坐标
 *	@param strTCPName: TCP name / TCP坐标名称
 *	@param strUCSName: UCS name / UCS坐标名称
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
//HANSROBOT_PRO_API int HRIF_MoveLTo(unsigned int boxID, unsigned int nRbtID, 
//                                    double dX, double dY, double dZ, double dRx, double dRy, double dRz, string strTCPName, string strUCSName);
                                    
/**
 *	@brief: Move to target joint positions / 运动到关节点位
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param J1-J6: target positions / 目标点关节坐标
 *	
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */	
//HANSROBOT_PRO_API int HRIF_MoveJTo(unsigned int boxID, unsigned int nRbtID, 
//                                    double J1, double J2, double J3, double J4, double J5, double J6);

/**
 *	@brief: Move to safe space / 移动到安全空间
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_MoveToSS(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief: set the collision sensitivity level / 设置碰撞安全等级
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nSafeLevel : sensitivity level, 0-5, 0 for the highest, 5 for the lowest / 安全等级(0-5) 0：高限制，安全系数高  5：低限制，高风险
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetCollideLevel(unsigned int boxID, unsigned int rbtID,int nSafeLevel);

/**
 *	@brief: read the collision sensitivity level / 读取碰撞安全等级
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nSafeLevel : sensitivity level, 0-5, 0 for the highest, 5 for the lowest / 安全等级(0-5) 0：高限制，安全系数高  5：低限制，高风险
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GetCollideLevel(unsigned int boxID, unsigned int rbtID,int &nSafeLevel);

/**
 *	@brief:设定EtherCatAO
 *	@param boxID : Control box ID, 0 as default / 电箱ID号，默认值=0
 *	@param rbtID : Robot ID, 0 as default / 机器人ID号，默认值=0
 *	@param nIndex: AO号
 *  @param dValue: 模拟量值
 *
 *	@return : nRet=0 : Function call succeeded / 函数调用成功
 *            nRet>0 : Error code of function call / 函数调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetECATAO(unsigned int boxID, unsigned int rbtID, int nIndex, double dValue);

#ifdef __cplusplus
}
#endif
#endif // !_HR_PRO_H_