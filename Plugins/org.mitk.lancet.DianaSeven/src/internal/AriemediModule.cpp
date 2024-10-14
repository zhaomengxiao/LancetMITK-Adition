#include "AriemediModule.h"

AriemediModule::AriemediModule(Ui::DianaSevenControls aDianaSevenControls, mitk::DataStorage* aDataStorage, AbstractCamera* aAbstractCamera)
{
	m_UI = aDianaSevenControls;
	m_Datastorage = aDataStorage;
	m_Camera = dynamic_cast<AriemediCamera*>(aAbstractCamera);
}

void AriemediModule::ConnectArieCameraBtnClicked()
{
	m_Camera->Connect();
}

void AriemediModule::StartArieCameraBtnClicked()
{
	m_Camera->Start();
}

void AriemediModule::StartSetAreaBtnClicked()
{

}

void AriemediModule::UpdateUI()
{
	for (auto item : m_ToolNameLabelMap)
	{
		Eigen::Vector3d tip = m_Camera->GetToolTipByName(item.first);
		QString str;
		str = QString::number(tip(0)) + "," + QString::number(tip(1)) + "," + QString::number(tip(2));
		item.second->setText(str);
	}
	auto size = this->m_Camera->GetImageSize();

	auto imageData = m_Camera->GetImageData();

	QImage leftImage(reinterpret_cast<const uchar*>(imageData.first), size.first, size.second, QImage::Format_Grayscale8);
	QImage rightImage(reinterpret_cast<const uchar*>(imageData.second), size.first, size.second, QImage::Format_Grayscale8);
	m_UI.LeftCameraImageLabel->setPixmap(QPixmap::fromImage(leftImage));
	m_UI.RightCameraImageLabel->setPixmap(QPixmap::fromImage(rightImage));
}

void AriemediModule::InitConnection()
{
	connect(m_UI.ConnectArieCameraBtn, &QPushButton::clicked, this, &AriemediModule::ConnectArieCameraBtnClicked);
	connect(m_UI.StartArieCameraBtn, &QPushButton::clicked, this, &AriemediModule::StartArieCameraBtnClicked);
	connect(m_UI.StartSetAreaBtn, &QPushButton::clicked, this, &AriemediModule::StartSetAreaBtnClicked);
}

void AriemediModule::InitGlobalVariable()
{
	m_ToolNameLabelMap = {
		{"RobotEndRF",m_UI.ArieRobotEndRFDataLabel},
		{ "RobotBaseRF",m_UI.ArieRobotBaseRFDataLabel},
		{ "Probe",m_UI.ArieProbeDataLabel} };
}
