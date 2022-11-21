/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief This is the header file that declares the navigation data collection 
 *        filter.
 * 
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \version V1.0.0
 * \date 2022-11-18 10:15:42
 * 
 * \par Modify History
 *		-# Sun initialization the version for 2022-11-18 10:15:42.
 *
 * \author Sun
 * \date 2022-11-18 10:15:42
 * \remark Non
 */
#ifndef LancetSpatialFittingNavigationToolCollector_H
#define LancetSpatialFittingNavigationToolCollector_H

#include "lancetSpatialFittingGlobal.h"

// mitk
#include <mitkNavigationDataToNavigationDataFilter.h>

// Qt
#include <QObject>

BEGIN_SPATIAL_FITTING_NAMESPACE

/**
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \brief This is the navigation pipeline data collection filter.
 *
 * If you are worried that the accuracy fluctuation of navigation data will 
 * affect your calculation results, this function class can solve your problems.
 * 
 * This function is to judge the accuracy by comparing the valid data of the 
 * nearest \c NavigationToolCollector::SetNumberForMean frame when you send the 
 * acquisition request. The acceptable accuracy range is configured by the \c 
 * NavigationToolCollector::SetAccuracyRange interface. If the collector encou-
 * nters tool failure or the accuracy fluctuation range is larger than the acc-
 * eptable range during the acquisition process, the collector will judge this 
 * processing failure and notify the followers by NavigationToolCollector::Fail 
 * signal.
 * 
 * <b>This filter does not affect any navigation data.</b>
 * 
 *
 * \code
 *	NavigationToolCollector collector = NavigationToolCollector::New();
 * collector->ConnectTo(NDI_Probe);
 * collector->SetInterval(20);
 * collector->SetNumberForMean(10);
 * 
 * // Link Qt communication interface.
 * connect(collector, SIGNAL(Complete(mitk::NavigationData*)), this,
 * [=](mitk::NavigationData* tool) {
 *   NavigationToolCollector* sender_collector = dynamic_cast<NavigationToolCollector*>(sender());
 *   if(sender_collector && tool) {
 *     std::cout << "data.collector.Name " << sender_collector->GetName() << std::endl;
 *     std::cout << "data.collector.FPS " << 1000 / sender_collector->GetInterval() << std::endl;
 *     std::cout << "data.collector.Number " << 1000 / sender_collector->GetNumberForMean() << std::endl;
 *
 *     std::cout << "data.collector.tool.Name " << tool->GetName() << std::endl;
 *   }
 * });
 * collector->Update(); // Navigation data has not changed.
 * \endcode
 *
 * \author Sun
 * \version V1.0.0
 * \date 2022-11-21 09:53:22
 * \remark todo: insert comments
 *
 * Contact: sh4a01@163.com
 *
 */
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	NavigationToolCollector 
	: public QObject
	, public mitk::NavigationDataToNavigationDataFilter
{
	Q_OBJECT
public:
	mitkClassMacro(NavigationToolCollector,
		mitk::NavigationDataToNavigationDataFilter);

	itkNewMacro(NavigationToolCollector)

	NavigationToolCollector();
	virtual~NavigationToolCollector();

	/**
	 * \enum WorkStatus
	 * 
	 * \brief Enumeration type of internal working state of the collector.
	 */
	enum WorkStatus
	{
		Unknown = 0x00000001,	///< Unknown status.
		Idle = 0x00000010,		///< Initial state. When the collector stops working, this is the normal state.
		Working = 0x00000100	///< The state when the collector is working.
	};

	/**
	 * \enum ErrorCodes
	 *
	 * \brief Enumeration type for the error state of the collector..
	 */
	enum ErrorCodes
	{
		Accuracy,							///< The accuracy exceeds the acceptable range of the collector.
		Invisible							///< During the acquisition process, the device or tool signal is unstable.
	};

	Q_ENUM(WorkStatus)
	Q_ENUM(ErrorCodes)
public:
	/**
	 * \brief This property holds the timeout interval in milliseconds
	 * 
   * The default value for this property is 0. A QTimer with a timeout interval 
	 * of 0 will time out as soon as all the events in the window system's event 
	 * queue have been processed.
	 * 
	 * Default initialization is 20 milliseconds.
	 * 
	 * \see QTimer::setInterval, QTimer::interval
	 */
	virtual long GetInterval() const;
	virtual void SetInterval(long interval);

	/**
	 * \brief Data Average Frames attribute.
	 *
	 * This attribute determines the accuracy of the collector's output results. 
	 * Theoretically, the larger the average frame number is, the more accurate 
	 * the collector's output data will be without considering the performance.
	 *
	 * \warning The working time of each collector is timer interval * average 
	 *          frames.
	 */
	virtual unsigned int GetNumberForMean() const;
	virtual void SetNumberForMean(unsigned int number);

	/**
	 * \brief Range property with acceptable precision.
	 *
	 * When the absolute value of the fluctuation of the intermediate data to be 
	 * collected is greater than the acceptable range, the collector will trigger 
	 * Qt signal \c NavigationToolCollector::Fail() to inform the followers of the 
	 * latest working status.
	 */
	virtual double GetAccuracyRange() const;
	virtual void SetAccuracyRange(double accuracy);

	/**
	 * \brief Current working state of the collector.
	 * 
	 * The collector is unique. If the collection task is not completed, the req-
	 * uest will fail again. Users are advised to take this factor into account 
	 * when using.
	 * 
	 * \code
	 * NavigationToolCollector::Pointer collector = NavigationToolCollector::New();
	 * if(collector->GetWorkState() == NavigationToolCollector::Working)
	 * {
	 *   collector->Stop();
	 * }
	 * collector->Start();
	 * \endcode
	 */
	virtual WorkStatus GetWorkState() const;
	virtual bool IsRunning() const;

	/**
	 * \brief Start the work of the collector.
	 *
	 * Start the work of the collector. It will not stop automatically until the 
	 * data collection succeeds or fails. At the same time, you can use Stop to 
	 * stop it. Otherwise, the work of the collector will not stop.
	 *
	 * \param interval
	 *              Filtering frequency of single data frame, in milliseconds.
	 * 
	 * \warning When the parameter is not 0, the Interval property value will be 
	 *          overwritten.
	 * 
	 * \return Successful startup returns true, otherwise false.
	 */
	virtual bool Start(long interval = 0);

	/**
	 * Forcibly stop the work of the collector.
	 */
	virtual bool Stop();

Q_SIGNALS:
	/**
	 * \brief The collector encountered an unacceptable error during operation.
	 *
	 * Generally speaking, only two conditions trigger this signal.
	 * -# When the input navigation data is determined to be invalid during the 
	 *    acquisition process, the collector will determine that the current nav-
	 *    igation equipment or environment is unstable, and the operation will be 
	 *    characterized by the conclusion of failure by default.
	 * -# In the acquisition process, if the fluctuation range of the accuracy of 
	 *    the input navigation data is greater than the acceptable range, the co-
	 *    llector will characterize this operation as a failure.
	 *
	 *
	 * \param errorCode
	 *              Enumeration type for the error state of the collector.
	 * 
	 * \see ErrorCodes
	 */
	void Fail(int);

	/**
	 * \brief Operation frame log of collector during operation.
	 *
	 * \param step
	 *              Current operation frames.
	 *
	 * \param data
	 *              Results of operation frame acquisition.
	 */
	void Step(int, mitk::NavigationData*);

	/**
	 * \brief The collector successfully completes the planned task.
	 *
	 * After triggering this signal, the working state of the collector will 
	 * return to Idea.
	 * 
	 * \param data
	 *              Results of operation frame acquisition.
	 */
	void Complete(mitk::NavigationData*);

protected Q_SLOTS:
	void on_QtTimerTrigger_timeout();

protected:
	virtual void GenerateData() override;

private:
	struct NavigationToolCollectorPrivateImp;
	std::shared_ptr<NavigationToolCollectorPrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingNavigationToolCollector_H
