#include "udpmessage.h"

#include <QList>

struct RobotUDPMessage::RobotUDPMessagePrivateImp
{
    long time;
    int frames;
    int validFrames;
    int errorCode;

    bool isConnectedUDP;
    bool isApplicationReadyToStart;
    bool isApplicationError;
    bool isLowerMachineSignalError;

    ApplicationState applicationState;

    bool isApplicationStart;
    bool isApplicationEnable;
};

RobotUDPMessage::RobotUDPMessage()
    : m_imp(std::make_shared<RobotUDPMessagePrivateImp>())
{

}

RobotUDPMessage::RobotUDPMessage(const QByteArray& stream)
    : m_imp(std::make_shared<RobotUDPMessagePrivateImp>())
{
    *this = stream;
}

RobotUDPMessage::RobotUDPMessage(const RobotUDPMessage& msg)
    : m_imp(std::make_shared<RobotUDPMessagePrivateImp>())
{
    *this = msg;
}

void RobotUDPMessage::operator=(const QByteArray& stream)
{
    *this = translateRobotUDPMessage(stream.data(), stream.length());
}

void RobotUDPMessage::operator=(const RobotUDPMessage& msg)
{
    *this->m_imp = *msg.m_imp;
}

long RobotUDPMessage::time() const
{
    return this->m_imp->time;
}

void RobotUDPMessage::setTime(long tm)
{
    this->m_imp->time = tm;
}

int RobotUDPMessage::frames() const
{
    return this->m_imp->frames;
}

void RobotUDPMessage::setFrames(int f)
{
    this->m_imp->frames = f;
}

int RobotUDPMessage::validFrames() const
{
    return this->m_imp->validFrames;
}

void RobotUDPMessage::setValidFrames(int f)
{
    this->m_imp->validFrames = f;
}

int RobotUDPMessage::errorCode() const
{
    return this->m_imp->errorCode;
}

void RobotUDPMessage::setErrorCode(int e)
{
    this->m_imp->errorCode = e;
}

bool RobotUDPMessage::isConnectedUDP() const
{
    return this->m_imp->isConnectedUDP;
}

void RobotUDPMessage::setConnectedUDP(bool c)
{
    this->m_imp->isConnectedUDP = c;
}

bool RobotUDPMessage::isApplicationReadyToStart() const
{
    return this->m_imp->isApplicationReadyToStart;
}

void RobotUDPMessage::setApplicationReadyToStart(bool b)
{
    this->m_imp->isApplicationReadyToStart = b;
}

bool RobotUDPMessage::isApplicationError() const
{
    return this->m_imp->isApplicationError;
}

void RobotUDPMessage::setApplicationError(bool b)
{
    this->m_imp->isApplicationError = b;
}

bool RobotUDPMessage::isLowerMachineSignalError() const
{
    return this->m_imp->isLowerMachineSignalError;
}

void RobotUDPMessage::setLowerMachineSignalError(bool b)
{
    this->m_imp->isLowerMachineSignalError = b;
}

RobotUDPMessage::ApplicationState RobotUDPMessage::applicationState() const
{
    return this->m_imp->applicationState;
}

void RobotUDPMessage::setApplicationState(const QString& state)
{
    this->setApplicationState(applicationStateToKey(state));
}

void RobotUDPMessage::setApplicationState(const RobotUDPMessage::ApplicationState& as)
{
    this->m_imp->applicationState = as;
}

bool RobotUDPMessage::isApplicationStart() const
{
    return this->m_imp->isApplicationStart;
}

void RobotUDPMessage::setApplicationStart(bool b)
{
    this->m_imp->isApplicationStart = b;
}

bool RobotUDPMessage::isApplicationEnable() const
{
    return this->m_imp->isApplicationEnable;
}

void RobotUDPMessage::setApplicationEnable(bool b)
{
    this->m_imp->isApplicationEnable = b;
}

RobotUDPMessage RobotUDPMessage::translateRobotUDPMessage(const char* stream, int lenght)
{
    RobotUDPMessage tmpMessage;

    QByteArray qstream = QByteArray(stream, lenght);

    auto results = qstream.split(';');

    if(results.size() >= 11)
    {
        tmpMessage.setTime(results[0].toLong());
        tmpMessage.setFrames(results[1].toInt());
        tmpMessage.setValidFrames(results[2].toInt());
        tmpMessage.setErrorCode(results[3].toInt());
        tmpMessage.setConnectedUDP(results[4].toLower() == "true" ? true : false);
        tmpMessage.setApplicationReadyToStart(results[5].toLower() == "true" ? true : false);
        tmpMessage.setApplicationError(results[6].toLower() == "true" ? true : false);
        tmpMessage.setLowerMachineSignalError(results[7].toLower() == "true" ? true : false);
        tmpMessage.setApplicationState(applicationStateToKey(results[8]));
        tmpMessage.setApplicationStart(results[9].toLower() == "true" ? true : false);
        tmpMessage.setApplicationEnable(results[10].toLower() == "true" ? true : false);
    }

    return tmpMessage;
}

QByteArray RobotUDPMessage::toUDPStream(const RobotUDPMessage& msg)
{
    QByteArray qstream;
    qstream += QString("%1;").arg(QString::asprintf("%ld", msg.time()));
    qstream += QString("%1;").arg(QString::number(msg.frames()));
    qstream += QString("%1;").arg(QString::number(msg.validFrames()));
    qstream += QString("%1;").arg(QString::number(msg.errorCode()));

    qstream += QString("%1;").arg(msg.isConnectedUDP() ? "true" : "false");
    qstream += QString("%1;").arg(msg.isApplicationReadyToStart() ? "true" : "false");
    qstream += QString("%1;").arg(msg.isApplicationError() ? "true" : "false");
    qstream += QString("%1;").arg(msg.isLowerMachineSignalError() ? "true" : "false");
    qstream += QString("%1;").arg(applicationStateToValue(msg.applicationState()));
    qstream += QString("%1;").arg(msg.isApplicationStart() ? "true" : "false");
    qstream += QString("%1").arg(msg.isApplicationEnable() ? "true" : "false");
    return qstream;
}

// TODO: Post optimization.
#define EM_ApplicationStateToKey(x) #x
RobotUDPMessage::ApplicationState RobotUDPMessage::applicationStateToKey(const QString& state)
{
    if(EM_ApplicationStateToKey(IDLE) == state.toUpper())
    {
        return IDLE;
    }
    else if(EM_ApplicationStateToKey(RUNNING) == state.toUpper())
    {
        return RUNNING;
    }
    else if(EM_ApplicationStateToKey(MOTIONPAUSED) == state.toUpper())
    {
        return MOTIONPAUSED;
    }
    else if(EM_ApplicationStateToKey(REPOSITIONING) == state.toUpper())
    {
        return REPOSITIONING;
    }
    else if(EM_ApplicationStateToKey(ERROR) == state.toUpper())
    {
        return ERROR;
    }
    else if(EM_ApplicationStateToKey(STARTING) == state.toUpper())
    {
        return STARTING;
    }
    else if(EM_ApplicationStateToKey(STOPPING) == state.toUpper())
    {
        return STOPPING;
    }

    return UNKNOWN;
}

QString RobotUDPMessage::applicationStateToValue(const RobotUDPMessage::ApplicationState& state)
{
    switch (state)
    {
        case ApplicationState::UNKNOWN:
            return "UNKNOWN";
        case ApplicationState::IDLE:
            return "IDLE";
        case ApplicationState::RUNNING:
            return "RUNNING";
        case ApplicationState::MOTIONPAUSED:
            return "MOTIONPAUSED";
        case ApplicationState::REPOSITIONING:
            return "REPOSITIONING";
        case ApplicationState::ERROR:
            return "ERROR";
        case ApplicationState::STARTING:
            return "STARTING";
        case ApplicationState::STOPPING:
            return "STOPPING";
    }
    return "UNKNOWN";
}
