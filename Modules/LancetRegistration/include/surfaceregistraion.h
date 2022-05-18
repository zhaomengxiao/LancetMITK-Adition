#ifndef SURFACEREGISTRATION_H
#define SURFACEREGISTRATION_H

#include "MitkLancetRegistrationExports.h"
#include <mitkPoint.h>
#include <mitkPointSet.h>
#include "mitkSurface.h"
#include <itkObject.h>
#include <itkObjectFactory.h>

namespace mitk
{
  /**Documentation
  * \brief Class for performing a surface registration using vtk landmark and icp algorithm
  * \ingroup IGT
  */
  class MITKLANCETREGISTRATION_EXPORT SurfaceRegistration : public itk::Object
  {
  public:
    mitkClassMacroItkParent(SurfaceRegistration, itk::Object);
    itkNewMacro(Self);

    itkSetMacro(LandmarksSrc, mitk::PointSet::Pointer);
    itkSetMacro(LandmarksTarget, mitk::PointSet::Pointer);
    itkSetMacro(IcpPoints, mitk::PointSet::Pointer);
    itkSetMacro(SurfaceSrc, mitk::Surface::Pointer);

    /** @brief add target landmark to LandmarkTarget pointSet
      */
    void AddLandMark(mitk::Point3D point);
    
    void AddIcpPoints(mitk::Point3D point);
    void ClearLandMarks();
    void ClearIcpPoints();

    /**
     * @brief Compute LandMark registration result if source and target landmarks are set.
     *
     * [Warning] this method don't support continues called.When called all previous result will be lost.
     * @return bool.true if landmark matrix updated,false if nothing happens.
     */
    bool ComputeLandMarkResult();
    /**
     * @brief Compute LandMark registration result if source and target landmarks are set.
     *
     * this method support continues called.The new registration is based on previous registration.
     * @return bool.true if landmark matrix updated,false if nothing happens.
     */

    bool ComputeIcpResult();

    /** @brief Give up the previous one registration result.
     *@return true if undo success,false if nothing to undo.
      */
    bool Undo();
    /** @brief Give up the all registration result.
      */
    void Clear();
    vtkMatrix4x4* GetResult();

    itkGetMacro(LandmarksSrc, mitk::PointSet::Pointer);
    itkGetMacro(LandmarksTarget, mitk::PointSet::Pointer);
    itkGetMacro(IcpPoints, mitk::PointSet::Pointer);
    itkGetMacro(MatrixLandMark, vtkMatrix4x4*);
    //itkGetMacro(MatrixICP, vtkMatrix4x4*);

        
  protected:
      SurfaceRegistration();
      ~SurfaceRegistration() override;

  private:
    mitk::PointSet::Pointer m_LandmarksSrc;
    mitk::Surface::Pointer m_SurfaceSrc;

    mitk::PointSet::Pointer m_LandmarksTarget;
    mitk::PointSet::Pointer m_IcpPoints;

    vtkMatrix4x4* m_MatrixLandMark;
    std::vector<vtkMatrix4x4*> m_MatrixList;
    vtkMatrix4x4* m_ResultMatrix;

    bool m_ContinuesRegist{ false };
  };
} // Ende Namespace


#endif // SURFACEREGISTRATION_H
