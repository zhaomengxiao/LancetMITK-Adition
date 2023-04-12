#include "tst_thaWorkbenchWindow.h"

#include <QEventLoop>
#include <QCloseEvent>

tst_thaWorkbenchWindow::tst_thaWorkbenchWindow(QWidget* parent/* = nullptr */)
	: QMainWindow(parent)
{
	this->event.reset(new QEventLoop());
}

void tst_thaWorkbenchWindow::exec()
{
	this->show();
	this->event->exec();
}

void tst_thaWorkbenchWindow::closeEvent(QCloseEvent* event)
{
	this->event->quit();
	return QMainWindow::closeEvent(event);
}
