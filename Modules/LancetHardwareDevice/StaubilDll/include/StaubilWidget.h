#ifndef WIDGET_H
#define WIDGET_H

#include "robotapi.h"
#include <QWidget>
#include <QTime>
#include <QTimer>
#include "Windows.h"

class IGTController;
namespace Ui {
	class StaubilWidget;
}

class StaubilWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StaubilWidget(QWidget *parent = 0);
	~StaubilWidget();

private:
	Ui::StaubilWidget *ui;

	IGTController *igt;
	//RobotApi *staubli;
	RobotRealtimeData ui_realtime_data;
	RobotRealtimeData ui_moveInitial_realtime_data;
	RobotCommandFeedbackData ui_commandfeedback_data;
	TcpInfo ui_tcp;
	QTimer *ui_realtimedata_timer;
	QTimer *ui_automove_timer;

	int move_rand_index;
	double cubeSidelength;
	int cube_point_number;
	bool initial_point_ready;

	//bool islimit;
	//bool isspeed;

	bool isIO;

	//test
	std::vector<double> ATIValue;
	std::vector<double> TCPValue;
	std::vector<double> JointValue;

signals:
	void signalRobotWorkMode(unsigned int);

private slots:
	void ui_isRobotConnected(bool p);
	//void ui_isRobotPowerOn(bool p);
	void ui_isRobotError(bool p);
	void ui_isRobotBlocked(bool p);
	void ui_RobotWorkMode(RobotWorkMode wm);
	void ui_ATIMode(int, double, double, double);
	void update_realtime_data();
	void robot_auto_move();
	void on_SocketConnect_clicked();
	void on_SocketDisConnect_clicked();
	void on_PowerOn_clicked();
	void on_PowerOff_clicked();
	void on_BackTestPose_clicked();
	void on_Testpose_clicked();
	void on_StartPollingData_clicked();
	void on_StopPollingData_clicked();
	void on_GetTcpInfo_clicked();
	void on_SetTcpInfo_clicked();
	void on_setCubesidelength_valueChanged(double arg1);
	void on_setInitialPoint_clicked();
	void on_nextPoint_clicked();
	void on_GetLimitData_clicked();
	void on_SetLimitData_clicked();
	void on_Foldrobot_clicked();
	void on_Unfolrobot_clicked();
	void on_positionInit_left_clicked();
	void on_positionInit_right_clicked();
	void on_kuka_mode_1_clicked();
	void on_kuka_mode_3_clicked();
	void on_SetSpeed_clicked();

	void on_setIO_clicked();
	////////////////////////////////////////////////////////////////////////////


	////add
	//void on_addJ1_pressed();
	//void on_addJ1_released();
	//void on_addJ2_pressed();
	//void on_addJ2_released();
	//void on_addJ3_pressed();
	//void on_addJ3_released();
	//void on_addJ4_pressed();
	//void on_addJ4_released();
	//void on_addJ5_pressed();
	//void on_addJ5_released();
	//void on_addJ6_pressed();
	//void on_addJ6_released();
	////sub
	//void on_subJ1_pressed();
	//void on_subJ1_released();
	//void on_subJ2_pressed();
	//void on_subJ2_released();
	//void on_subJ3_pressed();
	//void on_subJ3_released();
	//void on_subJ4_pressed();
	//void on_subJ4_released();
	//void on_subJ5_pressed();
	//void on_subJ5_released();
	//void on_subJ6_pressed();
	//void on_subJ6_released();
};

#endif // WIDGET_H
