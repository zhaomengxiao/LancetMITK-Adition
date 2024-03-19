#include "labstractmessage.h"

#include <QUuid>
#include <QThread>
#include <QDateTime>

LAbstractMessage::LAbstractMessage() : m_classname("LAbstractMessage")
{
    m_fromThreadIdentify.sprintf("%p", QThread::currentThread());
    // time
    QString __time = "empty";
    __time = QDateTime::currentDateTime().toString(("yyyy/MM/dd hh:mm:ss.zzz"));
    this->setTime(__time);
    // id
    this->setIdentify(QUuid::createUuid().toString());
}

LAbstractMessage::~LAbstractMessage()
{

}

QString LAbstractMessage::identify() const
{
    return this->m_identify;
}

void LAbstractMessage::setIdentify(const QString& id)
{
    this->m_identify = id;
}

QString LAbstractMessage::classname() const
{
    return this->m_classname;
}

QString LAbstractMessage::fromThreadIdentify() const
{
    return this->m_fromThreadIdentify;
}

LAbstractMessage::LMessageType LAbstractMessage::type() const
{
    return this->m_type;
}

void LAbstractMessage::setType(const LMessageType& _type)
{
    this->m_type = _type;
}

QString LAbstractMessage::moduleName() const
{
    return this->m_modulename;
}

void LAbstractMessage::setModuleName(const QString& name)
{
    this->m_modulename = name;
}

bool LAbstractMessage::isLErrorMessage() const
{
    return false;
}

bool LAbstractMessage::isLToolAttitudeMessage() const
{
    return false;
}

bool LAbstractMessage::isLComponentAttributeMessage() const
{
    return false;
}

bool LAbstractMessage::isLRecoveryMessage() const
{
    return false;
}

QString LAbstractMessage::time() const
{
    return this->m_time;
}

void LAbstractMessage::setTime(const QString& t)
{
    this->m_time = t;
}
