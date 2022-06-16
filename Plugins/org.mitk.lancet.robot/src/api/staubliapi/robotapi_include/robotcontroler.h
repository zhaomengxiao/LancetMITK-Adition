#ifndef ROBOT_CONTROLER_H
#define ROBOT_CONTROLER_H

#include "Windows.h"
#include <QApplication>
#include <QMutex>
#include <QReadWriteLock>
#include <QDateTime>

#include "robotsocket.h"


#define PI 3.1415926535897932384626433832795
//#define ROBOT_SERVER_ADDRESS "192.168.1.254"  //statubli
//#define ROBOT_COMMAND_PORT 1000
//#define ROBOT_DATA_PORT 2000
#define ROBOT_SERVER_ADDRESS "172.31.1.147"     //kuka
#define ROBOT_COMMAND_PORT 30007
#define ROBOT_DATA_PORT 30001
//#define ROBOT_SERVER_ADDRESS "127.0.0.1"
#define Rad2Degree 180.0/PI
#define MAXMOVEJVEL 0.5
#define MAXMOVEJACC 0.5
#define MAXMOVELVEL 0.5
#define MAXMOVELACC 0.5
#define TIMER_TIMEOUT 30

const QString cmd_arg_joint="%1,%2,%3,%4,%5,%6";
const QString cmd_arg_point="%1,%2,%3,%4,%5,%6";
const QString cmd_arg_tcpinfo = "%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11";
const QString cmd_arg_limitdata = "%1,%2,%3,%4,%5,%6,%7";
const QString cmd_arg_atimode = "%1,%2,%3,%4,%5,%6,%7,%8";
const QString cmd_arg_RobotMove="%1,%2";

// realtime data type
enum RealtimedataType
{
    ROBOTDATA,
    ATIDATA
};

// Robot Controler State
enum RobotWorkState
{
    ROBOT_STATE_DISCONNECTED                 = 0,
    ROBOT_STATE_CONFIRM_SAFETY				 = 1,
    ROBOT_STATE_BOOTING                      = 2,
    ROBOT_STATE_POWER_OFF                    = 3,
    ROBOT_STATE_POWER_ON                     = 4,
    ROBOT_STATE_IDLE                         = 5,
    ROBOT_STATE_BACKDRIVE                    = 6,
    ROBOT_STATE_RUNNING                      = 7,
    ROBOT_STATE_UPDATING_FIRMWARE            = 8,
    ROBOT_STATE_ERROR                        = 9
};

// Robot Work Mode
enum RobotWorkMode
{
    CONTROL_MODE_UNKNOWN    = 0,
    CONTROL_MODE_AUTO       = 40,
    CONTROL_MODE_POSITION   = 2,
    CONTROL_MODE_TEACH      = 3,
    CONTROL_MODE_FORCE_ATI_MODE1      = 41,
    CONTROL_MODE_FORCE_ATI_MODE2      = 42,
    CONTROL_MODE_FORCE_ATI_MODE3      = 43,
    CONTROL_MODE_FORCE_ATI_MODE4	  = 44,
    CONTROL_MODE_TORQUE     = 5
};

// Robot Error Code
enum RobotErrorCode
{
    ROBOT_OK                = 0,
    ROBOT_ERROE1            = 1
};

// Robot Joint
struct Joints
{
    double j1;
    double j2;
    double j3;
    double j4;
    double j5;
    double j6;
    double j7;
    void init()
    {
        j1=j2=j3=j4=j5=j6=j7=0.0;
    }
};

//Cartesian coordinates of the point: (x,y,z,a,b,c)
struct CartesianPose
{
    double x;
    double y;
    double z;
    double a;
    double b;
    double c;
    void init()
    {
        x=y=z=a=b=c=0.0;
    }
};

struct RobotIO
{
    int bit;
    int valu1e;
    void init()
    {
        bit = 0;
        valu1e = 0;
    }
};

struct ATIMODEDATA
{
    int atimode;
    double para1;
    double para2;
    double para3;
    double para4;
    double para5;
    double para6;
    double para7;
    void init()
    {
        atimode = -1;
        para1 = para2 = para3 = para4 = para5 = para6 =para7 = 0.0;
    }
};

struct RobotMoveDirection
{
    int Jointmode;
    int para1;
    void init()
    {
        Jointmode = -1;
        para1 = -1;
    }
};

struct ATIData
{
    bool status;
    double fx;
    double fy;
    double fz;
    double mx;
    double my;
    double mz;
    void init()
    {
        status = false;
        fx = fy = fz = mx = my = mz = 0.0;
    }
};

// Tcp type
enum TcpType
{
    TCP_ROD                 = 0,
    TCP_ROD_DRILL           = 1,
    TCP_ROD_DRILL_BALL      = 2,
    TCP_ROD_HAMMER          = 3
};

// The CenterOfGravity(CoG) argument is optional - if not provided, the Tool Center Point (TCP) will be used as the Center of Gravity (CoG).
struct CenterOfGravity
{
    double weight;
    double x;
    double y;
    double z;
    void init()
    {
        weight = x = y = z = 0.0;
    }
    // todo gravity center calculate
};

// TCP all infomation
struct TcpInfo
{
    TcpType tcptype;
    CartesianPose tcp;
    CenterOfGravity cog;
    void init()
    {
        tcptype = TCP_ROD;
        tcp.init();
        cog.init();
    }
};

// Traction Limit Data
struct TractionLimit
{
    CartesianPose p1;
    CartesianPose p2;
    double angle;
    void init()
    {
        p1.init();
        p2.init();
        angle = -1;
    }
};

// Everything about Joint
struct JointInfo
{
    double position;                 //Joint positions
    double velocitie;                //Joint velocities
    double acceleration;             //Joint accelerations
    double current;                  //Joint currents
    double torque;                   //Joint moments (torques)
    double control_currents;         //Joint control currents
    double voltages;                 //Joint voltages
    double temperature;              //Temperature of each joint in degrees celsius
    double control_mode;             //Joint control modes
    double actual_tcp_speed;         //Actual speed of the tool given in Cartesian coordinates
    double target_tcp_speed;         //Target speed of the tool given in Cartesian coordinates
    double tcp_forces;               //Generalised forces in the TCP
    double jointMinLimit;
    double jointMaxLimit;
    double jointMaxSpeed;
    double jointMaxAcceleration;
    double DHa;
    double DHd;
    double DHalpha;
    double DHtheta;
};

// 6-Joints
struct RobotJointsInfo
{
    JointInfo j1;
    JointInfo j2;
    JointInfo j3;
    JointInfo j4;
    JointInfo j5;
    JointInfo j6;
};

// Robot realtime data
struct RobotRealtimeData
{
    RobotWorkState wkstate;
    RealtimedataType datatype;
    RobotErrorCode errorcode;
    RobotWorkMode workmode;
    Joints joints;
    CartesianPose pose;
    RobotIO io;
    ATIData ati;
    double speed;
};
//Q_DECLARE_METATYPE(RobotRealtimeData);

// Addition Data
struct AdditionalInfo
{
    bool FreedriveButtonPressed;
    bool FreedriveButtonEnabled;
    bool IOEnabledFreedrive;
};

//Command feedback data
struct RobotCommandFeedbackData
{
    TcpInfo tcpinfo;
    TractionLimit limitdata;
};

// Command type
enum CommandType
{
    CONNECT_SOCKET,
    DISCONNECT_SOCKET,
    RESET,
    FREEDRIVE,
    END_FREEDRIVE,
    POWERON,
    POWEROFF,
    SETIO,
    CLOSESAFETYPOPUP,
    UNLOCKPRO,
    MOVEJ,
    MOVEP,
    MOVEL,
    STOP,
    SETLD,
    GETLD,
    ATIZERO,
    ATIMODE,
    SETSPEED,
    GETSPEED,
    SETWM,
    SETTCPINFO,
    GETTCPINFO,
    SETOUTPUT,
    RobotMove,
};

// Command seed data
struct RobotCommandSendData
{
    CommandType robot_command_type;
    int robot_errorcode;
    int robot_workmode;
    double robot_speed;
    int robot_digitaloutbit;
    bool robot_digitaloutbitvalue;
    RobotIO robot_io;
    ATIMODEDATA robot_ati_mode;
    RobotMoveDirection robot_handle_move;
    Joints robot_joint;
    CartesianPose robot_point;
    TcpInfo robot_tcp;
    TractionLimit robot_limitdata;
    QString JointString;
    QString PointString;
    QString TcpString;
    QString LimitdataString;
    QString AtiModeParaString;
    QString RobotMoveString;
    void init()
    {
        robot_errorcode = 0;
        robot_workmode = -1;
        robot_digitaloutbit = -1;
        robot_digitaloutbitvalue = false;
        robot_io.init();
        robot_limitdata.init();
        robot_tcp.init();
        robot_ati_mode.init();
        robot_handle_move.init();
    }
    void set_joints_string()
    {
        JointString = cmd_arg_joint.arg(robot_joint.j1).arg(robot_joint.j2).arg(robot_joint.j3).arg(robot_joint.j4).arg(robot_joint.j5).arg(robot_joint.j6);
    }
    void set_points_string()
    {
        PointString = cmd_arg_point.arg(robot_point.x).arg(robot_point.y).arg(robot_point.z).arg(robot_point.a).arg(robot_point.b).arg(robot_point.c);
    }

    void set_tcpinfo_string()
    {
        TcpString = cmd_arg_tcpinfo.arg(robot_tcp.tcptype).arg(robot_tcp.cog.weight)
                .arg(robot_tcp.cog.x).arg(robot_tcp.cog.y).arg(robot_tcp.cog.z)
                .arg(robot_tcp.tcp.x).arg(robot_tcp.tcp.y).arg(robot_tcp.tcp.z)
                .arg(robot_tcp.tcp.a).arg(robot_tcp.tcp.b).arg(robot_tcp.tcp.c);
        TcpString = cmd_arg_tcpinfo.arg(robot_tcp.tcptype);
    }
    void set_limitdata_string()
    {
        LimitdataString = cmd_arg_limitdata.arg(robot_limitdata.angle).arg(robot_limitdata.p1.x).arg(robot_limitdata.p1.y)
                .arg(robot_limitdata.p1.z).arg(robot_limitdata.p2.x).arg(robot_limitdata.p2.y)
                .arg(robot_limitdata.p2.z);
    }
    void set_atimode_string()
    {
        AtiModeParaString = cmd_arg_atimode.arg(robot_ati_mode.atimode).arg(robot_ati_mode.para1).arg(robot_ati_mode.para2).arg(robot_ati_mode.para3).arg(robot_ati_mode.para4).arg(robot_ati_mode.para5).arg(robot_ati_mode.para6).arg(robot_ati_mode.para7);
    }
    void set_RobotMove_string()
    {
        RobotMoveString = cmd_arg_RobotMove.arg(robot_handle_move.Jointmode).arg(robot_handle_move.para1);
    }
};

struct DataMap
{
    bool data_status;
    int data_type;
    QByteArray data_array;
    QString data_string;
    void clear()
    {
        data_status = false;
        data_type = 0;
        data_array = NULL;
        data_string = "";
    }

    //todo init
};

// Robot State
struct RobotState
{
    int socketnum;
    bool isCommandConnected;
    bool isRealtimedataConnected;
    bool isRobotBlocked;
    bool isRobotConnected;
    bool isRobotPowerOn;
    bool isRobotError;
    RobotWorkState robot_work_state;
    RobotWorkMode robot_work_mode;
    void clear()
    {
        socketnum = 0;
        isCommandConnected = false;
        isRealtimedataConnected = false;
        isRobotBlocked = false;
        isRobotConnected = false;
        isRobotPowerOn = false;
        isRobotError = false;
        robot_work_state = ROBOT_STATE_DISCONNECTED;
        robot_work_mode = CONTROL_MODE_UNKNOWN;
    }
};

// Socket connect info
struct ConnectInfo
{
    int type;
    QAbstractSocket::SocketState status;
    void clear()
    {
        type = 0;
        status = QAbstractSocket::UnconnectedState;
    }
};

class RobotControler : public QThread
{
    Q_OBJECT

public:

    RobotControler(QObject * parent, qintptr p);
    ~RobotControler();

    RobotRealtimeData controler_realtime_data;
    RobotCommandFeedbackData command_controler_feedback_data;
    RobotCommandSendData command_send_data;

    bool getIsRobotBlocked();
    bool getIsRobotConnected();
    bool getIsRobotPowerOn();
    bool getIsRobotError();
    RobotWorkState getRobotWorkState();
    RobotWorkMode getRobotWorkMode();


public slots:

    void handle_command_data(DataMap cmdresponse);                 // handle command data
    void handle_realtime_data(DataMap rtdata);                            // handle realtime data

    void RunRobotControler();
    void movej(double a, double b, double c, double d, double e, double f);
    void movep(double a, double b, double c, double d, double e, double f);
    void movel(double a, double b, double c, double d, double e, double f);
    void stop();
    void connectrobot();                    // socket connect robot
    void disconnectrobot();                 // socket disconnect robot
    void poweron();                         // power on
    void poweroff();                        // power off
    void setio(int b, int v);               // set io
    void settcpinfo(TcpInfo t);             // set tcp type Tool_Coordinates_Pose and Center of Gravity: x,y,z
    void gettcpinfo();                      // set tcp type Tool_Coordinates_Pose and Center of Gravity: x,y,z
    void setspeed(double s);                // percent 0-1.0
    void setworkmode(int s);
    void setlimitdata(TractionLimit tl);
    void getlimitdata();
    void atizero();
    void atimode(int m, double para1, double para2, double para3, double para4, double para5, double para6, double para7);
    void Robothandlemode(int joint, int Direction);
    void readPendingCmdFeedbackData(QByteArray r);
    void readPendingRealtimeData(QByteArray r);
    void isConnected();
    void isCConnected();
    void isRConnected();
    void disConnected();
    void response_realtimedata();
    void response_commanddata();

signals:

    void signal_sendcommand(QString);
    void signal_isRobotBlocked(bool);
    void signal_isRobotConnected(bool);
    void signal_isRobotPowerOn(bool);
    void signal_isRobotError(bool);
    void signal_RobotWorkState(RobotWorkState);
    void signal_RobotWorkMode(RobotWorkMode);
    void signal_realtimedata(DataMap);
    void signal_commanddata(DataMap);
    void signal_update_commanddata(RobotCommandFeedbackData newcmdfdata);
    void signal_update_realtimedata(RobotRealtimeData newrtdata);

private slots:



private:

    RobotState robot_state_pop;
    DataMap c_command_feedback_data;
    DataMap c_realtime_data;
    QString controler_string;
    DataMap datamap_realtime;
    DataMap datamap_realtime_tmp;
    DataMap datamap_command;
    DataMap datamap_command_tmp;
    qintptr ptr;
    RobotSocket *robot_command;
    RobotSocket *robot_realtimedata;
    QMutex realtime_mutex;
    QMutex command_mutex;
    RobotRealtimeData temp_realtime_data;
    RobotCommandFeedbackData temp_command_feedback_data;
    bool newcommandsk;
    bool newrealtimesk;

    void run_robot_command(bool t);
    void run_robot_realtimedata(bool t);
    void update_isRobotBlocked(bool s);
    void update_isRobotConnected(bool s);
    void update_isRobotPowerOn(bool s);
    void update_isRobotError(bool s);
    void update_RobotWorkState(RobotWorkState ws);
    void update_RobotWorkMode(RobotWorkMode wm);
    void SendCommand(RobotCommandSendData m_commanddata);
};

#endif // ROBOT_CONTROLER_H
