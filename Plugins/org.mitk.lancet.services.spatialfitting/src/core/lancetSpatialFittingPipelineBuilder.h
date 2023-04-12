/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief This is the header file of the abstract pipeline data producer.
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \version V1.0.0
 * \date 2022-11-18 10:17:50
 * 
 * \par Modify History
 *		-# Sun initialization the version for 2022-11-18 10:17:50.
 *
 * \author Sun
 * \date 2022-11-18 10:17:50
 * \remark Non
 */
#ifndef LancetSpatialFittingPipelineBuilder_H
#define LancetSpatialFittingPipelineBuilder_H

#include "lancetSpatialFittingGlobal.h"

#include "lancetSpatialFittingAbstractPipelineBuilder.h"

// mitk
#include <mitkNavigationData.h>

class vtkMatrix4x4;
BEGIN_SPATIAL_FITTING_NAMESPACE
class PipelineManager;

/**
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \brief This is an abstract pipeline data producer class.
 *
 * The production component class of the generator mode. The derived class of 
 * this class is responsible for creating complex pipeline data objects step by 
 * step.
 *
 * \author Sun
 * \version V1.0.0
 * \date 2022-11-21 14:56:17
 * \remark Non
 *
 * Contact: sh4a01@163.com
 *
 */
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	PipelineBuilder : public AbstractPipelineBuilder
{
public:
	mitkClassMacroItkParent(PipelineBuilder, AbstractPipelineBuilder);

	itkNewMacro(PipelineBuilder)

	PipelineBuilder();
	virtual ~PipelineBuilder();
	virtual void Reset() override;
	virtual itk::SmartPointer<PipelineManager> GetOutput() override;

public:
	/**
	 * \brief Produce a tool to tool pipeline data conversion filter.
	 *
	 * Produce a tool to tool pipeline data conversion filter, and configure it 
	 * to the subscript value memory corresponding to the pipeline data manager.
	 * 
	 * If the subscript value already has a data processing filter, the current 
	 * request will overwrite the target.
	 *
	 * \warning The subscript value determines the processing order of the data 
	 *          pipeline, with 0 as the starting point. followed by 1, 2....
	 *
	 * \param index
	 *              Filter subscript value, mapping table of pipeline data manager
	 * \param ucsTool
	 *              Target tool navigation data. Entering the tool navigation data 
	 *              will go to the tool coordinate system.
	 * 
	 * \see NavigationToolToNavigationToolFilter
	 */
	virtual void BuilderNavigationToolToNavigationToolFilter(int index,
		mitk::NavigationData* ucsTool);

	/**
	 * \brief Produce a space to space conversion filter.
	 *
	 * Produce a space to space conversion filter, and configure it to the subsc-
	 * ript value memory corresponding to the pipeline data manager.
	 *
	 * If the subscript value already has a data processing filter, the current
	 * request will overwrite the target.
	 *
	 * \warning The subscript value determines the processing order of the data
	 *          pipeline, with 0 as the starting point. followed by 1, 2....
	 *
	 * \param index
	 *              Filter subscript value, mapping table of pipeline data manager
	 * \param convert
	 *              Target tool navigation data. Entering the tool navigation data 
	 *              will be transferred to this spatial coordinate system.
	 *
	 * \see NavigationToolToSpaceFilter
	 */
	virtual void BuilderNavigationToolToSpaceFilter(int index,
		mitk::AffineTransform3D* convert);

	/**
	 * \brief Produce a navigation data acquisition filter.
	 *
	 * \warning The subscript value determines the processing order of the data
	 *          pipeline, with 0 as the starting point. followed by 1, 2....
	 *
	 * If the subscript value already has a data processing filter, the current
	 * request will overwrite the target.
	 * 
	 * \param index
	 *              Filter subscript value, mapping table of pipeline data manager
	 * \param number
	 *              Number property of the navigation data collection filter
	 * \param interval
	 *              Collection interval time of navigation data collection filter
	 * \param accuracyRange
	 *              Acceptable precision range of navigation data acquisition filter
	 * 
	 * \see NavigationToolCollector
	 */
	virtual void BuilderNavigationToolCollector(int index, unsigned int number,
		long interval = 20, double accuracyRange = 0.5);
protected:
	struct PipelineBuilderPrivateImp;
	std::shared_ptr<PipelineBuilderPrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPipelineBuilder_H
