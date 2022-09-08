#ifndef LancetWorkbenchWindowBarWidget_H
#define LancetWorkbenchWindowBarWidget_H

#include<QWidget>

namespace Ui
{
	class LancetWorkbenchWindowBarFrom;
} // namespace Ui

class QLancetWorkbenchWindowBarWidget 
	: public QWidget
{
	Q_OBJECT
public:
	explicit QLancetWorkbenchWindowBarWidget(QWidget* = nullptr);

private slots:
	void on_pushbtnExample1_clicked();
	void on_pushbtnExample2_clicked();
	//void on_pushbtnExample3_clicked();
private:
	Ui::LancetWorkbenchWindowBarFrom* ui;
};

#endif // !LancetWorkbenchWindowBarWidget_H