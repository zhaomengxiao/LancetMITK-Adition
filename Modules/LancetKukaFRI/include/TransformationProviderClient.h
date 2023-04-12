#ifndef _KUKA_FRI_TRANSFORMATION_PROVIDER_CLIENT_H
#define _KUKA_FRI_TRANSFORMATION_PROVIDER_CLIENT_H

#include <friTransformationClient.h>
#include <friUdpConnection.h>
#include <friClientApplication.h>
#include <friLBRClient.h>

#include "MitkLancetKukaFRIExports.h"
#include "mitkAffineTransform3D.h"
#include "itkObject.h"
#include "mitkCommon.h"
#include <mutex>
namespace lancet
{
  /**
 * \brief Example client for changing the transformation matrix of a robot frame.
 */
  class MITKLANCETKUKAFRI_EXPORT TransformationProviderClient : public KUKA::FRI::TransformationClient, public itk::Object
  {
  public:
    /*mitkClassMacroItkParent(TransformationProviderClient, itk::Object);
    itkFactorylessNewMacro(Self);
    itkCloneMacro(Self);*/

    

    /*itkGetConstMacro(TransformID, std::string);
    itkSetMacro(TransformID, std::string);*/
    /**
     * \brief Constructor.
     *
     */
    TransformationProviderClient();

    /**
     * \brief Destructor.
     */
    ~TransformationProviderClient() override;

    void SetTransformation(std::string id, mitk::AffineTransform3D::Pointer transform);

    /**
     * \brief Callback in which the new transformation matrix is calculated.
     */
    void provide() override;

  protected:
    mitk::AffineTransform3D::Pointer m_TransformMatrix;
	std::mutex m_TransformMatrixMutex;
    std::string m_TransformID;
  };

  class MITKLANCETKUKAFRI_EXPORT FriManager : public itk::Object
  {
  public:
    mitkClassMacroItkParent(FriManager, itk::Object);
    itkFactorylessNewMacro(Self);

    /**
     * \brief Constructor.
     *
     */
    FriManager();

    /**
     * \brief Destructor.
     */
    ~FriManager() override;

    bool Connect();

    bool IsConnected();

    void StartFriControl();

    void SetFriDynamicFrameTransform(mitk::AffineTransform3D::Pointer transform);

    void DisConnect();

	/**
	 * \brief Returns the running status of the background output processing thread 
	 *        of the FRI module of the manipulator.
	 *
	 * tips:
	 * The purpose of this method is to solve the crash caused by thread safety
	 * problems when the class is destructed. see #safeQuitWaitForThread().
	 *
	 * \retval Returns true when the thread is running, otherwise false.
	 */
	bool isRunningStep() const;
  protected:
    void stepThreadWorker();

	/**
	 * \brief Make the thread exit safely within a certain time.
	 *
	 * tips:
	 * The purpose of this method is to solve the crash caused by thread safety
	 * problems when the class is destructed.
	 *
	 * \retval The successful thread safety exit returns true, otherwise false.
	 */
	bool safeQuitWaitForThread(int timeout = 1000);
    
    int m_Port;
    std::string m_HostName;
    bool m_StepSuccess{true};
	bool m_IsConnected{ false };
    //std::thread m_stepThread;
	std::atomic_bool m_isRunningStep{false};
    mitk::AffineTransform3D::Pointer m_TransformMatrix;
	
    //std::string m_TransformID;
    KUKA::FRI::UdpConnection m_Connection;
    KUKA::FRI::ClientApplication* m_ClientApp;
    KUKA::FRI::LBRClient m_LbrClient;
    TransformationProviderClient m_TrafoClient;
  };
}


#endif // _KUKA_FRI_TRANSFORMATION_PROVIDER_CLIENT_H
