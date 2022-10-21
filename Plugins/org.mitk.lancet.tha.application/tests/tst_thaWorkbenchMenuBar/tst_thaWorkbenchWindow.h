#ifndef TST_THAWORKBENCHWINDOW_H
#define TST_THAWORKBENCHWINDOW_H

#include <QMainWindow>
#include <QSharedPointer>

class QEventLoop;
class QCloseEvent;
class tst_thaWorkbenchWindow : public QMainWindow
{
	Q_OBJECT
public:
	tst_thaWorkbenchWindow(QWidget* = nullptr);
	void exec();
protected:
	virtual void closeEvent(QCloseEvent* event) override;
private:
	QSharedPointer<QEventLoop> event;
};

#endif // !TST_THAWORKBENCHWINDOW_H
