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
#include <kukaRobotDevice.h>

#include "ui_StaubilWidget.h"
#include "udpsocketrobotheartbeat.h"
#include "kukaRobotAPI/kukaRobotAPI.h"
/**
  \brief RobotView

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
enum class WorkingState
{
	ON,
	OFF
};

class RobotView : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;
  ~RobotView() override;
protected:
	
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;

public slots:
	void ConnectDevice();
	void DisConnectDevice();

	void UpdateToolPosition();

  void SelfCheck();

  void SendCommand();

  void StartUDP();
	
private:
  void threadUDP_RobotInfo();

  lancet::KukaRobotDevice::Pointer m_device;
  // UdpSocketRobotHeartbeat m_udp;
  // Poco::Net::DatagramSocket m_udpSocket_RobotInfo;
  // Poco::Net::ServerSocket m_tcpSocket_RobotCommand;
  // Poco::Net::StreamSocket ss;
   std::thread m_Thread;
  // std::thread m_Thread2;
  lancet::KukaRobotAPI m_KukaRobotApi;
	QTimer m_timer;
	
	Ui::StaubilWidget m_Controls;
};

#endif // RobotView_h