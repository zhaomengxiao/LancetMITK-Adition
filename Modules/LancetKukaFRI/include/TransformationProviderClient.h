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

namespace lancet
{
  /**
 * \brief Example client for changing the transformation matrix of a robot frame.
 */
  class MITKLANCETKUKAFRI_EXPORT TransformationProviderClient : public KUKA::FRI::TransformationClient, public itk::Object
  {
  public:
    mitkClassMacroItkParent(TransformationProviderClient, itk::Object);
    itkFactorylessNewMacro(Self);
    itkCloneMacro(Self);

    

    itkGetConstMacro(TransformID, std::string);
    itkSetMacro(TransformID, std::string);
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

    void StartFriControl();

    void SetFriDynamicFrameTransform(mitk::AffineTransform3D::Pointer transform);

    void DisConnect();

  protected:
    void stepThreadWorker();

    
    int m_Port;
    std::string m_HostName;
    bool m_StepSuccess{true};
    std::thread m_stepThread;
    mitk::AffineTransform3D::Pointer m_TransformMatrix;
    //std::string m_TransformID;
    KUKA::FRI::UdpConnection m_Connection;
    KUKA::FRI::ClientApplication* m_ClientApp;
    KUKA::FRI::LBRClient m_LbrClient;
    TransformationProviderClient m_TrafoClient;
  };
}


#endif // _KUKA_FRI_TRANSFORMATION_PROVIDER_CLIENT_H
