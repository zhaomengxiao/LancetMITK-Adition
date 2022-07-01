#ifndef WIDGET_H
#define WIDGET_H

#include "robotapi_include/robotapi.h"
#include <QWidget>
#include <QTime>
#include <QTimer>
#include "Windows.h"

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
    RobotApi *staubli;
    RobotRealtimeData ui_realtime_data;
    RobotRealtimeData ui_moveInitial_realtime_data;
    RobotCommandFeedbackData ui_commandfeedback_data;
    TcpInfo ui_tcp;
    QTimer *ui_realtimedata_timer;
    QTimer *ui_automove_timer;

    double cubeSidelength;
    int cube_point_number;
    bool initial_point_ready;

signals:


private slots:

    void ui_isRobotConnected(bool p);

    void ui_isRobotPowerOn(bool p);

    void ui_isRobotError(bool p);

    void ui_isRobotBlocked(bool p);

    void ui_RobotWorkMode(RobotWorkMode wm);

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
    void on_printdataonce_clicked();
    void on_ati_init_clicked();
    void on_GetLimitData_clicked();
    void on_SetLimitData_clicked();
    void on_Foldrobot_clicked();
    void on_Unfolrobot_clicked();
    void on_nolimit_clicked();
};

#endif // WIDGET_H
