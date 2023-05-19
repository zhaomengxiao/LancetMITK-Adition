/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief This is the model of the navigation data pipeline for pelvic registration.
 *        
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \version V1.0.0
 * \date 2023-02-20 11:09:17
 * 
 * \par Modify History
 *		-# Sun initialization the version for 2023-02-20 11:09:17.
 *
 * \author Sun
 * \date 2023-02-20 11:09:17
 * \remark Non
 */

#ifndef LancetSpatialFittingPelvicRegistrationsModel_H
#define LancetSpatialFittingPelvicRegistrationsModel_H

// Include header files for Qt.
#include <QObject>

// Include header files for current model.
#include "lancetSpatialFittingGlobal.h"

// Include header files for lancet model.
#include <surfaceregistraion.h>


// define mitk namespace class.
namespace mitk { class NavigationDataSource; } // namespace mitk::NavigationDataSource.

// define namespace spatial_fitting for lancet.
namespace lancet::spatial_fitting { class PipelineManager; } // namespace lancet::spatial_fitting::PipelineManager.

BEGIN_SPATIAL_FITTING_NAMESPACE

class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
  PelvicRegistrationsModel : public QObject, public itk::Object
{
	Q_OBJECT
Q_SIGNALS:
  // 0: Rough; 1: Precision;
  void RegisterPointModify(int);

  // param see enum WidgetWorkingModel.
  void RegisterPointRenderModelModify(int);
public: 
  // define interface for mitk object.
  mitkClassMacroItkParent(PelvicRegistrationsModel, itk::Object)
  itkNewMacro(PelvicRegistrationsModel)

  // define construction interface for this class.
  PelvicRegistrationsModel();
  virtual ~PelvicRegistrationsModel();

  // define enumeration type for this class.
  enum WidgetWorkingModel
  {
    Collector,  ///< Collect registration point mode, mainly collecting registration space point data of Vega equipment.
    VerifyAccuracy  ///< Verify the registration accuracy mode, mainly verifying the registration error of the spatial data of the registration set.
  };

  // define extend class for this class.
  template<typename RegisterPointSet>
  class /*ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN*/ PelvicRegisterDataModel
  {
  public:
    PelvicRegisterDataModel();

    virtual int GetMaxPointSize() const;

    virtual mitk::Point3D GetImagePointOfRegister(int) const;
    virtual void SetImagePointArrayOfRegister(int, const mitk::Point3D&);
    virtual void SetImagePointArrayOfRegister(const RegisterPointSet&);

    // Return -1 value to indicate that the database is complete, otherwise return null subscript.
    virtual int GetImagePointArrayWaitModifyIndex() const;

    virtual mitk::Point3D GetVegaPointOfRegister(int) const;
    virtual void SetVegaPointArrayOfRegister(int, const mitk::Point3D&);
    virtual void SetVegaPointArrayOfRegister(const RegisterPointSet&);

    // Return -1 value to indicate that the database is complete, otherwise return null subscript.
    virtual int GetVegaPointArrayWaitModifyIndex() const;
  private:
    struct PrivateImp;
    std::shared_ptr<PrivateImp> imp;
  };

  // define get or set interface for this class.

  void StopUpdatePipeline();
  void StartUpdatePipeline();
  bool IsRunningUpdatePipeline() const;

  WidgetWorkingModel GetPelvicRoughRegisterWorkingModel() const;
  void SetPelvicRoughRegisterWorkingModel(const WidgetWorkingModel&) const;

  WidgetWorkingModel GetPelvicPrecisionRegisterWorkingModel() const;
  void SetPelvicPrecisionRegisterWorkingModel(const WidgetWorkingModel&) const;

#define itkPointer itk::SmartPointer

  itkPointer<mitk::NavigationDataSource> GetNdiNavigationDataSource() const;
  void SetNdiNavigationDataSource(const itkPointer<mitk::NavigationDataSource>&);

  itkPointer<PipelineManager> GetRegistrationPipeline() const;
  void SetRegistrationPipeline(const itkPointer<PipelineManager>&);

  itkPointer<PipelineManager> GetPelvisRF2ImagePipeline() const;
  void SetPelvisRF2ImagePipeline(const itkPointer<PipelineManager>&);

  virtual mitk::Surface::Pointer GetSurfaceSrc() const;
  virtual void SetSurfaceSrc(const mitk::Surface::Pointer&);

  typedef std::array<mitk::Point3D, 3> RoughRegisterPointSet;
  typedef std::array<mitk::Point3D, 40> PrecisionRegisterPointSet;

  virtual PelvicRegisterDataModel<RoughRegisterPointSet>& GetRoughRegisterDataModel() const;
  virtual void SetRoughRegisterDataModel(const PelvicRegisterDataModel<RoughRegisterPointSet>&);
  
  virtual PelvicRegisterDataModel<PrecisionRegisterPointSet>& GetPrecisionRegisterDataModel() const;
  virtual void SetPrecisionRegisterDataModel(const PelvicRegisterDataModel<PrecisionRegisterPointSet>&);
  
  // ![Warning] It is not recommended to operate this object directly.
  mitk::SurfaceRegistration::Pointer GetSurfaceRegistration() const;

  virtual bool ComputeRoughLandMarkResult(mitk::PointSet::Pointer,
      mitk::PointSet::Pointer, mitk::Surface::Pointer);
      
  virtual bool ComputePrecisionLandMarkResult(mitk::PointSet::Pointer,
      mitk::PointSet::Pointer, mitk::Surface::Pointer);
private:
  struct PrivateImp;
  std::shared_ptr<PrivateImp> imp;
};
#include "lancetSpatialFittingPelvicRegistrationsModel.inc"

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPelvicRegistrationsModel_H