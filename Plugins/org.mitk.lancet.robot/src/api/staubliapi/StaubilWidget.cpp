#include <QWidget>
#include <QDebug>
#include <QLabel>
#include "StaubilWidget.h"
#include "ui_StaubilWidget.h"


StaubilWidget::StaubilWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StaubilWidget)
{
    ui->setupUi(this);
    // timer
    ui_realtimedata_timer = new QTimer(this);
    connect(ui_realtimedata_timer, SIGNAL(timeout()), this, SLOT(update_realtime_data()));

    ui_automove_timer = new QTimer(this);
    connect(ui_automove_timer, SIGNAL(timeout()), this, SLOT(robot_auto_move()));

    ui->robotconnectstate->setText("Robot Disconnect");
    ui->powerstate->setText("PowerOff");
    ui->errorstate->setText("NO ERROR");
    ui->robotworkmode->setText("UNKNOWN");

    staubli = new RobotApi();
    connect(staubli, SIGNAL(signal_api_isRobotConnected(bool)), this, SLOT(ui_isRobotConnected(bool)));
    connect(staubli, SIGNAL(signal_api_isRobotPowerOn(bool)), this, SLOT(ui_isRobotPowerOn(bool)));
    connect(staubli, SIGNAL(signal_api_isRobotError(bool)), this, SLOT(ui_isRobotError(bool)));
    connect(staubli, SIGNAL(signal_api_isRobotBlocked(bool)), this, SLOT(ui_isRobotBlocked(bool)));
    //connect(staubli, SIGNAL(signal_api_RobotWorkState(RobotWorkState)), this, SLOT(ui_RobotWorkState(RobotWorkState)), Qt::DirectConnection);
    connect(staubli, SIGNAL(signal_api_RobotWorkMode(RobotWorkMode)), this, SLOT(ui_RobotWorkMode(RobotWorkMode)), Qt::DirectConnection);

    cubeSidelength = 50.0;
    cube_point_number = 0;
    initial_point_ready = false;
}

StaubilWidget::~StaubilWidget()
{
    delete ui_realtimedata_timer;
    delete staubli;
    delete ui;
}

void StaubilWidget::update_realtime_data()
{
    staubli->requestrealtimedata();
    ui_realtime_data = staubli->realtime_data;
    ui->j0->setText(QString::number(ui_realtime_data.joints.j1));
    ui->j1->setText(QString::number(ui_realtime_data.joints.j2));
    ui->j2->setText(QString::number(ui_realtime_data.joints.j3));
    ui->j3->setText(QString::number(ui_realtime_data.joints.j4));
    ui->j4->setText(QString::number(ui_realtime_data.joints.j5));
    ui->j5->setText(QString::number(ui_realtime_data.joints.j6));
    ui->x->setText(QString::number(ui_realtime_data.pose.x));
    ui->y->setText(QString::number(ui_realtime_data.pose.y));
    ui->z->setText(QString::number(ui_realtime_data.pose.z));
    ui->rx->setText(QString::number(ui_realtime_data.pose.rx));
    ui->ry->setText(QString::number(ui_realtime_data.pose.ry));
    ui->rz->setText(QString::number(ui_realtime_data.pose.rz));

    ui->fx->setText(QString::number(ui_realtime_data.ati.fx));
    ui->fy->setText(QString::number(ui_realtime_data.ati.fy));
    ui->fz->setText(QString::number(ui_realtime_data.ati.fz));
    ui->mx->setText(QString::number(ui_realtime_data.ati.mx));
    ui->my->setText(QString::number(ui_realtime_data.ati.my));
    ui->mz->setText(QString::number(ui_realtime_data.ati.mz));
}

void StaubilWidget::robot_auto_move()
{
    double para = qrand()%30-15;
    staubli->movej(-45.0+para, 60.0+para, 30.0+para, 0.0+para, 90.0+para, 0.0+para);
}

void StaubilWidget::on_SocketConnect_clicked()
{
    staubli->connectrobot();
    cubeSidelength = 50.0;
    cube_point_number = 0;
    initial_point_ready = false;
}

void StaubilWidget::on_SocketDisConnect_clicked()
{
    ui_realtimedata_timer->stop();
    ui_automove_timer->stop();
    //staubli->stoppollingdata();
    staubli->disconnectrobot();
    cubeSidelength = 50.0;
    cube_point_number = 0;
    initial_point_ready = false;
}

void StaubilWidget::on_PowerOn_clicked()
{
    staubli->poweron();
}

void StaubilWidget::on_PowerOff_clicked()
{
    staubli->poweroff();
}

void StaubilWidget::on_BackTestPose_clicked()
{
    staubli->requestrealtimedata();
    ui_moveInitial_realtime_data = staubli->realtime_data;
    //staubli->movej(false,-43.80,26.10,95.68,17.02,26.07,172.94);
    //staubli->movej(true,456.452,-388.168,43.1333,-16.8622,152.987,76.1078);
    ui_moveInitial_realtime_data.pose.x += 7;
    staubli->movel(ui_moveInitial_realtime_data.pose.x, ui_moveInitial_realtime_data.pose.y, ui_moveInitial_realtime_data.pose.z,
                   ui_moveInitial_realtime_data.pose.rx, ui_moveInitial_realtime_data.pose.ry, ui_moveInitial_realtime_data.pose.rz);
}

void StaubilWidget::on_Testpose_clicked()
{
    staubli->requestrealtimedata();
    ui_moveInitial_realtime_data = staubli->realtime_data;
    //staubli->movej(false,-43.1716, 28.0847, 95.6773, 17.03, 26.0695, 14.7911);
    //staubli->movej(true,450.719,-392.153,63.8555,161.381,29.1248,55.3463);
    //staubli->movel(450.719,-392.153,63.8555,161.381,29.1248,55.3463);
    ui_moveInitial_realtime_data.pose.x -= 7;
    staubli->movel(ui_moveInitial_realtime_data.pose.x, ui_moveInitial_realtime_data.pose.y, ui_moveInitial_realtime_data.pose.z,
                   ui_moveInitial_realtime_data.pose.rx, ui_moveInitial_realtime_data.pose.ry, ui_moveInitial_realtime_data.pose.rz);
}


void StaubilWidget::ui_isRobotConnected(bool p)
{
    if(p)
    {
        ui->robotconnectstate->setText("Robot Connected");
        qDebug()<<"Robot Connected";
        //staubli->startpollingdata();
        ui_realtimedata_timer->start(100);
    }
    else
    {
        ui->robotconnectstate->setText("Robot Disconnect");
        ui_realtimedata_timer->stop();
        qDebug()<<"Robot Disconnect";
    }
}
void StaubilWidget::ui_isRobotPowerOn(bool p)
{
    if(p)
        ui->powerstate->setText("PowerOn");
    else
        ui->powerstate->setText("PowerOff");
}
void StaubilWidget::ui_isRobotError(bool p)
{
    if(p)
        ui->errorstate->setText("Error!!");
    else
        ui->errorstate->setText("NO Error");
}

void StaubilWidget::ui_isRobotBlocked(bool p)
{
    /*if(p)
        ui->blockedstate->setText("Blocked: YES");
    else
        ui->blockedstate->setText("Blocked: NO");*/
}

void StaubilWidget::ui_RobotWorkMode(RobotWorkMode wm)
{
    /*if(wm == CONTROL_MODE_UNKNOWN)
        ui->robotworkmode->setText("WorkMode: UNKNOWN");
    if(wm == CONTROL_MODE_AUTO)
        ui->robotworkmode->setText("WorkMode: NORMAL");
    if(wm == CONTROL_MODE_FORCE)
        ui->robotworkmode->setText("WorkMode: ATI");*/
}


void StaubilWidget::on_StartPollingData_clicked()
{
    if(staubli->robot_state.isRobotConnected)
    {
        //staubli->startpollingdata();
        if(staubli->robot_state.isRobotPowerOn)
        {
            staubli->movej(-45.0, 60.0, 30.0, 0.0, 90.0, 0.0);
            qDebug() << "Set Initial Pose, Start Move Random!";
            ui_automove_timer->start(7000);
        }
        else
            qDebug() << "Power On Robot First!";
    }
    else
        qDebug() << "Connect Robot first";
}

void StaubilWidget::on_StopPollingData_clicked()
{
    //staubli->stoppollingdata();
    qDebug() << "Stop Move Random!";
    ui_automove_timer->stop();
}

void StaubilWidget::on_GetTcpInfo_clicked()
{
    staubli->gettcpinfo();
    staubli->requestcommandfeedbackdata();
    ui_commandfeedback_data = staubli->cammand_feedback_data;
    qDebug() << "Tcp Type: " << ui_commandfeedback_data.tcpinfo.tcptype
             << "Tcp Cog:" << ui_commandfeedback_data.tcpinfo.cog.x
             << ui_commandfeedback_data.tcpinfo.cog.y
             << ui_commandfeedback_data.tcpinfo.cog.z
             << "Tcp Info: " << ui_commandfeedback_data.tcpinfo.tcp.x
             << ui_commandfeedback_data.tcpinfo.tcp.y
             << ui_commandfeedback_data.tcpinfo.tcp.z
             << ui_commandfeedback_data.tcpinfo.tcp.rx
             << ui_commandfeedback_data.tcpinfo.tcp.ry
             << ui_commandfeedback_data.tcpinfo.tcp.rz;
}

void StaubilWidget::on_SetTcpInfo_clicked()
{
    ui_tcp.tcptype = TcpType::TCP_ROD_DRILL;
    ui_tcp.cog.weight = 0;
    ui_tcp.cog.x = 0;
    ui_tcp.cog.y = 0;
    ui_tcp.cog.z = 0;
    ui_tcp.tcp.x = 0;
    ui_tcp.tcp.y = 0;
    ui_tcp.tcp.z = 0;
    ui_tcp.tcp.rx = 0;
    ui_tcp.tcp.ry = 0;
    ui_tcp.tcp.rz = 0;

    staubli->settcpinfo(ui_tcp);
}

void StaubilWidget::on_setCubesidelength_valueChanged(double arg1)
{
    cubeSidelength = arg1*10.0;
    qDebug() << "Set Cubesidelength " << arg1 << "cm";
}

void StaubilWidget::on_setInitialPoint_clicked()
{
    staubli->setspeed(70);
    staubli->requestrealtimedata();
    ui_moveInitial_realtime_data = staubli->realtime_data;
    initial_point_ready = true;
    cube_point_number = 0;
    qDebug() << "Set InitialPoint finished!";
}

void StaubilWidget::on_nextPoint_clicked()
{
    if(!initial_point_ready)
        qDebug() << "SetInitialPoint First!!!";
    else
    {
        switch (cube_point_number)
        {
            case 0: //PC1
            qDebug() << ++cube_point_number;
            staubli->movej(staubli->realtime_data.joints.j1, 60.0, 30.0, 0.0, 90.0, 0.0);
//            staubli->movej(ui_moveInitial_realtime_data.joints.j1+qrand()%10-5,
//                           ui_moveInitial_realtime_data.joints.j2+qrand()%10-5,
//                           ui_moveInitial_realtime_data.joints.j3+qrand()%10-5,
//                           ui_moveInitial_realtime_data.joints.j4+qrand()%10-5,
//                           ui_moveInitial_realtime_data.joints.j5+qrand()%10-5,
//                           ui_moveInitial_realtime_data.joints.j6+qrand()%10-5);
            break;
            case 1: //PC2
            qDebug() << ++cube_point_number;
            //staubli->movej(true, ui_moveInitial_realtime_data.pose.x+qrand()%100-50,
            staubli->movel(ui_moveInitial_realtime_data.pose.x+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.y+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.z+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.rx,
                           ui_moveInitial_realtime_data.pose.ry,
                           ui_moveInitial_realtime_data.pose.rz);
            break;
            case 2: //PC3
            qDebug() << ++cube_point_number;
            //staubli->movej(true, ui_moveInitial_realtime_data.pose.x+cubeSidelength,
            staubli->movel(ui_moveInitial_realtime_data.pose.x+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.y+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.z+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.rx,
                           ui_moveInitial_realtime_data.pose.ry,
                           ui_moveInitial_realtime_data.pose.rz);
            break;
            case 3: //PC4
            qDebug() << ++cube_point_number;
            //staubli->movej(true, ui_moveInitial_realtime_data.pose.x,
            staubli->movel(ui_moveInitial_realtime_data.pose.x+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.y+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.z+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.rx,
                           ui_moveInitial_realtime_data.pose.ry,
                           ui_moveInitial_realtime_data.pose.rz);
            break;
            case 4: //PC5
            qDebug() << ++cube_point_number;
            staubli->movep(ui_moveInitial_realtime_data.pose.x+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.y+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.z+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.rx+qrand()%10-5,
                           ui_moveInitial_realtime_data.pose.ry+qrand()%10-5,
                           ui_moveInitial_realtime_data.pose.rz+qrand()%10-5);
            break;
            case 5:
            qDebug() << ++cube_point_number;
            staubli->movep(ui_moveInitial_realtime_data.pose.x+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.y+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.z+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.rx+qrand()%10-5,
                           ui_moveInitial_realtime_data.pose.ry+qrand()%10-5,
                           ui_moveInitial_realtime_data.pose.rz+qrand()%10-5);
            break;
            case 6:
            qDebug() << ++cube_point_number;
            staubli->movep(ui_moveInitial_realtime_data.pose.x+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.y+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.z+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.rx+qrand()%10-5,
                           ui_moveInitial_realtime_data.pose.ry+qrand()%10-5,
                           ui_moveInitial_realtime_data.pose.rz+qrand()%10-5);
            break;
            case 7:
            qDebug() << ++cube_point_number;
            staubli->movep(ui_moveInitial_realtime_data.pose.x+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.y+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.z+qrand()%50-25,
                           ui_moveInitial_realtime_data.pose.rx+qrand()%10-5,
                           ui_moveInitial_realtime_data.pose.ry+qrand()%10-5,
                           ui_moveInitial_realtime_data.pose.rz+qrand()%10-5);
            cube_point_number = 0;
            qDebug() << "reset cube_point_number: " << cube_point_number;
            break;
            default:
            qDebug() << "wrong cube_point_number: " << cube_point_number << ", and reset cube_point_number: 0";
            cube_point_number = 0;
            break;
        }
    }
}

void StaubilWidget::on_printdataonce_clicked()
{
    staubli->requestrealtimedata();
    RobotRealtimeData temp_realtime_data = staubli->realtime_data;
    qDebug() << "Vector3d PX(" << temp_realtime_data.pose.x <<","<< temp_realtime_data.pose.y <<","<< temp_realtime_data.pose.z <<"); Vector3d rx("
             << temp_realtime_data.pose.rx <<","<< temp_realtime_data.pose.ry <<","<< temp_realtime_data.pose.rz << "); Vector3d fx("
             << temp_realtime_data.ati.fx <<","<< temp_realtime_data.ati.fy <<","<< temp_realtime_data.ati.fz << "); Vector3d mx("
             << temp_realtime_data.ati.mx <<","<< temp_realtime_data.ati.my <<","<< temp_realtime_data.ati.mz << ");";
}

void StaubilWidget::on_ati_init_clicked()
{
    qDebug() << "ATI Initial";
    staubli->atizero();
    //cube_point_number = 3;
}

void StaubilWidget::on_GetLimitData_clicked()
{
    qDebug() << "Get Traction Limit Data";
    staubli->getlimitdata();
}

void StaubilWidget::on_SetLimitData_clicked()
{
    staubli->requestrealtimedata();
    RobotRealtimeData l_realtime_data = staubli->realtime_data;
    TractionLimit l;
    l.init();
    l.angle = 30;
    l.p1 = l_realtime_data.pose;
    l.p2 = l_realtime_data.pose;
    l.p1.z -= 50; l.p2.z += 50;
    staubli->setlimitdata(l);
}

void StaubilWidget::on_Foldrobot_clicked()
{
    staubli->movej(120.0,0.0,135.0,0.0,45.0,0.0);
}

void StaubilWidget::on_Unfolrobot_clicked()
{
    staubli->movej(-45.0,0.0,135.0,0.0,45.0,0.0);
}

void StaubilWidget::on_nolimit_clicked()
{
    TractionLimit l1;
    l1.init();
    l1.angle = -1;
    staubli->setlimitdata(l1);
}
