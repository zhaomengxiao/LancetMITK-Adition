#pragma once
#include "ui_DianaSevenControls.h"
#include <qwidget.h>
#include <AriemediCamera.h>
class AriemediModule : public QWidget
{
public:
	explicit AriemediModule(Ui::DianaSevenControls aDianaSevenControls, mitk::DataStorage* aDataStorage, AbstractCamera* aAbstractCamera);

	void ConnectArieCameraBtnClicked();

	void StartArieCameraBtnClicked();

	void StartSetAreaBtnClicked();
private:
	void UpdateUI();
	void InitConnection();
	void InitGlobalVariable();
private:
	Ui::DianaSevenControls m_UI;
	mitk::DataStorage* m_Datastorage;

	AriemediCamera* m_Camera;
	std::map<std::string, QLabel*> m_ToolNameLabelMap;
};

