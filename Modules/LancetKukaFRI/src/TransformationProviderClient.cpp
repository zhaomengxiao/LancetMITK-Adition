#include <cstdio>
// Visual studio needs extra define to use math constants
#define _USE_MATH_DEFINES

#include <thread>
#include <TransformationProviderClient.h>

#include "lancetMatrixConvert.h"

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
  std::lock_guard<std::mutex>lock(m_TransformMatrixMutex);
  m_TransformID = id;
  m_TransformMatrix = transform;
}

//******************************************************************************
void lancet::TransformationProviderClient::provide()
{
  m_TransformMatrixMutex.lock();
  auto tmpTransform = m_TransformMatrix->Clone();
  m_TransformMatrixMutex.unlock();

  double transformMatrix[3][4];
  lancet::ConvertMitkAffineTransform3DTo3x4Array(tmpTransform, transformMatrix);

  //printf("X:%f, Y:%f, Z:%f\n", transformationMatrix[0][3], transformationMatrix[1][3], transformationMatrix[2][3]);

  // Set new transformation matrix for frame with identifier"m_TransformID"
  setTransformation(m_TransformID.c_str(), transformMatrix, getTimestampSec(),
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
  this->DisConnect();
}

bool lancet::FriManager::Connect()
{
  // connect client application to KUKA Sunrise controller
  m_IsConnected = m_ClientApp->connect(m_Port, m_HostName.c_str());
  return m_IsConnected;
}

bool lancet::FriManager::IsConnected()
{
  return m_IsConnected;
}

void lancet::FriManager::StartFriControl()
{
	//m_stepThread = std::thread(&FriManager::stepThreadWorker, this);
  auto stepThread = std::thread(&FriManager::stepThreadWorker, this);
  stepThread.detach();
}

void lancet::FriManager::SetFriDynamicFrameTransform(mitk::AffineTransform3D::Pointer transform)
{
  m_TransformMatrix = transform;
}

void lancet::FriManager::DisConnect()
{
  m_IsConnected = false;
  m_StepSuccess = false;
  m_ClientApp->disconnect();
  while (!this->safeQuitWaitForThread())
  {
	  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  MITK_INFO << "Exit TransformationProvider Client Application";
}

bool lancet::FriManager::isRunningStep() const
{
	return this->m_isRunningStep;
}

void lancet::FriManager::stepThreadWorker()
{
  this->m_isRunningStep = true;
  // repeatedly call the step routine to receive and process FRI packets
  while (m_StepSuccess && m_IsConnected)
  {
    m_TrafoClient.SetTransformation("FriDynamicFrame", m_TransformMatrix);
    m_StepSuccess = m_ClientApp->step();

    // check if we are in IDLE because the FRI session was closed
    if (m_LbrClient.robotState().getSessionState() == IDLE)
    {
      // In this demo application we simply quit.
      // Waiting for a new FRI session would be another possibility.
      m_IsConnected = false;
      break;
    }
  }
  this->m_isRunningStep = false;
}

bool lancet::FriManager::safeQuitWaitForThread(int timeout)
{
	clock_t startCheckTime = clock();
	while (this->isRunningStep())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		if ((clock() - startCheckTime) >= timeout)
		{
			return false;
		}
	}

	return true;
}

// clean up additional defines
#ifdef _USE_MATH_DEFINES
#undef _USE_MATH_DEFINES
#endif
