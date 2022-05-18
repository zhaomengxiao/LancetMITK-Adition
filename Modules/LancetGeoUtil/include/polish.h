#ifndef POLISH_H
#define POLISH_H

#include <itkObject.h>
#include <itkCommand.h>
#include <mitkCommon.h>
#include "MitkLancetGeoUtilExports.h"
#include "mitkImage.h"
#include "mitkSurface.h"
#include "mitkSurfaceToImageFilter.h"
#include <vtkDiscreteFlyingEdges3D.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <itkPlatformMultiThreader.h>
#include <mutex>

class Timer
{
public:
  Timer();
  Timer(std::string name);
  ~Timer();
private:
  clock_t m_time_start{};
  float m_time_past{0.0};
  std::string m_name{"timer"};
};

class MITKLANCETGEOUTIL_EXPORT Polish : public itk::Command
{
public:
  mitkClassMacroItkParent(Polish, itk::Command);
  itkNewMacro(Self)

  enum PolishState { Setup, Ready, Polishing };

  void Execute(Object *caller, const itk::EventObject &event) override;
  void Execute(const Object *caller, const itk::EventObject &event) override;
  Polish();
  ~Polish() override;

  /**
     * \brief  change object state
     */
  void SetState(PolishState state);

  PolishState GetState() ;

  bool StartPolish();

  void doPolish();

  void run();
  void run2();
  /**
   * \brief static start method for the work thread.
   */
  static itk::ITK_THREAD_RETURN_TYPE ThreadRun(void* pInfoStruct);

  mitk::Surface::Pointer DiscreteFlyingEdges3D(mitk::Image *mitkImage);
  mitk::Image::Pointer SurfaceCutImage(mitk::Surface *surface,
                                       mitk::Image *image,
                                       bool isReverseStencil,
                                       bool binary
    );
  //static void imageBoolDiff(mitk::Image::Pointer& image1, mitk::Image::Pointer& image2);
  itkGetMacro(toolSurface, mitk::Surface::Pointer)
  itkGetMacro(boneImage, mitk::Image::Pointer)
  itkSetMacro(toolSurface, mitk::Surface::Pointer)
  void SetboneImage(mitk::Image::Pointer boneImage);
  itkGetMacro(resSurface, mitk::Surface::Pointer)
  itkGetMacro(resImage, mitk::Image::Pointer)
  itkGetMacro(boneSurface, mitk::Surface::Pointer);
  itkSetMacro(boneSurface, mitk::Surface::Pointer);
  
private:
  //input
  mitk::Surface::Pointer m_toolSurface{nullptr};
  mitk::Image::Pointer m_boneImage{nullptr};
  mitk::Surface::Pointer m_boneSurface{nullptr};
  //res
  mitk::Surface::Pointer m_resSurface{nullptr};
  mitk::Image::Pointer m_resImage{nullptr};
  //filter
  vtkSmartPointer<vtkDiscreteFlyingEdges3D> m_flyingEdgeFilter{nullptr};
  vtkSmartPointer<vtkWindowedSincPolyDataFilter> m_wsFilter{nullptr};
  mitk::SurfaceToImageFilter::Pointer m_surface2imagefilter{nullptr};
  //vtkSmartPointer<vtkPolyData> m_Femur_PolyData;
  itk::PlatformMultiThreader::Pointer m_MultiThreader;
  ///< creates tracking thread that continuously polls serial interface for new tracking data
  int m_ThreadID;

  PolishState m_State; ///< current object state (Setup, Ready, Polishing)
  //< signal stop to tracking thread
  bool m_StopPolish;
  std::mutex m_StopPolishMutex; ///< mutex to control access to m_StopTracking
  std::mutex m_PolishFinishedMutex; ///< mutex to manage control flow of StopTracking()
  std::mutex m_StateMutex; ///< mutex to control access to m_State
};

inline void Polish::SetboneImage(mitk::Image::Pointer boneImage)
{
  if (m_boneImage!=boneImage)
  {
      m_boneImage = boneImage;
      m_resImage = boneImage->Clone();
  }
  
}
#endif
