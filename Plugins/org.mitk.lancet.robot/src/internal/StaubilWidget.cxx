#pragma execution_character_set("utf-8");
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include "StaubilWidget.h"
#include "ui_StaubilWidget.h"
#include "IGTController.h"
#include <QFile>


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
	//ui->powerstate->setText("PowerOff");
	ui->errorstate->setText("NO ERROR");
	ui->robotworkmode->setText("UNKNOWN");

	igt = IGTController::getInStance();
	connect(igt->m_robotApi, SIGNAL(signal_api_isRobotConnected(bool)), this, SLOT(ui_isRobotConnected(bool)));
	connect(igt->m_robotApi, SIGNAL(signal_api_isRobotPowerOn(bool)), this, SLOT(ui_isRobotPowerOn(bool)));
	connect(igt->m_robotApi, SIGNAL(signal_api_isRobotError(bool)), this, SLOT(ui_isRobotError(bool)));
	connect(igt->m_robotApi, SIGNAL(signal_api_isRobotBlocked(bool)), this, SLOT(ui_isRobotBlocked(bool)));
	//connect(staubli, SIGNAL(signal_api_RobotWorkState(RobotWorkState)), this, SLOT(ui_RobotWorkState(RobotWorkState)), Qt::DirectConnection);
	connect(igt->m_robotApi, SIGNAL(signal_api_RobotWorkMode(RobotWorkMode)), this, SLOT(ui_RobotWorkMode(RobotWorkMode)), Qt::DirectConnection);
	connect(igt->m_robotApi, SIGNAL(signal_api_atimode(int, double, double, double)), this, SLOT(ui_ATIMode(RobotWorkMode)), Qt::DirectConnection);
	cubeSidelength = 50.0;
	cube_point_number = 0;
	initial_point_ready = false;
	move_rand_index = 0;

	//islimit = false;
	//isspeed = false;
	isIO = false;

	ATIValue.push_back(0);
	JointValue.push_back(0);
	TCPValue.push_back(0);
}

StaubilWidget::~StaubilWidget()
{
	delete ui_realtimedata_timer;
	delete ui;
}

void StaubilWidget::update_realtime_data()
{
	igt->m_robotApi->requestrealtimedata();
	ui_realtime_data = igt->m_robotApi->realtime_data;
	ui->j0->setText(QString::number(ui_realtime_data.joints.j1, 10, 3));
	ui->j1->setText(QString::number(ui_realtime_data.joints.j2, 10, 3));
	ui->j2->setText(QString::number(ui_realtime_data.joints.j3, 10, 3));
	ui->j3->setText(QString::number(ui_realtime_data.joints.j4, 10, 3));
	ui->j4->setText(QString::number(ui_realtime_data.joints.j5, 10, 3));
	ui->j5->setText(QString::number(ui_realtime_data.joints.j6, 10, 3));
	ui->j6->setText(QString::number(ui_realtime_data.joints.j7, 10, 3));

	ui->x->setText(QString::number(ui_realtime_data.pose.x, 10, 3));
	ui->y->setText(QString::number(ui_realtime_data.pose.y, 10, 3));
	ui->z->setText(QString::number(ui_realtime_data.pose.z, 10, 3));
	ui->a->setText(QString::number(ui_realtime_data.pose.a, 10, 3));
	ui->b->setText(QString::number(ui_realtime_data.pose.b, 10, 3));
	ui->c->setText(QString::number(ui_realtime_data.pose.c, 10, 3));
	//QString::number(ui_realtime_data.pose.c, 10, 3);

}

void StaubilWidget::robot_auto_move()
{
	double para = qrand() % 30 - 15;
	igt->m_robotApi->movej(-45.0 + para, 60.0 + para, 30.0 + para, 0.0 + para, 90.0 + para, 0.0 + para);
}

void StaubilWidget::on_SocketConnect_clicked()
{
	igt->m_robotApi->connectrobot();
	cubeSidelength = 50.0;
	cube_point_number = 0;
	initial_point_ready = false;
}

void StaubilWidget::on_SocketDisConnect_clicked()
{
	ui_realtimedata_timer->stop();
	ui_automove_timer->stop();
	igt->m_robotApi->disconnectrobot();
	cubeSidelength = 50.0;
	cube_point_number = 0;
	initial_point_ready = false;
}

void StaubilWidget::on_PowerOn_clicked()
{
	igt->m_robotApi->poweron();
}

void StaubilWidget::on_PowerOff_clicked()
{
	igt->m_robotApi->poweroff();
}

void StaubilWidget::on_BackTestPose_clicked()
{
	igt->m_robotApi->requestrealtimedata();
	ui_moveInitial_realtime_data = igt->m_robotApi->realtime_data;
	ui_moveInitial_realtime_data.pose.x += 7;
	igt->m_robotApi->movel(ui_moveInitial_realtime_data.pose.x, ui_moveInitial_realtime_data.pose.y, ui_moveInitial_realtime_data.pose.z,
		ui_moveInitial_realtime_data.pose.a, ui_moveInitial_realtime_data.pose.b, ui_moveInitial_realtime_data.pose.c);
}

void StaubilWidget::on_Testpose_clicked()
{
	igt->m_robotApi->requestrealtimedata();
	ui_moveInitial_realtime_data = igt->m_robotApi->realtime_data;
	ui_moveInitial_realtime_data.pose.x -= 7;
	igt->m_robotApi->movel(ui_moveInitial_realtime_data.pose.x, ui_moveInitial_realtime_data.pose.y, ui_moveInitial_realtime_data.pose.z,
		ui_moveInitial_realtime_data.pose.a, ui_moveInitial_realtime_data.pose.b, ui_moveInitial_realtime_data.pose.c);
}


void StaubilWidget::ui_isRobotConnected(bool p)
{
	if (p)
	{
		ui->robotconnectstate->setText("Robot Connected");
		qDebug() << "Robot Connected";
		ui_realtimedata_timer->start(100);
		igt->SetRunLancetThread(true);
	}
	else
	{
		ui->robotconnectstate->setText("Robot Disconnect");
		ui_realtimedata_timer->stop();
		qDebug() << "Robot Disconnect";
		igt->SetRunLancetThread(false);
	}
}

void StaubilWidget::ui_isRobotError(bool p)
{
	if (p)
		ui->errorstate->setText("Error!!");
	else
		ui->errorstate->setText("NO Error");
}

void StaubilWidget::ui_isRobotBlocked(bool p)
{
	//if(p)
	//    ui->blockedstate->setText("Blocked: YES");
	//else
	//    ui->blockedstate->setText("Blocked: NO");
}

void StaubilWidget::ui_RobotWorkMode(RobotWorkMode wm)
{
	switch (wm)
	{
	case CONTROL_MODE_UNKNOWN:
		emit signalRobotWorkMode(CONTROL_MODE_UNKNOWN);
		ui->robotworkmode->setText("WorkMode: UNKNOWN");
		break;
	case CONTROL_MODE_AUTO:
		emit signalRobotWorkMode(CONTROL_MODE_AUTO);
		//IGTController::staubilMode = CONTROL_MODE_AUTO;
		ui->robotworkmode->setText("WorkMode: NORMAL");
		break;
	case CONTROL_MODE_FORCE_ATI_MODE1:
		emit signalRobotWorkMode(CONTROL_MODE_FORCE_ATI_MODE1);
		//IGTController::staubilMode = CONTROL_MODE_FORCE_ATI_MODE1;
		ui->robotworkmode->setText("WorkMode: ATI_MODE");
		break;
	default:
		break;
	}
}

void StaubilWidget::ui_ATIMode(int i, double rx, double ry, double rz)
{
	switch (i)
	{
	case 1:
		emit signalRobotWorkMode(CONTROL_MODE_FORCE_ATI_MODE1);
		ui->robotworkmode->setText("WorkMode: ATI_MODE1");
		break;
	case 2:
		emit signalRobotWorkMode(CONTROL_MODE_FORCE_ATI_MODE2);
		ui->robotworkmode->setText("WorkMode: ATI_MODE2");
		break;
	default:
		break;
	}
}


void StaubilWidget::on_StartPollingData_clicked()
{
	if (igt->m_robotApi->robot_state.isRobotConnected)
	{
		if (igt->m_robotApi->robot_state.isRobotPowerOn)
		{
			igt->m_robotApi->movej(-45.0, 60.0, 30.0, 0.0, 90.0, 0.0);
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
	move_rand_index = 1;
	//staubli->stoppollingdata();
	qDebug() << "Stop Move Random!";
	ui_automove_timer->stop();
}

void StaubilWidget::on_GetTcpInfo_clicked()
{
	igt->m_robotApi->gettcpinfo();
	igt->m_robotApi->requestcommandfeedbackdata();
	ui_commandfeedback_data = igt->m_robotApi->cammand_feedback_data;
	qDebug() << "Tcp Type: " << ui_commandfeedback_data.tcpinfo.tcptype;
}

void StaubilWidget::on_SetTcpInfo_clicked()
{
	ui_tcp.tcptype = TcpType::TCP_ROD_DRILL;
	igt->m_robotApi->settcpinfo(ui_tcp);
}

void StaubilWidget::on_setCubesidelength_valueChanged(double arg1)
{
	cubeSidelength = arg1 * 10.0;
	qDebug() << "Set Cubesidelength " << arg1 << "cm";
}

void StaubilWidget::on_setInitialPoint_clicked()
{
	//igt->m_robotApi->setspeed(70);
	//igt->m_robotApi->requestrealtimedata();
	//ui_moveInitial_realtime_data = igt->m_robotApi->realtime_data;
	//initial_point_ready = true;
	//cube_point_number = 0;
	//qDebug() << "Set InitialPoint finished!";
	igt->m_robotApi->setworkmode(2);
	igt->m_robotApi->movep(1, 0, 0, 0, 0, 0);
	qDebug() << "on_setInitialPoint_clicked";
}

void StaubilWidget::on_nextPoint_clicked()
{
	if (!initial_point_ready)
		qDebug() << "SetInitialPoint First!!!";
	else
	{
		switch (cube_point_number)
		{
		case 0: //PC1
			qDebug() << ++cube_point_number;
			igt->m_robotApi->movej(igt->m_robotApi->realtime_data.joints.j1, 60.0, 30.0, 0.0, 90.0, 0.0);
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
			igt->m_robotApi->movel(ui_moveInitial_realtime_data.pose.x + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.y + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.z + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.a,
				ui_moveInitial_realtime_data.pose.b,
				ui_moveInitial_realtime_data.pose.c);
			break;
		case 2: //PC3
			qDebug() << ++cube_point_number;
			//staubli->movej(true, ui_moveInitial_realtime_data.pose.x+cubeSidelength,
			igt->m_robotApi->movel(ui_moveInitial_realtime_data.pose.x + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.y + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.z + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.a,
				ui_moveInitial_realtime_data.pose.b,
				ui_moveInitial_realtime_data.pose.c);
			break;
		case 3: //PC4
			qDebug() << ++cube_point_number;
			//staubli->movej(true, ui_moveInitial_realtime_data.pose.x,
			igt->m_robotApi->movel(ui_moveInitial_realtime_data.pose.x + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.y + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.z + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.a,
				ui_moveInitial_realtime_data.pose.b,
				ui_moveInitial_realtime_data.pose.c);
			break;
		case 4: //PC5
			qDebug() << ++cube_point_number;
			igt->m_robotApi->movep(ui_moveInitial_realtime_data.pose.x + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.y + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.z + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.a + qrand() % 10 - 5,
				ui_moveInitial_realtime_data.pose.b + qrand() % 10 - 5,
				ui_moveInitial_realtime_data.pose.c + qrand() % 10 - 5);
			break;
		case 5:
			qDebug() << ++cube_point_number;
			igt->m_robotApi->movep(ui_moveInitial_realtime_data.pose.x + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.y + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.z + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.a + qrand() % 10 - 5,
				ui_moveInitial_realtime_data.pose.b + qrand() % 10 - 5,
				ui_moveInitial_realtime_data.pose.c + qrand() % 10 - 5);
			break;
		case 6:
			qDebug() << ++cube_point_number;
			igt->m_robotApi->movep(ui_moveInitial_realtime_data.pose.x + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.y + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.z + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.a + qrand() % 10 - 5,
				ui_moveInitial_realtime_data.pose.b + qrand() % 10 - 5,
				ui_moveInitial_realtime_data.pose.c + qrand() % 10 - 5);
			break;
		case 7:
			qDebug() << ++cube_point_number;
			igt->m_robotApi->movep(ui_moveInitial_realtime_data.pose.x + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.y + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.z + qrand() % 50 - 25,
				ui_moveInitial_realtime_data.pose.a + qrand() % 10 - 5,
				ui_moveInitial_realtime_data.pose.b + qrand() % 10 - 5,
				ui_moveInitial_realtime_data.pose.c + qrand() % 10 - 5);
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

void StaubilWidget::on_GetLimitData_clicked()
{
	qDebug() << "Get Traction Limit Data";
	igt->m_robotApi->getlimitdata();
}

void StaubilWidget::on_SetLimitData_clicked()
{
	igt->m_robotApi->requestrealtimedata();
	RobotRealtimeData l_realtime_data = igt->m_robotApi->realtime_data;
	TractionLimit l;
	l.init();
	l.angle = 30;
	l.p1 = l_realtime_data.pose;
	l.p2 = l_realtime_data.pose;
	l.p1.z -= 50; l.p2.z += 50;
	igt->m_robotApi->setlimitdata(l);
}

void StaubilWidget::on_Foldrobot_clicked()
{
	igt->m_robotApi->movej(120.0, 0.0, 135.0, 0.0, 45.0, 0.0);
}

void StaubilWidget::on_Unfolrobot_clicked()
{
	igt->m_robotApi->movej(-45.0, 0.0, 135.0, 0.0, 45.0, 0.0);
}

void StaubilWidget::on_positionInit_left_clicked()
{
	qDebug() << "on_positionInit_left_clicked";
	igt->m_robotApi->setworkmode(2);
	igt->m_robotApi->movep(2,0,0,0,0,0);
}

void StaubilWidget::on_positionInit_right_clicked()
{
	qDebug() << "on_positionInit_right_clicked";
	igt->m_robotApi->setworkmode(2);
	igt->m_robotApi->movep(3, 0, 0, 0, 0, 0);
}

void StaubilWidget::on_kuka_mode_1_clicked()
{
	igt->m_robotApi->setworkmode(1);
	MITK_INFO << "on_kuka_mode_1_clicked";
}

void StaubilWidget::on_kuka_mode_3_clicked()
{
	igt->m_robotApi->setworkmode(3);
	MITK_INFO << "on_kuka_mode_3_clicked";
}

void StaubilWidget::on_SetSpeed_clicked()
{	

}

void StaubilWidget::on_setIO_clicked()
{
	if (!isIO)
	{
		igt->m_robotApi->setio(1, 1);
		isIO = true;
		this->ui->setIO->setText("设置骨钻(关)");
		IGTController::issetio = false;
	}
	else
	{
		igt->m_robotApi->setio(1, 0);
		isIO = false;
		this->ui->setIO->setText("设置骨钻(开)");
		IGTController::issetio = true;
	}
}

//void StaubilWidget::on_pushButton_clicked()
//{
//	QFile fileati("./RobotData/RobotATIData.txt");
//	if (fileati.open(QIODevice::Append | QIODevice::Text))
//	{
//		QTextStream out(&fileati);
//		out << "Vx" << "," << "Vy" << "," << "Vz" << "," << "Rx" << "," << "Ry" << "," << "Rz" << "," << endl;
//		for (int i = 1; i < ATIValue.size(); i++)
//		{
//			out << ATIValue[i] << " , ";
//			if ( (i % 6) == 0)
//			{
//				out << endl;
//			}
//		}
//		fileati.close();
//	}
//	QFile filejiont("./RobotData/RobotJointData.txt");
//	if (filejiont.open(QIODevice::Append | QIODevice::Text))
//	{
//		QTextStream out(&filejiont);
//		out << "Vx" << "," << "Vy" << "," << "Vz" << "," << "Rx" << "," << "Ry" << "," << "Rz" << "," << endl;
//		for (int i = 1; i < JointValue.size(); i++)
//		{
//			out << JointValue[i] << " , ";
//			if ((i % 6) == 0)
//			{
//				out << endl;
//			}
//		}
//		filejiont.close();
//	}
//	QFile filetcp("./RobotData/RobotTCPData.txt");
//	if (filetcp.open(QIODevice::Append | QIODevice::Text))
//	{
//		QTextStream out(&filetcp);
//		out << "Vx" << "," << "Vy" << "," << "Vz" << "," << "Rx" << "," << "Ry" << "," << "Rz" << "," << endl;
//		for (int i = 1; i < TCPValue.size(); i++)
//		{		
//			out << TCPValue[i] << " , ";
//			if ((i % 6) == 0)
//			{
//				out << endl;
//			}
//		}
//		filetcp.close();
//	}
//}
//

//void StaubilWidget::on_addJ1_pressed()
//{
//	igt->m_robotApi->handleMove(1, 1);
//}
//
//void StaubilWidget::on_addJ1_released()
//{
//	igt->m_robotApi->handleMove(1, 0);
//}
//
//void StaubilWidget::on_addJ2_pressed()
//{
//	igt->m_robotApi->handleMove(2, 1);
//}
//
//void StaubilWidget::on_addJ2_released()
//{
//	igt->m_robotApi->handleMove(2, 0);
//}
//
//void StaubilWidget::on_addJ3_pressed()
//{
//	igt->m_robotApi->handleMove(3, 1);
//}
//
//void StaubilWidget::on_addJ3_released()
//{
//	igt->m_robotApi->handleMove(3, 0);
//}
//
//void StaubilWidget::on_addJ4_pressed()
//{
//	igt->m_robotApi->handleMove(4, 1);
//}
//
//void StaubilWidget::on_addJ4_released()
//{
//	igt->m_robotApi->handleMove(4, 0);
//}
//
//void StaubilWidget::on_addJ5_pressed()
//{
//	igt->m_robotApi->handleMove(5, 1);
//}
//
//void StaubilWidget::on_addJ5_released()
//{
//	igt->m_robotApi->handleMove(5, 0);
//}
//
//void StaubilWidget::on_addJ6_pressed()
//{
//	igt->m_robotApi->handleMove(6, 1);
//}
//
//void StaubilWidget::on_addJ6_released()
//{
//	igt->m_robotApi->handleMove(6, 0);
//}
//
//void StaubilWidget::on_subJ1_pressed()
//{
//	igt->m_robotApi->handleMove(1, -1);
//}
//
//void StaubilWidget::on_subJ1_released()
//{
//	igt->m_robotApi->handleMove(1, 0);
//}
//
//void StaubilWidget::on_subJ2_pressed()
//{
//	igt->m_robotApi->handleMove(2, -1);
//}
//
//void StaubilWidget::on_subJ2_released()
//{
//	igt->m_robotApi->handleMove(2, 0);
//}
//
//void StaubilWidget::on_subJ3_pressed()
//{
//	igt->m_robotApi->handleMove(3, -1);
//}
//
//void StaubilWidget::on_subJ3_released()
//{
//	igt->m_robotApi->handleMove(3, 0);
//}
//
//void StaubilWidget::on_subJ4_pressed()
//{
//	igt->m_robotApi->handleMove(4, -1);
//}
//
//void StaubilWidget::on_subJ4_released()
//{
//	igt->m_robotApi->handleMove(4, 0);
//}
//
//void StaubilWidget::on_subJ5_pressed()
//{
//	igt->m_robotApi->handleMove(5, -1);
//}
//
//void StaubilWidget::on_subJ5_released()
//{
//	igt->m_robotApi->handleMove(5, 0);
//}
//
//void StaubilWidget::on_subJ6_pressed()
//{
//	igt->m_robotApi->handleMove(6, -1);
//}
//
//void StaubilWidget::on_subJ6_released()
//{
//	igt->m_robotApi->handleMove(6, 0);
//}
