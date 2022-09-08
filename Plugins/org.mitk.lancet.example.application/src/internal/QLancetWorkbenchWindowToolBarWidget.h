#ifndef QLancetWorkbenchWindowToolBarWidget_H
#define QLancetWorkbenchWindowToolBarWidget_H

#include <QToolBar>

class QResizeEvent;
class QLancetWorkbenchWindowToolBarWidget : public QToolBar
{
	Q_OBJECT
public:

	explicit QLancetWorkbenchWindowToolBarWidget(QWidget* = nullptr);
	virtual ~QLancetWorkbenchWindowToolBarWidget();

	QWidget* toolBarWidget() const;
	void setToolBarWidget(QWidget*);
	
protected:
	virtual void resizeEvent(QResizeEvent *event) override;
private:
	struct Imp;
	std::shared_ptr<Imp> imp;
};

#endif // !QLancetWorkbenchWindowToolBarWidget_H