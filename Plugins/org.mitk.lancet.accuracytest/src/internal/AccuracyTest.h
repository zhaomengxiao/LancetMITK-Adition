/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef AccuracyTest_h
#define AccuracyTest_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>
#include "ui_AccuracyTestControls.h"

#include <mitkNavigationData.h>

#include "mitkNavigationDataSource.h"
#include "mitkNavigationTool.h"

/**
  \brief AccuracyTest

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class AccuracyTest : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;
protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;



  bool CheckInitialization(bool requireRF = true); //<<< checks if the Probe and (if required) the Reference Frame is initialized. Displays a warning and returns false if not.
public:
	mitk::Point3D computeAverageOfPosition(mitk::PointSet&);
	double eachDifference(const mitk::Point3D&, const mitk::Point3D&);
	double standardError(const std::vector<double>&);
	double distanceCompute(const mitk::PointSet&, const mitk::Point3D& point);
	std::vector<double> distanceCompare(const std::vector<double>&);
	double averageValueCompute(const std::vector<double>&);
protected slots:
	void SetProbe();
	void SetReferenceFrame();
	void UpdateTrackingTimer();
	void AddPivotPoint();
	void AddTopplePoint();
	void AddTiltPoint();
	void compute();
	void computeTopple();
	void computeTilt();
private:
  //mitk::NavigationTool::Pointer m_ToolToCalibrate; ///< tool that will be calibrated
  int m_IDofProbe; ///< id of the Probe (of the navigation data source)
  int m_IDofRF; ///< id of the reference frame (of the corresponding navigation data source)
  mitk::NavigationDataSource::Pointer m_NavigationDataSourceOfProbe; ///< navigation data source of the Probe
  mitk::NavigationDataSource::Pointer m_NavigationDataSourceOfRF; ///< navigation data source of the Reference Frame
  
  mitk::PointSet::Pointer m_PointSetPivoting; //Used to record the coordinates of points captured by rotating the probe on an axis
  mitk::DataNode::Pointer m_PointSetPivotingNode; //Used to record the coordinates of points captured by rotating the probe on an axis
  mitk::PointSet::Pointer m_PointSetTopple;
  mitk::DataNode::Pointer m_PointSetToppleNode;
  mitk::PointSet::Pointer m_PointSetTilt;
  mitk::DataNode::Pointer m_PointSetTiltNode;

  QTimer* m_TrackingTimer; //<<< tracking timer that updates the status widgets
  Ui::AccuracyTestControls m_Controls;
};

#endif // AccuracyTest_h
