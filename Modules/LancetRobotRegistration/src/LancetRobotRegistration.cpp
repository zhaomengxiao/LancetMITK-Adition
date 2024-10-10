#include <LancetRobotRegistration.h>

LancetRobotRegistration::LancetRobotRegistration(AbstractRobot* aRobot, AbstractCamera* aCamera)
{
	m_Robot = aRobot;  
	m_Camera = aCamera; 
}

void LancetRobotRegistration::SetTCPToFlange()
{
	m_Robot->SetTCPToFlange();
}

void LancetRobotRegistration::RecordInitialPos()
{
	m_Robot->RecordInitialPos();
}

void LancetRobotRegistration::GoToInitialPos()
{
	m_Robot->GoToInitialPos();
}

void LancetRobotRegistration::xp()
{
	double templeArray[3] = { 1, 0.0, 0.0 };
	m_Robot->Translate(templeArray, Distance);
}

void LancetRobotRegistration::yp()
{
	double templeArray[3] = { 0.0, 1, 0.0 };
	m_Robot->Translate(templeArray, Distance);
}

void LancetRobotRegistration::zp()
{
	double templeArray[3] = { 0.0, 0.0, 1 };
	m_Robot->Translate(templeArray, Distance);
}

void LancetRobotRegistration::xm()
{
	double templeArray[3] = { 1, 0.0, 0.0 };
	m_Robot->Translate(templeArray, Distance);
}

void LancetRobotRegistration::ym()
{
	double templeArray[3] = { 0.0, 1, 0.0 };
	m_Robot->Translate(templeArray, Distance);
}

void LancetRobotRegistration::zm()
{
	double templeArray[3] = { 0.0, 0.0, 1 };
	m_Robot->Translate(templeArray, Distance);
}

void LancetRobotRegistration::rxp()
{
	double templeArray[3] = { 1, 0.0, 0.0 };
	m_Robot->Rotate(templeArray, Angle);
}

void LancetRobotRegistration::ryp()
{
	double templeArray[3] = { 0.0, 1, 0.0 };
	m_Robot->Rotate(templeArray, Angle);
}

void LancetRobotRegistration::rzp()
{
	double templeArray[3] = { 0.0, 0.0, 1 };
	m_Robot->Rotate(templeArray, Angle);
}

void LancetRobotRegistration::rxm()
{
	double templeArray[3] = { 1, 0.0, 0.0 };
	m_Robot->Rotate(templeArray, Angle);
}

void LancetRobotRegistration::rym()
{
	double templeArray[3] = { 0.0, 1, 0.0 };
	m_Robot->Rotate(templeArray, Angle);
}

void LancetRobotRegistration::rzm()
{
	double templeArray[3] = { 0.0, 0.0, 1 };
	m_Robot->Rotate(templeArray, Angle);
}

void LancetRobotRegistration::captureRobot()
{
	if (m_RobotRegistration.PoseCount() < 5) //The first five translations, 
	{
		CapturePose(true);
	}
	else if (m_RobotRegistration.PoseCount() >= 5 && m_RobotRegistration.PoseCount() < 10)
	{
		CapturePose(false);
	}
	else
	{
		vtkNew<vtkMatrix4x4> robotEndToFlangeMatrix;
		m_RobotRegistration.GetTCPmatrix(robotEndToFlangeMatrix);
		robotEndToFlangeMatrix->Invert();
		std::cout << "Registration RMS: " << m_RobotRegistration.RMS() << std::endl;
	}
}

void LancetRobotRegistration::CapturePose(bool translationOnly)
{
	std::cout << "CapturePose" <<std::endl;
	//get T_BaseToFlanger
	vtkSmartPointer<vtkMatrix4x4>T_BaseToFlange = m_Robot->GetBaseToFlange();
	//get T_CameraToEnd
	vtkSmartPointer<vtkMatrix4x4>T_CameraToEnd = m_Camera->GetToolMatrixByName("RobotEndRF");
	//get T_BaseRFToCamera
	vtkSmartPointer<vtkMatrix4x4>T_BaseRFToCamera;
	vtkMatrix4x4::Invert(m_Camera->GetToolMatrixByName("RobotBaseRF"), T_BaseRFToCamera);
	//get T_BaseRFToEnd
	vtkSmartPointer<vtkMatrix4x4> T_BaseRFToEnd;

	vtkMatrix4x4::Multiply4x4(T_BaseRFToCamera, T_CameraToEnd, T_BaseRFToEnd);
	//Robotic arm registration
	m_RobotRegistration.AddPoseWithVtkMatrix(T_BaseToFlange, T_BaseRFToEnd, translationOnly);
}

void LancetRobotRegistration::waitMove()
{
	std::cout << "waitMove" << std::endl;
	bool bDone = false;
	while (!bDone)
	{
		int nRet = HRIF_IsMotionDone(0, 0, bDone);
		if (bDone == false)
		{
			QThread::msleep(10);
			QApplication::processEvents();
			std::cout << "is still moving" << std::endl;
		}
		else
		{
			int ret = HRIF_GrpStop(0, 0);
			std::cout << "robot is stop please continue" << std::endl;
			return;
		}
	}
}

void LancetRobotRegistration::autoCollection()
{
	std::cout << "Starting automatic registration!" << std::endl;
	int moveCount = 1;
	m_RobotRegistration.RemoveAllPose();
	if (isAutoCollectionFlag)
	{
		m_Robot->SetTCPToFlange();
		isAutoCollectionFlag = FALSE;
	}
	while (moveCount <= 10)
	{
		m_Robot->GoToInitialPos();
		waitMove();
		switch (moveCount) {
		case 1:
			xp();
			waitMove();
			QThread::msleep(200);

			captureRobot();
			break;
		case 2:
			yp();
			waitMove();
			QThread::msleep(200);

			captureRobot();
			break;
		case 3:
			zp();
			waitMove();
			QThread::msleep(200);

			captureRobot();
			break;
		case 4:
			xm();
			waitMove();
			QThread::msleep(200);

			captureRobot();
			break;
		case 5:
			ym();
			waitMove();
			QThread::msleep(200);

			captureRobot();
			break;
		case 6:
			rxp();
			waitMove();
			QThread::msleep(200);

			captureRobot();
			break;
		case 7:
			ryp();
			waitMove();
			QThread::msleep(200);

			captureRobot();
			break;
		case 8:
			rzp();
			waitMove();
			QThread::msleep(200);

			captureRobot();
			break;
		case 9:
			rxm();
			waitMove();
			QThread::msleep(200);

			captureRobot();
			break;
		case 10:
			rym();
			waitMove();
			QThread::msleep(200);

			captureRobot();
			break;
		default:
			return;
		}
		QThread::msleep(200);
		moveCount++;
	}
	if (m_RobotRegistration.PoseCount() == 10)
	{
		std::cout << "Fully automatic registration successed!" << std::endl;
	}
}

