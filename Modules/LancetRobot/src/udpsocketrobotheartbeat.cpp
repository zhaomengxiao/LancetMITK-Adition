#include "udpsocketrobotheartbeat.h"

#include <sys/timeb.h>

#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QPointer>
#include <QNetworkDatagram>

#include "udpmessage.h"
#include "mitkLog.h"
#include "mitkLogMacros.h"
struct UdpSocketRobotHeartbeat::UdpSocketRobotHeartbeatPrivateImp
{
    uint remoteHostPort;
    int deathTimeout;
    RunningModel runningModel;
    QString remoteHostAddress;
    QTimer repetitiveHeartbeatInterval;
    static long int frameRate;
    RobotUDPMessage lastRobotUDPMessage;
    QVector<QByteArray> sendDataStream;

    QPointer<QThread> thread;
    QAtomicInteger<bool> isStartRepetitiveHeartbeat;
};
long int UdpSocketRobotHeartbeat::UdpSocketRobotHeartbeatPrivateImp::frameRate = 0;

UdpSocketRobotHeartbeat::UdpSocketRobotHeartbeat(QObject* parent)
    : QUdpSocket(parent)
    , m_imp(std::make_shared<UdpSocketRobotHeartbeatPrivateImp>())
{
    connect(&this->m_imp->repetitiveHeartbeatInterval, SIGNAL(timeout()), this, SLOT(updateRobotHeartbeat()));
    connect(this, &QUdpSocket::readyRead, this, &UdpSocketRobotHeartbeat::onReadyRead);
}

UdpSocketRobotHeartbeat::~UdpSocketRobotHeartbeat()
{
    this->stopRepetitiveHeartbeat();
    disconnect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    disconnect(&this->m_imp->repetitiveHeartbeatInterval, SIGNAL(timeout()), this, SLOT(updateRobotHeartbeat()));
    this->abort();
}

long UdpSocketRobotHeartbeat::timeStamp() const
{
    timeb t;
    ftime(&t);
    return t.time * 1000 + t.millitm;
}

bool UdpSocketRobotHeartbeat::routineHeartbeat()
{
    int frame_count = this->lastRobotUDPMessage().validFrames();
    if(false == this->simulateSpecialAction(frame_count + 1, false))
    {
        return false;
    }

    // wait udp buffer flsh
    QThread::msleep(20);
    if(false == this->simulateSpecialAction(frame_count + 2, true))
    {
        return false;
    }

    return true;
}

bool UdpSocketRobotHeartbeat::initializeRemoteRobot()
{
    //%ld;0;App_Start;false
    //%ld;1;App_Start;true
    if(false == this->simulateSpecialAction(0, false))
    {
        return false;
    }
    QThread::msleep(20);

//    // initialize
    if(false == this->simulateSpecialAction(1, true))
    {
        return false;
    }

    return true;
}

bool UdpSocketRobotHeartbeat::simulateSpecialAction(int count, bool isTrue)
{
    QString dataStream;
    long requestTime = this->timeStamp();
    dataStream.sprintf("%ld;%d;App_Start;%s", requestTime, count, (isTrue ? "true" : "false"));
    MITK_DEBUG << "try sent data: " << dataStream;
    //this->m_imp->sendDataStream.push_back(dataStream.toLatin1().data());

    if(-1 == this->writeDatagram(dataStream.toLatin1().data(), QHostAddress(this->remoteHostAddress()), this->remoteHostPort()))
    {
        return false;
    }
    MITK_DEBUG << "try sent data: " << dataStream << " OK";
    return true;
}

bool UdpSocketRobotHeartbeat::simulateSpecialActionOfAppGetState(int count, bool isTrue)
{
    MITK_DEBUG << "log";
    QString dataStream;
    long requestTime = this->timeStamp();
    dataStream.sprintf("%d;%d;Get_State;%s", requestTime, count, (isTrue ? "true" : "false"));
    MITK_DEBUG << "try sent data: " << dataStream;
    //this->m_imp->sendDataStream.push_back(dataStream.toLatin1().data());

    if(-1 == this->writeDatagram(dataStream.toLatin1().data(), QHostAddress(this->remoteHostAddress()), this->remoteHostPort()))
    {
        return false;
    }
    MITK_DEBUG << "try sent data: " << dataStream << " OK";
    return true;
}
void UdpSocketRobotHeartbeat::stopRepetitiveHeartbeat()
{
    MITK_DEBUG << "log";
    if(true == this->m_imp->isStartRepetitiveHeartbeat && false == this->m_imp->repetitiveHeartbeatInterval.isActive())
    {
        // thread
        MITK_INFO << "log.stop.thread";
        this->m_imp->isStartRepetitiveHeartbeat = false;
        while(true)
        {
            if(this->m_imp->thread->isRunning())
            {
                MITK_INFO << "The thread did not stop, waiting for the thread to stop";
                QThread::msleep(1);
            }
            else
            {
                MITK_INFO << "UDP worker exited";
                break;
            }
        }
    }
    else
    {
        MITK_INFO << "log.stop.timer";
        this->m_imp->repetitiveHeartbeatInterval.stop();
    }
}

bool UdpSocketRobotHeartbeat::isRepetitiveHeartbeatRunning() const
{
    switch (this->m_imp->runningModel)
    {
        case RunningModel::Thread:
            {
                return this->m_imp->thread->isRunning();
            }
        case RunningModel::Timer:
            return this->m_imp->repetitiveHeartbeatInterval.isActive();
    }
    return false;
}

void UdpSocketRobotHeartbeat::startRepetitiveHeartbeat(int runningModel)
{
    MITK_DEBUG << "log";

    switch (runningModel)
    {
        case RunningModel::Thread:
            {
                MITK_INFO << "log.start.thread";
                this->m_imp->thread = QThread::create(&UdpSocketRobotHeartbeat::onThreadHandle, this);
                this->stopRepetitiveHeartbeat();
                MITK_INFO << "log.thread.object " << this->m_imp->thread;
                this->m_imp->isStartRepetitiveHeartbeat = true;
                this->m_imp->thread->start();
                break;
            }
        case RunningModel::Timer:
            MITK_INFO << "log.start.timer";
            this->m_imp->repetitiveHeartbeatInterval.start();
            break;
    }
    this->m_imp->runningModel = (RunningModel)runningModel;
}

QString UdpSocketRobotHeartbeat::remoteHostAddress() const
{
    return this->m_imp->remoteHostAddress;
}

void UdpSocketRobotHeartbeat::setRemoteHostAddress(const QString& address)
{
    this->m_imp->remoteHostAddress = address;
}

uint UdpSocketRobotHeartbeat::remoteHostPort() const
{
    return this->m_imp->remoteHostPort;
}

void UdpSocketRobotHeartbeat::setRemoteHostPort(const uint& port)
{
    this->m_imp->remoteHostPort = port;
}

long UdpSocketRobotHeartbeat::frameRate() const
{
    return this->m_imp->frameRate;
}

int UdpSocketRobotHeartbeat::repetitiveHeartbeatInterval() const
{
    return this->m_imp->repetitiveHeartbeatInterval.interval();
}

void UdpSocketRobotHeartbeat::setRepetitiveHeartbeatInterval(int msec)
{
    this->m_imp->repetitiveHeartbeatInterval.setInterval(std::chrono::milliseconds(msec));
}

int UdpSocketRobotHeartbeat::deathTimeout() const
{
    return this->m_imp->deathTimeout;
}

void UdpSocketRobotHeartbeat::setDeathTimeout(int t)
{
    this->m_imp->deathTimeout = t;
}

RobotUDPMessage UdpSocketRobotHeartbeat::lastRobotUDPMessage() const
{
    return this->m_imp->lastRobotUDPMessage;
}

void UdpSocketRobotHeartbeat::setLastRobotUDPMessage(const RobotUDPMessage& o)
{
    this->m_imp->lastRobotUDPMessage = o;
}

void UdpSocketRobotHeartbeat::updateRobotHeartbeat()
{
    MITK_DEBUG << "log";

    MITK_DEBUG << "info.fream " << this->lastRobotUDPMessage().errorCode();
    if(this->lastRobotUDPMessage().errorCode() != 0)
    {
        MITK_DEBUG << "try repair error code " << this->lastRobotUDPMessage().errorCode();
        if(this->tryRepairErrorCodeOf(this->lastRobotUDPMessage().errorCode()))
        {
            MITK_INFO << "repair error code sucess.";
        }
        else
        {
            MITK_WARN << "repair error code faild.";
        }
    }

    static clock_t deathClock = 0;
    if(false == this->routineHeartbeat())
    {
        if((clock() - deathClock) > this->deathTimeout())
        {
            // 通信中断
        }
    }
    else
    {
        deathClock = clock();
    }
}

void UdpSocketRobotHeartbeat::onReadyRead()
{
    MITK_DEBUG << __func__ << ": log";
    while (this->hasPendingDatagrams())
    {
        QByteArray stream;
        QNetworkDatagram datagram = this->receiveDatagram();
        this->setLastRobotUDPMessage(datagram.data());
    }
}

void UdpSocketRobotHeartbeat::onSendDataStream()
{

}

bool UdpSocketRobotHeartbeat::tryRepairErrorCodeOf(int error)
{
    MITK_DEBUG << "log.error.value " << error;
    switch (error)
    {
        case -3:
            {
                int frame_count = this->lastRobotUDPMessage().validFrames();
                MITK_DEBUG << "log.count.value.App_Strat.false " << frame_count + 1;
                if(false == this->simulateSpecialAction(frame_count + 1, false))
                {
                    return false;
                }

                // wait udp buffer flsh
                QThread::msleep(20);
                MITK_DEBUG << "log.count.value.App_Strat.true " << frame_count + 2;
                if(false == this->simulateSpecialAction(frame_count + 2, true))
                {
                    return false;
                }

//                if(false == this->simulateSpecialActionOfAppGetState(frame_count + 3, true))
//                {
//                    return false;
//                }
                return true;
            }
            break;
    }
    return false;
}

void UdpSocketRobotHeartbeat::onThreadHandle(UdpSocketRobotHeartbeat* __this)
{
    clock_t tclock_tab = clock();
    while (__this->m_imp->isStartRepetitiveHeartbeat)
    {
        // security restore
        MITK_DEBUG << "info.fream " << __this->lastRobotUDPMessage().errorCode();
        if(__this->lastRobotUDPMessage().errorCode() != 0)
        {
            MITK_DEBUG << "try repair error code " << __this->lastRobotUDPMessage().errorCode();
            if(__this->tryRepairErrorCodeOf(__this->lastRobotUDPMessage().errorCode()))
            {
                MITK_INFO << "repair error code sucess.";
            }
            else
            {
                MITK_WARN << "repair error code faild.";
            }
        }

        // sleep ms
        if((clock() - tclock_tab) < __this->repetitiveHeartbeatInterval())
        {
            MITK_DEBUG << "wait fream update " << (__this->repetitiveHeartbeatInterval() - (clock() - tclock_tab));
            QThread::msleep((__this->repetitiveHeartbeatInterval() - (clock() - tclock_tab)));
            tclock_tab = clock();
        }
        __this->routineHeartbeat();
    }
    MITK_INFO << "log.thread.close";
}
