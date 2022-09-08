#include "QLancetWorkbenchWindowToolBarWidget.h"

#include <QResizeEvent>
#include <QSharedPointer>

struct QLancetWorkbenchWindowToolBarWidget::Imp
{
	QSharedPointer<QWidget> toolBarWidget;
};

QLancetWorkbenchWindowToolBarWidget::QLancetWorkbenchWindowToolBarWidget(QWidget* parent)
	: QToolBar(parent)
	, imp(std::make_shared<Imp>())
{
	this->setToolBarWidget(nullptr);
}

QLancetWorkbenchWindowToolBarWidget::~QLancetWorkbenchWindowToolBarWidget()
{
	
}

QWidget* QLancetWorkbenchWindowToolBarWidget::toolBarWidget() const
{
	return this->imp->toolBarWidget.data();
}

void QLancetWorkbenchWindowToolBarWidget::setToolBarWidget(QWidget* widget)
{
	this->imp->toolBarWidget.reset(widget);
}

void QLancetWorkbenchWindowToolBarWidget::resizeEvent(QResizeEvent *event)
{
	if(nullptr != this->toolBarWidget())
	{
		this->toolBarWidget()->setMinimumSize(event->size());
		this->toolBarWidget()->setMaximumSize(event->size());
	}
	return QToolBar::resizeEvent(event);
}