#pragma once
#ifndef LANCETROBOTREGISTRATION_h
#define LANCETROBOTREGISTRATION_h
#endif

#include "AbstractCamera.h"
#include "AbstractRobot.h"
#include "AimCamera.h"
#include "LancetHansRobot.h"
#include "robotRegistration.h"
class LancetRobotRegistration 
{
public:
	LancetRobotRegistration(AbstractRobot* aRobot, AbstractCamera* aCamera);
	void SetTCPToFlange();
	void RecordInitialPos();
	void GoToInitialPos();
	void xp();
	void yp();
	void zp();
	void xm();
	void ym();
	void zm();
	void rxp();
	void ryp();
	void rzp();
	void rxm();
	void rym();
	void rzm();
	void captureRobot();
	void CapturePose(bool);
	void waitMove();
	void autoCollection();
public:
	int Distance = 50;
	int Angle = 15;
private:
	AbstractRobot* m_Robot;
	AbstractCamera* m_Camera;
	RobotRegistration m_RobotRegistration;
	bool isAutoCollectionFlag;
};