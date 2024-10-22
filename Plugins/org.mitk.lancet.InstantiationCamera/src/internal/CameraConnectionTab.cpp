#include "CameraConnectionTab.h"

CameraConnectionTab::CameraConnectionTab(Ui::InstantiationCameraControls ui, mitk::DataStorage* aDataStorage,
	AriemediCamera* aAriemediCamera, QWidget* parent)
{
	m_UI = ui;
	m_DataStorage = aDataStorage;
	m_Camera = aAriemediCamera;
	m_CameraRectLabel = new CameraRectLabel(m_UI.LeftImageLabel);
	InitConnection();
}

void CameraConnectionTab::InitConnection()
{
	connect(m_UI.CameraConnectBtn, &QPushButton::clicked, this, &CameraConnectionTab::CameraConnectBtnClicked);
	connect(m_UI.CameraDisconnectBtn, &QPushButton::clicked, this, &CameraConnectionTab::CameraDisconnectBtnClicked);
	connect(m_UI.CameraStartBtn, &QPushButton::clicked, this, &CameraConnectionTab::CameraStartBtnClicked);
	connect(m_UI.DrawRectBtn, &QPushButton::clicked, this, &CameraConnectionTab::DrawRectBtnClicked);

	connect(m_Camera, &AriemediCamera::CameraUpdateClock, this, &CameraConnectionTab::UpdateUIDisplay);
}

void CameraConnectionTab::CameraConnectBtnClicked()
{
	m_Camera->Connect();
}

void CameraConnectionTab::CameraDisconnectBtnClicked()
{
	m_Camera->Disconnect();
}

void CameraConnectionTab::CameraStartBtnClicked()
{
	m_Camera->Start();
	auto imageSize = m_Camera->GetImageSize();
	m_ImageWidth = imageSize.first;
	m_ImageHeight = imageSize.second;
	//m_UI.LeftImageLabel;
}

void CameraConnectionTab::DrawRectBtnClicked()
{
	m_CameraRectLabel->StartDraw();
}

void CameraConnectionTab::UpdateUIDisplay()
{
	UpdateUIToolsData();
	UpdateUIImages();
}

void CameraConnectionTab::UpdateUIToolsData()
{
	GetAndUpdateToolTip(m_Camera->GetToolTipByName("PKADrill"), m_UI.label_2);
}

void CameraConnectionTab::UpdateUIImages()
{
	auto images = m_Camera->GetImageData();
	UpdateSingleImage(images.first, m_UI.LeftImageLabel);
	UpdateSingleImage(images.second, m_UI.RightImageLabel);
}

void CameraConnectionTab::InitUI()
{
	m_UI.label->setText("Probe");
}

void CameraConnectionTab::GetAndUpdateToolTip(Eigen::Vector3d tempTip, QLabel* label)
{
	if (tempTip[0] == 0 && tempTip[1] == 0 && tempTip[2] == 0)
	{
		if (label != nullptr)
		{
			QString str = "x:0  y:0 z:0";
			label->setText(str);
			/*std::cout << str << std::endl;*/
			label->setStyleSheet("QLabel { color : red; }");
		}
	}
	else
	{
		if (label != nullptr)
		{
			QString str = "x:" + QString::number(tempTip[0]) + " "
				+ "y:" + QString::number(tempTip[1]) + " "
				+ "z:" + QString::number(tempTip[2]);
			label->setText(str);
			label->setStyleSheet("QLabel { color : green; }");
			/*std::cout << str << std::endl;*/

		}
	}

}

void CameraConnectionTab::UpdateSingleImage(char* aImage, QLabel* aLabel)
{
	QImage image(reinterpret_cast<uchar*>(aImage), m_ImageWidth, m_ImageHeight, QImage::Format_Grayscale8);

	aLabel->setPixmap(QPixmap::fromImage(image));
}


