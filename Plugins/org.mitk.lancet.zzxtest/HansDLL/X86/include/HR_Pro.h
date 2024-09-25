#pragma once
/*******************************************************************
 *	Copyright(c) 2020-2022 Company Name
 *  All rights reserved.
 *	
 *	FileName:HR_Pro.h
 *	Descriptio:C++SDK
 *  version:1.0.0.0
 *	
 *	Date:2022-08-29
 *	Author:chenpeng
 *	Descriptio:增加函数说明
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
//**    回调类函数接口
//**    以下的接口，boxID是代表哪个电箱，最多可以连接5个电箱    
//**    默认使用欧拉角表示，单位是毫米，度                                                                  
//************************************************************************/

/**
 *	@brief:日志回调函数
 *	@param boxID : 电箱ID号，默认值=0
 *	@param logLevel : 日志等级
 *	@param strLog : 日志信息
 *	@param arg:
 *	
 *	@return : 回调结果
 */
typedef void (*LogDbgCallback)(int logLevel, const string& strLog, void* arg);
HANSROBOT_PRO_API int HRIF_SetLogDbgCB(unsigned int boxID, LogDbgCallback ptr, void* arg);

/**
 *	@brief:事件回调函数
 *	@param boxID : 电箱ID号，默认值=0
 *	@param nErrorCode : 错误码
 *	@param nState : 状态
 *	@param strState: 错误信息
 *	@param arg:
 *	
 *	@return : 回调结果
 */
typedef void (*EventCallback)(int nErrorCode, int nState, const string& strState, void* arg);
HANSROBOT_PRO_API int HRIF_SetEventCB(unsigned int boxID, EventCallback ptr, void* arg);


//************************************************************************/
//**    初始化函数接口                                                             
//************************************************************************/
/**
 *	@brief:连接控制器10003端口
 *	@param boxID : 电箱ID号，默认值=0
 *	@param hostName : 控制器IP地址，根据实际设置的IP地址定义
 *	@param nPort : 控制器端口号，默认值=10003
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_Connect(unsigned int boxID, const char* hostName, unsigned short nPort);

/**
 *	@brief:断开连接控制器10003端口
 *	@param boxID : 电箱ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_DisConnect(unsigned int boxID);

/**
 *	@brief:控制器是否连接
 *	@param boxID : 电箱ID号，默认值=0
 *	@return : false : 控制器未连接
 *            true : 控制器已连接
 */
HANSROBOT_PRO_API bool HRIF_IsConnected(unsigned int boxID);

/**
 *	@brief:控制器断电(断开机器人供电，系统关机)
 *	@param boxID : 电箱ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ShutdownRobot(unsigned int boxID);

/**
 *	@brief:连接控制器电箱
 *	@param boxID : 电箱ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_Connect2Box(unsigned int boxID);

/**
 *	@brief:机器人上电
 *	@param boxID : 电箱ID号，默认值=0
  *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_Electrify(unsigned int boxID);

/**
 *	@brief:机器人断电
 *	@param boxID : 电箱ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_Blackout(unsigned int boxID);

/**
 *	@brief:连接过程中会启动主站，初始化从站，配置参数，检查配置，完成后跳转到去使能状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_Connect2Controller(unsigned int boxID);

/**
 *	@brief:判断是否为模拟机器人
 *	@param boxID : 电箱ID号，默认值=0
 *	@param nSimulateRobot ：是否为模拟机器人
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_IsSimulateRobot(unsigned int boxID, int& nSimulateRobot);

/**
 *	@brief:控制器是否启动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param nSimulateRobot : 0 : 真实机器人
 *                           1 : 模拟机器人
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_IsControllerStarted(unsigned int boxID, int& nStarted);

/**
 *	@brief:读取控制器版本号
版本号的格式以.分隔，数字的意义如下
CPSVer.controlVer.BoxVerMajor.BoxVerMid.BoxVerMin.AlgorithmVer.ElfinFirmwareVer
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param strVer : 整体版本号
 *	@param nCPSVersion : CPS版本
 *	@param nCodesysVersion : 控制器版本
 *	@param nBoxVerMajor : 电箱版本号:0：模拟电箱 1-4：电箱版本号
 *	@param nBoxVerMid : 控制板固件版本
 *	@param nBoxVerMin : 控制板固件版本
 *	@param nAlgorithmVer : 算法版本
 *	@param nElfinFirmwareVer : 固件版本
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadVersion(unsigned int boxID, unsigned int rbtID, string& strVer, int& nCPSVersion, int& nCodesysVersion,
                     int &nBoxVerMajor, int &nBoxVerMid, int &nBoxVerMin,
                     int &nAlgorithmVer, int &nElfinFirmwareVer);

/**
 *	@brief:读取机器人类型
 *	@param boxID : 电箱ID号，默认值=0
 *	@param strModel : 机器人类型
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadRobotModel(unsigned int boxID, string& strModel);

/**
 *	@brief:机器人使能命令
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpEnable(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief:机器人去使能命令
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpDisable(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief:机器人复位命令
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpReset(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief:停止运动命令
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpStop(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief:暂停运动命令
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpInterrupt(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief:继续运动命令
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpContinue(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief:开启自由驱动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpOpenFreeDriver(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief:关闭自由驱动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GrpCloseFreeDriver(unsigned int boxID, unsigned int rbtID);

//************************************************************************/
//**    脚本函数接口                                                             
//************************************************************************/
/**
 *	@brief:运行指定脚本函数
 *	@param boxID : 电箱ID号，默认值=0
 *	@param strFuncName : 指定脚本函数名称，对应示教器界面的函数
 *	@param param : 参数
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_RunFunc(unsigned int boxID, string strFuncName, vector<string> param);

/**
 *	@brief:执行脚本Main函数，调用后执行示教器页面编译好的脚本文件
 *	@param boxID : 电箱ID号，默认值=0
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_StartScript(unsigned int boxID);

/**
 *	@brief:停止脚本，调用后停止示教器页面正在执行脚本文件
 *	@param boxID : 电箱ID号，默认值=0
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_StopScript(unsigned int boxID);

/**
 *	@brief:暂停脚本，调用后暂停示教器页面正在执行脚本文件
 *	@param boxID : 电箱ID号，默认值=0
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PauseScript(unsigned int boxID);

/**
 *	@brief:继续脚本，调用后继续运行示教器页面正在暂停的脚本文件，不处于暂停状态则返回20018错误
 *	@param rbtID : 机器人ID号，默认值=0
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ContinueScript(unsigned int boxID);


//************************************************************************/
//**    电箱控制函数接口                                                             
//************************************************************************/
/**
 *	@brief:读取电箱信息
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nConnected : 电箱连接状态
 *	@param n48V_ON : 48V电压值
 *	@param d48OUT_Voltag : 48V输出电压值
 *	@param d48OUT_Current : 48V输出电流值
 *	@param nRemoteBTN : 远程急停状态
 *	@param nThreeStageBTN : 三段按钮状态
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxInfo(unsigned int boxID, unsigned int rbtID,
                    int &nConnected, 
                    int &n48V_ON, 
                    double &d48OUT_Voltag, 
                    double &d48OUT_Current, 
                    int &nRemoteBTN, 
                    int &nThreeStageBTN);

/**
 *	@brief:读取电箱控制数字输入状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param nBit : 控制数字输入位
 *	@param nVal : 控制数字输入对应位状态
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxCI(unsigned int boxID, int nBit, int& nVal);

/**
 *	@brief:读取电箱通用数字输入状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param nBit : 通用数字输入位
 *	@param nVal : 通用数字输入对应位状态
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxDI(unsigned int boxID, int nBit, int& nVal);

/**
 *	@brief:读取电箱控制器数字输出状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param nBit : 控制器数字输出位
 *	@param nVal : 控制数字输出对应位状态
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxCO(unsigned int boxID, int nBit, int& nVal);

/**
 *	@brief:读取电箱通用数字输出状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param nBit : 通用数字输出位
 *	@param nVal : 通用数字输出对应位状态
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxDO(unsigned int boxID, int nBit, int& nVal);

/**
 *	@brief:读取电箱模拟量输入值
 *	@param boxID : 电箱ID号，默认值=0
 *	@param nBit : 模拟量输入通道
 *	@param dbVal : 电流模式:4-20mA,电压模式:0-10V
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxAI(unsigned int boxID, int nBit, double& dbVal);

/**
 *	@brief:读取电箱模拟量输出模式和值
 *	@param boxID : 电箱ID号，默认值=0
 *	@param nBit : 模拟量输入通道
 *	@param nMode : 对应位模拟量通道模式
 *	@param dbVal : 电流模式:4-20mA,电压模式:0-10V
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadBoxAO(unsigned int boxID, int nBit, int& nMode, double& dbVal);

/**
 *	@brief:设置控制数字输出状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param nBit : 控制数字输出位
 *	@param nVal : 控制器数字输出目标状态
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetBoxCO(unsigned int boxID, int nBit, int nVal);

/**
 *	@brief:设置电箱通用数字输出状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param nBit : 通用数字输出位
 *	@param nVal : 通用数字输出目标状态
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetBoxDO(unsigned int boxID, int nBit, int nVal);

/**
 *	@brief:设置电箱模拟量输出模式
 *	@param boxID : 电箱ID号，默认值=0
 *	@param nBit : 模拟量输出模式通道
 *	@param nMode : 模拟量输出模式目标状态
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetBoxAOMode(unsigned int boxID, int nBit, int nMode);

/**
 *	@brief:设置电箱模拟量输出值
 *	@param boxID : 电箱ID号，默认值=0
 *	@param nBit : 模拟量输出通道
 *	@param nMode : 模拟量输出目标值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetBoxAOVal(unsigned int boxID, int nBit, double dbVal, int nMode);

/**
 *	@brief:读取末端数字输入状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nBit : 末端数字输入索引
 *	@param nVal : 末端数字输入状态值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadEndDI(unsigned int boxID, unsigned int rbtID, int& nBit, int& nVal);

/**
 *	@brief:读取末端数字输出状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nBit : 末端数字输出索引
 *	@param nVal : 末端数字输出状态值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadEndDO(unsigned int boxID, unsigned int rbtID, int& nBit, int& nVal);

/**
 *	@brief:设置末端数字输出状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nBit : 末端数字输出索引
 *	@param nVal : 末端数字输出状态值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetEndDO(unsigned int boxID, unsigned int rbtID, int nBit, int nVal);

/**
 *	@brief:读取末端模拟量输入状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nBit : 末端模拟量输入索引
 *	@param nVal : 末端模拟量输入状态值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadEndAI(unsigned int boxID, unsigned int rbtID, int& nBit, double& dVal);

/**
 *	@brief:读取末端模拟量输入状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nBit1 : 末端按键1状态
 *	@param nBit2 : 末端按键2状态
 *	@param nBit3 : 末端按键3状态
 *	@param nBit4 : 末端按键4状态
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadEndBTN(unsigned int boxID, unsigned int rbtID, int& nBit1, int& nBit2, int& nBit3, int& nBit4);

//////////////////////////////////////////////////////////////////////////////////////
/**
 *	@brief:设置速度比
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dbOverride : 需要设置的速度比(0.01-1)
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetOverride(unsigned int boxID, unsigned int rbtID, double dOverride);

/**
 *	@brief:开启或关闭Tool坐标系运动模式
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nState : 0(关闭)/1(开启)
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetToolMotion(unsigned int boxID, unsigned int rbtID, int nState);

//速度单位是毫米每秒，度每秒，加速度毫米每秒平方，度每秒平方
/**
 *	@brief:设置当前负载参数
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param Mass：质量
 *	@param dX：质心X方向偏移
 *	@param dY：质心Y方向偏移
 *	@param dZ：质心Z方向偏移
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetPayload(unsigned int boxID, unsigned int rbtID, double dMass, double dX, double dY, double dZ);
/**
 *	@brief:设置关节最大速度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dJ1-dJ6 : J1-J6轴最大速度，单位[°/ s]
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetJointMaxVel(unsigned int boxID, unsigned int rbtID, double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6);

/**
 *	@brief:设置关节最大加速度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dJ1-dJ6 : J1-J6轴轴最大加速度，单位[°/ s2]
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetJointMaxAcc(unsigned int boxID, unsigned int rbtID, double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6);

/**
 *	@brief:设置直线运动最大速度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dMaxVel : 直线运动最大速度
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetLinearMaxVel(unsigned int boxID, unsigned int rbtID, double dMaxVel);

/**
 *	@brief:设置直线运动最大加速度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dMaxAcc : 直线运动最大加速度
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetLinearMaxAcc(unsigned int boxID, unsigned int rbtID, double dMaxAcc);

/**
 *	@brief:设置最大关节运动范围
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dMaxJ1-dMaxJ6 : 最大关节角度
 *	@param dMinJ1-dMinJ6 : 最小关节角度
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetMaxAcsRange(unsigned int boxID, unsigned int rbtID,
                        double dMaxJ1, double dMaxJ2, double dMaxJ3, double dMaxJ4, double dMaxJ5, double dMaxJ6,
                        double dMinJ1, double dMinJ2, double dMinJ3, double dMinJ4, double dMinJ5, double dMinJ6);

/**
 *	@brief:设置空间最大运动范围
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dMaxX-dMaxZ : XYZ最大范围
 *	@param dMinX-dMinZ : XYZ最小范围
 *	@param dUcs_X-dUcs_Rz : 基于用户坐标
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetMaxPcsRange(unsigned int boxID, unsigned int rbtID,
                        double dMaxX, double dMaxY, double dMaxZ, double dMinX, double dMinY, double dMinZ,
                        double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz);

/**
 *	@brief:设置关节最大运动范围
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dJ1-dJ6 : J1-J6轴最大运动范围
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadJointMaxVel(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief:读取关节最大加速度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dJ1-dJ6 : J1-J6轴最大加速度
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadJointMaxAcc(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief:读取关节最大加加速度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dJ1-dJ6 : J1-J6轴最大加加速度
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadJointMaxJerk(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief:读取直线运动速度参数
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dMaxVel : 直线运动速度
 *	@param dMaxAcc : 直线运动加速度
 *	@param dMaxJerk : 直线运动加加速度
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadLinearMaxSpeed(unsigned int boxID, unsigned int rbtID, double& dMaxVel, double& dMaxAcc, double& dMaxJerk);

//////////////////////////////////////////////////////////////////////////////////////
/**
 *	@brief:读取急停信息
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nESTO_Error:急停回路有两路，当两路信号不相同时，则认为急停回路有错误，这个则为1
 *	@param nESTO：急停信号，发生急停时，会断48V输出到本体的供电，但是不会断220V到48V的供电
 *	@param nSafetyGuard_Error:安全光幕回路有两路，当两路信号不相同时，则认为安全光幕回路有错误，这个则为1
 *	@param nSafetyGuard：安全光幕信号，发生安全光幕时，会停止机器人运动，并且不再接受运动指令，不会断本体供电
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadEmergencyInfo(unsigned int boxID, unsigned int rbtID,
                            int &nESTO_Error, 
                            int &nESTO, 
                            int &nSafetyGuard_Error, 
                            int &nSafetyGuard);

/**
 *	@brief:读取当前机器人状态标志
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nMovingState : 运动状态
 *	@param nEnableState : 使能状态
 *	@param nErrorState : 错误状态
 *	@param nErrorCode : 错误码
 *	@param nErrorAxis : 错误轴ID
 *	@param nBreaking : 抱闸是否打开状态
 *	@param nPause : 暂停状态
 *	@param nEmergencyStop : 急停状态
 *	@param nSaftyGuard : 安全光幕状态
 *	@param nElectrify : 上电状态
 *	@param nIsConnectToBox : 连接电箱状态
 *	@param nBlendingDone : WayPoint运动完成状态
 *	@param nInPos : 运动命令位置与实际位置是否到位
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
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
 *	@brief:读取当前机器人状态标志
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nMovingState : 运动状态
 *	@param nEnableState : 使能状态
 *	@param nErrorState : 错误状态
 *	@param nErrorCode : 错误码
 *	@param nErrorAxis : 错误轴ID
 *	@param nBreaking : 抱闸是否打开状态
 *	@param nPause : 暂停状态
 *	@param nEmergencyStop : 急停状态
 *	@param nSaftyGuard : 安全光幕状态
 *	@param nElectrify : 上电状态
 *	@param nIsConnectToBox : 连接电箱状态
 *	@param nBlendingDone : WayPoint运动完成状态
 *	@param nPos : 运动命令位置与实际位置是否到位
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
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
 *	@brief:读取WayPoint当前运动ID号
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param strCurWaypointID : 当前ID号
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCurWaypointID(unsigned int boxID, unsigned int rbtID, string& strCurWaypointID);

/**
 *	@brief:读取错误码
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nErrorCode : 错误码
 *	@param Params : 每个轴(J1~J6)的错误码，如果没有错误则为0
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadAxisErrorCode(unsigned int boxID, unsigned int rbtID, int& nErrorCode, int& nJ1, int& nJ2, int& nJ3, int& nJ4, int& nJ5, int& nJ6);

/**
 *	@brief:读取当前状态机状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nCurFSM : 当前状态机状态，具体描述见接口说明文档
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCurFSM(unsigned int boxID, unsigned int rbtID, int& nCurFSM, string& strCurFSM);

/**
 *	@brief:读取状态机状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nCurFSM : 当前状态机状态
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCurFSMFromCPS(unsigned int boxID, unsigned int rbtID, int& nCurFSM);

/**
 *	@brief:读取当前位置信息
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dX-dRz : 迪卡尔坐标
 *	@param dJ1-dJ6 : 关节坐标
 *	@param dTcp_X-dTcp_Rz : TCP坐标
 *	@param dUcs_X-dUcs_Rz : 用户坐标
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadActPos(unsigned int boxID, unsigned int rbtID,
					 double &dX, double &dY, double &dZ, double &dRx, double &dRy, double &dRz,
					 double &dJ1, double &dJ2, double &dJ3, double &dJ4, double &dJ5, double &dJ6,
					 double &dTcp_X, double &dTcp_Y, double &dTcp_Z, double &dTcp_Rx, double &dTcp_Ry, double &dTcp_Rz,
                    double &dUcs_X, double &dUcs_Y, double &dUcs_Z, double &dUcs_Rx, double &dUcs_Ry, double &dUcs_Rz);

/**
 *	@brief:读取命令关节位置
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dJ1-dJ6 : 命令关节位置
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCmdJointPos(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief:读取实际关节位置
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dJ1-dJ6 : 实际关节位置
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadActJointPos(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief:读取命令TCP位置
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dX-dRz : 命令TCP位置
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCmdTcpPos(unsigned int boxID, unsigned int rbtID, double& dX, double& dY, double& dZ, double& dRx, double& dRy, double& dRz);

/**
 *	@brief:读取实际TCP位置
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dX-dRz : 实际TCP位置
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadActTcpPos(unsigned int boxID, unsigned int rbtID, double& dX, double& dY, double& dZ, double& dRx, double& dRy, double& dRz);

/**
 *	@brief:读取命令关节速度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dJ1-dJ6 : 命令关节速度
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCmdJointVel(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief:读取实际关节速度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dJ1-dJ6 : 实际关节速度
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadActJointVel(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief:读取命令TCP速度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dX-dRz : 命令TCP速度
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCmdTcpVel(unsigned int boxID, unsigned int rbtID, double& dX, double& dY, double& dZ, double& dRx, double& dRy, double& dRz);

/**
 *	@brief:读取实际TCP速度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dX-dRz : 实际TCP速度
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadActTcpVel(unsigned int boxID, unsigned int rbtID, double& dX, double& dY, double& dZ, double& dRx, double& dRy, double& dRz);

/**
 *	@brief:读取命令关节电流
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dJ1-dJ6 : 命令关节电流
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCmdJointCur(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief:读取实际关节电流
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dJ1-dJ6 : 实际关节电流
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadActJointCur(unsigned int boxID, unsigned int rbtID, double& dJ1, double& dJ2, double& dJ3, double& dJ4, double& dJ5, double& dJ6);

/**
 *	@brief:读取末端TCP速度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dCmdVel : 命令速度mm/s
 *	@param dActVel : 实际速度mm/s
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadTcpVelocity(unsigned int boxID, unsigned int rbtID, double& dCmdVel, double& dActVel);

//************************************************************************/
//**    计算类函数接口                                                             
//************************************************************************/
/**
 *	@brief:四元素转欧拉角，欧拉角的单位是度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dQuaW : 四元素W
 *	@param dQuaX : 四元素X
 *	@param dQuaY : 四元素Y
 *	@param dQuaZ : 四元素Z
 *	@param dRx: 欧拉角rx
 *	@param rRy: 欧拉角ry
 *	@param dRz: 欧拉角rz
 *	
 *	@return : 是否成功
 */
HANSROBOT_PRO_API int HRIF_Quaternion2RPY(unsigned int boxID, unsigned int rbtID, double dQuaW, double dQuaX, double dQuaY, double dQuaZ, double& dRx, double& dRy, double& dRz);

/**
 *	@brief:欧拉角转四元素，欧拉角的单位是度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dRx: 欧拉角rx
 *	@param dRy: 欧拉角ry
 *	@param dRz: 欧拉角rz
 *	@param dQuaW : 四元素W
 *	@param dQuaX : 四元素X
 *	@param dQuaY : 四元素Y
 *	@param dQuaZ : 四元素Z
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_RPY2Quaternion(unsigned int boxID, unsigned int rbtID, double dRx, double dRy, double dRz, double& dQuaW, double& dQuaX, double& dQuaY, double& dQuaZ);
/**
 *	@brief:逆解，由指定用户坐标系位置和工具坐标系下的迪卡尔坐标计算对应的关节坐标位置
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dCoord : 指定用户坐标系和工具坐标系下的迪卡尔坐标位置(需要逆解的迪卡尔坐标位置)
 *	@param dTcp : PCS所对应的工具坐标
 *	@param dUcs : PCS所对应的用户坐标
 *	@param dJ1 : 参考关节坐标，对于多个解用于选取接近的关节坐标位置
 *	@param dTargetJ1 : 逆解
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_GetInverseKin(unsigned int boxID, unsigned int rbtID, double dCoord_X, double dCoord_Y, double dCoord_Z, double dCoord_Rx, double dCoord_Ry, double dCoord_Rz,
                 double dTcp_X, double dTcp_Y, double dTcp_Z, double dTcp_Rx, double dTcp_Ry, double dTcp_Rz,
                 double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz,
                 double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,
                 double &dTargetJ1, double &dTargetJ2, double &dTargetJ3, double &dTargetJ4, double &dTargetJ5, double &dTargetJ6);

/**
 *	@brief:正解，由关节坐标位置计算指定用户坐标系和工具坐标系下的迪卡尔坐标位置
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dJ1 : 需要计算的关节坐标位置
 *	@param dTcp : Target所对应的工具坐标
 *	@param dUcs : Target所对应的用户坐标
 *	@param dTarget : 指定用户坐标系和工具坐标系下的迪卡尔坐标位置
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */				 
HANSROBOT_PRO_API int HRIF_GetForwardKin(unsigned int boxID, unsigned int rbtID, double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,
                 double dTcp_X, double dTcp_Y, double dTcp_Z, double dTcp_Rx, double dTcp_Ry, double dTcp_Rz,
                 double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz,
                 double &dTarget_X, double &dTarget_Y, double &dTarget_Z, double &dTarget_Rx, double &dTarget_Ry, double &dTarget_Rz);

/**
 *	@brief:由基座坐标系下的坐标位置计算指定用户坐标系和工具坐标系下的迪卡尔坐标位置
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dCoord : 基座坐标系下的迪卡尔坐标位置
 *	@param dTcp : dTarget所对应的工具坐标
 *	@param dUcs : dTarget所对应的用户坐标
 *	@param dTarget : 指定用户坐标系和工具坐标系下的迪卡尔坐标
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */				
HANSROBOT_PRO_API int HRIF_Base2UcsTcp(unsigned int boxID, unsigned int rbtID, double dCoord_X, double dCoord_Y, double dCoord_Z, double dCoord_Rx, double dCoord_Ry, double dCoord_Rz,
                     double dTcp_X, double dTcp_Y, double dTcp_Z, double dTcp_Rx, double dTcp_Ry, double dTcp_Rz,
                     double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz,
                     double &dTarget_X, double &dTarget_Y, double &dTarget_Z, double &dTarget_Rx, double &dTarget_Ry, double &dTarget_Rz);
					 
/**
 *	@brief:由指定用户坐标系和工具坐标系下的迪卡尔坐标位置计算基座坐标系下的坐标位置
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dCoord : 指定用户坐标系和工具坐标系下的迪卡尔坐标
 *	@param dTcp : dCoord所对应的工具坐标
 *	@param dUcs : dCoord所对应的用户坐标
 *	@param dTarget : 基座坐标系下的迪卡尔坐标位置
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */		
HANSROBOT_PRO_API int HRIF_UcsTcp2Base(unsigned int boxID, unsigned int rbtID, double dCoord_X, double dCoord_Y, double dCoord_Z, double dCoord_Rx, double dCoord_Ry, double dCoord_Rz,
                     double dTcp_X, double dTcp_Y, double dTcp_Z, double dTcp_Rx, double dTcp_Ry, double dTcp_Rz,
                     double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz,
                     double &dTarget_X, double &dTarget_Y, double &dTarget_Z, double &dTarget_Rx, double &dTarget_Ry, double &dTarget_Rz);

/**
 *	@brief: 点位加法计算
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dPose1_X-dPose1_Rz : 坐标1
 *	@param dPose2_X-dPose2_Rz : 坐标2
 *	@param dPose3_Y : 计算结果
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_PoseAdd(unsigned int boxID, unsigned int rbtID, double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,
                    double &dPose3_X,double &dPose3_Y,double &dPose3_Z,double &dPose3_Rx,double &dPose3_Ry,double &dPose3_Rz);

/**
 *	@brief: 点位减法计算
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dPose1_X-dPose1_Rz : 坐标1
 *	@param dPose2_X-dPose2_Rz : 坐标2
 *	@param dPose3_Y : 计算结果
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	                  
HANSROBOT_PRO_API int HRIF_PoseSub(unsigned int boxID, unsigned int rbtID, double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,
                    double &dPose3_X,double &dPose3_Y,double &dPose3_Z,double &dPose3_Rx,double &dPose3_Ry,double &dPose3_Rz);

/**
 *	@brief: 坐标变换,组合运算HRIF_PoseTrans(p1,HRIF_PoseInverse(p2))，得到的就是基坐标系下的p1,在用户坐标系p2下的位置
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dPose1_X-dPose1_Rz : 坐标1
 *	@param dPose2_X-dPose2_Rz : 坐标2
 *	@param dPose3_Y : 计算结果
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	                       
HANSROBOT_PRO_API int HRIF_PoseTrans(unsigned int boxID, unsigned int rbtID, double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,
                    double &dPose3_X,double &dPose3_Y,double &dPose3_Z,double &dPose3_Rx,double &dPose3_Ry,double &dPose3_Rz);

/**
 *	@brief: 坐标逆变换,组合运算HRIF_PoseTrans(p1,HRIF_PoseInverse(p2))，得到的就是基坐标系下的p1,在用户坐标系p2下的位置
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dPose1_X-dPose1_Rz : 坐标1
 *	@param dPose3_Y : 计算结果
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	                       
HANSROBOT_PRO_API int HRIF_PoseInverse(unsigned int boxID, unsigned int rbtID, double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double &dPose3_X,double &dPose3_Y,double &dPose3_Z,double &dPose3_Rx,double &dPose3_Ry,double &dPose3_Rz);

/**
 *	@brief: 计算点位距离
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dPose1_X-dPose1_Rz : 坐标1
 *	@param dPose2_X-dPose2_Rz : 坐标2
 *	@param dDistance : 点位距离，单位[mm]
 *	@param dAngle : 姿态距离，单位[°]
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	                       
HANSROBOT_PRO_API int HRIF_PoseDist(unsigned int boxID, unsigned int rbtID, double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,
                    double &dDistance,double &dAngle);


/**
 *	@brief: 空间位置直线插补计算
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dPose1_X-dPose1_Rz : 坐标1
 *	@param dPose2_X-dPose2_Rz : 坐标2
 *	@param dAlpha : 插补比例
 *	@param dPose3_Y : 计算结果
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	                      
HANSROBOT_PRO_API int HRIF_PoseInterpolate(unsigned int boxID, unsigned int rbtID, double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose1_Rx, double dPose1_Ry, double dPose1_Rz,
                    double dPose2_X,double dPose2_Y,double dPose2_Z,double dPose2_Rx,double dPose2_Ry,double dPose2_Rz,double dAlpha,
                    double &dPose3_X,double &dPose3_Y,double &dPose3_Z,double &dPose3_Rx,double &dPose3_Ry,double &dPose3_Rz);

/**
 *	@brief: 轨迹中心旋转计算，p1,p2,p3为旋转前轨迹的特征点，p4,p5,p6为旋转后的轨迹的特征点
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dPose1_X-dPose1_Z : 坐标1
 *	@param dPose2_X-dPose2_Z : 坐标2
 *	@param dPose3_X-dPose3_Z : 坐标1
 *	@param dPose4_X-dPose4_Z : 坐标2
 *	@param dPose5_X-dPose5_Z : 坐标1
 *	@param dPose6_X-dPose6_Z : 坐标2
 *	@param dUcs_X-dUcs_Z : 计算结果
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	  
HANSROBOT_PRO_API int HRIF_PoseDefdFrame(unsigned int boxID, unsigned int rbtID, double dPose1_X, double dPose1_Y, double dPose1_Z, double dPose2_X, double dPose2_Y, double dPose2_Z,
                    double dPose3_X,double dPose3_Y,double dPose3_Z,double dPose4_X,double dPose4_Y,double dPose4_Z,
                    double dPose5_X,double dPose5_Y,double dPose5_Z,double dPose6_X,double dPose6_Y,double dPose6_Z,
                    double &dUcs_X,double &dUcs_Y,double &dUcs_Z,double &dUcs_Rx,double &dUcs_Ry,double &dUcs_Rz);

									 
//////////////////////////////////////////////////////////////////////////////////////
/**
 *	@brief:设置当前工具坐标
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dTcp_x-dTcp_Rz : 需设置的工具坐标值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetTCP(unsigned int boxID, unsigned int rbtID, double dTcp_X, double dTcp_Y, double dTcp_Z, double dTcp_Rx, double dTcp_Ry, double dTcp_Rz);

/**
 *	@brief:设置当前用户坐标
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dUcs_x-dUcs_Rz : 需设置的用户坐标值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetUCS(unsigned int boxID, unsigned int rbtID, double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz);

/**
 *	@brief:读取当前工具坐标
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dTcp_x-dTco_Rz : 读取的工具坐标值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCurTCP(unsigned int boxID, unsigned int rbtID, double& dTcp_X, double& dTcp_Y, double& dTcp_Z, double& dTcp_Rx, double& dTcp_Ry, double& dTcp_Rz);

/**
 *	@brief:读取当前用户坐标
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dUcs_x-dUcs_Rz : 读取的用户坐标值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadCurUCS(unsigned int boxID, unsigned int rbtID, double& dUcs_X, double& dUcs_Y, double& dUcs_Z, double& dUcs_Rx, double& dUcs_Ry, double& dUcs_Rz);

/**
 *	@brief:通过名称设置TCP列表中的值为当前TCP坐标
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param sTcpName : TCP坐标名称
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetTCPByName(unsigned int boxID, unsigned int rbtID, string sTcpName);

/**
 *	@brief:通过名称设置用户坐标列表中的值为当前用户坐标
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param sUcsName : UCS坐标名称
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetUCSByName(unsigned int boxID, unsigned int rbtID, string sUcsName);

/**
 *	@brief:通过名称读取指定TCP坐标
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param sName : TCP名称，和示教器页面的TCP对应
 *	@param dX-dRz : 读取到的对应TCP值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
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
 *	@brief:通过名称读取指定UCS坐标
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param sName : UCS名称，和示教器页面的UCS对应
 *	@param dX-dRz : 读取到的对应UCS值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
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
 *	@brief:配置已示教的指定名称的TCP坐标值
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param sTcpName : 已示教的指定的TCP名称
 *	@param x-rz : 需要修改的TCP值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
 //int HRIF_ConfigTCP(unsigned int boxID, string sTcpName,double dX, double dY, double dZ, double dRx, double dRy, double dRz);



HANSROBOT_PRO_API int HRIF_GetForceParams(unsigned int boxID, unsigned int rbtID, double* dInertia, double* dDamping, double* dStiffness);

/**
 *	@brief:设置力控参数
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dInertia : 质量参数(size=6)
 *	@param dDamping : 阻尼参数(size=6)
 *	@param dStiffness : 刚度参数(size=6)
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceParams(unsigned int boxID, unsigned int rbtID, double* dInertia, double* dDamping, double* dStiffness);

/**
 *	@brief:设置主动探寻参数
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dForceLimit : 探寻最大力限制(size=6)
 *	@param dDistLimit : 探寻最大距离限制(size=6)
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceControlParams(unsigned int boxID, unsigned int rbtID, double* dForceLimit, double* dDistLimit);

/**
 *	@brief:设置开启或者关闭力控模式
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dEnbaleFlag : 开启(TRUE)/关闭(FALSE)
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceMode(unsigned int boxID, unsigned int rbtID, bool dEnbaleFlag);


 
//////////////////////////////////////////////////////////////////////////////////////
/**
 *	@brief:设置力控状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nState : 0:关闭力控运动/1:开启力控运动
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceControlState(unsigned int boxID, unsigned int rbtID, int nState);
/**
 *	@brief:读取力控状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nState : 力控状态 0:关闭状态,1:力控探寻状态,2:力控探寻完成状态,3:力控自由驱动状态
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadForceControlState(unsigned int boxID, unsigned int rbtID, int& nState);
/**
 *	@brief:设置力传感器方向为tool坐标方向
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nState : 0(力传感器方向不为tool坐标方向)/1(力传感器方向为tool坐标方向)
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceToolCoordinateMotion(unsigned int boxID, unsigned int rbtID, int nMode);
/**
 *	@brief:暂停力控运动，仅暂停力控功能，不暂停运动和脚本
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ForceControlInterrupt(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief:继续力控运动，仅继续力控运动功能，不继续运动和脚本
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ForceControlContinue(unsigned int boxID, unsigned int rbtID);
/**
 *	@brief:清零力传感器
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceZero(unsigned int boxID, unsigned int rbtID);
/**
 *	@brief:设置力控探寻的最大速度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dMaxLinearVelocity : 直线探寻最大速度
 *	@param dMaxAngularVelocity : 姿态探寻最大速度
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetMaxSearchVelocities(unsigned int boxID, unsigned int rbtID, double dMaxLinearVelocity, double dMaxAngularVelocity);

/**
 *	@brief:设置力控探寻自由度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nX-nRz : 各方向力控自由度状态 0：关闭  1：开启
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetControlFreedom(unsigned int boxID, unsigned int rbtID, int nX, int nY, int nZ, int nRx, int nRy, int nRz);

/**
 *	@brief:设置控制策略
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nState : 控制策略 0：柔顺模式 1：恒力模式
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceControlStrategy(unsigned int boxID, unsigned int rbtID, int nState);
/**
 *	@brief:设置力传感器中心相对于法兰盘的安装位置和姿态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dX-dRz : 力传感器中心相对于法兰盘的安装位置和姿态
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetFreeDrivePositionAndOrientation(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz);
/**
 *	@brief:设置力控探寻PID参数
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dFp-dTd : PID参数
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetPIDControlParams(unsigned int boxID, unsigned int rbtID, double dFp, double dFi, double dFd, double dTp, double dTi, double dTd);
/**
 *	@brief:设置惯量控制参数
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dX-dRz : 惯量控制参数
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetMassParams(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz);
/**
 *	@brief:设置阻尼(b)控制参数
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dX-dRz : 阻尼(b)控制参数
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetDampParams(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz);
/**
 *	@brief:设置刚度(k)控制参数
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dX-dRz : 刚度(k)控制参数
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetStiffParams(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz);
/**
 *	@brief:设置力控控制目标值
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param X-Rz: 对应目标力
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceControlGoal(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRX, double dRY, double dRZ);
/**
 *	@brief:设置力控探寻目标力大小
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dWrench(dX-dRz) : 各方向探寻目标力
 *	@param dDistance(dX-dRz) : 各方向探寻距离(暂未启用，可全部设置为0)
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetControlGoal(unsigned int boxID, unsigned int rbtID, double dWrench_X, double dWrench_Y, double dWrench_Z, double dWrench_Rx, double dWrench_Ry, double dWrench_Rz,
                        double dDistance_X,double dDistance_Y, double dDistance_Z,double dDistance_Rx,double dDistance_Ry, double dDistance_Rz);

/**
 *	@brief:设置力控限制范围-力传感器超过此范围后控制器断电
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dMax(dX-dRz) : 各方向传感器限制最大值
 *	@param dMin(dX-dRz) : 各方向传感器限制最小值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_SetForceDataLimit(unsigned int boxID, unsigned int rbtID, double dMax_X, double dMax_Y, double dMax_Z, double dMax_Rx, double dMax_Ry, double dMax_Rz,
                            double dMin_X,double dMin_Y, double dMin_Z,double dMin_Rx,double dMin_Ry, double dMin_Rz);
/**
 *	@brief:设置力控形变范围
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dAllowDistance : 允许的探寻距离
 *	@param dStrengthLevel : 位置与边界设置偏离距离的幂次项，设成2，就表示阻力与偏离边界的平方项成比例；设成3，就表示阻力与偏离边界的立方项成比例
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceDistanceLimit(unsigned int boxID, unsigned int rbtID, double dAllowDistance, double dStrengthLevel);

/**
 *	@brief:设置开启或者关闭力控自由驱动模式
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nEnableFlag : 0(关闭)/1(开启)
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetForceFreeDriveMode(unsigned int boxID, unsigned int rbtID, bool bEnable);

/**
 *	@brief:读取末端力传感器值(标定后)
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dX-dRz : 需读取到的对应力传感器值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadFTCabData(unsigned int boxID, unsigned int rbtID, double& dX, double& dY, double& dZ, double& dRX, double& dRY, double& dRZ);


//HANSROBOT_PRO_API bool HRIF_ReadTCPInCorrectValue(unsigned int boxID, unsigned int rbtID);

//************************************************************************/
//**    运动类函数接口                                                             
//************************************************************************/
/**
 *	@brief:关节短点动 运动距离2°，最大速度<10°/s
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nAxisId : 需要运动的关节索引0-5
 *	@param nDerection : 需要运动的关节运动方向：0(负方向)/1(正方向)
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ShortJogJ(unsigned int boxID, unsigned int rbtID, int nAxisId, int nDerection);

/**
 *	@brief:迪卡尔坐标短点动 运动距离2mm，最大速度<10mm/s
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nAxisId : 需要运动的关节索引0-5
 *	@param nDerection : 需要运动的关节运动方向：0(负方向)/1(正方向)
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ShortJogL(unsigned int boxID, unsigned int rbtID, int nAxisId, int nDerection);

/**
 *	@brief:关节长点动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nAxisId : 指定运动的关节索引0-5
 *	@param nDerection : 指定运动的关节方向：0(负方向)/1(正方向)
 *	@param nState : 0(关闭)/1(开启)
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_LongJogJ(unsigned int boxID, unsigned int rbtID, int nAxisId, int nDerection, int nState);

/**
 *	@brief:迪卡尔坐标长点动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nAxisId : 指定运动的迪卡尔坐标索引0-5
 *	@param nDerection : 指定运动的迪卡尔坐标方向：0(负方向)/1(正方向)
 *	@param nState : 0(关闭)/1(开启)
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_LongJogL(unsigned int boxID, unsigned int rbtID, int nAxisId, int nDerection, int nState);

/**
 *	@brief:长点动继续指令，当开始长点动之后，要按 500 毫秒或更短时间为时间周期发送一次该指令，否则长点动会停止
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_LongMoveEvent(unsigned int boxID, unsigned int rbtID);

//////////////////////////////////////////////////////////////////////////////////////

/**
 *	@brief:判断运动是否停止
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param bDone : true: 完成；false: 未完成
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码，此时bDone的取值无意义
 */
HANSROBOT_PRO_API int HRIF_IsMotionDone(unsigned int boxID, unsigned int rbtID, bool& bDone);

/**
 *	@brief:判断路点是否运行完成
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param bDone : true: 完成；false: 未完成；
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码，此时bDone的取值无意义
 */
HANSROBOT_PRO_API int HRIF_IsBlendingDone(unsigned int boxID, unsigned int rbtID, bool& bDone);

/**
 *	@brief:执行路点运动-建议直接使用HRIF_MoveJ和HRIF_MoveL
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nMoveType : 运动类型
			enum EN_MoveType
			{
				enMoveType_MoveJ=0,
				enMoveType_MoveL,
			};
 *	@param dX-dRz : 目标迪卡尔位置-当nIsUseJoint=1时无效，nIsUseJoint=0时，用此迪卡尔坐标作为目标位置，通过逆解计算得到关节坐标为目标关节坐标
 *	@param dJ1-dJ6 : 目标关节位置-当nIsUseJoint=1时，使用此关节坐标作为目标关节坐标，nIsUseJoint=0时，此关节坐标仅作为计算逆解时选解的参考关节坐标
 *	@param dTcp_X-dTcp_Rz : 目标迪卡尔坐标所处的工具坐标系名称，与示教器页面的名称对应，当nIsUseJoint=1时无效，可设置为0
 *	@param dUcs_X-dUcs_Rz : 目标迪卡尔坐标所处的用户坐标系名称，与示教器页面的名称对应-当nIsUseJoint=1时无效，可设置为0：
 *	@param dVelocity : 运动速度，速度单位是毫米每秒，度每秒，加速度毫米每秒平方，度每秒平方
 *	@param dAcc : 运动加速度，速度单位是毫米每秒，度每秒，加速度毫米每秒平方，度每秒平方
 *	@param dRadius : 是过渡半径，单位毫米
 *	@param nIsJoint : 是否使用关节角度作为目标点，如果nMoveType＝＝enMoveType_MoveJ，则nisJoint有起作用
 *	@param nIsSeek,nIOBit,nIOState：探寻参数，当nisSeek为1，则开启探寻，这时电箱的DO nIOBit位为nIOState时，就停止运动，否则运动到目标点再停止
 *	@param strCmdID：当前路点ID，可以自定义，也可以按顺序设置为“1”，“2”，“3”
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_WayPointEx(unsigned int boxID, unsigned int rbtID, int nMoveType,
                    double dX, double dY, double dZ, double dRx, double dRy, double dRz,                        
                    double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,                      
                    double dTcp_X, double dTcp_Y, double dTcp_Z, double dTcp_Rx, double dTcp_Ry, double dTcp_Rz,
                    double dUcs_X, double dUcs_Y, double dUcs_Z, double dUcs_Rx, double dUcs_Ry, double dUcs_Rz,
                    double dVelocity, double dAcc, double dRadius, int nIsJoint, int nIsSeek, int nIOBit, int nIOState, string strCmdID);
/**
 *	@brief:执行路点运动-建议直接使用HRIF_MoveJ和HRIF_MoveL
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nMoveType : 运动类型
			enum EN_MoveType
			{
				enMoveType_MoveJ=0,
				enMoveType_MoveL,
			};
 *	@param dX-dRz : 目标迪卡尔位置-当nIsUseJoint=1时无效，nIsUseJoint=0时，用此迪卡尔坐标作为目标位置，通过逆解计算得到关节坐标为目标关节坐标
 *	@param dJ1-dJ6 : 目标关节位置-当nIsUseJoint=1时，使用此关节坐标作为目标关节坐标，nIsUseJoint=0时，此关节坐标仅作为计算逆解时选解的参考关节坐标
 *	@param sTcpName : 目标迪卡尔坐标所处的工具坐标系名称，与示教器页面的名称对应，当nIsUseJoint=1时无效，可使用默认名称”TCP”
 *	@param sUcsName : 目标迪卡尔坐标所处的用户坐标系名称，与示教器页面的名称对应-当nIsUseJoint=1时无效，可使用默认名称”Base”
 *	@param dVelocity : 运动速度，速度单位是毫米每秒，度每秒，加速度毫米每秒平方，度每秒平方
 *	@param dAcc : 运动加速度，速度单位是毫米每秒，度每秒，加速度毫米每秒平方，度每秒平方
 *	@param dRadius : 是过渡半径，单位毫米
 *	@param nIsUseJoint : 是否使用关节角度作为目标点，如果nMoveType＝＝enMoveType_MoveJ，则nisJoint有起作用
 *	@param nIsSeek,当nisSeek为1，则开启探寻，这时电箱的nIOBit索引的DI的状态为nIOState满足时，就停止运动，否则运动到目标点再停止运动
 *	@param strCmdID：当前路点ID，可以自定义，也可以按顺序设置为“1”，“2”，“3”
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_WayPoint(unsigned int boxID, unsigned int rbtID, int nMoveType,
                double dX, double dY, double dZ, double dRx, double dRy, double dRz,                         
                double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,
                string sTcpName, string sUcsName, double dVelocity, double dAcc, double dRadius, 
                int nIsUseJoint,int nIsSeek, int nIOBit, int nIOState, string strCmdID);

/**
 *	@brief:执行路点运动-建议直接使用HRIF_MoveJ和HRIF_MoveL
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nMoveType : 运动类型,0:
 *	@param dEndPos_X-dEndPos_Rz : 目标迪卡尔位置-当nIsUseJoint=1时无效，nIsUseJoint=0时，用此迪卡尔坐标作为目标位置，通过逆解计算得到关节坐标为目标关节坐标
*	@param dAuxPos_X-dAuxPos_Rz :                                  
 *	@param dJ1-dJ6 : 目标关节位置-当nIsUseJoint=1时，使用此关节坐标作为目标关节坐标，nIsUseJoint=0时，此关节坐标仅作为计算逆解时选解的参考关节坐标
 *	@param sTcpName : 目标迪卡尔坐标所处的工具坐标系名称，与示教器页面的名称对应，当nIsUseJoint=1时无效，可使用默认名称”TCP”
 *	@param sUcsName : 目标迪卡尔坐标所处的用户坐标系名称，与示教器页面的名称对应-当nIsUseJoint=1时无效，可使用默认名称”Base”
 *	@param dVelocity : 运动速度，速度单位是毫米每秒，度每秒，加速度毫米每秒平方，度每秒平方
 *	@param dAcc : 运动加速度，速度单位是毫米每秒，度每秒，加速度毫米每秒平方，度每秒平方
 *	@param dRadius : 是过渡半径，单位毫米
 *	@param nIsUseJoint : 是否使用关节角度作为目标点，如果nMoveType＝＝enMoveType_MoveJ，则nisJoint有起作用
 *	@param nIsSeek,当nisSeek为1，则开启探寻，这时电箱的nIOBit索引的DI的状态为nIOState满足时，就停止运动，否则运动到目标点再停止运动
 *	@param strCmdID：当前路点ID，可以自定义，也可以按顺序设置为“1”，“2”，“3”
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_WayPoint2(unsigned int boxID, unsigned int rbtID, int nMoveType,
                double dEndPos_X, double dEndPos_Y, double dEndPos_Z, double dEndPos_Rx, double dEndPos_Ry, double dEndPos_Rz,   
                double dAuxPos_X, double dAuxPos_Y, double dAuxPos_Z, double dAuxPos_Rx, double dAuxPos_Ry, double dAuxPos_Rz,                       
                double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,
                string sTcpName, string sUcsName, double dVelocity, double dAcc, double dRadius, 
                int nIsUseJoint,int nIsSeek, int nIOBit, int nIOState, string strCmdID);


/**
 *	@brief:机器人运动到指定的角度坐标位置
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dX-dRz : 目标迪卡尔位置-当nIsUseJoint=1时无效，nIsUseJoint=0时，用此迪卡尔坐标作为目标位置，通过逆解计算得到关节坐标为目标关节坐标
 *	@param dJ1-dJ6 : 目标关节位置-当nIsUseJoint=1时，使用此关节坐标作为目标关节坐标，nIsUseJoint=0时，此关节坐标仅作为计算逆解时选解的参考关节坐标
 *	@param sTcpName : 目标迪卡尔坐标所处的工具坐标系名称，与示教器页面的名称对应-当nIsUseJoint=1时无效
 *	@param sUcsName : 目标迪卡尔坐标所处的用户坐标系名称，与示教器页面的名称对应-当nIsUseJoint=1时无效
 *	@param dVelocity : 运动速度，速度单位是毫米每秒，度每秒，加速度毫米每秒平方，度每秒平方
 *	@param dAcc : 运动加速度，速度单位是毫米每秒，度每秒，加速度毫米每秒平方，度每秒平方
 *	@param dRadius : 是过渡半径，单位毫米
 *	@param nIsUseJoint : 是否使用关节角度作为目标点，则nIsUseJoint有起作用
 *	@param nIsSeek,nIOBit,nIOState：探寻参数，当nisSeek为1，则开启探寻，这时电箱的DO nIOBit位为nIOState时，就停止运动，否则运动到目标点再停止
 *	@param strCmdID：当前路点ID，可以自定义，也可以按顺序设置为“1”，“2”，“3”
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_MoveJ(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz,
                double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,
                string sTcpName, string sUcsName, double dVelocity, double dAcc, double dRadius, 
                int nIsUseJoint,int nIsSeek, int nIOBit, int nIOState, string strCmdID);

/**
 *	@brief:机器人直线运动到指定的空间坐标位置
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dX-dRz : 目标迪卡尔位置
 *	@param dJ1-dJ6 : 关节位置-建议使用目标迪卡尔坐标附近的关节坐标值。
 *	@param sTcpName : 目标迪卡尔坐标所处的工具坐标系名称
 *	@param sUcsName : 目标迪卡尔坐标所处的用户坐标系名称
 *	@param dVelocity : 运动速度，速度单位是毫米每秒，度每秒，加速度毫米每秒平方，度每秒平方
 *	@param dAcc : 运动加速度，速度单位是毫米每秒，度每秒，加速度毫米每秒平方，度每秒平方
 *	@param dRadius : 是过渡半径，单位毫米
 *	@param nIsSeek,nIOBit,nIOState：探寻参数，当nisSeek为1，则开启探寻，这时电箱的DO nIOBit位为nIOState时，就停止运动，否则运动到目标点再停止
 *	@param strCmdID：当前路点ID，可以自定义，也可以按顺序设置为“1”，“2”，“3”
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_MoveL(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz,
                double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6,
                string sTcpName, string sUcsName, double dVelocity, double dAcc, double dRadius,
                int nIsSeek, int nIOBit, int nIOState, string strCmdID);

/**
 *	@brief: 圆弧轨迹运动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dStartPos : 圆弧开始位置
 *	@param dAuxPos : 圆弧经过位置
 *	@param dEndPos : 圆弧结束位置
 *	@param nFixedPosure : 0:不使用固定姿态，1:使用固定姿态
 *	@param nMoveCType : 0:圆弧轨迹，1:整圆轨迹
 *	@param dRadLen : 当nMoveCType=0时参数无效，由三个点确定圆弧轨迹
 *			         当nMoveCType=1时，参数为整圆的圈数,通过三个点位确定圆弧路径，当使用整圆运动时表示整圆的圈数，小数部分无效。
 *	@param dVelocity : 速度
 *	@param dAcc : 加速度
 *	@param nRadius : 过渡半径
 *	@param sTcpName : 刀具坐标名称
 *	@param sUcsName : 用户坐标名称
 *	@param sCmdID : 命令ID
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_MoveC(unsigned int boxID, unsigned int rbtID,
                double dStartPos_X,double dStartPos_Y,double dStartPos_Z,double dStartPos_Rx,double dStartPos_Ry,double dStartPos_Rz,
                double dAuxPos_X,double dAuxPos_Y,double dAuxPos_Z,double dAuxPos_Rx,double dAuxPos_Ry,double dAuxPos_Rz,
                double dEndPos_X,double dEndPos_Y,double dEndPos_Z,double dEndPos_Rx,double dEndPos_Ry,double dEndPos_Rz,
                int nFixedPosure,int nMoveCType,double dRadLen,double dVelocity, double dAcc, 
                double ddRadius, string sTcpName, string sUcsName, string sCmdID);

/**
 *	@brief: Z型轨迹运动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dStartPos : 开始位置
 *	@param dEndPos : 结束位置
 *	@param dPlanePos : 确定平面点位置
 *	@param dVelocity : 速度
 *	@param dAcc : 加速度
 *	@param dWIdth : 宽度
 *	@param dDensity : 密度
 *	@param nEnableDensity : 是否使用密度(0:不使用，1:使用)
 *	@param nEnablePlane : 是否使用平面点，不使用时根据选择的用户坐标确定XYZ平面
 *	@param nEnableWaiTime : 是否开启转折点等待时间(0:不使用，1:使用)
 *	@param nPosiTime : 正向转折点等待时间ms
 *	@param nNegaTime : 负向转折点等待时间ms
 *	@param dRadius : 过渡半径
 *	@param sTcpName : 刀具坐标名称
 *	@param sUcsName : 用户坐标名称
 *	@param sCmdID : 命令ID
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_MoveZ(unsigned int boxID, unsigned int rbtID,
                double dStartPos_X,double dStartPos_Y,double dStartPos_Z,double dStartPos_Rx,double dStartPos_Ry,double dStartPos_Rz,
                double dEndPos_X,double dEndPos_Y,double dEndPos_Z,double dEndPos_Rx,double dEndPos_Ry,double dEndPos_Rz,
                double dPlanePos_X,double dPlanePos_Y,double dPlanePos_Z,double dPlanePos_Rx,double dPlanePos_Ry,double dPlanePos_Rz,
                double dVelocity, double dAcc, double dWIdth, double dDensity, int nEnableDensity, int nEnablePlane, int nEnableWaiTime, 
                int nPosiTime, int nNegaTime, double dRadius, string sTcpName, string sUcsName, string sCmdID);

/**
 *	@brief: 椭圆轨迹运动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dP1 : 示教点1,示教点位必须按照轨迹顺序排列
 *	@param dP2 : 示教点2
 *	@param dP3 : 示教点3
 *	@param dP4 : 示教点4
 *	@param dP5 : 示教点5
 *	@param dP6 : 示教点6
 *	@param nOrientMode : 弧运动类型：0：圆弧，1：整圆
 *	@param nMoveType : 0:不使用固定姿态，1:使用固定姿态
 *	@param dArcLength : 弧长
 *	@param dVelocity : 速度
 *	@param dAcc : 加速度
 *	@param dRadius : 过渡半径
 *	@param sTcpName : 刀具坐标名称
 *	@param sUcsName : 用户坐标名称
 *	@param sCmdID : 命令ID
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
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
 *	@brief: 螺旋运动轨迹运动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dSpiralIncrement : 螺旋运动每圈增量半径
 *	@param dSpiralDiameter : 螺旋运动结束半径
 *	@param dVelocity : 速度
 *	@param dAcc : 加速度
 *	@param dRadius : 过渡半径
 *	@param sUcsName : 用户坐标名称
 *	@param sCmdID : 命令ID
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_MoveS(unsigned int boxID, unsigned int rbtID, double dSpiralIncrement, double dSpiralDiameter,
                double dVelocity, double dAcc, double dRadius, string sTCPName, string sUcsName, string sCmdID);

/**
 *	@brief:关节相对运动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nAxisId: 关节编号
 *	@param nDirection: 运动方向，1为正方向，0为负方向
 *	@param dDistance: 运动距离(°)
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_MoveRelJ(unsigned int boxID, unsigned int rbtID, int nAxisId, int nDirection, double dDistance);

/**
 *	@brief:空间相对运动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nAxisId: 关节编号
 *	@param nDirection: 运动方向，1为正方向，0为负方向
 *	@param dDistance: 运动距离(mm)
 *  @param nToolMotion : 运动座标类型，0为按当前用户坐标系，1为当前Tool坐标系
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_MoveRelL(unsigned int boxID, unsigned int rbtID, int nAxisId, int nDirection, double dDistance, int nToolMotion);

/**
 *	@brief:路点相对运动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nType: 运动类型，0为关节运动，1为直线运动
 *  @param nPointList: 是否使用点位列表的点位，0为不使用，1为使用
 *  @param dPos_X/dPos_Y/dPos_Z/dPos_Rx/dPos_Ry/dPos_Rz: 空间目标位置
 *  @param dPos_J1/dPos_J2/dPos_J3/dPos_J4/dPos_J5/dPos_J6: 关节目标位置
 *  @param nrelMoveType: 相对运动类型，0为绝对值，1为叠加值
 *  @param nAxisMask_1/nAxisMask_2/nAxisMask_3/nAxisMask_4/nAxisMask_5/nAxisMask_6: 各轴\各方向是否运动，0为不运动，1为运动
 *  @param dTarget_1/dTarget_2/dTarget_3/dTarget_4/dTarget_5/dTarget_6: 运动距离，nType=0 并 nAxisMask=1：该方向运动绝对距离或叠加距离；nType=1 并 nAxisMask=1：该轴运动绝对距离或叠加距离；nAxisMask=0：本参数无效
 *  @param sTcpName: 工具坐标名称，目标空间坐标所处的工具坐标系名称，与示教器页面的名称对应，当 nIsUseJoint=1 时无效，可使用默认名称 “TCP”
 *  @param sUcsName: 用户坐标名称，目标空间坐标所处的用户坐标系名称，与示教器页面的名称对应，当 nIsUseJoint=1 时无效，可使用默认名称 “Base”
 *  @param dVelocity: 运动速度，关节运动时单位[°/s]，空间运动时 X，Y，Z 单位[mm/s]，Rx，Ry，Rz 单位[°/s]
 *  @param dAcc: 运动加速度，关节运动时单位[°/s2]，空间运动时 X，Y，Z 单位[mm/s2]，Rx，Ry，Rz 单位[°/s2]
 *  @param dRadius: 过渡半径（mm）
 *  @param nIsUseJoint: 是否使用关节角度，是否使用关节角度作为目标点，如果nMoveType=0 时，则 nIsUseJoint 有效：0：不使用关节角度1：使用关节角度
 *  @param nIsSeek: 是否使用检测 DI 停止，如果 nIsSeek 为 1，则开启检测 DI 停止，路点运动过程中如果电箱的 nIOBit 位索引的 DI 的状态=nIOState 时，机器人停止运动，否则运动到目标点完成运动
 *  @param nIOBit: 电箱对应 DI 索引，nIsSeek=0 时无效
 *  @param nIOState: 检测的 DI 状态，，nIsSeek=0 时无效
 *  @param strCmdID: 当前路点 ID，可以自定义，也可以按顺序设置为“1”,“2”,“3”.
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
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

/**
 *	@brief:初始化关节连续轨迹运动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param sTrackName : 轨迹名称
 *	@param dSpeedRatio : 轨迹运动速度比
 *	@param dRadius : 过渡半径,单位[mm]
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_StartPushMovePathJ(unsigned int boxID, unsigned int rbtID, string sTrackName, double dSpeedRatio, double dRadius);

/**
 *	@brief:下发运动轨迹
调用HRIF_StartPushMovePath，可多次调用此函数，一般情况下点位数量需要>4
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param sTrackName : 轨迹名称
 *	@param J1-J6 : 关节位置
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PushMovePathJ(unsigned int boxID, unsigned int rbtID, string sTrackName, double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6);

/**
 *	@brief:轨迹下发完成，开始计算轨迹
调用HRIF_StartPushMovePath，可多次调用此函数，一般情况下点位数量需要>4
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param sTrackName : 轨迹名称
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_EndPushMovePathJ(unsigned int boxID, unsigned int rbtID, string sTrackName);

/**
 *	@brief:执行指定的轨迹运动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param sTrackName : 轨迹名称
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_MovePathJ(unsigned int boxID, unsigned int rbtID, string sTrackName);

/**
 *	@brief:读取当前的轨迹状态
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param sTrackName : 轨迹名称
 *	@param nResult : 当前轨迹状态
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadMovePathJState(unsigned int boxID, unsigned int rbtID, string sTrackName, int& nResult);

/**
 *	@brief:更新指定轨迹的名称
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param sTrackName : 轨迹原名称
 *	@param sTrackNewName : 需要更新的轨迹名称
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_UpdateMovePathJName(unsigned int boxID, unsigned int rbtID, string trackName, string sTrackNewName);

/**
 *	@brief:删除指定轨迹
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param sTrackName : 需要删除的轨迹名称
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_DelMovePathJ(unsigned int boxID, unsigned int rbtID, string sTrackName);

/**
 *	@brief:设置 MovePath 运动速度比
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dOverride: 速度参数 [1~100]
 *
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetMovePathOverride(unsigned int boxID, unsigned int rbtID, double dOverride);

/**
 *	@brief:读取轨迹进度
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dProcess : 当前运动进度(0-1),>0.999999表示运动完成
 *	@param nIndex : 当前轨迹运动到的点位索引
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadTrackProcess(unsigned int boxID, unsigned int rbtID, double& dProcess, int& nIndex);


/**
 *	@brief:初始化迪卡尔坐标轨迹运动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param sTrackName : 轨迹名称，目前迪卡尔坐标运动的轨迹名称没有作用可以任意定义，
						同一轨迹运行需要同执行HRIF_InitMovePathL，HRIF_PushMovePaths，HRIF_EndMovePathL
						调用HRIF_EndMovePathL后会计算完轨迹后直接开始运动，计算时间2-4s左右，根据实际轨迹大小确定
 *	@param dVelocity : 运动速度
 *	@param dAcceleration : 运动加速度
 *	@param jerk : 运动加加速度
 *	@param sUcsName : 指定轨迹所在的用户坐标系名称
 *	@param sTcpName : 指定轨迹所在的工具坐标值名称
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_InitMovePathL(unsigned int boxID, unsigned int rbtID, string sTrackName, double dVelocity, double dAceleration, double djerk, string sUcsName, string sTcpName);

/**
 *	@brief:下发轨迹点位-调用一次下发一个目标点位
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param trackName : 轨迹名称，目前迪卡尔坐标运动的轨迹名称没有作用可以任意定义，
						同一轨迹运行需要同执行HRIF_InitMovePathL，HRIF_PushMovePaths，HRIF_EndMovePathL
						调用HRIF_EndMovePathL后会计算完轨迹后直接开始运动，计算时间2-4s左右，根据实际轨迹大小确定
 *	@param dX-dRz : 目标迪卡尔坐标位置 
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PushMovePathL(unsigned int boxID, unsigned int rbtID, string sTrackName, double dX, double dY, double dZ, double dRX, double dRY, double dRZ);

/**
 *	@brief:批量下发轨迹点位-调用一次可下发多个点位数据，以,分隔
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param trackName : 轨迹名称，目前迪卡尔坐标运动的轨迹名称没有作用可以任意定义，
						同一轨迹运行需要同执行HRIF_InitMovePathL，HRIF_PushMovePaths，HRIF_EndMovePathL
						调用HRIF_EndMovePathL后会计算完轨迹后直接开始运动，计算时间2-4s左右，根据实际轨迹大小确定
 *	@param nMoveType : 运动类型-MovePathJ可以共用 0(MovePathJ)/1(MovePathL)
 *	@param nPointsSize : 轨迹点位数量
 *	@param sPoints : 轨迹点位数量
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PushMovePaths(unsigned int boxID, unsigned int rbtID, string sTrackName, int nMoveType, int nPointsSize, string sPoints);

/**
 *	@brief:执行迪卡尔坐标轨迹运动，
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param trackName : 轨迹名称，目前迪卡尔坐标运动的轨迹名称没有作用可以任意定义，
						同一轨迹运行需要同执行HRIF_InitMovePathL，HRIF_PushMovePaths，HRIF_EndMovePathL
						调用HRIF_EndMovePathL后会计算完轨迹后直接开始运动，计算时间2-4s左右，根据实际轨迹大小确定
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_MovePathL(unsigned int boxID, unsigned int rbtID, string sTrackName);


/**
 *	@brief:启动机器人在线控制（servoJ 或 servoP）时，设定位置固定更新的周期和前瞻时间
 *  * 注：HRIF_StartServo/HRIF_PushServoJ/HRIF_PushServoP为一套接口，其他servo指令为一套接口，不共用。
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dServoTime : 固定更新的周期 s
 *	@param dLookaheadTime : 前瞻时间 s
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_StartServo(unsigned int boxID, unsigned int rbtID, double dServoTime, double dLookaheadTime);

/**
 *	@brief:在线关节位置命令控制，以 StartServo 设定的固定更新时间发送关节位置，机器人将实时的跟踪关节位置指令
 * * 注：HRIF_StartServo/HRIF_PushServoJ/HRIF_PushServoP为一套接口，其他servo指令为一套接口，不共用。
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param J1-J6 : 更新的关节位置
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PushServoJ(unsigned int boxID, unsigned int rbtID, double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6);

/**
 *	@brief:在线末端TCP位置命令控制，以 StartServo 设定的固定更新时间发送 TCP 位置，机器人将实时的跟踪目标 TCP 位置逆运算转换后的关节位置指令
 *   * 注：HRIF_StartServo/HRIF_PushServoJ/HRIF_PushServoP为一套接口，其他servo指令为一套接口，不共用。
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dCoord : 更新的目标迪卡尔坐标位置
 *	@param vecUcs : 目标位置对应的UCS
 *	@param vecTcp : 目标位置对应的TCP
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PushServoP(unsigned int boxID, unsigned int rbtID, vector<double>& dCoord, vector<double>& vecUcs, vector<double>& vecTcp);

/**
 *	@brief:初始化在线控制模式，清空缓存点位
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_InitServoEsJ(unsigned int boxID, unsigned int rbtID);

/**
 *	@brief:启动在线控制模式，开始运动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dServoTime : 固定更新的周期 s
 *	@param dLookaheadTime : 前瞻时间 s
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_StartServoEsJ(unsigned int boxID, unsigned int rbtID, double dServoTime, double dLookheadTime);

/**
 *	@brief:批量下发在线控制点位
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nPointsSize: 点位数量，最大允许下发500个点位
 *	@param sPoints: 点位信息，例如："0,0,0,0,0,0,1,1,1,1,1,1,1"
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_PushServoEsJ(unsigned int boxID, unsigned int rbtID, int nPointsSize, string sPoints);

/**
 *	@brief:读取当前是否可以继续下发点位信息，循环读取间隔>20ms
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nState: 0：允许下发点位
 *			        1：不允许下发点位
 *	@param nIndex: 当前运行点位索引
 *	@param nCount: 当前点位数量
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_ReadServoEsJState(unsigned int boxID, unsigned int rbtID, int& nState, int& nIndex, int& nCount);

/**
 *	@brief:设置相对跟踪运动控制参数
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nState: 0:关闭相对跟踪，1:开启相对跟踪
 *	@param dDistance: 相对跟踪运动保持的相对距离
 *	@param dAwayVelocity: 相对跟踪的运动的远离速度
 *	@param dGobackVelocity: 相对跟踪的运动的返回速度
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetMoveTraceParams(unsigned int boxID, unsigned int rbtID, int nState, double dDistance, double dAwayVelocity, double dGobackVelocity);

/**
 *	@brief:设置相对跟踪运动初始化参数
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param dK,dB: 计算公式y = dK * x + dB
 *	@param dMaxLimit: 激光传感器检测距离最大值
 *	@param dMinLinit: 激光传感器检测距离最小值
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetMoveTraceInitParams(unsigned int boxID, unsigned int rbtID, double dK, double dB, double dMaxLimit, double dMinLinit);

/**
 *	@brief:设置相对跟踪运动的跟踪探寻方向
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param X-Rx: 跟踪探寻方向
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetMoveTraceUcs(unsigned int boxID, unsigned int rbtID, double dX, double dY, double dZ, double dRx, double dRy, double dRz);

/**
 *	@brief:设置传送带跟踪运动
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nState: 0:关闭传送带跟踪，1:开启传送带跟踪
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_SetTrackingState(unsigned int boxID, unsigned int rbtID, int nState);

/**
 *	@brief:执行插件app命令
 *	@param boxID : 电箱ID号，默认值=0
 *	@param sCmdName: 命令名称
 *	@param sParams: 参数列表
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_HRAppCmd(unsigned int boxID, string sCmdName, string sParams);

/**
 *	@brief:读末端连接的modbus从站寄存器
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nSlaveID: Modbus从站ID
 *	@param nFunction: 功能码
 *	@param nRegAddr: 起始地址
 *	@param nRegCount: 寄存器数量，最大11个
 *	@param vecData: 需要写的寄存器值，vector的大小与寄存器数量一致
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */
HANSROBOT_PRO_API int HRIF_WriteEndHoldingRegisters(unsigned int boxID, unsigned int rbtID, int nSlaveID, int nFunction, int nRegAddr, int nRegCount, vector<int> vecData);

/**
 *	@brief:读末端连接的modbus从站寄存器
 *	@param boxID : 电箱ID号，默认值=0
 *	@param rbtID : 机器人ID号，默认值=0
 *	@param nSlaveID: Modbus从站ID
 *	@param nFunction: 功能码
 *	@param nRegAddr: 起始地址
 *	@param nRegCount: 寄存器数量，最大12个
 *	@param vecData: 返回读取的寄存器值，vector的大小与寄存器数量一致
 *	
 *	@return : nRet=0 : 返回函数调用成功
 *            nRet>0 : 返回调用失败的错误码
 */	
HANSROBOT_PRO_API int HRIF_ReadEndHoldingRegisters(unsigned int boxID, unsigned int rbtID, int nSlaveID, int nFunction, int nRegAddr, int nRegCount, vector<int>& vecData);
#ifdef __cplusplus
}
#endif
#endif // !_HR_PRO_H_