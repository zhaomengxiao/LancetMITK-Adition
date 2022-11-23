/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief Declare the device management service function.
 * \ingroup org_mitk_lancet_
 * \version V1.0.0
 * \data 2022-11-10 16:42:57
 * 
 * \par Modify History
 *
 * \author jrl
 * \data 2022-11-10 16:42:57
 * \remark Non
 */
#ifndef LancetTrackingDeviceManage_H
#define LancetTrackingDeviceManage_H

#include <QObject.h>
#include <berryObject.h>
#include <itkSmartPointer.h>

 // ORG_MITK_LANCET_SERVICES_DEVICESADMINISTRATION_PLUGIN
#include "org_mitk_lancet_services_devicesadministration_Export.h"

// Avoid external references to too many modules, and declare the target type 
// in advance.
namespace mitk
{
  class DataStorage;
  class TrackingDevice;
  class TrackingDeviceSource;
  class NavigationToolStorage;
}

class QTimer;
namespace lancet
{
  class NavigationObjectVisualizationFilter;
  /**
   * \ingroup org_mitk_lancet_
   * \brief This is the device management service function class.
   *
   * The user can preload the target device by registering (\c #InstallTrackingDevice()) 
   * and remove the device by unloading (\c #UnInstallTrackingDevice) the management 
   * module of all device objects of the lancet. This abstract method is used to manage the 
   * target equipment, which ensures the scalability and maintainability of functions to 
   * the greatest extent, and also unifies the unified caliber of external users.
   * 
   * \author jrl
   * \version V1.0.0
   * \date 2022-11-10 16:39:23
   * \remark todo: insert comments
   *
   * Contact: sh4a01@163.com
   *
   */
  class ORG_MITK_LANCET_SERVICES_DEVICESADMINISTRATION_PLUGIN TrackingDeviceManage 
      : public QObject, public berry::Object
  {
    Q_OBJECT
  public:
    berryObjectMacro(lancet::TrackingDeviceManage);
    TrackingDeviceManage();
    virtual ~TrackingDeviceManage();

  public:
      enum TrackingDeviceState
      {
          UnInstall = 0x00000000,        ///< 卸载状态
          Install = 0x00000001,          ///< 安装就绪状态
          Connected = 0x00000010,        ///< 连接状态
          Tracking = 0x00000100          ///< 跟踪状态
      };
      Q_ENUM(lancet::TrackingDeviceManage::TrackingDeviceState)
 Q_SIGNALS:
     /**
      * \brief 设备状态改变信号
      *
      * 在设备状态有更改的时候会触发这个信号通知关注者，这个信号发出时，设备内部状态已经更新.
      *
      * \params name
      *
      * \params state
      *
      *
      * \code
      * TrackingDeviceManage* sender = new TrackingDeviceManage();
      * connect(sender, SIGNAL(TrackingDeviceStateChange(std::string, TrackingDeviceState)),
      *   this, [=](std::string name, TrackingDeviceState state) {
      *     // UnInstall ?
      *     bool isUnInstall = state & TrackingDeviceState::UnInstall;
      *     // Install ?
      *     bool isInstall = state & TrackingDeviceState::Install;
      *     // Connected ?
      *     bool isConnected = state & TrackingDeviceState::Connected;
      *     // Tracking ?
      *     bool isTracking = state & TrackingDeviceState::Tracking;
      *  });
      * \endcode
      */
      void TrackingDeviceStateChange(std::string, lancet::TrackingDeviceManage::TrackingDeviceState);
  public:
    /**
     * \brief Install target device.
     *
     * Install the device to which the lancet belongs. The installation here refers 
     * to preloading the device into the management model without any operation on 
     * the device itself. If you are interested in other functions of the device, 
     * you should use the interface of the management model to operate on the device 
     * or data itself.
		 *
		 * \pre Demonstrate how to install the device
		 * \code
		 * lancet::TrackingDeviceManage* trackingDeviceManage = o->GetService();
		 * if(false == trackingDeviceManage->IsInstallTrackingDevice("Robot"))
		 * {
		 *   if(false == trackingDeviceManage->InstallTrackingDevice("Robot", "./toolDataStroage.IGTTool", nullptr))
		 *   {
		 *     std::cout << "install robot tracking device faild! error: " << trackingDeviceManage->GetErrorString();
		 *   }
     *   else
     *   {
     *      trackingDeviceManage->GetTrackingDevice("Robot")->OpenConnection();
     *   }
		 * }
		 * \endcoed
     * 
     * \warning This operation may fail, and the failure information is provided by 
     *          \c #GetErrorString()
     *
     * \params name
		 *              Equipment name. Each type of equipment should have an independent 
     *              name.
		 * \params filename
		 *              Compressed file of tool data set, the target should be in the file 
     *              system or Qt resource system.
		 * \params dataStorage
		 *              Storage target of equipment data visualization.The passed in nullptr 
     *              value will use the dataset object in the Mitk micro service, otherwise 
     *              the passed in dataset object will be used.
     * 
     * \return Return false if failed, otherwise true.
     * 
     * \see UnInstallTrackingDevice(), GetErrorString(), GetTrackingDevice()
     */
    bool InstallTrackingDevice(const std::string& name, const std::string& filename,
      itk::SmartPointer<mitk::DataStorage> dataStorage = itk::SmartPointer<mitk::DataStorage>());

    /**
     * \brief Uninstall target device.
     * 
     * Uninstall the device to which the lancet belongs. 
		 *
		 * \pre Show me how to uninstall a device
		 * \code
		 * lancet::TrackingDeviceManage* trackingDeviceManage = o->GetService();
		 * if(trackingDeviceManage->IsInstallTrackingDevice("Robot"))
		 * {
		 *   if(false == trackingDeviceManage->UnInstallTrackingDevice("Robot", nullptr))
		 *   {
		 *     std::cout << "uninstall robot tracking device faild! error: " << trackingDeviceManage->GetErrorString();
		 *   }
		 * }
		 * \endcoed
		 *
     * \warning This method will actively stop all activities of the target device 
     *          and destroy it. !!!Do not perform such operations externally, 
     *          otherwise uncontrollable situations may occur!!!
		 *
		 * \params name
		 *              Equipment name. Each type of equipment should have an independent
		 *              name.
		 * \params dataStorage
		 *              Storage target of equipment data visualization.The passed in nullptr
		 *              value will use the dataset object in the Mitk micro service, otherwise
		 *              the passed in dataset object will be used.
     * 
     * \return Return false if failed, otherwise true.
     */
    bool UnInstallTrackingDevice(const std::string& name, 
        itk::SmartPointer<mitk::DataStorage> dataStorage = itk::SmartPointer<mitk::DataStorage>());

    /**
		 * \brief Get Target Device Object.
		 *
		 * \params name
		 *              Equipment name. Each type of equipment should have an independent
		 *              name.
     * 
     * \warning If the target device is not installed, nullptr will be returned.
     * 
     * \see mitk::TrackingDevice
     */
		itk::SmartPointer<mitk::TrackingDevice> 
      GetTrackingDevice(const std::string& name);

    /**
     * \brief Get Target Device Object Source.
     *
     * \params name
     *              Equipment name. Each type of equipment should have an independent
     *              name.
     *
     * \warning If the target device is not installed, nullptr will be returned.
     *
     * \see mitk::TrackingDeviceSource
     */
		itk::SmartPointer<mitk::TrackingDeviceSource> 
      GetTrackingDeviceSource(const std::string& name) const;

    /**
     * \brief Get Target Device Navigation Tool Storage Object.
     *
     * \params name
     *              Equipment name. Each type of equipment should have an independent
     *              name.
     *
     * \warning If the target device is not installed, nullptr will be returned.
     *
     * \see mitk::NavigationToolStorage
     */
        itk::SmartPointer<mitk::NavigationToolStorage>
      GetNavigationToolStorage(const std::string& name) const;

    /**
     * \brief Get Target Device Navigation Data Render Pipeline Object.
     *
     * \params name
     *              Equipment name. Each type of equipment should have an independent
     *              name.
     *
     * \warning If the target device is not installed, nullptr will be returned.
     *
     * \see mitk::NavigationDataToNavigationDataFilter
     */
      itk::SmartPointer<lancet::NavigationObjectVisualizationFilter>
      GetNavigationDataToNavigationDataFilter(const std::string& name) const;

    /**
     * \brief Set the refresh interval of equipment data pipeline, in mm.
     * 
     * The default interval is 500 milliseconds.
     */
    void SetNavigationDataFilterInterval(long time);
    long GetNavigationDataFilterInterval() const;

    // Return number of currently installed devices.
    int GetInstallTrackingDeviceSize() const;
	public:
    // eom, The specific error is not clear. The interface is temporarily declared 
    // here to get through the infrastructure of the model.
		int GetErrorCode() const;

    /**
     * \brief Return error information encountered during internal processing.
     * 
     * During internal processing, errors may occur in many cases. You may want to 
     * check the specific cause by combining the error code.
     * 
     * \see GetErrorCode()
     */
    std::string GetErrorString() const;

    /**
     * \brief Detect whether the target device is installed.
		 *
		 * \params name
		 *              Equipment name. Each type of equipment should have an independent
		 *              name.
     * 
     * \return Return false if uninstall, otherwise true.
     * 
     * \see IsTrackingDeviceConnected(), IsStartTrackingDevice()
     */
    bool IsInstallTrackingDevice(const std::string&) const;

    /**
     * \brief Detect whether the target device is installed.
     *
     * \params name
     *              Equipment name. Each type of equipment should have an independent
     *              name.
     *
     * \return Return false if uninstall, otherwise true.
     *
     * \see IsInstallTrackingDevice(), IsStartTrackingDevice()
     */
    bool IsTrackingDeviceConnected(const std::string&) const;

    /**
     * \brief Detect whether the target device is installed.
     *
     * \params name
     *              Equipment name. Each type of equipment should have an independent
     *              name.
     *
     * \return Return false if uninstall, otherwise true.
     *
     * \see IsTrackingDeviceConnected(), IsInstallTrackingDevice()
     */
    bool IsStartTrackingDevice(const std::string&) const;
  protected:
      itk::SmartPointer<mitk::TrackingDevice> CreateTrackingDevice(const std::string&);
  private:
    struct TrackingDeviceManagePrivateImp;
    std::shared_ptr<TrackingDeviceManagePrivateImp> imp;
  };
} // namespace lancet

#endif // !LancetTrackingDeviceManage_H