#include <cstdio>
// Visual studio needs extra define to use math constants
#define _USE_MATH_DEFINES

#include <thread>
#include <TransformationProviderClient.h>

using namespace KUKA::FRI;
//******************************************************************************
lancet::TransformationProviderClient::TransformationProviderClient()
{
  MITK_INFO << "TransformationProviderClient initialized:";
  m_TransformID = "FriDynamicFrame";
  m_TransformMatrix = mitk::AffineTransform3D::New();
}

//******************************************************************************
lancet::TransformationProviderClient::~TransformationProviderClient()
{
}

void lancet::TransformationProviderClient::SetTransformation(std::string id, mitk::AffineTransform3D::Pointer transform)
{
  m_TransformID = id;
  m_TransformMatrix = transform;
}

//******************************************************************************
void lancet::TransformationProviderClient::provide()
{
  double transformationMatrix[3][4];
  transformationMatrix[0][0] = m_TransformMatrix->GetMatrix()[0][0]; transformationMatrix[0][1] = m_TransformMatrix->GetMatrix()[0][1]; transformationMatrix[0][2] = m_TransformMatrix->GetMatrix()[0][2]; transformationMatrix[0][3] = m_TransformMatrix->GetOffset()[0];
  transformationMatrix[1][0] = m_TransformMatrix->GetMatrix()[1][0]; transformationMatrix[1][1] = m_TransformMatrix->GetMatrix()[1][1]; transformationMatrix[1][2] = m_TransformMatrix->GetMatrix()[1][2]; transformationMatrix[1][3] = m_TransformMatrix->GetOffset()[1];
  transformationMatrix[2][0] = m_TransformMatrix->GetMatrix()[2][0]; transformationMatrix[2][1] = m_TransformMatrix->GetMatrix()[2][1]; transformationMatrix[2][2] = m_TransformMatrix->GetMatrix()[2][2]; transformationMatrix[2][3] = m_TransformMatrix->GetOffset()[2];

  printf("X:%f, Y:%f, Z:%f\n", transformationMatrix[0][3], transformationMatrix[1][3], transformationMatrix[2][3]);

  // Set new transformation matrix for frame with identifier"m_TransformID"
  setTransformation(m_TransformID.c_str(), transformationMatrix, getTimestampSec(),
         getTimestampNanoSec());
}

lancet::FriManager::FriManager()
{
  m_ClientApp = new KUKA::FRI::ClientApplication{ m_Connection,m_LbrClient,m_TrafoClient };
  m_Port = 30200;
  m_HostName = "172.31.1.147";
  m_TransformMatrix = mitk::AffineTransform3D::New();
}

lancet::FriManager::~FriManager()
{
}

bool lancet::FriManager::Connect()
{
  // connect client application to KUKA Sunrise controller
  return m_ClientApp->connect(m_Port, m_HostName.c_str());
}

void lancet::FriManager::StartFriControl()
{
  m_stepThread = std::thread(&FriManager::stepThreadWorker, this);
}

void lancet::FriManager::SetFriDynamicFrameTransform(mitk::AffineTransform3D::Pointer transform)
{
  m_TransformMatrix = transform;
}

void lancet::FriManager::DisConnect()
{
  m_ClientApp->disconnect();

  printf("\nExit TransformationProvider Client Application");
}

void lancet::FriManager::stepThreadWorker()
{
  // repeatedly call the step routine to receive and process FRI packets
  while (m_StepSuccess)
  {
    m_TrafoClient.SetTransformation("FriDynamicFrame", m_TransformMatrix);
    m_StepSuccess = m_ClientApp->step();

    // check if we are in IDLE because the FRI session was closed
    if (m_LbrClient.robotState().getSessionState() == IDLE)
    {
      // In this demo application we simply quit.
      // Waiting for a new FRI session would be another possibility.
      break;
    }
  }
}

// clean up additional defines
#ifdef _USE_MATH_DEFINES
#undef _USE_MATH_DEFINES
#endif
