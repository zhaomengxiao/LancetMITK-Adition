/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief This is the model of the navigation data pipeline for rough pelvic 
 *        registration.
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
#ifndef LancetSpatialFittingPelvicRoughRegistrationsModel_H
#define LancetSpatialFittingPelvicRoughRegistrationsModel_H

#include <QObject>

#include "lancetSpatialFittingGlobal.h"
#include <surfaceregistraion.h>
#include <mitkNavigationDataSource.h>

BEGIN_SPATIAL_FITTING_NAMESPACE
class PipelineManager;

class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
  PelvicRoughRegistrationsModel : public QObject, public itk::Object
{
	Q_OBJECT
public:
	mitkClassMacroItkParent(PelvicRoughRegistrationsModel, itk::Object)

	itkNewMacro(PelvicRoughRegistrationsModel)

	PelvicRoughRegistrationsModel();

	void Start();
	void Stop();
public:
	mitk::NavigationDataSource::Pointer GetNdiNavigationDataSource() const;
	void SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer source);

	itk::SmartPointer<PipelineManager> GetRegistrationPipeline() const;
	void SetRegistrationPipeline(const itk::SmartPointer<PipelineManager>&);

	void ConfigureRegistrationsPipeline();
public:
	virtual void SetSurfaceSrc(const mitk::Surface::Pointer&);
	virtual mitk::Surface::Pointer GetSurfaceSrc() const;

	virtual void SetImagePointArray(int, const mitk::Point3D&);
	virtual void SetImagePointArray(const std::array<mitk::Point3D, 3>&);
	virtual mitk::Point3D GetImagePoint(int) const;

	virtual void SetVegaPointArray(int, const mitk::Point3D&);
	virtual void SetVegaPointArray(const std::array<mitk::Point3D, 3>&);
	virtual mitk::Point3D GetVegaPoint(int) const;
	virtual int GetVegaPointVaildIndex() const;

	// [Warning] It is not recommended to operate this object directly.
	mitk::SurfaceRegistration::Pointer GetSurfaceRegistration() const;

	virtual bool ComputeLandMarkResult(mitk::PointSet::Pointer,
		mitk::PointSet::Pointer, mitk::Surface::Pointer);
Q_SIGNALS:
	void VegaPointChange();
private:
  struct PelvicRoughRegistrationsModelPrivateImp;
  std::shared_ptr<PelvicRoughRegistrationsModelPrivateImp> imp;
};
END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPelvicRoughRegistrationsModel_H