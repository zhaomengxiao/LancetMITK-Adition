#include "QLinkingHardWareEditor.h"
#include "ui_QLinkingHardWareEditor.h"
#include "internal/org_mitk_lancet_linkinghardware_editor_Activator.h"

#include <mitkLogMacros.h>
#include <mitkTrackingDevice.h>
#include <mitkDataStorageEditorInput.h>

#include <berryUIException.h>
#include <berryIWorkbenchPage.h>
#include <berryIBerryPreferences.h>
#include <berryIPreferencesService.h>

// ctk
#include <ctkServiceTracker.h>

// lancet
#include <lancetIDevicesAdministrationService.h>
#include <internal/lancetTrackingDeviceManage.h>

const QString QLinkingHardWareEditor::EDITOR_ID = "org.mitk.lancet.linkinghardware.editor";

struct QLinkingHardWareEditor::QLinkingHardWareEditorPrivateImp
{
	QLinkingHardWareEditorPrivateImp()
		: prefServiceTracker(PluginActivator::GetContext()) 
	{
	}

	QWidget* shallowCopyWidget = nullptr;
	Ui::QLinkingHardWareEditor m_Controls;

	berry::IBerryPreferences::Pointer preferences;
	
	ctkServiceTracker<berry::IPreferencesService*> prefServiceTracker;
};

QLinkingHardWareEditor::QLinkingHardWareEditor()
	: imp(std::make_shared<QLinkingHardWareEditorPrivateImp>())
{
	MITK_DEBUG << "log";
	this->imp->prefServiceTracker.open();
}

QLinkingHardWareEditor::~QLinkingHardWareEditor()
{
	this->imp->shallowCopyWidget = nullptr;
	this->GetSite()->GetPage()->RemovePartListener(this);
}

void QLinkingHardWareEditor::CreatePartControl(QWidget* parent)
{
	this->imp->m_Controls.setupUi(parent);

	this->imp->shallowCopyWidget = this->imp->m_Controls.widget;
	this->UpdateQtWidgetQssStyle(":/org.mitk.lancet.linkinghardwareeditor/linkinghardwareeditor.qss");
	this->ConnectedQtSlotForDevicesService();
	this->UpdateQtWidgetStyleOfDeviceService("Vega");
	this->UpdateQtWidgetStyleOfDeviceService("Kuka");
}

berry::IPreferences::Pointer QLinkingHardWareEditor::GetPreferences() const
{
	MITK_DEBUG << "log";
	berry::IPreferencesService* prefService = this->imp->prefServiceTracker.getService();
	if (prefService != nullptr)
	{
		return prefService->GetSystemPreferences()->Node(GetSite()->GetId());
	}
	return berry::IPreferences::Pointer(nullptr);
}

void QLinkingHardWareEditor::Init(berry::IEditorSite::Pointer site, berry::IEditorInput::Pointer input)
{
	if (input.Cast<mitk::DataStorageEditorInput>().IsNull())
		throw berry::PartInitException("Invalid Input: Must be mitk::DataStorageEditorInput");

	this->SetSite(site);
	this->SetInput(input);

	this->imp->preferences = this->GetPreferences().Cast<berry::IBerryPreferences>();
	if (this->imp->preferences.IsNotNull())
	{
		this->imp->preferences->OnChanged.AddListener(berry::MessageDelegate1<QLinkingHardWareEditor, const berry::IBerryPreferences*>
			(this, &QLinkingHardWareEditor::OnPreferencesChanged));
	}

	// open service
	if (this->GetDevicesService())
	{
		auto scanner = this->GetDevicesService()->GetConnector();
	}
}

berry::IPartListener::Events::Types QLinkingHardWareEditor::GetPartEventTypes() const
{
	return Events::CLOSED | Events::OPENED | Events::HIDDEN | Events::VISIBLE;
}

lancet::IDevicesAdministrationService* QLinkingHardWareEditor::GetDevicesService()
{
	auto context = PluginActivator::GetContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	return context->getService<lancet::IDevicesAdministrationService>(serviceRef);
}

void QLinkingHardWareEditor::SetFocus()
{
	if (this->imp->shallowCopyWidget)
	{
		this->imp->shallowCopyWidget->setFocus();
	}
}

bool QLinkingHardWareEditor::ConnectedQtSlotForDevicesService()
{
	auto sender = this->GetDevicesService();
	if (sender)
	{
		lancet::IDevicesAdministrationService* o = sender;
		QObject::connect(o, &lancet::IDevicesAdministrationService::TrackingDeviceStateChange,
			this, &QLinkingHardWareEditor::OnDevicesStatePropertyChange);
		return true;
	}
	return false;
}

void QLinkingHardWareEditor::UpdateQtWidgetStyleOfDeviceService(const QString& name)
{
	if (this->GetDevicesService() && this->GetDevicesService()->GetConnector().IsNotNull())
	{
		auto state = this->GetDevicesService()->GetConnector()->IsStartTrackingDevice(name.toStdString());
		this->OnDevicesStatePropertyChange(name.toStdString(), lancet::TrackingDeviceManage::TrackingDeviceState::Tracking);
	}
	this->OnDevicesStatePropertyChange(name.toStdString(), lancet::TrackingDeviceManage::TrackingDeviceState::UnInstall);
}

void QLinkingHardWareEditor::UpdateQtWidgetQssStyle(const QString& filename)
{
	QFile qssFile(filename);

	if (!qssFile.open(QIODevice::ReadOnly))
	{
		MITK_WARN << __func__ << __LINE__ << ":" << "error load file "
			<< filename << "\n"
			<< "error: " << qssFile.errorString();
	}
	this->imp->m_Controls.widget->setStyleSheet(QLatin1String(qssFile.readAll()));
}

void QLinkingHardWareEditor::OnPreferencesChanged(const berry::IBerryPreferences*)
{
}

#define GenerateDeviceTrackingRCFileName(ndi, robor) \
QString("border-image: url(:/org.mitk.lancet.linkinghardwareeditor/registration/robot_%1_NID_%2.png);")\
    .arg(QString::number(robor)) \
    .arg(QString::number(ndi))

void QLinkingHardWareEditor::OnDevicesStatePropertyChange(std::string name, lancet::TrackingDeviceManage::TrackingDeviceState state)
{
	bool vegeTrackingState = false;
	bool kukaTrackingState = false;
	if (name == "Vega" && state & lancet::TrackingDeviceManage::TrackingDeviceState::Tracking)
	{
		vegeTrackingState = true;
	}
	else if (name == "Kuka" && state & lancet::TrackingDeviceManage::TrackingDeviceState::Tracking)
	{
		kukaTrackingState = true;
	}

	QString styleSheet = GenerateDeviceTrackingRCFileName(vegeTrackingState, kukaTrackingState);
	this->imp->m_Controls.widget->setStyleSheet(styleSheet);
}