#ifndef LANCET_DEVICETRACKINGWIDGET_H
#define LANCET_DEVICETRACKINGWIDGET_H

#include <QWidget>
#include <QCheckBox>

#include <itkSmartPointer.h>

namespace mitk { class NavigationDataSource; }
namespace lancet {

class DeviceTrackingWidget : public QWidget
{
  Q_OBJECT
public:
	struct Tools
	{		
		static const char* const Cart;
		static const char* const Femur;
		static const char* const Pelvis;
		static const char* const Probe;
		static const char* const Robot;
	};
public:
  explicit DeviceTrackingWidget(QWidget *parent = nullptr);
	virtual ~DeviceTrackingWidget();

	virtual bool HasTrackingToolSource(const QString&) const;
	virtual void AddTrackingToolSource(itk::SmartPointer<mitk::NavigationDataSource>);
	virtual bool RemoveTrackingToolSource(const QString&);
	virtual itk::SmartPointer<mitk::NavigationDataSource> GetTrackingToolSource(const QString&) const;

	virtual bool IsInitializeTrackingTool(const QString&) const;
	virtual void InitializeTrackingToolVisible(const QString&);
	virtual void UnInitializeTrackingToolVisible(const QString&);

	virtual void SetTrackingToolVisible(const QString&, bool);
	virtual bool GetTrackingToolVisible(const QString&) const;
	virtual QStringList GetTrackingToolNameOfWidgets() const;

	virtual void SetTrackingToolEnable(const QString&, bool);
	virtual bool GetTrackingToolEnable(const QString&) const;

public slots:
	void OnTrackingToolStateUpdate();
protected:
	QCheckBox* FindTrackingToolWidget(const QString&) const;
private:
    struct DeviceTrackingWidgetPrivateImp;
    std::shared_ptr<DeviceTrackingWidgetPrivateImp> imp;
};

} // namespace lancet

#endif // LANCET_DEVICETRACKINGWIDGET_H
