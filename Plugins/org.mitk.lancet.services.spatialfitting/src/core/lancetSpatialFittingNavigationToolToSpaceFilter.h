/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief This is the header file that declares the data processing filter from 
 *        the tool of space fitting to the tool coordinate system.
 *
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \version V1.0.0
 * \date 2022-11-18 10:17:14
 * 
 * \par Modify History
 *		-# Sun initialization the version for 2022-11-18 10:17:14.
 *
 * \author Sun
 * \date 2022-11-18 10:17:14
 * \remark Non
 */
#ifndef LancetSpatialFittingNavigationToolToSpaceFilter_H
#define LancetSpatialFittingNavigationToolToSpaceFilter_H

#include "lancetSpatialFittingGlobal.h"

// vtk
#include <vtkMatrix4x4.h>

// mitk
#include <mitkNavigationDataToNavigationDataFilter.h>

BEGIN_SPATIAL_FITTING_NAMESPACE

/**
 * \ingroup org_mitk_lancet_services_spatialfitting
 *
 * \brief This is the conversion filter between spatial fitting data space and 
 *        space.
 *
 * The filter transfers the upstream navigation tool data to the target spatial 
 * coordinate system according to the input spatial transformation matrix, and 
 * outputs the data to the downstream filter.<br>
 * 
 * Please refer to
 * <a href="https://docs.mitk.org/nightly/classmitk_1_1NavigationDataToNavigationDataFilter.html">
 * mitk::NavigationDataToNavigationDataFilter</a> for details.
 *
 * <b>Class diagram:</b><br>
 * \image html SpatialFittingNavigationToolToSpaceFilter.svg
 *
 * <b>Attention</b><br> 
 * The pipeline work data flow processing is calculated from right to left. 
 * Here, it means to process the output first and then input.
 *
 * \author Sun
 * \version V1.0.0
 * \date 2022-11-18 17:53:00
 * \remark Non
 *
 * Contact: sh4a01@163.com
 *
 */
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	NavigationToolToSpaceFilter
	: public mitk::NavigationDataToNavigationDataFilter
{
public:
	mitkClassMacro(NavigationToolToSpaceFilter,
		mitk::NavigationDataToNavigationDataFilter);

	itkNewMacro(NavigationToolToSpaceFilter)

	NavigationToolToSpaceFilter();
	virtual ~NavigationToolToSpaceFilter();

	virtual vtkMatrix4x4* GetConvertMatrix4x4() const;

	/**
	 * \Warning This interface is a deep copy of the input parameters.
	 */
	virtual void SetConvertMatrix4x4(const vtkMatrix4x4* matrix);
protected:
	virtual void GenerateData() override;
private:
	struct NavigationToolToSpaceFilterPrivateImp;
	std::shared_ptr<NavigationToolToSpaceFilterPrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingNavigationToolToSpaceFilter_H
