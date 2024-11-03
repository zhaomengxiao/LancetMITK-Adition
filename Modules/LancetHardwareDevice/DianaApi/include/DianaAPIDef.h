/**
 * 此文件是为API接口数据结构定义的头文件
 */
#pragma once

#pragma pack(1)

#ifdef _WIN32
#include <stdint.h>
#else
#include <sys/types.h>
#endif
#include "Environment.h"

// 下列 ifdef 块是创建使从 DLL 导出更简单的宏的标准方法。
// 此 DLL 中的所有文件都是用命令行上定义的 DIANA_API_EXPORTS 符号编译的。
// 在使用此 DLL 的任何其他项目上不应定义此符号。
// 这样，源文件中包含此文件的任何其他项目都会将 DIANA_API_EXPORTS 函数视为是从 DLL 导入的，
// 而此 DLL 则将用此宏定义的符号视为是被导出的。

#if defined _WIN32
#ifdef DIANA_API_EXPORTS
#define DIANA_API extern "C" __declspec(dllexport)
#else
#define DIANA_API extern "C" __declspec(dllimport)
#endif
#else
#if __GNUC__ >= 4
#define DIANA_API extern "C" __attribute__((visibility("default")))
#else
#define DIANA_API extern "C"
#endif
#endif

// 这里是工业和医疗共有数据结构
#define _API_SUPPORT_V2_

#define MAX_SUPPORT_ROBOTARM_NUM 3

extern "C" typedef void (*FNCERRORCALLBACK)(int e, const char *strIpAddress);
extern "C" typedef void (*FNCWARNINGCALLBACK)(int e, const char *strIpAddress);

extern "C" typedef struct _SrvNetSt {
    char SrvIp[32];
    unsigned short LocHeartbeatPort;
    unsigned short LocRobotStatePort;
    unsigned short LocSrvPort;
    unsigned short LocRealtimeSrvPort;
    unsigned short LocPassThroughSrvPort;
} srv_net_st;

extern "C" typedef enum _MoveTCPDirection {
    T_MOVE_X_UP,
    T_MOVE_X_DOWN,
    T_MOVE_Y_UP,
    T_MOVE_Y_DOWN,
    T_MOVE_Z_UP,
    T_MOVE_Z_DOWN
} tcp_direction_e;

extern "C" typedef enum _MoveJointDirection {
    T_MOVE_UP = 0,
    T_MOVE_DOWN,
} joint_direction_e;

typedef enum _Mode {
    T_MODE_INVALID = -1,
    T_MODE_POSITION = 0,
    T_MODE_JOINT_IMPEDANCE,
    T_MODE_CART_IMPEDANCE,
} mode_e;

enum COMPLEX_PATH_TYPE {
    NORMAL_JOINT_PATH = 0,
    MOVEP_JOINT_PATH = 1,
    NORMAL_POSE_PATH = 2,
    MOVEP_POSE_PATH = 3,
};

extern "C" typedef enum _CONTROLLER_FEATURE_CODE {
    NONE_FEATURE = 0,
    AUTO_SWITCH_TO_IMPEDANCE_MODE_WHEN_COLLISION_DETECTED = 1,
} controller_feature;

extern "C" typedef struct _ErrorInfo {
    int errorId;
    int errorType;
    int errorCode;
    char errorMsg[64];
} StrErrorInfo;

#define USER_MAXIMUM_DOUBLE_SIZE 40
#define USER_MAXIMUM_INT8_SIZE 160

extern "C" typedef struct _CustomStateInfo {
    double dblField[USER_MAXIMUM_DOUBLE_SIZE];
    int8_t int8Field[USER_MAXIMUM_INT8_SIZE];
} StrCustomStateInfo;

// 这里是工业API特有的数据结构
//  TrajectoryState
extern "C" typedef struct _TrajectoryState {
    int taskId;
    int segCount;
    int segIndex;
    int errorCode;
    int isControllerPaused;
    int isSafetyDriving;
    int isFreeDriving;
    int isZeroSpaceFreeDriving;
    int isRobotHoldBrake;
    int isProgramRunningOrPause;
    int isTeachPendantPaused;
    int isControllerTerminated;
    int isSafetyHandling;
    int isRealtimeTorque;
    int isForceMode;
} StrTrajectoryState;

extern "C" typedef struct _WarningInfo {
    int warningId;
    int warningCode;
    int warningType;
    char warningMsg[64];
} StrWarningInfo;

// 用户层机械臂反馈状态结构体
extern "C" typedef struct _RobotStateInfo {
    double jointPos[7];
    double jointAngularVel[7];
    double jointCurrent[7];
    double jointTorque[7];
    double tcpPos[6];
    double tcpExternalForce;
    bool bCollision;
    bool bTcpForceValid;
    double tcpForce[6];
    double jointForce[7];
    StrTrajectoryState trajState;
    StrErrorInfo errorInfo;
    StrWarningInfo warningInfo;
} StrRobotStateInfo;

extern "C" typedef void (*FNCSTATECALLBACK)(StrRobotStateInfo *pinfo, const char *strIpAddress);

extern "C" typedef enum _Coordinate_E {
    E_BASE_COORDINATE = 0,
    E_TOOL_COORDINATE,
    E_WORK_PIECE_COORDINATE,
    E_VIEW_COORDINATE
} coordinate_e;

extern "C" typedef enum _FreeDrivingMode {
    T_DISABLE_FREEDRIVING = 0,
    T_NORMAL_FREEDRIVING = 1,
    T_FORCE_FREEDRIVING = 2,
} free_driving_mode_e;

#define ROBOTSTATE_CUSTOM_BASIC \
    (0x00000001)  // 基本状态信息，必须推送，不可定制，包括 BasicRobotState_1 中 stateBits, trajectoryState及
                  //  BasicRobotState2 中所有数据
#define ROBOTSTATE_CUSTOM_JOINTPOS (0x00000002)           // 关节反馈位置
#define ROBOTSTATE_CUSTOM_LINKJOINTPOS (0x00000004)       // 低速侧关节反馈位置
#define ROBOTSTATE_CUSTOM_JOINTANGULARVEL (0x00000008)    // 关节反馈速度
#define ROBOTSTATE_CUSTOM_JOINTCURRENT (0x00000010)       // 关节反馈电流
#define ROBOTSTATE_CUSTOM_ORIGINJOINTTORQUE (0x00000020)  // 关节反馈力矩(含零偏)
#define ROBOTSTATE_CUSTOM_JOINTTORQUEOFFSET (0x00000040)  // 关节扭矩传感器零偏
#define ROBOTSTATE_CUSTOM_JOINTFORCE (0x00000080)         // 关节外力
#define ROBOTSTATE_CUSTOM_TCPFORCE (0x00000100)           // 末端受到的外力

#define ROBOTSTATE_CUSTOM_ALL (0xffffffff)                // 定制所有

enum _CustomRobotStateAction {
    API_CUSTOM_ADD = 0,    // 定制
    API_CUSTOM_DEL = 1,    // 取消
    API_CUSTOM_RESET = 2,  // 重置
};

#pragma pack()