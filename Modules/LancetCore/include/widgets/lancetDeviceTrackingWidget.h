#ifndef lancetDeviceTrackingWidget_H
#define lancetDeviceTrackingWidget_H

#include <QWidget>
#include <itkSmartPointer.h>


namespace mitk
{
  class NavigationDataSource;
}

namespace lancet
{

class DeviceTrackingWidget 
  : public QWidget
{
  Q_OBJECT
public:
  DeviceTrackingWidget(QWidget* parent = nullptr);
  virtual ~DeviceTrackingWidget();
public:
  virtual void SetTrackingToolSource(itk::SmartPointer<mitk::NavigationDataSource>);
  virtual itk::SmartPointer<mitk::NavigationDataSource> GetTrackingToolSource() const;

  virtual void SetToolVisible(const QString& name, bool visible);
  virtual bool GetToolVisible(const QString& name) const;
  virtual QList<QString> GetToolVisibleArray() const;
private:
  struct DeviceTrackingWidgetPrivateImp;
  std::shared_ptr<DeviceTrackingWidgetPrivateImp> imp;
};

} // !namespace lancet

#endif // !lancetDeviceTrackingWidget_H