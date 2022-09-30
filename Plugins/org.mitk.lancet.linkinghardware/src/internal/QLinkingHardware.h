/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef QLinkingHardware_h
#define QLinkingHardware_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_QLinkingHardwareControls.h"

/**
  \brief QLinkingHardware

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class QLinkingHardware : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;
  ~QLinkingHardware() override;
  void setStartHardware(int staubli, int ndi);


	virtual void CreateQtPartControl(QWidget* parent) override;
	virtual void SetFocus() override;
	//void setStartHardware(int staubli, int ndi);

Q_SIGNALS:
	void signalHardWareFinished();
	void signalStatus(int, int);

private slots:
	void on_pushButton_auto_clicked();
	void on_pushButton_success_clicked();

public Q_SLOTS:
	void RobotStatus(int i);
	void NDIStatus(int i);
private:
	QMetaObject::Connection RobotStatusConnect;
	QMetaObject::Connection NDIStatusConnect;
	QTimer* MyConnect;
	bool isauto;
	int staubliStatus;
	int ndiStatus;
protected:
	Ui::QLinkingHardwareControls m_Controls;
};

#endif // QLinkingHardware_h
