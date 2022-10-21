#ifndef QLancetThaApplicationPerspective_H
#define QLancetThaApplicationPerspective_H

// Berry
#include <berryIPerspectiveFactory.h>

// Qt
#include <QObject>
#include "org_mitk_lancet_tha_application_Export.h"

class org_mitk_lancet_tha_application_EXPORT QLancetThaApplicationPerspective
	: public QObject, public berry::IPerspectiveFactory
{
	Q_OBJECT
	Q_INTERFACES(berry::IPerspectiveFactory)

public:
	explicit QLancetThaApplicationPerspective(QObject* = nullptr);

public:
	virtual void CreateInitialLayout(berry::IPageLayout::Pointer layout) override;
};

#endif // !QLancetThaApplicationPerspective_H
