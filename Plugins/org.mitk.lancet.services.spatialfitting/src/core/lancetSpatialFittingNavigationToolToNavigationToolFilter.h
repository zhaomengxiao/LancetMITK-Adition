/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief This is the header file that declares the data processing filter from 
 *        space to space coordinate system of space fitting.
 * 
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \version V1.0.0
 * \date 2022-11-18 10:16:37
 * 
 * \par Modify History
 *		-# Sun initialization the version for 2022-11-18 10:16:37.
 *
 * \author Sun
 * \date 2022-11-18 10:16:37
 * \remark Non
 */
#ifndef LancetSpatialFittingNavigationToolToNavigationToolFilter_H
#define LancetSpatialFittingNavigationToolToNavigationToolFilter_H

#include "lancetSpatialFittingGlobal.h"

// mitk
#include <mitkNavigationDataToNavigationDataFilter.h>

BEGIN_SPATIAL_FITTING_NAMESPACE

/**
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \brief This is a tool to tool spatial data conversion filter.
 *
 * This filter will transfer the navigation tool input data to the UCS tool 
 * coordinate system.
 * 
 * \warning The filter itself will not monitor the validity of any data. If 
 *          correct output data is required, please ensure the validity of the 
 *          input data.<br>
 *
 * Please refer to
 * <a href="https://docs.mitk.org/nightly/classmitk_1_1NavigationDataToNavigationDataFilter.html">
 * mitk::NavigationDataToNavigationDataFilter</a> for details.
 *
 * <b>Class diagram:</b><br>
 * \image html SpatialFittingNavigationToolToNavigationToolFilter.svg
 *
 * \author Sun
 * \version V1.0.0
 * \date 2022-11-18 17:21:18
 * \remark Non
 *
 * Contact: sh4a01@163.com
 *
 */
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	NavigationToolToNavigationToolFilter
	: public mitk::NavigationDataToNavigationDataFilter
{
public:
	mitkClassMacro(NavigationToolToNavigationToolFilter,
		mitk::NavigationDataToNavigationDataFilter);

	itkNewMacro(NavigationToolToNavigationToolFilter)

	NavigationToolToNavigationToolFilter();
	virtual ~NavigationToolToNavigationToolFilter();

	/**
	 * \brief Get the tool data object from the input data to the target coordinate 
	 *        system.
	 * 
	 * UCS: User Coordinate system.
	 */
	virtual mitk::NavigationData::Pointer GetUCSTool() const;
	virtual void SetUCSTool(mitk::NavigationData::Pointer tool);
protected:
	virtual void GenerateData() override;
private:
	struct NavigationToolToNavigationToolFilterPrivateImp;
	std::shared_ptr<NavigationToolToNavigationToolFilterPrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE


#endif // !lancetSpatialFittingNavigationToolToNavigationToolFilter_H
