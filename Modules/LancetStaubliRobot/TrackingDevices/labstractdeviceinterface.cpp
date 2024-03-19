#include "labstractdeviceinterface.h"

LAbstractDeviceInterface::LAbstractDeviceInterface(QObject* parent)
    : QObject(parent)
    , m_isValid(false)
    , m_workState(WorkerState::unknown)
{

}

LAbstractDeviceInterface::~LAbstractDeviceInterface()
{

}

QString LAbstractDeviceInterface::devicename() const
{
    return this->m_deviceName;
}

void LAbstractDeviceInterface::setDeviceName(const QString& name)
{
    this->m_deviceName = name;
}

bool LAbstractDeviceInterface::isValid() const
{
    return this->m_isValid;
}

bool LAbstractDeviceInterface::isEnabled() const
{
    return this->m_workState == WorkerState::working;
}

bool LAbstractDeviceInterface::isDisable() const
{
    return this->m_workState ^ WorkerState::working;
}

QString LAbstractDeviceInterface::ipAddress() const
{
    return this->m_ipAddress;
}

void LAbstractDeviceInterface::setIpAddress(const QString& ip)
{
    this->m_ipAddress = ip;
}

QString LAbstractDeviceInterface::port() const
{
    return this->m_port;
}

void LAbstractDeviceInterface::setPort(const QString& port)
{
    this->m_port = port;
}

QString LAbstractDeviceInterface::hostname() const
{
    return QString("%1:%2").arg(ipAddress()).arg(port());
}

void LAbstractDeviceInterface::setHostname(const QString& input)
{
    auto __paramter = input.split(":", QString::SkipEmptyParts);
    if(input.indexOf(':') != -1 && __paramter.size() == 2)
    {
        this->setIpAddress(__paramter[0]);
        this->setPort(__paramter[1]);
    }
}

QMap<QString, QString> LAbstractDeviceInterface::romfiles() const
{
    return this->m_romfiles;
}

void LAbstractDeviceInterface::appendRomFile(const QString& name, const QString& file)
{
    if(this->m_romfiles.find(name) == this->m_romfiles.end())
    {
        this->m_romfiles[name] = file;
    }
}

void LAbstractDeviceInterface::removeRomFile(const QString& name)
{
    auto __removeIterator = this->m_romfiles.find(name);
    if(__removeIterator != this->m_romfiles.end())
    {
        this->m_romfiles.erase(__removeIterator);
    }
}

void LAbstractDeviceInterface::updateRomFile(const QString& name, const QString& file)
{
    if(this->m_romfiles.find(name) != this->m_romfiles.end())
    {
        this->m_romfiles[name] = file;
    }
}

QString LAbstractDeviceInterface::findRomFile(const QString& name) const
{
    if(this->m_romfiles.find(name) == this->m_romfiles.end())
    {
        return this->m_romfiles[name];
    }
    return "";
}
