#ifndef LUDPSOCKETROBOTHEARTBEAT_H
#define LUDPSOCKETROBOTHEARTBEAT_H

#include <QUdpSocket>
#include "MitkLancetRobotExports.h"

class RobotUDPMessage;
class MITKLANCETROBOT_EXPORT UdpSocketRobotHeartbeat
    : public QUdpSocket
{
    Q_OBJECT
    struct UdpSocketRobotHeartbeatPrivateImp;
public:
    explicit UdpSocketRobotHeartbeat(QObject* = nullptr);
    virtual ~UdpSocketRobotHeartbeat();

    enum RunningModel
    {
        Thread,
        Timer
    };

public:
    virtual long timeStamp() const;

    virtual bool routineHeartbeat();
    virtual bool initializeRemoteRobot();
    virtual bool simulateSpecialAction(int, bool);
    virtual bool simulateSpecialActionOfAppGetState(int, bool);

    virtual void stopRepetitiveHeartbeat();
    virtual bool isRepetitiveHeartbeatRunning() const;
    virtual void startRepetitiveHeartbeat(int runningModel = RunningModel::Thread);

    virtual QString remoteHostAddress() const;
    virtual void setRemoteHostAddress(const QString&);

    virtual uint remoteHostPort() const;
    virtual void setRemoteHostPort(const uint&);

    virtual long int frameRate() const;

    virtual int repetitiveHeartbeatInterval() const;
    virtual void setRepetitiveHeartbeatInterval(int msec);

    virtual int deathTimeout() const;
    virtual void setDeathTimeout(int);

    virtual RobotUDPMessage lastRobotUDPMessage() const;
    virtual void setLastRobotUDPMessage(const RobotUDPMessage&);
protected slots:
    virtual void updateRobotHeartbeat() final;
    virtual void onReadyRead() final;
    virtual void onSendDataStream() final;
protected:
    bool tryRepairErrorCodeOf(int);
    static void onThreadHandle(UdpSocketRobotHeartbeat*);
protected:
    std::shared_ptr<UdpSocketRobotHeartbeatPrivateImp> m_imp;
};

#endif // LUDPSOCKETROBOTHEARTBEAT_H
