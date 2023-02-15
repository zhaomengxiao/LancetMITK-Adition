#include "lancetdevicetrackingwidget.h"
#include "ui_lancetDeviceTrackingWidgetControls.h"

// Qt
#include <QTimer>
#include <QImage>

// mitk
#include <mitkNavigationDataSource.h>

// us
#include "usGetModuleContext.h"
#include "usModuleContext.h"
#include "usModule.h"
#include "usModuleResource.h"
#include "usModuleResourceStream.h"

namespace lancet {

struct DeviceTrackingWidget::DeviceTrackingWidgetPrivateImp
{
    Ui::DeviceTrackingWidgetControls ui;

    QTimer tm;
		QVector<mitk::NavigationDataSource::Pointer> toolSourceArray;

		static QMap<QString, QString> mapToolTrackingQSS;
};
QMap<QString, QString>
DeviceTrackingWidget::DeviceTrackingWidgetPrivateImp::mapToolTrackingQSS =
{
	{"Probe", 
"QCheckBox::indicator { width: 0px; }\
QCheckBox:enabled {border: 1px solid rgb(0, 170, 0); image: url(:/markers/marker/probe_marker.png); }\
QCheckBox:!enabled {background-color: rgb(170, 0, 0); border-image: url(:/markers/marker/probe_marker.png); }"},
	{"Robot",
"QCheckBox::indicator { width: 0px; }\
QCheckBox:enabled {border: 1px solid rgb(0, 170, 0); image: url(:/markers/marker/robot_marker.png); }\
QCheckBox:!enabled {background-color: rgb(170, 0, 0); border-image: url(:/markers/marker/robot_marker.png); }"},
};

DeviceTrackingWidget::DeviceTrackingWidget(QWidget *parent)
    : QWidget(parent)
    , imp(std::make_shared<DeviceTrackingWidgetPrivateImp>())
{
	Q_INIT_RESOURCE(mitkLancetCoreResources);
  this->imp->ui.setupUi(this);

	connect(&this->imp->tm, &QTimer::timeout, 
		this, &DeviceTrackingWidget::OnTrackingToolStateUpdate);

	this->imp->tm.setInterval(100);
	this->imp->tm.start();
}

DeviceTrackingWidget::~DeviceTrackingWidget()
{
	this->imp->tm.stop();
}

bool DeviceTrackingWidget::HasTrackingToolSource(const QString& dataSourceName) const
{
	return this->GetTrackingToolSource(dataSourceName).IsNotNull();
}

void DeviceTrackingWidget::AddTrackingToolSource(itk::SmartPointer<mitk::NavigationDataSource> dataSource)
{
	if (dataSource.IsNotNull() && false == this->HasTrackingToolSource(dataSource->GetName().c_str()))
	{
		this->imp->toolSourceArray.push_back(dataSource);
	}
}

bool DeviceTrackingWidget::RemoveTrackingToolSource(const QString& dataSourceName)
{
	for (int index = 0; index < this->imp->toolSourceArray.size(); ++index)
	{
		if (this->imp->toolSourceArray[index].IsNotNull()
			&& dataSourceName == this->imp->toolSourceArray[index]->GetName().c_str())
		{
			this->imp->toolSourceArray.removeAt(index);
			return true;
		}
	}
	return false;
}

itk::SmartPointer<mitk::NavigationDataSource> 
DeviceTrackingWidget::GetTrackingToolSource(const QString& dataSourceName) const
{
	for (int index = 0; index < this->imp->toolSourceArray.size(); ++index)
	{
		if (this->imp->toolSourceArray[index].IsNotNull()
			&& dataSourceName == this->imp->toolSourceArray[index]->GetName().c_str())
		{
			return this->imp->toolSourceArray[index];
		}
	}
	return itk::SmartPointer<mitk::NavigationDataSource>();
}

bool DeviceTrackingWidget::IsInitializeTrackingTool(const QString& widgetName) const
{
	return this->FindTrackingToolWidget(widgetName) != nullptr;
}

void DeviceTrackingWidget::InitializeTrackingToolVisible(const QString& widgetName)
{
	if (this->IsInitializeTrackingTool(widgetName))
	{
		return;
	}

	auto findToolValue = this->imp->mapToolTrackingQSS.find(widgetName);
	if (findToolValue != this->imp->mapToolTrackingQSS.end())
	{
		QCheckBox* widget = new QCheckBox(this->imp->ui.widgetCenter);
		widget->setObjectName(QString("%1").arg(findToolValue.key()));
		widget->setStyleSheet(findToolValue.value());
		widget->setVisible(true);
		widget->setMinimumSize(QSize(60, 60));
		widget->setMaximumSize(QSize(60, 60));
		QImage* widgetBackgroundImage = new QImage();
		MITK_INFO << widgetBackgroundImage->load(":/markers/marker/robot_marker.png");

		this->imp->ui.horizontalLayoutCenter->addWidget(widget);
	}
	else
	{

	}
}

void DeviceTrackingWidget::UnInitializeTrackingToolVisible(const QString& widgetName)
{
	QCheckBox* widget = this->FindTrackingToolWidget(widgetName);
	if (widget != nullptr)
	{
		widget->deleteLater();
	}
}

void DeviceTrackingWidget::SetTrackingToolVisible(const QString& widgetName, bool visible)
{
	QCheckBox* widget = this->FindTrackingToolWidget(widgetName);
	if (widget != nullptr)
	{
		widget->setVisible(visible);
	}
}

bool DeviceTrackingWidget::GetTrackingToolVisible(const QString& widgetName) const
{
	QCheckBox* widget = this->FindTrackingToolWidget(widgetName);
	if (widget != nullptr)
	{
		return widget->isVisible();
	}
	return false;
}

QStringList DeviceTrackingWidget::GetTrackingToolNameOfWidgets() const
{
	return this->imp->mapToolTrackingQSS.keys();
}

void DeviceTrackingWidget::SetTrackingToolEnable(const QString& widgetName, bool isEnable)
{
	QCheckBox* widget = this->FindTrackingToolWidget(widgetName);
	if (nullptr != widget)
	{
		widget->setEnabled(isEnable);
	}
}

bool DeviceTrackingWidget::GetTrackingToolEnable(const QString& widgetName) const
{
	QCheckBox* widget = this->FindTrackingToolWidget(widgetName);
	if (nullptr != widget)
	{
		return widget->isEnabled();
	}
	return false;
}

void DeviceTrackingWidget::OnTrackingToolStateUpdate()
{
	for (auto& trackingToolSource : this->imp->toolSourceArray)
	{
		if (trackingToolSource.IsNull()) continue;

		for (auto& toolName : this->GetTrackingToolNameOfWidgets())
		{
			auto findToolIndex = trackingToolSource->GetOutputIndex(toolName.toStdString());
			if (-1 != findToolIndex)
			{
				auto trackingTool = trackingToolSource->GetOutput(findToolIndex);
				if (nullptr != trackingTool 
					&& trackingTool->IsDataValid() != this->GetTrackingToolEnable(toolName))
				{
					this->SetTrackingToolEnable(toolName, trackingTool->IsDataValid());
				}
				break;
			}
		}
	}
}

QCheckBox* DeviceTrackingWidget::FindTrackingToolWidget(const QString& widgetName) const
{
	QList<QCheckBox*> toolWidgetList = this->findChildren<QCheckBox*>();

	for (auto& toolWidget : toolWidgetList)
	{
		if (widgetName == toolWidget->objectName())
		{
			return toolWidget;
		}
	}
	return nullptr;
}

} // namespace lancet
