/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "DianaSeven.h"


// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

const std::string DianaSeven::VIEW_ID = "org.mitk.views.dianaseven";

const double PI = 3.1415926;

void DianaSeven::SetFocus()
{
  //m_Controls.buttonPerformImageProcessing->setFocus();
}

void DianaSeven::CreateQtPartControl(QWidget* parent)
{
	// create GUI widgets from the Qt Designer's .ui file
	m_Controls.setupUi(parent);
	
	InitGlobalVariable();
	m_DianaAimHardwareService = new lancetAlgorithm::DianaAimHardwareService();
	m_PrecisionTab = new PrecisionTab(m_Controls, this->GetDataStorage(), m_DianaAimHardwareService, parent);
	connect(m_Controls.pushButton_initDiana, &QPushButton::clicked, this, &DianaSeven::initDianaNet);
	connect(m_Controls.pushButton_stopDiana, &QPushButton::clicked, this, &DianaSeven::StopDiana);
	connect(m_Controls.pushButton_initDiana_2, &QPushButton::clicked, this, &DianaSeven::initDianaNet);
	connect(m_Controls.pushButton_cleanErrorInfo, &QPushButton::clicked, this, &DianaSeven::cleanAllErrorInfo);
	//vega
	connect(m_Controls.pushButton_connectVega, &QPushButton::clicked, this, &DianaSeven::UseVega);
	connect(m_Controls.reciveData, &QPushButton::clicked, this, &DianaSeven::ReciveRobotData);
	connect(m_Controls.servopOn, &QPushButton::clicked, this, &DianaSeven::ServoP);
	//connect(m_Controls.closeServopOFF, &QPushButton::clicked, this, &DianaSeven::closeServopOff);
	connect(&m_ServoPTimer, &QTimer::timeout, this, &DianaSeven::OnServoPSendCommand);
	connect(m_Controls.pushButton_setTcp, &QPushButton::clicked, this, &DianaSeven::setTcp);
	

	connect(m_Controls.pushButton_PosAccuracy, &QPushButton::clicked, this, &DianaSeven::PosAccuracy);
	connect(m_Controls.pushButton_PosRepeaibility, &QPushButton::clicked, this, &DianaSeven::PosRepeatability);

	connect(m_Controls.pushButton_MoveZZJ, &QPushButton::clicked, this, &DianaSeven::move_zzj);
	connect(m_Controls.pushButton_MoveTest, &QPushButton::clicked, this, &DianaSeven::move_test);
	connect(m_Controls.pushButton_Initial, &QPushButton::clicked, this, &DianaSeven::GoInitial);
	connect(m_Controls.pushButton_1, &QPushButton::clicked, this, &DianaSeven::Position1);
	connect(m_Controls.pushButton_2, &QPushButton::clicked, this, &DianaSeven::Position2);
	connect(m_Controls.pushButton_4, &QPushButton::clicked, this, &DianaSeven::Position3);

	connect(m_Controls.pushButton_openFreeDriving, &QPushButton::clicked, this, &DianaSeven::OpenHandGuiding);
	connect(m_Controls.pushButton_closeFreeDriving, &QPushButton::clicked, this, &DianaSeven::closeHandGuiding);
	connect(m_Controls.pushButton_JointImpendance, &QPushButton::clicked, this, &DianaSeven::changeToJointImpendance);
	connect(m_Controls.pushButton_CartImpendance, &QPushButton::clicked, this, &DianaSeven::changeToCartImpendance);

	InitHardwareDeviceTabConnection();
	InitRobotRegistrationTabConnection();
}

void DianaSeven::initDianaNet()
{
	m_Controls.textBrowser->append("hello world\n");

	srv_net_st* pinfo = new srv_net_st();
	memset(pinfo->SrvIp, 0x00, sizeof(pinfo->SrvIp));
	memcpy(pinfo->SrvIp, "192.168.10.75", strlen("192.168.10.75"));
	pinfo->LocHeartbeatPort = 0;
	pinfo->LocRobotStatePort = 0;
	pinfo->LocSrvPort = 0;
	int ret = initSrv(nullptr, nullptr, pinfo);
	if (ret < 0)
	{
		m_Controls.textBrowser->append("192.168.10.75 initSrv failed! Return value =-1");
	}
	if (pinfo)
	{
		delete pinfo;
		pinfo = nullptr;
	}
	releaseBrake();
}

void DianaSeven::StopDiana()
{
	stop(m_RobotIpAddress);
}

void DianaSeven::PosAccuracy()//�Ÿ���
{
}

void DianaSeven::PosRepeatability()//���������ض�
{
}



void DianaSeven::move_zzj()
{
	/*����˼·����ȡ��е�۵�ǰ�ؽ�λ��DJ1-7��->ת��Ϊ���->���ת��α任����A����ǰλ���£�
	->�ҳ�һ���ƶ�����B��ƽ�ƣ�-���õ����ƻ�е���˶��ľ���Ŀ��λ�ˣ�T-��
		��α任����TתĿ�����-����ȡĿ����ǲ��˶��������*/

	//getJointPos(joints, strIpAddress);//��ȡDJ1-7
	//printf("getJointPos: %f ,%f, %f, %f, %f, %f, %f\n", joints[0], joints[1], joints[2], joints[3], joints[4], joints[5], joints[6]);
	//
	//forward(joints, pose, nullptr, strIpAddress);//Dj11-7תx y z Rx Ry Rz���
	//printf(" forward succeed! Pose: %f, %f, %f, %f, %f, %f\n ", pose[0], pose[1], pose[2], pose[3], pose[4], pose[5]);

	getTcpPos(pose, m_RobotIpAddress);
	printf(" forward succeed! Pose: %f, %f, %f, %f, %f, %f\n ", pose[0], pose[1], pose[2], pose[3], pose[4], pose[5]);

	pose2Homogeneous(pose, forward_Matrix);//���ת��α任����
	//PrintDataHelper::CoutMatrix("forward_Matrix", forward_Matrix);
	//PrintDataHelper::PrintMatrix("forward_Matrix", forward_Matrix);

	//forward_Matrix->�ҳ�һ���ƶ�����->�õ�һ��T_newtargetMatrix
	double x = m_Controls.lineEdit_X->text().toDouble() / 1000;
	double y = m_Controls.lineEdit_Y->text().toDouble() / 1000;
	double z = m_Controls.lineEdit_Z->text().toDouble() / 1000;

	vtkNew<vtkTransform> tmpTrans;
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(forward_Matrix);
	vtkNew<vtkMatrix4x4> transposedMatrix1;
	transposedMatrix1->DeepCopy(tmpTrans->GetMatrix());
	transposedMatrix1->Transpose();
	tmpTrans->SetMatrix(transposedMatrix1);
	tmpTrans->Translate(x, y, z);
	vtkNew<vtkMatrix4x4> transposedMatrix2;
	transposedMatrix2->DeepCopy(tmpTrans->GetMatrix());
	transposedMatrix2->Transpose();
	tmpTrans->SetMatrix(transposedMatrix2);

	tmpTrans->Update();
	std::cout << std::fixed << std::setprecision(5);
	tmpTrans->Print(std::cout);

	vtkMatrix4x4* matrix = tmpTrans->GetMatrix();
	auto T_newtargetMatrix = matrix->GetData();
	//PrintDataHelper::CoutMatrix("T_newtargetMatrix", T_newtargetMatrix);
	//PrintDataHelper::PrintMatrix("T_newtargetMatrix", T_newtargetMatrix);

	homogeneous2Pose(T_newtargetMatrix, pose);
	printf("Diana API homogeneous2Pose got: %f, %f, %f, %f, %f, %f\n", pose[0], pose[1], pose[2], pose[3], pose[4], pose[5]);

	inverse(pose, joints_final, nullptr, m_RobotIpAddress);
	//moveJToPose(pose, vel, acc, nullptr, zv_shaper_order, zv_shaper_frequency, zv_shaper_damping_ratio, strIpAddress);
	//printf(" moveJToPose Pose: %f, %f, %f, %f, %f, %f\n ", pose[0], pose[1], pose[2], pose[3], pose[4], pose[5]);
	moveJToTarget(joints_final, 0.2, 0.4);

	wait_move(m_RobotIpAddress);
}

void DianaSeven::move_test()
{
	//getJointPos(joints, strIpAddress);//��ȡDJ1-7
	//printf("getJointPos: %f ,%f, %f, %f, %f, %f, %f\n", joints[0], joints[1], joints[2], joints[3], joints[4], joints[5], joints[6]);

	//forward(joints, pose, nullptr, strIpAddress);//Dj11-7תx y z Rx Ry Rz���
	//printf(" forward succeed! Pose: %f, %f, %f, %f, %f, %f\n ", pose[0], pose[1], pose[2], pose[3], pose[4], pose[5]);

	//pose2Homogeneous(pose, forward_Matrix);//���ת��α任����
	////PrintDataHelper::CoutMatrix("forward_Matrix", forward_Matrix);
	////PrintDataHelper::PrintMatrix("forward_Matrix", forward_Matrix);

	////forward_Matrix->�ҳ�һ���ƶ�����->�õ�һ��T_newtargetMatrix
	//double x = m_Controls.lineEdit_X->text().toDouble();
	//double y = m_Controls.lineEdit_Y->text().toDouble();
	//double z = m_Controls.lineEdit_Z->text().toDouble();

	//vtkNew<vtkTransform> tmpTrans;
	//tmpTrans->PostMultiply();
	//tmpTrans->SetMatrix(forward_Matrix);
	//vtkNew<vtkMatrix4x4> transposedMatrix1;
	//transposedMatrix1->DeepCopy(tmpTrans->GetMatrix());
	//transposedMatrix1->Transpose();
	//tmpTrans->SetMatrix(transposedMatrix1);
	//tmpTrans->Translate(x, y, z);
	//vtkNew<vtkMatrix4x4> transposedMatrix2;
	//transposedMatrix2->DeepCopy(tmpTrans->GetMatrix());
	//transposedMatrix2->Transpose();
	//tmpTrans->SetMatrix(transposedMatrix2);
	//tmpTrans->Update();

	//std::cout << std::fixed << std::setprecision(5);
	//tmpTrans->Print(std::cout);

	//vtkMatrix4x4* matrix = tmpTrans->GetMatrix();
	//auto T_newtargetMatrix = matrix->GetData();
	////PrintDataHelper::CoutMatrix("T_newtargetMatrix", T_newtargetMatrix);
	////PrintDataHelper::PrintMatrix("T_newtargetMatrix", T_newtargetMatrix);

	//homogeneous2Pose(T_newtargetMatrix, pose);
	//printf("Diana API homogeneous2Pose got: %f, %f, %f, %f, %f, %f\n", pose[0], pose[1], pose[2], pose[3], pose[4], pose[5]);

	//inverse_ext(ref_joints, pose, joints, nullptr, strIpAddress);
	//printf("inverse_ext: %f ,%f, %f, %f, %f, %f, %f\n", joints[0], joints[1], joints[2], joints[3], joints[4], joints[5], joints[6]);
	//moveLToTarget(joints, vel, acc, zv_shaper_order, zv_shaper_frequency, zv_shaper_damping_ratio);
	//wait_move(strIpAddress);

}

void DianaSeven::Refmove()
{
	//getJointPos(joints, strIpAddress);//��ȡDJ1-7
	//printf("getJointPos: %f ,%f, %f, %f, %f, %f, %f\n", joints[0], joints[1], joints[2], joints[3], joints[4], joints[5], joints[6]);

	//forward(joints, pose, nullptr, strIpAddress);//Dj11-7תx y z Rx Ry Rz���
	//printf(" forward succeed! Pose: %f, %f, %f, %f, %f, %f\n ", pose[0], pose[1], pose[2], pose[3], pose[4], pose[5]);

	//pose2Homogeneous(pose, forward_Matrix);//���ת��α任����
	////PrintDataHelper::CoutMatrix("forward_Matrix", forward_Matrix);
	////PrintDataHelper::PrintMatrix("forward_Matrix", forward_Matrix);

	////forward_Matrix->�ҳ�һ���ƶ�����->�õ�һ��T_newtargetMatrix
	//double x = m_Controls.lineEdit_X->text().toDouble();
	//double y = m_Controls.lineEdit_Y->text().toDouble();
	//double z = m_Controls.lineEdit_Z->text().toDouble();

	//vtkNew<vtkTransform> tmpTrans;
	//tmpTrans->PostMultiply();//�����ҳ�
	//tmpTrans->SetMatrix(forward_Matrix); // ���õ�ǰ����ξ���
	//tmpTrans->Translate(x, y, z); // Ӧ��ƽ�Ʊ任
	//tmpTrans->Update(); // ���±任

	//// ��ȡ�任���󲢽���ת��
	//vtkMatrix4x4* matrix = tmpTrans->GetMatrix();
	//vtkNew<vtkMatrix4x4> transposedMatrix;
	//transposedMatrix->DeepCopy(matrix);
	//transposedMatrix->Transpose();
	////PrintDataHelper::PrintMatrix("transposedMatrix", transposedMatrix);
	//// ��ת�þ�������ݸ��Ƶ�double������
	//double T_newtargetMatrix[16];
	//for (int i = 0; i < 16; ++i) {
	//	T_newtargetMatrix[i] = transposedMatrix->GetElement(i / 4, i % 4);
	//}
	////PrintDataHelper::PrintMatrix("T_newtargetMatrix", T_newtargetMatrix);
	//homogeneous2Pose(T_newtargetMatrix, pose);
	//printf("Diana API homogeneous2Pose got: %f, %f, %f, %f, %f, %f\n", pose[0], pose[1], pose[2], pose[3], pose[4], pose[5]);

	//inverse_ext(ref_joints, pose, joints, nullptr, strIpAddress);
	//printf("inverse_ext: %f ,%f, %f, %f, %f, %f, %f\n", joints[0], joints[1], joints[2], joints[3], joints[4], joints[5], joints[6]);
	//moveLToTarget(joints, vel, acc, zv_shaper_order, zv_shaper_frequency, zv_shaper_damping_ratio);
	//wait_move(strIpAddress);

}

void DianaSeven::wait_move(const char* m_RobotIpAddress)
{
	QThread::msleep(20);
	while (true)
	{
		const char state = getRobotState();
		if (state != 0)
		{
			//qDebug() << "222";
			MITK_INFO << "222";
			break;
		}
		else
		{
			QThread::msleep(1);
			QApplication::processEvents();
		}
	}
	stop();
}

void DianaSeven::GoInitial()
{
	//double pose2[6] = { 0.5069,0.1828,0.5044,-0.002217,-0.00238,0.003824 };
	double pose2[6] = { 0.272069,0.355601,0.347205,0.01958,-0.088089,0.01899 };
	moveJToPose(pose2, vel, acc, nullptr, zv_shaper_order, zv_shaper_frequency, zv_shaper_damping_ratio);
	wait_move(m_RobotIpAddress);
}

void DianaSeven::Position1()
{
	//double aaa[7] = { -1.87254e-07,0.349064,-1.21715e-05,1.83259,-1.87254e-07,1.30899,-1.87254e-07 };
	double aaa[7] = { -0.51068333913,-0.44664720888,0.073478361509,2.499259129,0.03511602455,-1.0516132342,-0.51033427328 };
	moveJToTarget(aaa, 0.2, 0.4);
	//double Position[6] = { 0.309263,0.133743,0.275510,-0.00791,-0.00177,0.029970 };
	//moveJToPose(Position, vel, acc, nullptr, zv_shaper_order, zv_shaper_frequency, zv_shaper_damping_ratio);
	wait_move(m_RobotIpAddress);
}

void DianaSeven::Position2()
{
	double Position2[6] = { 0.521397,0.393198,0.616475,0.000298,-0.060823,0.036656 };
	moveJToPose(Position2, vel, acc, nullptr, zv_shaper_order, zv_shaper_frequency, zv_shaper_damping_ratio);
	wait_move(m_RobotIpAddress);
}

void DianaSeven::Position3()
{
	double Position[6] = { 0.309263,0.133743,0.275510,-0.00791,-0.00177,0.029970 };
	moveJToPose(Position, vel, acc, nullptr, zv_shaper_order, zv_shaper_frequency, zv_shaper_damping_ratio);
	wait_move(m_RobotIpAddress);
}



void DianaSeven::OpenHandGuiding()
{
	changeControlMode(T_MODE_POSITION, m_RobotIpAddress);
	freeDriving(1, m_RobotIpAddress);
}

void DianaSeven::closeHandGuiding()
{
	freeDriving(0, m_RobotIpAddress);
}

void DianaSeven::changeToCartImpendance()
{
	changeControlMode(T_MODE_CART_IMPEDANCE, m_RobotIpAddress);
}

void DianaSeven::changeToJointImpendance()
{
	changeControlMode(T_MODE_JOINT_IMPEDANCE, m_RobotIpAddress);
}

void DianaSeven::cleanAllErrorInfo()
{
	int p=cleanErrorInfo(m_RobotIpAddress);
	if (p < 0)
	{
		MITK_INFO << "cleanErrorInfo failed!";
		printf("cleanErrorInfo failed!\n");
	}

}

void DianaSeven::setTcp()
{
	setDefaultActiveTcpPose(activeTcpPose, m_RobotIpAddress);
	//setDefaultActiveTcp(nowActiveTcp,strIpAddress);
	//double RTf2t[3] = { -113.079,145.354,-97.1639 };
	//setFla2TcpRT(RTf2t, strIpAddress);

}

void DianaSeven::UseVega()
{
	m_Controls.pushButton_connectVega->setText("Vega Connecting...");
	//read in filename
	QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/C:/Users/lancet/Desktop/fx",
		tr("Tool Storage Files (*.IGTToolStorage)"));
	if (filename.isNull()) return;

	mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
		GetDataStorage());
	m_VegaToolStorage = myDeserializer->Deserialize(filename.toStdString());
	m_VegaToolStorage->SetName(filename.toStdString());

	MITK_INFO << "Vega tracking";
	//QMessageBox::warning(nullptr, "Warning", "You have to set the parameters for the NDITracking device inside the code (QmitkIGTTutorialView::OnStartIGT()) before you can use it.");
	lancet::NDIVegaTrackingDevice::Pointer vegaTrackingDevice = lancet::NDIVegaTrackingDevice::New(); //instantiate

	//Create Navigation Data Source with the factory class, and the visualize filter.
	lancet::TrackingDeviceSourceConfiguratorLancet::Pointer vegaSourceFactory =
		lancet::TrackingDeviceSourceConfiguratorLancet::New(m_VegaToolStorage, vegaTrackingDevice);

	m_VegaSource = vegaSourceFactory->CreateTrackingDeviceSource(m_VegaVisualizer);
	m_VegaSource->SetToolMetaDataCollection(m_VegaToolStorage);
	m_VegaSource->Connect();

	m_VegaSource->StartTracking();


	//update visualize filter by timer
	if (m_VegaVisualizeTimer == nullptr)
	{
		m_VegaVisualizeTimer = new QTimer(this); //create a new timer
	}
	connect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(OnVegaVisualizeTimer()));
	//connect the timer to the method OnTimer()
	connect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(UpdateToolStatusWidget()));
	connect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(CalculatePositionAverage(mitk::AffineTransform3D::Pointer  m_ProbeRealTimePose)));

	connect(m_VegaVisualizeTimer, &QTimer::timeout, this, &DianaSeven::ComputeProbeToProbeNewMatrix);

	//connect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(TargetPointData()));

	// m_probeToProbeNewMatrix = ComputeProbeToProbeNewMatrix(dCoord);
	// TargetPointData();


	//connect the timer to the method OnTimer()
	ShowToolStatus_Vega();
	m_VegaVisualizeTimer->start(10); //Every 100ms the method OnTimer() is called. -> 10fps


	auto probeindex = m_VegaToolStorage->GetToolIndexByName("Probe");

	MITK_INFO << "probeindex:" << probeindex;

	mitk::NavigationData::Pointer const InitNavigationData = m_VegaSource->GetOutput(probeindex)->Clone();

	mitk::TransferItkTransformToVtkMatrix(InitNavigationData->GetAffineTransform3D().GetPointer(), m_ndiToprobe);

	m_ndiToprobe->Print(std::cout);

	MITK_INFO << "Center:" << InitNavigationData->GetAffineTransform3D()->GetCenter()[0];


	auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
	mitk::RenderingManager::GetInstance()->InitializeViews(geo);

	//use navigation scene filter
	cout << "Vega Connected Successfully!" << std::endl;
	m_Controls.pushButton_connectVega->setText("Vega Connected Successfully!");
}

void DianaSeven::OnVegaVisualizeTimer()
{
	if (m_VegaVisualizer.IsNotNull())
	{
		m_VegaVisualizer->Update();
		this->RequestRenderWindowUpdate();
		//update probe pose
		auto  probe = m_VegaSource->GetOutput("Probe")->GetAffineTransform3D(); //F_NDI_Probe
		auto objectRf = m_VegaSource->GetOutput("ObjectRf");
		if (probe != nullptr && objectRf != nullptr)
		{
			probe->Compose(m_VegaSource->GetOutput("ObjectRf")->GetInverse()->GetAffineTransform3D());  //F_ObjectRf_NDI
		}
	}
}

void DianaSeven::UpdateToolStatusWidget()
{
	m_Controls.m_StatusWidgetVegaToolToShow->Refresh();
}

void DianaSeven::ShowToolStatus_Vega()
{
	m_VegaNavigationData.clear();
	for (std::size_t i = 0; i < m_VegaSource->GetNumberOfOutputs(); i++)
	{
		m_VegaNavigationData.push_back(m_VegaSource->GetOutput(i));
	}
	//initialize widget
	m_Controls.m_StatusWidgetVegaToolToShow->RemoveStatusLabels();
	m_Controls.m_StatusWidgetVegaToolToShow->SetShowPositions(true);
	m_Controls.m_StatusWidgetVegaToolToShow->SetTextAlignment(Qt::AlignLeft);
	m_Controls.m_StatusWidgetVegaToolToShow->SetNavigationDatas(&m_VegaNavigationData);
	m_Controls.m_StatusWidgetVegaToolToShow->ShowStatusLabels();
}

void DianaSeven::on_pushButton_getProbeInit_clicked()
{
	auto probeindex = m_VegaToolStorage->GetToolIndexByName("Probe");

	MITK_INFO << "probeindex:" << probeindex;

	mitk::NavigationData::Pointer const InitNavigationData = m_VegaSource->GetOutput(probeindex)->Clone();

	mitk::TransferItkTransformToVtkMatrix(InitNavigationData->GetAffineTransform3D().GetPointer(), m_ndiToprobe);

	m_ndiToprobe->Print(std::cout);

	MITK_INFO << "Center:" << InitNavigationData->GetAffineTransform3D()->GetCenter()[0];
}

void DianaSeven::ReciveRobotData()
{
	getJointPos(joints, m_RobotIpAddress);//��ȡDJ1-7
	printf("getJointPos: %f ,%f, %f, %f, %f, %f, %f\n", joints[0], joints[1], joints[2], joints[3], joints[4], joints[5], joints[6]);

	dCoord = { dX, dY, dZ, dRx, dRy, dRz };
}

void DianaSeven::ServoP()
{
	if (this->m_ServoPTimer.isActive())
	{
		this->m_ServoPTimer.stop();
	}
	else
	{
		double* TargetPose = dTargetPoint.data();//ʹ��data����������vector����ת��Ϊdouble

		int StartServo = servoJ_ex(TargetPose, 0.002, 0.08, 150, false, m_RobotIpAddress);
		MITK_INFO << "TargetPose:" << TargetPose[0] << "," << TargetPose[1] << "," << TargetPose[2] << "," << TargetPose[3]
			<< "," << TargetPose[4] << "," << TargetPose[5];
		if (StartServo==0)
		{
			this->m_ServoPTimer.start(80);
		}
	}
}

void DianaSeven::OnServoPSendCommand()
{
	clock_t start;
	start = clock();//����ʱ�䡢��ʱ��

	double* TargetPose = dTargetPoint.data();//ʹ��data����������vector����ת��Ϊdouble

	servoJ_ex(TargetPose, 0.002, 0.08, 150, false,m_RobotIpAddress);
	//servoL_ex(dTargetPoint, 0.02, 0.08, 150, false);
	MITK_INFO << "TargetPose:" << TargetPose[0] << "," << TargetPose[1] << "," << TargetPose[2] << "," << TargetPose[3]
		<< "," << TargetPose[4] << "," << TargetPose[5];
}

void DianaSeven::closeServopOff()
{

}

void DianaSeven::TargetPointData()//?
{
	MITK_INFO << "TargetPointData m_probeToProbeNewMatrix";
	m_probeToProbeNewMatrix->Print(std::cout);
	Eigen::Matrix3d m_probeToprobeNew;

	m_probeToprobeNew << m_probeToProbeNewMatrix->GetElement(0, 0), m_probeToProbeNewMatrix->GetElement(0, 1), m_probeToProbeNewMatrix->GetElement(0, 2),
		m_probeToProbeNewMatrix->GetElement(1, 0), m_probeToProbeNewMatrix->GetElement(1, 1), m_probeToProbeNewMatrix->GetElement(1, 2),
		m_probeToProbeNewMatrix->GetElement(2, 0), m_probeToProbeNewMatrix->GetElement(2, 1), m_probeToProbeNewMatrix->GetElement(2, 2);

	Eigen::Vector3d eulerAngle = m_probeToprobeNew.eulerAngles(0, 1, 2);
	// ת��Ϊ�Ƕ�
	eulerAngle = eulerAngle * (180.0 / PI);
	//��ת˳��z-y-x
	dTargetPoint.clear();
	dTargetPoint.push_back(m_probeToProbeNewMatrix->GetElement(0, 3));
	dTargetPoint.push_back(m_probeToProbeNewMatrix->GetElement(1, 3));
	dTargetPoint.push_back(m_probeToProbeNewMatrix->GetElement(2, 3));
	dTargetPoint.push_back(eulerAngle(2));
	dTargetPoint.push_back(eulerAngle(1));
	dTargetPoint.push_back(eulerAngle(0));
	MITK_INFO << "dTargetPoint:" << dTargetPoint[0] << "," << dTargetPoint[1] << "," << dTargetPoint[2] << "," << dTargetPoint[3]
		<< "," << dTargetPoint[4] << "," << dTargetPoint[5];

}

void DianaSeven::InitHardwareDeviceTabConnection()
{
	connect(m_Controls.ConnectKukaBtn, &QPushButton::clicked, this, &DianaSeven::ConnectRobotBtnClicked);
	connect(m_Controls.PowerOnBtn, &QPushButton::clicked, this, &DianaSeven::PowerOnBtnClicked);
	connect(m_Controls.PowerOffBtn, &QPushButton::clicked, this, &DianaSeven::PowerOffBtnClicked);
	connect(m_Controls.ConnectNDIBtn, &QPushButton::clicked, this, &DianaSeven::ConnectCameraClicked);
	connect(m_Controls.UpdateCameraBtn, &QPushButton::clicked, this, &DianaSeven::UpdateCameraBtnClicked);
	connect(m_DianaAimHardwareService, &lancetAlgorithm::DianaAimHardwareService::CameraUpdateClock, this, &DianaSeven::HandleUpdateRenderRequest);
}

void DianaSeven::InitRobotRegistrationTabConnection()
{
	//Translate
	connect(m_Controls.RobotXPlusBtn, &QPushButton::clicked, this, [=]() {
		double movementDirection[3] = { 1, 0,0 };
		Translate(movementDirection);
		});
	connect(m_Controls.RobotXMinusBtn, &QPushButton::clicked, this, [=]() {
		double movementDirection[3] = { -1, 0,0 };
		Translate(movementDirection);
		});
	connect(m_Controls.RobotYPlusBtn, &QPushButton::clicked, this, [=]() {
		double movementDirection[3] = { 0, 1,0 };
		Translate(movementDirection);
		});
	connect(m_Controls.RobotYMinusBtn, &QPushButton::clicked, this, [=]() {
		double movementDirection[3] = { 0, -1,0 };
		Translate(movementDirection);
		});
	connect(m_Controls.RobotZPlusBtn, &QPushButton::clicked, this, [=]() {
		double movementDirection[3] = { 0, 0,1 };
		Translate(movementDirection);
		});
	connect(m_Controls.RobotZMinusBtn, &QPushButton::clicked, this, [=]() {
		double movementDirection[3] = { 0, 0,-1 };
		Translate(movementDirection);
		});

	//Rotate
	connect(m_Controls.RobotRXPlusBtn, &QPushButton::clicked, this, [=]() {
		double rotationDirection[3] = { 1, 0,0 };
		Rotate(rotationDirection);
		});
	connect(m_Controls.RobotRXMinusBtn, &QPushButton::clicked, this, [=]() {
		double rotationDirection[3] = { -1, 0,0 };
		Rotate(rotationDirection);
		});
	connect(m_Controls.RobotRYPlusBtn, &QPushButton::clicked, this, [=]() {
		double rotationDirection[3] = { 0, 1,0 };
		Rotate(rotationDirection);
		});
	connect(m_Controls.RobotRYMinusBtn, &QPushButton::clicked, this, [=]() {
		double rotationDirection[3] = { 0, -1,0 };
		Rotate(rotationDirection);
		});
	connect(m_Controls.RobotRZPlusBtn, &QPushButton::clicked, this, [=]() {
		double rotationDirection[3] = { 0, 0,1 };
		Rotate(rotationDirection);
		});
	connect(m_Controls.RobotRZMinusBtn, &QPushButton::clicked, this, [=]() {
		double rotationDirection[3] = { 0, 0,-1 };
		Rotate(rotationDirection);
		});

	connect(m_Controls.SetTcpToFlangeBtn, &QPushButton::clicked, this, [this]() {m_DianaAimHardwareService->SetTCP2Flange(); } );
	connect(m_Controls.RecordInitPosBtn, &QPushButton::clicked, this, [this]() {m_DianaAimHardwareService->RecordIntialPos(); } );
	connect(m_Controls.GoToInitPosBtn, &QPushButton::clicked, this, [this]() {m_DianaAimHardwareService->GoToInitPos(); });
	connect(m_Controls.CaptureRobotBtn, &QPushButton::clicked, this, [this]() 
		{m_Controls.CaptureCountLineEdit->setText(QString::number(m_DianaAimHardwareService->CaptureRobot())); });
	connect(m_Controls.RobotAutoRegistationBtn, &QPushButton::clicked, this, [this]() {m_DianaAimHardwareService->RobotAutoRegistration(); } );
	connect(m_Controls.ResetRobotRegistrationBtn, &QPushButton::clicked, this, [this]()
		{m_Controls.CaptureCountLineEdit->setText(QString::number(m_DianaAimHardwareService->ResetRobotRegistration())); });
	connect(m_Controls.SaveRobotRegistrationBtn, &QPushButton::clicked, this, [this]() {std::cout << "save" << std::endl; });
	connect(m_Controls.ReuseRobotRegistationBtn, &QPushButton::clicked, this, [this]() {std::cout << "Reuse" << std::endl; });
	connect(m_Controls.StopRobotMoveBtn, &QPushButton::clicked, this, [this]() {m_DianaAimHardwareService->StopMove(); });
	connect(m_Controls.ClearRobotErrorInfoBtn, &QPushButton::clicked, this, [this]() {m_DianaAimHardwareService->CleanRobotErrorInfo(); });

	connect(m_Controls.ReadRobotJointAnglesBtn, &QPushButton::clicked, this, &DianaSeven::ReadRobotJointAnglesBtnClicked);
	connect(m_Controls.SetRobotJointAnglesBtn, &QPushButton::clicked, this, &DianaSeven::SetRobotJointAnglesBtnClicked);
	connect(m_Controls.GetRobotJointsLimitBtn, &QPushButton::clicked, this, &DianaSeven::GetRobotJointsLimitBtnClicked);

	connect(m_Controls.SetRobotPositionModeBtn, &QPushButton::clicked, this, [this]() {m_DianaAimHardwareService->SetPositionMode(); });
	connect(m_Controls.SetRobotJointsImpedanceModelBtn, &QPushButton::clicked, this, [this]() {m_DianaAimHardwareService->SetJointImpendanceMode(); });
	connect(m_Controls.SetRobotCartImpedanceModeBtn, &QPushButton::clicked, this, [this]() {m_DianaAimHardwareService->SetCartImpendanceMode(); });
	
	connect(m_Controls.ReadRobotImpedaBtn, &QPushButton::clicked, this, &DianaSeven::ReadRobotImpedaBtnClicked);
	connect(m_Controls.SetRobotImpedaBtn, &QPushButton::clicked, this, &DianaSeven::SetRobotImpedaBtnClicked);
	
	connect(m_Controls.AppendToolTipBtn, &QPushButton::clicked, this, &DianaSeven::AppendToolTipBtnClicked);
	connect(m_Controls.AppendToolMatrixBtn, &QPushButton::clicked, this, &DianaSeven::AppendToolMatrixBtnClicked);
}

bool DianaSeven::Translate(const double axis[3])
{
	m_DianaAimHardwareService->Translate(axis, m_Controls.TranslateDistanceLineEdit->text().toDouble());
	return true;
}

bool DianaSeven::Rotate(const double axis[3])
{
	m_DianaAimHardwareService->Rotate(axis, m_Controls.RotateAngleLineEdit->text().toDouble());
	return true;
}

void DianaSeven::ComputeProbeToProbeNewMatrix()
{
	// ��ȡ,probe��index, ����ȡ��ǰʱ���� NavigationData
	auto probeIndex = m_VegaToolStorage->GetToolIndexByName("Probe");
	mitk::NavigationData::Pointer const currentNavigationData = m_VegaSource->GetOutput(probeIndex)->Clone();

	m_currentProbeMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

	// Navigationdata ת��Ϊ4x4����

	mitk::TransferItkTransformToVtkMatrix(currentNavigationData->GetAffineTransform3D().GetPointer(), m_currentProbeMatrix);
	//MITK_INFO << "probe runtime";
	//m_currentProbeMatrix->Print(std::cout);
	// Convert Euler angle and Tranlate to 4x4 Translation Matrix
	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity(); // initialize
	tmpTrans->PostMultiply(); // tmpTrans->Premultiply();
	tmpTrans->RotateZ(dCoord[3]);
	tmpTrans->RotateY(dCoord[4]);
	tmpTrans->RotateX(dCoord[5]);

	double translation[3] = { dCoord[0], dCoord[1], dCoord[2] };

	tmpTrans->Translate(translation);
	tmpTrans->Update();
	m_init_robotMatrix = tmpTrans->GetMatrix();



	// ����Fprobe��Fprobe_new�ı任����
	vtkSmartPointer<vtkMatrix4x4> probeToprobeNew = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix4x4::Invert(m_ndiToprobe, probeToprobeNew); // ���� m_ndiToprobe ������󲢴洢�� resultMatrix ��
	//MITK_INFO << "m_ndiToprobe";
	//m_ndiToprobe->Print(std::cout);

	vtkMatrix4x4::Multiply4x4(probeToprobeNew, m_currentProbeMatrix, probeToprobeNew); // ����������� m_ndiToprobe_new �ĳ˻������� resultMatrix
	//MITK_INFO << "m_ndiToprobe_new";
	//probeToprobeNew->Print(std::cout);
	vtkMatrix4x4::Multiply4x4(m_init_robotMatrix, probeToprobeNew, probeToprobeNew); // ����������� T_robot �ĳ˻������� resultMatrix

	m_probeToProbeNewMatrix = probeToprobeNew;

	TargetPointData();
}

void DianaSeven::ConnectRobotBtnClicked()
{
	m_DianaAimHardwareService->ConnectRobot();
}

void DianaSeven::PowerOnBtnClicked()
{
	m_DianaAimHardwareService->RobotPowerOn();
}

void DianaSeven::PowerOffBtnClicked()
{
	m_DianaAimHardwareService->RobotPowerOff();
}

void DianaSeven::ConnectCameraClicked()
{
	m_DianaAimHardwareService->ConnectCamera();
}

void DianaSeven::UpdateCameraBtnClicked()
{
	std::cout << "UpdateCameraBtnClicked" << std::endl;
	std::vector<QLabel*>* lables = new std::vector<QLabel*>();
	lables->push_back(m_Controls.PKARobotBaseRF);
	//lables->push_back(m_Controls.PKAFemurRF);
	lables->push_back(m_Controls.PKATibiaRF);
	//lables->push_back(m_Controls.BluntProbe);
	lables->push_back(m_Controls.PKARobotEndRF);
	lables->push_back(m_Controls.PKAProbe);
	//lables->push_back(m_Controls.PKADrill);

	/*std::vector<std::string> toolsName = {"RobotBaseRF", "PKAFemurRF", "PKATibiaRF", "BluntProbe", "RobotEndRF", "PKAProbe", "PKADrill"};*/
	std::vector<std::string> toolsName = { "RobotBaseRF",  "VerificationBlock", "RobotEndRF", "Probe"};
	m_DianaAimHardwareService->InitToolsName(toolsName, std::move(lables));
	m_DianaAimHardwareService->StartCamera();
	//m_DianaAimHardwareService->UpdateCamera();
}

void DianaSeven::HandleUpdateRenderRequest()
{
	this->RequestRenderWindowUpdate();
}

void DianaSeven::ReadRobotJointAnglesBtnClicked()
{
	auto angles = m_DianaAimHardwareService->GetJointAngles();
	for (int i = 0; i < angles.size(); ++i)
	{
		m_JointAngleLineEdits[i]->setText(QString::number(angles[i] * 180 / PI));
	}
}

void DianaSeven::SetRobotJointAnglesBtnClicked()
{
	double angles[7] = { 0.0 };
	for (int i = 0; i < m_JointAngleLineEdits.size(); ++i)
	{
		angles[i] = m_JointAngleLineEdits[i]->text().toDouble() / 180 * PI;
	}

	bool ret = m_DianaAimHardwareService->SetJointAngles(angles);
}

void DianaSeven::GetRobotJointsLimitBtnClicked()
{
	auto range = m_DianaAimHardwareService->GetJointsPositionRange();

	auto convertAndPrint = [](const std::vector<double>& values, const std::string& label) {
		std::cout << label;
		for (const auto& val : values)
		{
			std::cout << (val * 180 / PI) << " ";
		}
		std::cout << std::endl;
	};
	convertAndPrint(range[0], "min Range: ");
	convertAndPrint(range[1], "max Range: ");
}

void DianaSeven::ReadRobotImpedaBtnClicked()
{
	auto Impeda = m_DianaAimHardwareService->GetRobotImpeda();
	for (int i = 0; i < m_ImpedaLineEdits.size(); ++i)
	{
		m_ImpedaLineEdits[i]->setText(QString::number(Impeda[i]));
	}
}

void DianaSeven::SetRobotImpedaBtnClicked()
{
	double data[7] = { 0.0 };
	for (int i = 0; i < m_ImpedaLineEdits.size(); ++i)
	{
		data[i] = m_ImpedaLineEdits[i]->text().toDouble();
	}

	m_DianaAimHardwareService->SetRobotImpeda(data);
}

void DianaSeven::AppendToolTipBtnClicked()
{
	auto name = m_Controls.ToolNameComboBox->currentText();
	auto tip = m_DianaAimHardwareService->GetTipByName(name.toStdString());
	
	QString str;
	for (int i = 0; i < tip.size(); ++i)
	{
		str += QString::number(tip[i]) + " ";
	}
	str = QString(name) + " " + str;
	m_Controls.textBrowser->append(str);
}

void DianaSeven::AppendToolMatrixBtnClicked()
{
	auto name = m_Controls.ToolNameComboBox->currentText();
	auto matrix = m_DianaAimHardwareService->GetMatrixByName(name.toStdString())->GetData();

	m_Controls.textBrowser->append(name);
	for (int i = 0; i < 4; ++i)
	{
		QString row;
		for (int j = 0; j < 4; ++j)
		{
			row += QString::number(matrix[i * 4 + j]) + " ";
		}
		m_Controls.textBrowser->append(row);
	}
}

void DianaSeven::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  //// iterate all selected objects, adjust warning visibility
  foreach (mitk::DataNode::Pointer node, nodes)
  {
    if (node.IsNotNull() && dynamic_cast<mitk::Image *>(node->GetData()))
    {
  //    m_Controls.labelWarning->setVisible(false);
  //    m_Controls.buttonPerformImageProcessing->setEnabled(true);
      return;
    }
  }

  //m_Controls.labelWarning->setVisible(true);
  //m_Controls.buttonPerformImageProcessing->setEnabled(false);
}

void DianaSeven::DoImageProcessing()
{
  //QList<mitk::DataNode::Pointer> nodes = this->GetDataManagerSelection();
  //if (nodes.empty())
  //  return;

  //mitk::DataNode *node = nodes.front();

  //if (!node)
  //{
  //  // Nothing selected. Inform the user and return
  //  QMessageBox::information(nullptr, "Template", "Please load and select an image before starting image processing.");
  //  return;
  //}

  //// here we have a valid mitk::DataNode

  //// a node itself is not very useful, we need its data item (the image)
  //mitk::BaseData *data = node->GetData();
  //if (data)
  //{
  //  // test if this data item is an image or not (could also be a surface or something totally different)
  //  mitk::Image *image = dynamic_cast<mitk::Image *>(data);
  //  if (image)
  //  {
  //    std::stringstream message;
  //    std::string name;
  //    message << "Performing image processing for image ";
  //    if (node->GetName(name))
  //    {
  //      // a property called "name" was found for this DataNode
  //      message << "'" << name << "'";
  //    }
  //    message << ".";
  //    MITK_INFO << message.str();

  //    // actually do something here...
  //  }
  //}
}

void DianaSeven::InitGlobalVariable()
{
	m_JointAngleLineEdits.push_back(m_Controls.RobotJoint1AngleLineEdit);
	m_JointAngleLineEdits.push_back(m_Controls.RobotJoint2AngleLineEdit);
	m_JointAngleLineEdits.push_back(m_Controls.RobotJoint3AngleLineEdit);
	m_JointAngleLineEdits.push_back(m_Controls.RobotJoint4AngleLineEdit);
	m_JointAngleLineEdits.push_back(m_Controls.RobotJoint5AngleLineEdit);
	m_JointAngleLineEdits.push_back(m_Controls.RobotJoint6AngleLineEdit);
	m_JointAngleLineEdits.push_back(m_Controls.RobotJoint7AngleLineEdit);

	m_ImpedaLineEdits.push_back(m_Controls.arrStiffXLineEdit);
	m_ImpedaLineEdits.push_back(m_Controls.arrStiffYLineEdit);
	m_ImpedaLineEdits.push_back(m_Controls.arrStiffZLineEdit);
	m_ImpedaLineEdits.push_back(m_Controls.arrStiffRXLineEdit);
	m_ImpedaLineEdits.push_back(m_Controls.arrStiffRYLineEdit);
	m_ImpedaLineEdits.push_back(m_Controls.arrStiffRZLineEdit);
	m_ImpedaLineEdits.push_back(m_Controls.DampingRatioLineEdit);
}

//int main()
//{
//	std::cout << "Hello World!\n";
//	srv_net_st* pinfo = new srv_net_st();
//	memset(pinfo->SrvIp, 0x00, sizeof(pinfo->SrvIp));
//	memcpy(pinfo->SrvIp, "192.168.10.75", strlen("192.168.10.75"));
//	pinfo->LocHeartbeatPort = 0;
//	pinfo->LocRobotStatePort = 0;
//	pinfo->LocSrvPort = 0;
//	int ret = initSrv(nullptr, nullptr, pinfo);
//	if (ret < 0)
//	{
//		printf("192.168.10.75 initSrv failed! Return value = %d\n", ret);
//	}
//	if (pinfo)
//	{
//		delete pinfo;
//		pinfo = nullptr;
//	}
//	releaseBrake();
//}


