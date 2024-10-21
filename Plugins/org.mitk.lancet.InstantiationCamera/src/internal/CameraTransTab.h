#pragma once
#include "ui_InstantiationCameraControls.h"
#include <AriemediCamera.h>

class CameraTransTab
{
public:
	explicit CameraTransTab(Ui::InstantiationCameraControls ui, AriemediCamera* aAriemediCamera, QWidget* parent = nullptr);
	void InitConnection();
	void ConverRomFileBtnClicked();
private:
	Ui::InstantiationCameraControls m_UI;
	AriemediCamera* m_Camera;
};

