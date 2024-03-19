/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef RobotView_h
#define RobotView_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_StaubilWidget.h"
#include "robotapi.h"
//#include "Plugins/org.mitk.lancet.robot/ui_StaubilWidget.h"

/**
  \brief RobotView

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class RobotView : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;
  virtual ~RobotView();
protected:
	
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;
signals:
	  void signalRobotWorkMode(unsigned int);
private slots:

  void update_realtime_data();

  void robot_auto_move();

  void on_SocketConnect_clicked();

  void on_SocketDisConnect_clicked();

  void on_PowerOn_clicked();

  void on_PowerOff_clicked();

  void on_BackTestPose_clicked();

  void on_Testpose_clicked();

  void ui_isRobotConnected(bool p);

  void ui_isRobotError(bool p) const;

  void ui_isRobotBlocked(bool p);

  void ui_RobotWorkMode(RobotWorkMode wm);

  void ui_ATIMode(int i, double rx, double ry, double rz);

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

private:
	static ITK_THREAD_RETURN_TYPE HeartBeatThread(void* pInfoStruct);
private:
  RobotRealtimeData ui_realtime_data;
  RobotRealtimeData ui_moveInitial_realtime_data;
  RobotCommandFeedbackData ui_commandfeedback_data;
  TcpInfo ui_tcp;
  QTimer* ui_realtimedata_timer;
  QTimer* ui_automove_timer;

  int move_rand_index;
  double cubeSidelength;
  int cube_point_number;
  bool initial_point_ready;

  bool isIO;
  int settcp;

  //test
  std::vector<double> ATIValue;
  std::vector<double> TCPValue;
  std::vector<double> JointValue;

  int Moveindex;
  double A[6];
  double B[6];
  double J[6];

  RobotApi* m_robotApi;

  Ui::StaubilWidget m_Controls;

  itk::MultiThreader::Pointer m_MultiThreader;
  ///< creates tracking thread that continuously polls serial interface for new tracking data
  int m_ThreadID{0};                                  ///< ID of tracking thread
  bool m_heartBeat{ false };
  bool m_runTread{ false };
};

#endif // RobotView_h
