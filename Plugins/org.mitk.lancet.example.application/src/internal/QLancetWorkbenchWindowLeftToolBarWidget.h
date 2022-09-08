#ifndef QLancetWorkbenchWindowLeftToolBarWidget_H
#define QLancetWorkbenchWindowLeftToolBarWidget_H

#include <QToolBar>

namespace Ui
{
	class QLancetWorkbenchWindowLeftToolBarForm;
} // namespace Ui

class QLancetWorkbenchWindowLeftToolBarWidget : public QWidget
{
	Q_OBJECT
public:
	explicit QLancetWorkbenchWindowLeftToolBarWidget(QWidget* = nullptr);
	virtual ~QLancetWorkbenchWindowLeftToolBarWidget();

private:
	Ui::QLancetWorkbenchWindowLeftToolBarForm* ui;
};

#endif // !QLancetWorkbenchWindowLeftToolBarWidget_H