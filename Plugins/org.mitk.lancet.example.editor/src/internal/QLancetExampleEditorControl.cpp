#include "QLancetExampleEditorControl.h"

#include "ui_qlancetexampleeditorfrom.h"

QLancetExampleEditorControl::QLancetExampleEditorControl(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::QLancetExampleEditorFrom())
{
	ui->setupUi(parent);
}

QLancetExampleEditorControl::~QLancetExampleEditorControl()
{
}
