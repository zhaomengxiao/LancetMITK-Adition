/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief This is the header file for declaring abstract registration function 
 *        modules in generator mode.
 * 
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \version V1.0.0
 * \date 2022-11-18 16:37:22
 * 
 * \par Modify History
 *		-# Sun initialization the version for 2023-05-09 14:35:22.
 *
 * \author Sun
 * \remark Non
 */
#ifndef lancetSpatialFittingAbstractModel_H
#define lancetSpatialFittingAbstractModel_H

#include "lancetSpatialFittingGlobal.h"

// Include header files of Qt.
#include <QString>


// Pre declare the navigation data source data object of the mitk module.
namespace mitk { class NavigationDataSource; } 

// Pre declaration of the basic attribute object type of the mitk module.
namespace mitk { class BaseProperty; }


BEGIN_SPATIAL_FITTING_NAMESPACE

class PipelineManager;  // Pre declaration pipeline data type.

class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN AbstractModel
    : public berry::Object
{
public:
	berryObjectMacro(lancet::spatial_fitting::AbstractModel);
    /**
     * \brief Initialize model pure virtual interface.
     * 
     * \note Derived classes must override this interface to implement model initialization 
     *       functionality.
     * 
     * \return Output True if the operation is successful, otherwise output false.
     * 
     * \see bool AbstractModel#Uninitialize()
     */
    virtual bool Initialize() = 0;
    
    /**
     * \brief Deconstructive Pure Virtual Interface for Unloading Models.
     * 
     * \note Derived classes must override this interface to implement model initialization 
     *       functionality.
     * 
     * \return Output True if the operation is successful, otherwise output false.
     * 
     * \see bool AbstractModel#Initialize()
     */
    virtual bool Uninitialize() = 0;

    /**
     * \brief Check if the model is initialized.
     * 
     * \note Derived classes must override this interface to implement model initialization 
     *       functionality.
     * 
     * \return If the model has been initialized and is in a waiting state, return true.
     * 
     * \see bool AbstractModel#Initialize(), bool AbstractModel#Uninitialize()
     */
    virtual bool isInitialize() const = 0;

		/**
		 * \brief Start the workflow for the model.
		 *
		 * Start the workflow of the model, make it run automatically, handle it at 
		 * any time, or prepare to provide the user with a feature set.
		 *
		 * \see bool AbstractModel#StopWorking(), bool AbstractModel#IsWorking()
		 */
		virtual bool StartWorking() = 0;

		/**
		 * \brief Stop the workflow for the model.
		 *
		 * Stop the workflow of the model and leave it in a silent state, no longer 
		 * processing or providing active functionality, but the passive functionality 
		 * is not affected.
		 *
		 * \see bool AbstractModel#StartWorking(), bool AbstractModel#IsWorking()
		 */
		virtual bool StopWorking() = 0;
		virtual bool IsWorking() const = 0;

    /**
     * \param Serial number of the model.
     * 
     * \see void AbstractModel#SetSerialNumber(const QString&), QString GetSerialNumber() const
     */
    AbstractModel(const QString& = QString("None"));
public:

    virtual QString GetSerialNumber() const;
    virtual void SetSerialNumber(const QString&);

    /**
     * \brief Set the navigation data source object for NDI devices.
     * 
     * \warning If it is null, a null smart pointer is returned. See itk::SmartPointer 
     *          for details.
     * 
     * \see void AbstractModel#SetNdiNavigationDataSource()
     */
    virtual itk::SmartPointer<mitk::NavigationDataSource> 
        GetNdiNavigationDataSource() const;

    /**
     * \brief Returns the navigation data source object for NDI devices.
     * 
     * \see itk::SmartPointer<mitk::NavigationDataSource> AbstractModel#GetNdiNavigationDataSource()
     */
    virtual void 
    SetNdiNavigationDataSource(const itk::SmartPointer<mitk::NavigationDataSource>&);

    /**
     * \brief Set the navigation data source object for robotics devices.
     * 
     * \warning If it is null, a null smart pointer is returned. See itk::SmartPointer 
     *          for details.
     * 
     * \see void AbstractModel#SetRoboticsNavigationDataSource()
     */
    virtual itk::SmartPointer<mitk::NavigationDataSource> 
    GetRoboticsNavigationDataSource() const;

    /**
     * \brief Returns the navigation data source object for NDI devices.
     * 
     * \see itk::SmartPointer<mitk::NavigationDataSource> AbstractModel#GetRoboticsNavigationDataSource()
     */
	virtual void 
    SetRoboticsNavigationDataSource(const itk::SmartPointer<mitk::NavigationDataSource>&);

	virtual void SetProperty(const QString&, const itk::SmartPointer<mitk::BaseProperty>&);

	virtual itk::SmartPointer<mitk::BaseProperty> GetProperty(const QString&) const;
private:
    struct PrivateImp;
    std::shared_ptr<PrivateImp> imp;
};


END_SPATIAL_FITTING_NAMESPACE
#endif // !lancetSpatialFittingAbstractModel_H