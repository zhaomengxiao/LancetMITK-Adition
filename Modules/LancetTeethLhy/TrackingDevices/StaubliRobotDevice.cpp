#include "StaubliRobotDevice.h"

#include "mitkIGTTimeStamp.h"

//#include "lancetKukaTrackingDeviceTypeInformation.h"

#include "unitls.h"
#include <RRTC.h>



#define d2r 57.2957795130

namespace lancet
{


	QString sFootAllowed = "0";
	bool isShortcutKeyDown = false;

	bool StaubliRobotDevice::OpenConnection()
	{
		m_RobotApi.connectrobot();
		return true;
	}

	bool StaubliRobotDevice::CloseConnection()
	{
		m_RobotApi.disconnectrobot();
		SetState(TrackingDeviceState::Setup);
		return true;
	}

	bool  StaubliRobotDevice::StartTracking()
	{
		if (this->GetState() != Ready)
			return false;

		this->SetState(Tracking);          // go to mode Tracking
		this->m_StopTrackingMutex.lock(); // update the local copy of m_StopTracking
		this->m_StopTracking = false;
		this->m_StopTrackingMutex.unlock();

		m_Thread = std::thread(&StaubliRobotDevice::ThreadStartTracking, this);//todo start tracking agine crash here 
		// start a new thread that executes the TrackTools() method
		mitk::IGTTimeStamp::GetInstance()->Start(this);
		MITK_INFO << "lancet kuka robot start tracking";
		return true;
	}

	mitk::TrackingTool* StaubliRobotDevice::GetTool(unsigned toolNumber) const
	{
		std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
		if (toolNumber < m_StaubliEndEffectors.size())
			return m_StaubliEndEffectors.at(toolNumber);
		return nullptr;
	}

	mitk::TrackingTool* StaubliRobotDevice::GetToolByName(std::string name) const
	{
		std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
		auto end = m_StaubliEndEffectors.end();
		for (auto iterator = m_StaubliEndEffectors.begin(); iterator != end; ++iterator)
			if (name.compare((*iterator)->GetToolName()) == 0)
				return *iterator;
		return nullptr;
	}

	mitk::TrackingTool* StaubliRobotDevice::GetInternalTool()
	{
		return m_StaubliEndEffectors[0]; //only 1 end effector tracked;
	}

	mitk::TrackingTool* StaubliRobotDevice::AddTool(const char* toolName, const char* fileName)
	{
		mitk::TrackingTool::Pointer t = mitk::TrackingTool::New();
		t->SetToolName(toolName);
		//todo SetToolTipPosition to TCP
		if (this->InternalAddTool(t) == false)
		{
			MITK_ERROR << "InternalAddTool failed";
			return nullptr;
		}

		return t.GetPointer();
	}

	unsigned StaubliRobotDevice::GetToolCount() const
	{
		return static_cast<unsigned int>(this->m_StaubliEndEffectors.size());
	}

	void StaubliRobotDevice::TrackTools()
	{
		m_RobotApi.requestrealtimedata();
		
		m_TrackingData[0] = m_RobotApi.realtime_data.pose.x;
		m_TrackingData[1] = m_RobotApi.realtime_data.pose.y;
		m_TrackingData[2] = m_RobotApi.realtime_data.pose.z;

		m_TrackingData[3] = m_RobotApi.realtime_data.pose.rx;
		m_TrackingData[4] = m_RobotApi.realtime_data.pose.ry;
		m_TrackingData[5] = m_RobotApi.realtime_data.pose.rz;
	}

	std::array<double, 6> StaubliRobotDevice::GetTrackingData()
	{
		return m_TrackingData;
	}

	bool StaubliRobotDevice::RequestExecOperate(const QString& funname, const QStringList& param)
	{
		auto logPrintIn = [](const QStringList& stringList) 
		{
			QString list;

			for (auto string : stringList)
			{
				list += string + ";";
			}

			return list.toStdString();
		};
		MITK_INFO << "log.funcname " << funname.toStdString() << "; log.param " << logPrintIn(param);
    if(funname.indexOf("movep") != -1  && param.size() == 6)
    {
        this->m_RobotApi.movep(param.at(0).toDouble(),
                              param.at(1).toDouble(),
                              param.at(2).toDouble(),
                              param.at(3).toDouble(),
                              param.at(4).toDouble(),
                              param.at(5).toDouble());
        return true;
    }
    if(funname.indexOf("movej") != -1  && param.size() == 6)
    {
        this->m_RobotApi.movej(param.at(0).toDouble(),
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
        this->m_RobotApi.atimode(param.at(0).toInt());
        return true;
    }
    else if(funname.indexOf("movel") != -1  && param.size() == 6){
        this->m_RobotApi.movel(param.at(0).toDouble(),
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

        this->m_RobotApi.settcpinfo(ui_tcp);
        return true;
    }

    else if(funname.indexOf("poweron")!=-1)
    {
        this->m_RobotApi.poweron();
    }
    else if(funname.indexOf("poweroff")!=-1){
        this->m_RobotApi.poweroff();
    }
    else if(funname.indexOf("update") != -1)
    {
        //emit this->updateToolPosition(this->devicename(), this->realtimeData());
    }
	else if (funname.indexOf("start") != -1)
	{
		this->StartTracking();
	}
	else if (funname.indexOf("stop") != -1)
	{
		this->StopTracking();
	}
	
		return false;
	}

	void StaubliRobotDevice::RobotMove(vtkMatrix4x4* T_robot, double result[9])
	{
		///err
		const double R[3][3] = { {T_robot->GetElement(0, 0), T_robot->GetElement(0, 1), T_robot->GetElement(0, 2)},
		  {T_robot->GetElement(1, 0), T_robot->GetElement(1, 1), T_robot->GetElement(1, 2)},
		  {T_robot->GetElement(2, 0), T_robot->GetElement(2, 1), T_robot->GetElement(2, 2) }
		};

		std::vector<double> abc = staublimatrix2angle(R);

		QString str1 = QString::number(T_robot->GetElement(0, 3), 'f', 5);
		QString str2 = QString::number(T_robot->GetElement(1, 3), 'f', 5);
		QString str3 = QString::number(T_robot->GetElement(2, 3), 'f', 5);
		QString str4 = QString::number(abc[0], 'f', 5);
		QString str5 = QString::number(abc[1], 'f', 5);
		QString str6 = QString::number(abc[2], 'f', 5);
		
		result[0] = T_robot->GetElement(0, 3);
		result[1] = T_robot->GetElement(1, 3);
		result[2] = T_robot->GetElement(2, 3);
		result[3] = abc[0];
		result[4] = abc[1];
		result[5] = abc[2];
		result[6] = abc[3];
		result[7] = abc[4];
		result[8] = abc[5];

		QString param1 = "1,0,0,0,0,0";
		RequestExecOperate("movej", param1.split(','));
		QThread::msleep(300);
		// TODO
		QString param = str1 + "," + str2 + "," + str3 + "," + str4 + "," + str5 + "," + str6;
		MITK_INFO << "Robot move to:" << param.toStdString();
		RequestExecOperate("movep", param.split(','));
		
	}

	std::vector<double> StaubliRobotDevice::staublimatrix2angle(const double matrix3x3[3][3])
	{
		Eigen::Matrix3d Re;

		Re << matrix3x3[0][0], matrix3x3[0][1], matrix3x3[0][2],
			matrix3x3[1][0], matrix3x3[1][1], matrix3x3[1][2],
			matrix3x3[2][0], matrix3x3[2][1], matrix3x3[2][2];


		// Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);
		Eigen::Vector3d eulerAngle = Re.eulerAngles(0, 1, 2);
		Eigen::Vector3d eulerAngle_1 = Re.eulerAngles(2, 1, 0);

		std::vector<double> i;

		i.push_back(180 * eulerAngle(0) / PI);
		i.push_back(180 * eulerAngle(1) / PI);
		i.push_back(180 * eulerAngle(2) / PI);


		i.push_back(180 * eulerAngle_1(0) / PI);
		i.push_back(180 * eulerAngle_1(1) / PI);
		i.push_back(180 * eulerAngle_1(2) / PI);

		return i;
	}

	//std::vector<double> StaubliRobotDevice::staublimatrix2angle(const double matrix3x3[3][3])
	//{
	//	double nx = matrix3x3[0][0];
	//	double ny = matrix3x3[0][1];
	//	double nz = matrix3x3[0][2];
	//	double ox = matrix3x3[1][0];
	//	double oy = matrix3x3[1][1];
	//	double oz = matrix3x3[1][2];
	//	double ax = matrix3x3[2][0];
	//	double ay = matrix3x3[2][1];
	//	double az = matrix3x3[2][2];

	//	//Calculate Rx��Ry��Rz
	//	//call Atan2(ax,sqrt(power(nx,2)+power(ox,2)),ry1)
	//	//double cry = (double)sqrt(nx * nx + ox * ox);
	//	double rx, ry, rz, rx2, ry2, rz2, rx_Origin, ry_Origin, rz_Origin;
	//	double FlagRx = 0.0, FlagRy = 0.0, FlagRz = 0.0;
	//	ry = (float)atan2(ax, (double)sqrt(nx * nx + ox * ox));
	//	if (ry == 90)
	//	{
	//		rx = 0;
	//		//call Atan2(ny,-nz,rz1)
	//		rz = (float)atan2(ny, -nz);
	//	}
	//	else if (ry == -90)
	//	{
	//		rx = 0;
	//		rz = (float)atan2(ny, nz);
	//		//call Atan2(ny,nz,rz1);
	//	}
	//	else
	//	{
	//		//call Atan2(-ay/cos(ry1),az/cos(ry1),rx1)
	//		//call Atan2(-ox/cos(ry1),nx/cos(ry1),rz1)
	//		rx = (float)atan2(-ay / cos(ry), az / cos(ry));
	//		rz = (float)atan2(-ox / cos(ry), nx / cos(ry));
	//	}

	//	//call Atan2(ax,-sqrt(power(nx,2)+power(ox,2)),ry2)
	//	ry2 = (float)atan2(ax, -(double)sqrt(nx * nx + ox * ox));
	//	if (ry2 == 90)
	//	{
	//		rx2 = 0;
	//		//call Atan2(ny,-nz,rz1)
	//		rz2 = (float)atan2(ny, -nz);
	//	}
	//	else if (ry2 == -90)
	//	{
	//		rx2 = 0;
	//		rz2 = (float)atan2(ny, nz);
	//		//call Atan2(ny,nz,rz1);
	//	}
	//	else
	//	{
	//		//call Atan2(-ay/cos(ry1),az/cos(ry1),rx1)
	//		//call Atan2(-ox/cos(ry1),nx/cos(ry1),rz1)
	//		rx2 = (float)atan2(-ay / cos(ry2), az / cos(ry2));
	//		rz2 = (float)atan2(-ox / cos(ry2), nx / cos(ry2));
	//	}

	//	std::cout << "rz11:" << -rz * (180.0f / PI) << "   ry:" << -ry * (180.0f / PI) << "   rx:" << -rx * (180.0f / PI) << "\n";
	//	std::cout << "rz22:" << -rz2 * (180.0f / PI) << "   ry:" << -ry2 * (180.0f / PI) << "   rx:" << -rx2 * (180.0f / PI) << "\n";
	//	if (abs(rx) + abs(ry) + abs(rz) <= abs(rx2) + abs(ry2) + abs(rz2))
	//	{
	//		rx_Origin = -rx;
	//		ry_Origin = -ry;
	//		rz_Origin = -rz;
	//	}
	//	else
	//	{
	//		rx_Origin = -rx2;
	//		ry_Origin = -ry2;
	//		rz_Origin = -rz2;
	//	}
	//	std::vector<double> i;


	//	if (abs(ry * (180.0f / PI)) < 90)
	//	{
	//		i.push_back(-rz * (180.0f / PI));
	//		i.push_back(-ry * (180.0f / PI));
	//		i.push_back(-rx * (180.0f / PI));
	//	}
	//	else
	//	{
	//		i.push_back(-rz2 * (180.0f / PI));
	//		i.push_back(-ry2 * (180.0f / PI));
	//		i.push_back(-rx2 * (180.0f / PI));
	//	}
	//	//MITK_INFO << i[0] << "," << i[1] << "," << i[2];
	//	return i;
	//}

	StaubliRobotDevice::StaubliRobotDevice()
		:TrackingDevice()
	{
		//m_Data = lancet::KukaRobotTypeInformation::GetDeviceDataLancetKukaTrackingDevice();
		//m_StaubliEndEffectors.clear();
		//////udp service
		//m_udp.setRepetitiveHeartbeatInterval(500);
		//m_udp.setRemoteHostPort(m_RemotePort.toUInt());
		//m_udp.setRemoteHostAddress(QString::fromStdString(m_RemoteIpAddress));
		//if (!m_udp.bind(QHostAddress(QString::fromStdString(m_IpAddress)), m_Port.toInt()))
		//{
		//	MITK_ERROR << QString("bind to %1:%2 error!- %3").arg(QString::fromStdString(m_IpAddress)).arg(m_Port.toInt()).arg(m_udp.error()).toStdString();
		//}
		//MITK_INFO << QString("bind udp %1:%2 at fps:%3").arg(QString::fromStdString(m_IpAddress)).arg(m_Port.toInt()).arg(m_udp.repetitiveHeartbeatInterval()).toStdString();
		//m_udp.startRepetitiveHeartbeat();
		
		connect(&m_RobotApi, SIGNAL(signal_api_isRobotConnected(bool)),
			this, SLOT(IsRobotConnected(bool)));
	}

	StaubliRobotDevice::~StaubliRobotDevice()
	{
		//m_udp.disconnect();
	}

	bool StaubliRobotDevice::InternalAddTool(mitk::TrackingTool* tool)
	{
		MITK_INFO << "InternalAddTool Called!!!!";
		if (tool == nullptr)
			return false;
		mitk::TrackingTool::Pointer p = tool;
		//p->Enable();
		int returnValue;
		/* if the connection to the tracking device is already established, add the new tool to the device now */
		if (this->GetState() == Ready)
		{
			MITK_INFO << "State Ready";
			// Request a port handle to load a passive tool into
			//todo add tool tcp to robot
			/* now that the tool is added to the device, add it to list too */
			m_ToolsMutex.lock();
			this->m_StaubliEndEffectors.push_back(p);
			m_ToolsMutex.unlock();
			this->Modified();
			return true;
		}
		else if (this->GetState() == Setup)
		{
			MITK_INFO << "State Setup";
			/* In Setup mode, we only add it to the list, so that OpenConnection() can add it later */
			m_ToolsMutex.lock();
			this->m_StaubliEndEffectors.push_back(p);
			m_ToolsMutex.unlock();
			this->Modified();
			return true;
		}
		else // in Tracking mode, no tools can be added
			return false;
	}

	void StaubliRobotDevice::IsRobotConnected(bool isConnect)
	{
		//m_IsConnected = isConnect;
		if (isConnect)
		{
			SetState(Ready);

			m_Heartbeat = QThread::create(heartbeatThreadWorker, this);
			//m_Heartbeat->start();
			MITK_INFO << "!connect!";
			emit this->deviceEnabled("Robot_Staubli");
		}
		else
		{
			SetState(Setup);
			m_Heartbeat->terminate();
			MITK_INFO << "!disconnect!";
			emit this->deviceDisable("Robot_Staubli");
		}
	}

	QVector<double> translateOfVtkMatrix4x4(const RobotRealtimeData& date)
	{
		QVector<double> _vtkMatrix4x4;
		static RRTC m_rrtc;

		Eigen::Vector3d rxyz(date.pose.rx, date.pose.ry, date.pose.rz);
		Eigen::Matrix3d R = m_rrtc.calculateR(rxyz);
		Eigen::Vector3d T(date.pose.x, date.pose.y, date.pose.z);

		_vtkMatrix4x4 = { R(0, 0), R(0, 1), R(0, 2), T[0],
						  R(1, 0), R(1, 1), R(1, 2), T[1],
						  R(2, 0), R(2, 1), R(2, 2), T[2],
						  0, 0, 0, 1
		};

		return _vtkMatrix4x4;
	}

	void StaubliRobotDevice::heartbeatThreadWorker(StaubliRobotDevice* _this)
	{
		while (_this->GetState() != Setup)
		{
			_this->m_RobotApi.setspeed(50);
			QThread::msleep(500);
		}
	}

	void StaubliRobotDevice::ThreadStartTracking()
	{
		MITK_INFO << "tracktools called";
		/* lock the TrackingFinishedMutex to signal that the execution rights are now transfered to the tracking thread */
		// keep lock until end of scope
		std::lock_guard<std::mutex> lock(m_TrackingFinishedMutex);
		if (this->GetState() != Tracking)
		{
			MITK_INFO << "TrackTools Return: state not tracking";
			return;
		}

		bool localStopTracking;
		// Because m_StopTracking is used by two threads, access has to be guarded by a mutex. To minimize thread locking, a local copy is used here
		this->m_StopTrackingMutex.lock(); // update the local copy of m_StopTracking
		localStopTracking = this->m_StopTracking;
		this->m_StopTrackingMutex.unlock();

		while ((this->GetState() == Tracking) && (localStopTracking == false))
		{
#if ZHAO_CODE
			//MITK_INFO << "tracking";
			m_RobotApi.requestrealtimedata();
			m_TrackingData[0] = m_RobotApi.realtime_data.pose.x;
			m_TrackingData[1] = m_RobotApi.realtime_data.pose.y;
			m_TrackingData[2] = m_RobotApi.realtime_data.pose.z;

			m_TrackingData[3] = m_RobotApi.realtime_data.pose.rx;
			m_TrackingData[4] = m_RobotApi.realtime_data.pose.ry;
			m_TrackingData[5] = m_RobotApi.realtime_data.pose.rz;

			Eigen::Quaterniond q;
			//kuka
			//Eigen::AngleAxisd rx(m_TrackingData[3] / d2r, Eigen::Vector3d::UnitZ());
			//Eigen::AngleAxisd ry(m_TrackingData[4] / d2r, Eigen::Vector3d::UnitY());
			//Eigen::AngleAxisd rz(m_TrackingData[5] / d2r, Eigen::Vector3d::UnitX());
			//return rz.matrix()*ry.matrix()*rx.matrix(); // kuka

			//Staubli
			 Eigen::AngleAxisd rx(m_TrackingData[3] / d2r, Eigen::Vector3d::UnitX());
			 Eigen::AngleAxisd ry(m_TrackingData[3] / d2r, Eigen::Vector3d::UnitY());
			 Eigen::AngleAxisd rz(m_TrackingData[3] / d2r, Eigen::Vector3d::UnitZ());
			 //Eigen::AngleAxisd rz(rxyz(2, 0) / d2r, Eigen::Vector3d::UnitZ());
			//return rx.matrix()*ry.matrix()*rz.matrix(); // staubli
			q = rx * ry * rz;

			mitk::TrackingTool::Pointer tool = GetInternalTool();
			mitk::Quaternion quaternion{ q.x(),q.y(),q.z(),q.w() };
			tool->SetOrientation(quaternion);
			mitk::Point3D position;
			position[0] = m_TrackingData[0];
			position[1] = m_TrackingData[1];
			position[2] = m_TrackingData[2];

			tool->SetPosition(position);
			tool->SetTrackingError(0);
			tool->SetErrorMessage("");
			//tool->SetFrameNumber(/*toolData.frameNumber*/); //todo add frameNumber in robot data for debug and frame-rate count ;
			tool->SetDataValid(true);
#endif // ZHAO_CODE
			{
				m_RobotApi.requestrealtimedata();
				LToolAttitudeMessage _retval;

				_retval.setValid(true);
				_retval.setToolName("Robot_Tool");
				_retval.setModuleName("Robot");
				_retval.setMatrix4x4(translateOfVtkMatrix4x4(this->m_RobotApi.realtime_data));
				_retval.setErrorIdentify(this->m_RobotApi.realtime_data.workmode);
				QVector<double> extendDataField;
				extendDataField.push_back(this->m_RobotApi.realtime_data.joints.j1);
				extendDataField.push_back(this->m_RobotApi.realtime_data.joints.j2);
				extendDataField.push_back(this->m_RobotApi.realtime_data.joints.j3);
				extendDataField.push_back(this->m_RobotApi.realtime_data.joints.j4);
				extendDataField.push_back(this->m_RobotApi.realtime_data.joints.j5);
				extendDataField.push_back(this->m_RobotApi.realtime_data.joints.j6);
				extendDataField.push_back(this->m_RobotApi.realtime_data.pose.x);
				extendDataField.push_back(this->m_RobotApi.realtime_data.pose.y);
				extendDataField.push_back(this->m_RobotApi.realtime_data.pose.z);
				extendDataField.push_back(this->m_RobotApi.realtime_data.pose.rx);
				extendDataField.push_back(this->m_RobotApi.realtime_data.pose.ry);
				extendDataField.push_back(this->m_RobotApi.realtime_data.pose.rz);
				
				
				//    extendDataField.push_back(QString::number(this->m_RobotApi.realtime_data.io.bit).toDouble());
				extendDataField.push_back(QString::number(this->m_RobotApi.realtime_data.io.valu1e).toDouble());
				_retval.setExtendedDataField(extendDataField);
				emit this->updateToolPosition("Robot_Staubli", _retval);
			}
			QThread::msleep(10);
			/* Update the local copy of m_StopTracking */
			this->m_StopTrackingMutex.lock();
			localStopTracking = m_StopTracking;
			this->m_StopTrackingMutex.unlock();
		}
		/* StopTracking was called, thus the mode should be changed back to Ready now that the tracking loop has ended. */

		// returnvalue = m_capi.stopTracking();
		// if (warningOrError(returnvalue, "m_capi.stopTracking()"))
		// {
		//   MITK_INFO << "m_capi.stopTracking() err";
		//   return;
		// }
		// MITK_INFO << "m_capi stopTracking()";
		return;
		// returning from this function (and ThreadStartTracking()) this will end the thread and transfer control back to main thread by releasing trackingFinishedLockHolder
	}
}

