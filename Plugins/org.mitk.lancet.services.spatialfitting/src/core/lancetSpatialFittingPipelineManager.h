/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief $Document description$
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \version V1.0.0
 * \date 2022-11-18 10:18:17
 * 
 * \par Modify History
 *		-# Sun initialization the version for 2022-11-18 10:18:17.
 *
 * \author Sun
 * \date 2022-11-18 10:18:17
 * \remark Non
 */
#ifndef LancetSpatialFittingPipelineManager_H
#define LancetSpatialFittingPipelineManager_H

#include "lancetSpatialFittingGlobal.h"

#include <mitkNavigationDataToNavigationDataFilter.h>


BEGIN_SPATIAL_FITTING_NAMESPACE

/**
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \brief This is the manager class of the navigation data collection filter.
 *
 * This class itself is a filter. It internally manages a complete channel data 
 * filter and automatically configures it according to the data flow processing 
 * order.
 *
 * This class can be produced using the builder mode, because the initialization 
 * of this class is relatively complex.
 * 
 * \author Sun
 * \version V1.0.0
 * \date 2022-11-21 15:44:21
 * \remark Non
 *
 * Contact: sh4a01@163.com
 *
 */
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	PipelineManager 
	: public mitk::NavigationDataToNavigationDataFilter
{
public:
	mitkClassMacro(PipelineManager, mitk::NavigationDataToNavigationDataFilter);

	itkNewMacro(PipelineManager)
public:
	PipelineManager();
	virtual ~PipelineManager();

	bool AddFilter(int index, mitk::NavigationDataToNavigationDataFilter::Pointer);

	bool RemoveFilter(int index);
	bool RemoveFilter(const std::string&);

	bool ModifyFilter(int index, 
		mitk::NavigationDataToNavigationDataFilter::Pointer);
	bool ModifyFilter(const std::string& name, 
		mitk::NavigationDataToNavigationDataFilter::Pointer);

	mitk::NavigationDataToNavigationDataFilter::Pointer FindFilter(int index);
	mitk::NavigationDataToNavigationDataFilter::Pointer 
		FindFilter(const std::string& name);

	int GetIndexByName(const std::string& name);

	bool IsEmpty() const;

	int GetSize() const;
protected:
	virtual void GenerateData() override;
private:
	struct PipelineManagerPrivateImp;
	std::shared_ptr<PipelineManagerPrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPipelineManager_H
