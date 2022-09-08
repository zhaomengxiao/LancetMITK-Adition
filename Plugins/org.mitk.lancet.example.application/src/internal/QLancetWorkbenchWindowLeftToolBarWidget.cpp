#include "QLancetWorkbenchWindowLeftToolBarWidget.h"

#include "ui_qlancetworkbenchwindowlefttoolbarform.h"
#include <QResizeEvent>
#include <QSharedPointer>


QLancetWorkbenchWindowLeftToolBarWidget::QLancetWorkbenchWindowLeftToolBarWidget(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::QLancetWorkbenchWindowLeftToolBarForm)
{
	this->ui->setupUi(this);
}

QLancetWorkbenchWindowLeftToolBarWidget::~QLancetWorkbenchWindowLeftToolBarWidget()
{

}
