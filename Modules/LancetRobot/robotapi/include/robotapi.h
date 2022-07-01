#ifndef ROBOTAPI_H
#define ROBOTAPI_H

#include <QObject>
#include <QThread>
#include <QMetaType>
#include <QDateTime>

#include "robotcontroler.h"

#define VERSION "RobotApi 3.01.201117_alpha"
#define DATA_RATE_TIME 100 // data frame rate is 1000ms/100ms = 10

class RobotApi : public QObject
{
    Q_OBJECT

public:
    RobotApi();
    ~RobotApi();

    RobotState robot_state;
    RobotRealtimeData realtime_data;
    RobotCommandFeedbackData cammand_feedback_data;

    QString Version();
    void connectrobot();
    void disconnectrobot();
    void poweron();
    void poweroff();
    void enablefreedrive();
    void disenablefreedrive();
    void closesafetypopup();
    void unlockprotectivestop();
    void setio(int b, int v);
    void settcpinfo(TcpInfo t);
    void gettcpinfo();
    void setspeed(double s);
    void setworkmode(int s);
    void movej(double a, double b, double c, double d, double e, double f);
    void movep(double a, double b, double c, double d, double e, double f);
    void movel(double a, double b, double c, double d, double e, double f);
    void stop();
    void setlimitdata(TractionLimit tl);
    void getlimitdata();
    void atizero();
    void atimode(int m, double para1, double para2, double para3,double para4,double para5,double para6,double para7);
    void setTcpNum(int Joint, int Direction);
    void requestrealtimedata();
    void requestcommandfeedbackdata();

signals:

    void signal_api_connectrobot();
    void signal_api_disconnectrobot();
    void signal_api_poweron();
    void signal_api_poweroff();
    void signal_api_setio(int, int);
    void signal_api_settcpinfo(TcpInfo);
    void signal_api_gettcpinfo();
    void signal_api_movej(double, double, double, double, double, double);
    void signal_api_movep(double, double, double, double, double, double);
    void signal_api_movel(double, double, double, double, double, double);
    void signal_api_stop();
    void signal_api_setspeed(double);
    void signal_api_setworkmode(int);
    void signal_api_setlimitdata(TractionLimit);
    void signal_api_getlimitdata();
    void signal_api_atizero();
    void signal_api_atimode(int, double, double, double,double,double,double,double);
    void signal_api_RobotMove(int,int);
    void signal_api_requestrealtimedata();
    void signal_api_requestcommandfeedbackdata();
    void signal_api_isRobotConnected(bool);
    void signal_api_isRobotPowerOn(bool);    
    void signal_api_isRobotError(bool);
    void signal_api_isRobotBlocked(bool);
    void signal_api_RobotWorkState(RobotWorkState);
    void signal_api_RobotWorkMode(RobotWorkMode);

public slots:

    void api_isRobotConnected(bool s);
    void api_isRobotBlocked(bool s);
    void api_isRobotPowerOn(bool s);
    void api_isRobotError(bool s);
    void api_RobotWorkState(RobotWorkState ws);
    void api_RobotWorkMode(RobotWorkMode wm);
    void api_getrealtimedata(RobotRealtimeData d);
    void api_getcommandfeedbackdata(RobotCommandFeedbackData sd);

private:

    qintptr socketDescriptor;
    QThread *newThread;
    QMutex apiRealtimedataMutex;
    QMutex apiCommanddataMutex;
    QTimer *realtimedata_timer;
    RobotControler *robotControler;

private slots:

    //void startpolling();
};

#endif // ROBOTAPI_H
