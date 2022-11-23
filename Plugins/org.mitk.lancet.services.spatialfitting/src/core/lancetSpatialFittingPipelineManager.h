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

	/**
	 * \brief Add a filter element.
	 * 
	 * The subscript value will determine the time when the filter is called. The 
	 * calling sequence starts from subscript 0 and increases in order.
	 * 
	 * \warning When the subscript value is greater than the total number of 
	 *          elements in the container, the filter is added to the end of the 
	 *          container by default.
	 * 
	 * \param index
	 *              Element subscript value
	 * \param data
	 *              Data Field Object.
	 * 
	 * \return Returns true if the addition is successful, otherwise false.
	 * 
	 * \see mitk::NavigationDataToNavigationDataFilter::Pointer
	 */
	bool AddFilter(int index, mitk::NavigationDataToNavigationDataFilter::Pointer data);

	bool RemoveFilter(int index);
	bool RemoveFilter(const std::string&);

	/**
	 * \brief Modify the element of target subscript.
	 * 
	 * This will override the way target subscript elements are modified.
	 *
	 * \param index
	 *              Element subscript value
	 * \param data
	 *              Data Field Object.
	 *
	 * \return Returns true if the addition is successful, otherwise false.
	 * 
	 * \see mitk::NavigationDataToNavigationDataFilter::Pointer
	 */
	bool ModifyFilter(int index, 
		mitk::NavigationDataToNavigationDataFilter::Pointer);

	/**
	 * It is recommended to use subscripts.
	 */
	bool ModifyFilter(const std::string& name, 
		mitk::NavigationDataToNavigationDataFilter::Pointer);

	/**
	 * \brief Query the data element corresponding to the subscript value.
	 *
	 * \warning The return value may be nullptr, please pay attention to security 
	 *          processing.
	 *
	 * \param index
	 *              Element subscript value
	 * 
	 * \see mitk::NavigationDataToNavigationDataFilter::Pointer
	 */
	mitk::NavigationDataToNavigationDataFilter::Pointer FindFilter(int index);

	/**
	 * It is recommended to use subscripts.
	 */
	mitk::NavigationDataToNavigationDataFilter::Pointer 
		FindFilter(const std::string& name);

	/**
	 * \brief Returns the subscript value corresponding to the element name attribute
	 * 
	 * It is not recommended to use this interface frequently, because when there 
	 * are two elements with the same name attribute in the element, the return 
	 * value will point to the subscript of the first element that matches the 
	 * name attribute.
	 *
	 * \param name
	 *              mitk::NavigationDataToNavigationDataFilter#GetName()
	 */
	int GetIndexByName(const std::string& name);

	bool IsEmpty() const;

	int GetSize() const;

	void UpdateFilter();

protected:
	virtual void UpdateConntedToFilter();
	virtual void UpdateDisConntedToFilter();

	virtual void GenerateData() override;
private:
	struct PipelineManagerPrivateImp;
	std::shared_ptr<PipelineManagerPrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPipelineManager_H
