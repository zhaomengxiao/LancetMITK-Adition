#include "widgets/lancetDeviceTrackingWidget.h"
#include "ui_lancetDeviceTrackingWidget.h"
// Qt
#include <QTimer>

// mitk
#include <mitkNavigationDataSource.h>
namespace lancet
{
	struct DeviceTrackingWidget::DeviceTrackingWidgetPrivateImp
	{
		Ui::DeviceTrackingWidgetForm ui;

		QTimer tm;
		QList<QString> toolVisibleList;
		mitk::NavigationDataSource::Pointer toolSource;
	};

DeviceTrackingWidget::DeviceTrackingWidget(QWidget* parent)
  : QWidget(parent)
  , imp(std::make_shared<DeviceTrackingWidgetPrivateImp>())
{
	this->imp->ui.setupUi(this);

	connect(&this->imp->tm, &QTimer::timeout, this, [=]()
	{
    QList<QWidget*> toolWidgetList = this->findChildren<QWidget*>();
    for(auto toolWidget: toolWidgetList)
    {
      // toolname + Widget = Instance object of tool widget object.
      // Extract tool name string using small period object name.
      QString toolName = toolWidget->objectName().replace("Widget", "");
      if(this->GetToolVisible(toolName))
      {
        if(this->GetTrackingToolSource().IsNotNull())
        {
          auto toolDataIndex = this->GetTrackingToolSource()->GetOutputIndex(toolName.toStdString());
          if(toolDataIndex != -1)
          {
            auto toolTrackingData = this->GetTrackingToolSource()->GetOutput(toolDataIndex);
            toolWidget->setEnabled(toolTrackingData->IsDataValid());
          }
        }
      }
      else if(toolWidget->isVisible())
      {
        toolWidget->setVisible(false);
      }
    }
  });
	this->imp->tm.start(60);
}

DeviceTrackingWidget::~DeviceTrackingWidget()
{
  this->imp->tm.stop();
}

void DeviceTrackingWidget::SetTrackingToolSource(itk::SmartPointer<mitk::NavigationDataSource> toolSource)
{
  this->imp->toolSource = toolSource;
}

itk::SmartPointer<mitk::NavigationDataSource> DeviceTrackingWidget::GetTrackingToolSource() const
{
  return this->imp->toolSource;
}

void DeviceTrackingWidget::SetToolVisible(const QString& name, bool visible)
{
  if(true == visible && false == this->imp->toolVisibleList.contains(name))
  {
    this->imp->toolVisibleList.append(name);
  }else if(false == visible && this->imp->toolVisibleList.contains(name))
  {
    this->imp->toolVisibleList.removeAll(name);
  }
}

bool DeviceTrackingWidget::GetToolVisible(const QString& name) const
{
  return this->imp->toolVisibleList.contains(name);
}

QList<QString> DeviceTrackingWidget::GetToolVisibleArray() const
{
  return this->imp->toolVisibleList;
}

}