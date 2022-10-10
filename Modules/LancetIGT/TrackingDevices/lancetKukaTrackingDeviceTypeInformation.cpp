#include "lancetKukaTrackingDeviceTypeInformation.h"
#include "kukaRobotDevice.h"
#include "lancetVegaTrackingDevice.h"

namespace lancet
{
    std::string KukaRobotTypeInformation::GetTrackingDeviceName()
    {
        return "Kuka";
    }

    mitk::TrackingDeviceData KukaRobotTypeInformation::GetDeviceDataLancetKukaTrackingDevice()
    {
        mitk::TrackingDeviceData data = { KukaRobotTypeInformation::GetTrackingDeviceName(), "Kuka Robot", "cube", "X" };
        return data;
    }

    KukaRobotTypeInformation::KukaRobotTypeInformation()
    {
        this->m_DeviceName = GetTrackingDeviceName();
        this->m_TrackingDeviceData.push_back(GetDeviceDataLancetKukaTrackingDevice());
    }

    mitk::TrackingDeviceSource::Pointer KukaRobotTypeInformation::CreateTrackingDeviceSource(
        mitk::TrackingDevice::Pointer trackingDevice,
        mitk::NavigationToolStorage::Pointer navigationTools,
        std::string* errorMessage,
        std::vector<int>* toolCorrespondencesInToolStorage)
    {
        
        mitk::TrackingDeviceSource::Pointer returnValue = mitk::TrackingDeviceSource::New();
        KukaRobotDevice::Pointer thisDevice = dynamic_cast<KukaRobotDevice*>(trackingDevice.GetPointer());
        *toolCorrespondencesInToolStorage = std::vector<int>();
        //add single tool to kuka robot
        mitk::NavigationTool::Pointer thisNavigationTool = navigationTools->GetTool(0);
        toolCorrespondencesInToolStorage->push_back(0);
        bool toolAddSuccess = thisDevice->AddTool(thisNavigationTool->GetToolName().c_str(), thisNavigationTool->GetCalibrationFile().c_str());
        if (!toolAddSuccess)
        {
          //todo: error handling
          errorMessage->append("Can't add tool, is the SROM-file valid?");
          return nullptr;
        }
        thisDevice->GetTool(0)->SetToolTipPosition(thisNavigationTool->GetToolTipPosition(), thisNavigationTool->GetToolAxisOrientation());

        returnValue->SetTrackingDevice(thisDevice);
        return returnValue;
    }
}
