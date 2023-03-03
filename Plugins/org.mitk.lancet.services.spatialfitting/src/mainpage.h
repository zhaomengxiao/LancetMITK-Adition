#ifndef MainPage_H
#define MainPage_H

/**
 * \defgroup org_mitk_lancet_services_spatialfitting
 *
 * \brief Spatial fitting data processing module.
 *
 * <b>Namespaces</b><br>
 * \c lancet::spatial_fitting 
 * Contains all functions and declarations related to spatial fit.
 * 
 * <b>Detailed Description</b><br>
 * 
 * The spatial computing module mainly aims at data calculation or processing
 * between the mechanical arm, NDI and images. These spatial calculations are
 * complex, and the spatial fitting module just shields these complex operations
 * from the UI or users.
 * 
 * To use this module function in mitk, you need to add the following commands 
 * in manifest_headers.cmake:
 *
 * \code
 * // ...
 * set(Require-Plugin org_mitk_lancet_services_spatialfitting)
 * \endcode
 *
 * <b>Class stream:</b><br>
 * \image html ClassDiagram.svg
 *
 * <b>Spatial Fitting Overview</b><br>
 * Spatial fitting is a functional module for spatial calculation based on Mitk 
 * framework. Spatial fitting provides common functions in spatial processing, 
 * aiming to simplify the compilation of spatial processing in Mitk.<br>
 * 
 * Spatial data processing is mainly driven by the pipeline mode. In the module, 
 * you can implement the functions or data you want by extending user-defined 
 * filters or builders.
 * 
 * 
 * <b>Create a custom filter</b><br>
 * To create a custom filter, subclass mitk::NavigationDataToNavigationDataFilter 
 * and override its GenerateData interface. This interface is the processing 
 * unit of the pipeline filter. In the processing unit, you can process the input 
 * data and store the processed data in the output cache object.
 * 
 * After subclassing the mitk::NavigationDataToNavigationDataFilter, you should 
 * integrate the customized filter into the module. The specification in the 
 * module is Mitk, that is, you need to use the code macro to extend the following 
 * specification code.
 * 
 * - \c mitkClassMacro(className, SuperClassName) 
 *   Extending the basic specification interface of Mitk class.
 * - \c itkNewMacro(x) 
 *   Extend the basic specification interface created by the Itk class.
 * 
 * \code
 * class MyCustomFilter : public mitk::NavigationDataToNavigationDataFilter
 * {
 * public:
 *   mitkClassMacro(MyCustomFilter, mitk::NavigationDataToNavigationDataFilter)
 *   itkNewMacro(MyCustomFilter)
 *   MyCustomFilter() {}
 * protected:
 *   virtual void GenerateData() override { // Do something. }
 * };
 * \endcode
 * 
 * 
 * <b>Create a custom filter builder</b><br>
 * 
 * The filter builder is based on the creator model, and the role of the filter 
 * builder is the constructor. When we extend a custom filter, we need to extend 
 * a custom filter builder to create ideal pipeline data without changing the 
 * original code.
 * 
 * Create a custom filter builder, You need to subclass AbstractPipelineBuilder 
 * and override the AbstractPipelineBuilder#Reset() and AbstractPipelineBuilder#GetOutput() 
 * interfaces, which will function as the internal operation of the spatial 
 * fitting model.
 *
 * After subclassing the mitk::NavigationDataToNavigationDataFilter, you should
 * integrate the customized filter into the module. The specification in the
 * module is Mitk, that is, you need to use the code macro to extend the following
 * specification code.
 *
 * - \c mitkClassMacroItkParent(className, SuperClassName)
 *   Extending the standard basic interface of subclassed itk classes.
 * - \c itkNewMacro(x)
 *   Extend the basic specification interface created by the Itk class.
 *
 * \code
 * class MyCustomFilterBuilder : public AbstractPipelineBuilder
 * {
 * public:
 *   mitkClassMacroItkParent(MyCustomFilterBuilder, AbstractPipelineBuilder)
 *   itkNewMacro(MyCustomFilterBuilder)
 *   MyCustomFilterBuilder() {}
 * public:
 *   virtual void Reset() override {}
 *   virtual itk::SmartPointer<PipelineManager> GetOutput() override { // Do something. }
 * };
 * \endcode
 * 
 * 
 * <b>Create a custom build director</b><br>
 * 
 * The construction supervisor is the director of spatial fitting planning and 
 * construction of data pipeline. The construction supervisor decides the 
 * processing procedure and flow direction of a data pipeline.
 *
 * To create a custom construction supervisor, you need to subclass AbstractDirector, 
 * rewrite the AbstractDirector#Builder() interface, and implement planning tasks 
 * within the interface.
 *
 * After subclassing the mitk::NavigationDataToNavigationDataFilter, you should
 * integrate the customized filter into the module. The specification in the
 * module is Mitk, that is, you need to use the code macro to extend the following
 * specification code.
 *
 * - \c mitkClassMacroItkParent(className, SuperClassName)
 *   Extending the standard basic interface of subclassed itk classes.
 * - \c itkNewMacro(x)
 *   Extend the basic specification interface created by the Itk class.
 *
 * \code
 * class MyCustomDirector : public AbstractDirector
 * {
 * public:
 *   mitkClassMacroItkParent(MyCustomDirector, AbstractDirector)
 *   itkNewMacro(MyCustomDirector)
 *   MyCustomDirector() {}
 * public:
 *   virtual bool Builder() override { // Do something. }
 * };
 * \endcode
 */
#endif // !MainPage_H