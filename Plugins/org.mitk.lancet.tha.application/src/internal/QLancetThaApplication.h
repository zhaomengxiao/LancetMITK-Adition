#ifndef QLancetThaApplication_H
#define QLancetThaApplication_H

// Berry
#include <berryIApplication.h>

// Qt
#include <QObject>
#include <QVariant>
#include "org_mitk_lancet_tha_application_Export.h"

class org_mitk_lancet_tha_application_EXPORT QLancetThaApplication 
	: public QObject, public berry::IApplication
{
	Q_OBJECT
	Q_INTERFACES(berry::IApplication)
public:
	explicit QLancetThaApplication(QObject* = nullptr);
	virtual ~QLancetThaApplication();

	virtual QVariant Start(berry::IApplicationContext* context) override;

	virtual void Stop() override;
};

#endif // !QLancetThaApplication_H
