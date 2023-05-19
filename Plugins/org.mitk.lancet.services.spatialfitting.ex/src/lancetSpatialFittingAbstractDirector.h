/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief This is the header file that declares the abstract decision-maker in 
 *        the generator pattern.
 * 
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \version V1.0.0
 * \date 2022-11-18 16:37:22
 * 
 * \par Modify History
 *		-# Sun initialization the version for 2022-11-18 16:37:22.
 *
 * \author Sun
 * \remark Non
 */
#ifndef LancetSpatialFittingAbstractDirector_H
#define LancetSpatialFittingAbstractDirector_H

#include "lancetSpatialFittingGlobal.h"

BEGIN_SPATIAL_FITTING_NAMESPACE
class AbstractPipelineBuilder;

/**
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \brief This is the decision-maker class of the abstract generator pattern.
 *
 * Strictly speaking, the supervisor class is not necessarily required in the 
 * program. The client code can directly call the creation steps in a specific 
 * order. However, it is very suitable to put various routine construction 
 * processes into the supervisor class for repeated use in the program.
 *
 * In addition, for client code, the supervisor class completely hides the det-
 * ails of product construction. The client only needs to associate a generator 
 * with the supervisor class, and then use the supervisor class to construct the 
 * product, so that the construction result can be obtained from the generator.
 * 
 * <b>Class diagram:</b><br>
 * \image html SpatialFittingAbstractDirector.svg
 *
 * <b>Example:</b><br>
 * \code
 * // Building Entity Decision Maker and Generator Objects.
 * AbstractDirector::Pointer director = MyDirector::New();
 * director->SetBuilder(MyPipelineBuilder::New());
 * // Using Polymorphic Features to Call the Construction Method of Entity 
 * // Decision Maker.
 * director->Builder();
 * //Get the pipeline data object produced by the builder.
 * PipelineManager::Pointer pipeline = director->GetBuilder()->GetOutput();
 * \endcode
 *
 * \author Sun
 * \version V1.0.0
 * \date 2022-11-18 16:45:08
 * \remark Non
 *
 * Contact: sh4a01@163.com
 *
 */
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN 
	AbstractDirector : public itk::Object
{
public:
	mitkClassMacroItkParent(AbstractDirector, itk::Object);

	AbstractDirector();

	/**
	 * \brief Implement the construction order or method of pipeline data.
	 *
	 * In the spatial fitting module, the functional components of various pipe-
	 * line data objects may be the same component, but the production order has 
	 * some deviations. The derived class needs to correctly control the produce
	 * order of pipeline data for this interface.
	 *
	 * \return True Production succeeded, otherwise it failed.
	 */
	virtual bool Builder() = 0;

	/**
	 * \warning Before calling this interface, please ensure that the AbstractDirector#Builder() 
	 *          interface has been called and returns to normal working state.
	 */
	virtual itk::SmartPointer<AbstractPipelineBuilder> GetBuilder() const;
	virtual void SetBuilder(itk::SmartPointer<AbstractPipelineBuilder> builder);
private:
	struct AbstractDirectorPrivateImp;
	std::shared_ptr<AbstractDirectorPrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingAbstractDirector_H
