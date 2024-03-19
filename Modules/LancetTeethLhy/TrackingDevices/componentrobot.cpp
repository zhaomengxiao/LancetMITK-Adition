#include "componentrobot.h"
#include <Eigen/Dense>
#include <math.h>
#include <QDebug>
#include <QTimer>
#include <QThread>
//
//#pragma comment(lib, "E:/LancetMITK-Adition/Modules/LancetIGT/robotapi/lib/RRTCD_Staubli.lib")
//#pragma comment(lib, "E:/LancetMITK-Adition/Modules/LancetIGT/robotapi/lib/robotapid_Staubli.lib")

TcpInfo ui_tcp;

ComponentRobot::ComponentRobot()
    : m_isConnect(false)
{
    robotApi = new RobotApi;

    m_time.setInterval(50);

    connect(&m_time, SIGNAL(timeout()), this, SLOT(on_updateToolPosition()));
    connect(robotApi, SIGNAL(signal_api_isRobotConnected(bool)),
            this, SLOT(on_robotApi_signal_api_isRobotConnected(bool)));
}

ComponentRobot::~ComponentRobot()
{

}

void ComponentRobot::enabled()
{
    robotApi->connectrobot();
    this->m_time.start();
}

void ComponentRobot::disable()
{
    this->m_time.stop();
    this->m_workState = WorkerState::off_line;
}

void ComponentRobot::enabledOnThread()
{
    this->enabled();
}

bool ComponentRobot::isEnabledOnThread() const
{
    return false;
}

QString ComponentRobot::nameString() const
{
    return "lancet.device.robot";
}

QString ComponentRobot::version() const
{
    return "1.0.0";
}

bool ComponentRobot::toolEnabled(const QString&) const
{
    return true;
}

bool ComponentRobot::hasTool(const QString&) const
{
    return true;
}

void ComponentRobot::setToolEnabled(const QString&)
{

}

bool ComponentRobot::toolDisable(const QString&) const
{
    return true;
}

void ComponentRobot::setToolDisable(const QString&)
{

}

bool ComponentRobot::isValidTool(const QString& toolname)
{
    return toolname == "Robot_Tool" && m_isConnect;
}

QVariant ComponentRobot::optionValue(const QString&) const
{
    return "";
}

void ComponentRobot::setOptionValue(const QString&, const QVariant&)
{

}

bool ComponentRobot::requestExecOperate(const QString& funname, const QStringList& param)
{
	qWarning() << __func__ << __LINE__ << "log." << "call " << funname << "|" << param << endl;
    if(funname.indexOf("movep") != -1  && param.size() == 6)
    {
        this->robotApi->movep(param.at(0).toDouble(),
                              param.at(1).toDouble(),
                              param.at(2).toDouble(),
                              param.at(3).toDouble(),
                              param.at(4).toDouble(),
                              param.at(5).toDouble());
        return true;
    }
    if(funname.indexOf("movej") != -1  && param.size() == 6)
    {
        this->robotApi->movej(param.at(0).toDouble(),
                              param.at(1).toDouble(),
                              param.at(2).toDouble(),
                              param.at(3).toDouble(),
                              param.at(4).toDouble(),
                              param.at(5).toDouble());
        return true;
    }
    else if(funname.indexOf("atimode") != -1  && param.size() == 1)
    {
//        this->setWorkMode(param.at(0).toInt());
        this->robotApi->atimode(param.at(0).toInt());
        return true;
    }
    else if(funname.indexOf("movel") != -1  && param.size() == 6){
        this->robotApi->movel(param.at(0).toDouble(),
                              param.at(1).toDouble(),
                              param.at(2).toDouble(),
                              param.at(3).toDouble(),
                              param.at(4).toDouble(),
                              param.at(5).toDouble());
        return true;
    }
    else if(funname.indexOf("setTcp") != -1  ){
        ui_tcp.tcptype = TcpType::TCP_ROD_DRILL;
        ui_tcp.cog.weight = param.at(0).toDouble();
        ui_tcp.cog.x = param.at(1).toDouble();
        ui_tcp.cog.y = param.at(2).toDouble();
        ui_tcp.cog.z = param.at(3).toDouble();
        ui_tcp.tcp.x = param.at(4).toDouble();
        ui_tcp.tcp.y = param.at(5).toDouble();
        ui_tcp.tcp.z = param.at(6).toDouble();
        ui_tcp.tcp.rx = param.at(7).toDouble();
        ui_tcp.tcp.ry = param.at(8).toDouble();
        ui_tcp.tcp.rz = param.at(9).toDouble();

        this->robotApi->settcpinfo(ui_tcp);
        return true;
    }

    else if(funname.indexOf("poweron")!=-1 && param.size()==0)
    {
        this->robotApi->poweron();
    }
    else if(funname.indexOf("poweroff")!=-1 && param.size()==0){
        this->robotApi->poweroff();
    }
    else if(funname.indexOf("update") != -1  && param.size() == 0)
    {
        emit this->updateToolPosition(this->devicename(), this->realtimeData());
    }
    return false;
}

void ComponentRobot::heartbeat()
{
    this->robotApi->setspeed(50);
}

LToolAttitudeMessage ComponentRobot::realtimeData()
{
    LToolAttitudeMessage _retval;
    this->robotApi->requestrealtimedata();

    _retval.setValid(true);
    _retval.setToolName("Robot_Tool");
    _retval.setModuleName("Robot");
    _retval.setMatrix4x4(this->translateOfVtkMatrix4x4(this->robotApi->realtime_data));
    _retval.setErrorIdentify(this->robotApi->realtime_data.workmode);
    QVector<double> extendDataField;
    extendDataField.push_back(this->robotApi->realtime_data.joints.j1);
    extendDataField.push_back(this->robotApi->realtime_data.joints.j2);
    extendDataField.push_back(this->robotApi->realtime_data.joints.j3);
    extendDataField.push_back(this->robotApi->realtime_data.joints.j4);
    extendDataField.push_back(this->robotApi->realtime_data.joints.j5);
    extendDataField.push_back(this->robotApi->realtime_data.joints.j6);
    extendDataField.push_back(this->robotApi->realtime_data.pose.rz);
    extendDataField.push_back(this->robotApi->realtime_data.pose.ry);
    extendDataField.push_back(this->robotApi->realtime_data.pose.rx);
    extendDataField.push_back(this->robotApi->realtime_data.pose.x);
    extendDataField.push_back(this->robotApi->realtime_data.pose.y);
    extendDataField.push_back(this->robotApi->realtime_data.pose.z);
//    extendDataField.push_back(QString::number(this->robotApi->realtime_data.io.bit).toDouble());
    extendDataField.push_back(QString::number(this->robotApi->realtime_data.io.valu1e).toDouble());
    _retval.setExtendedDataField(extendDataField);
    return _retval;
}

int ComponentRobot::workMode() const
{
    return 0;
}

void ComponentRobot::setWorkMode(int mode)
{
//    this->robotApi->setworkmode(mode);
}

void ComponentRobot::on_robotApi_signal_api_isRobotConnected(bool connected)
{
    m_isConnect = connected;
    if(connected)
    {
        this->m_isValid = true;
        this->m_workState = WorkerState::working;

        m_thHeartbeat = QThread::create(ComponentRobot::workingOnHeartbeat, this);
//        m_thHeartbeat->start();
        emit this->deviceEnabled(this->devicename());
        emit this->noticeToolEnabled("Robot_Tool");
    }
    else
    {
        this->m_isValid = false;
        this->m_workState = WorkerState::off_line;
        emit this->deviceDisable(this->devicename());
        emit this->toolDisable("Robot_Tool");
    }
}

void ComponentRobot::on_updateToolPosition()
{
    auto tooldata =  this->realtimeData();
    if(tooldata.isValidData())
    {
        emit this->updateToolPosition(this->devicename(), this->realtimeData());
    }
}

void ComponentRobot::workingOnHeartbeat(ComponentRobot* _this)
{
    while(_this->m_workState == WorkerState::working)
    {
        _this->heartbeat();
        QThread::msleep(1000);
    }
}

QVector<double> ComponentRobot::translateOfVtkMatrix4x4(const RobotRealtimeData& date)
{
    QVector<double> _vtkMatrix4x4;

    Eigen::Vector3d rxyz(date.pose.rx, date.pose.ry, date.pose.rz);
    Eigen::Matrix3d R = m_rrtc.calculateR(rxyz);
    Eigen::Vector3d T(date.pose.x, date.pose.y, date.pose.z);

    _vtkMatrix4x4 = { R(0, 0), R(0, 1), R(0, 2), T[0],
                      R(1, 0), R(1, 1), R(1, 2), T[1],
                      R(2, 0), R(2, 1), R(2, 2), T[2],
                      0, 0, 0, 1
                    };

    return QVector<double>();
}
