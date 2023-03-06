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

#include "lancetTreeCoords.h"
#include "mitkNavigationDataSource.h"
#include "mitkNavigationTool.h"
#include "mitkTrackingDeviceSource.h"
#include "lancetNavigationDataInReferenceCoordFilter.h"
#include "mitkNavigationDataToPointSetFilter.h"

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
	mitk::Point3D computeAverageOfPosition(mitk::PointSet::Pointer);
	double eachDifference(const mitk::Point3D&, const mitk::Point3D&);
	double standardError(const std::vector<double>&);
	std::vector<double> distanceCompare(const std::vector<double>&);
	double averageValueCompute(const std::vector<double>&);
protected slots:
	void SetProbe();
	void SetReferenceFrame();
	void UpdateTrackingTimer();
  void AddPoint(mitk::PointSet::Pointer pointSet);
  void AddPivotPoint();
	void AddTopplePoint();
	void AddTiltPoint();
	void AddDistancePoint();
	void compute();
	void computeTopple();
	void computeTilt();
	void computeDistance();
private:
  // void CreateNavigationTreeFromTrackingDeviceSource(mitk::TrackingDeviceSource::Pointer trackingDeviceSource, NavigationTree::Pointer tree)
  // {
  //   auto parentNode = NavigationNode::New();
  //   parentNode->SetNodeName(trackingDeviceSource->GetName());
  //   parentNode->SetNavigationData(mitk::NavigationData::New());
  //   tree->Init(parentNode);
  //
  //   auto outputs = trackingDeviceSource->GetOutputs();
  //   for (int i = 0; i < trackingDeviceSource->GetNumberOfOutputs(); i++)
  //   {
  //     auto node = NavigationNode::New();
  //     node->SetNodeName(trackingDeviceSource->GetOutput(i)->GetName());
  //     node->SetNavigationData(trackingDeviceSource->GetOutput(i));
  //
  //     tree->AddChild(node, parentNode);
  //   }
  // }
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
  mitk::PointSet::Pointer m_distancePointSet;
  mitk::DataNode::Pointer m_distancePointSetNode;
  QTimer* m_TrackingTimer; //<<< tracking timer that updates the status widgets
  Ui::AccuracyTestControls m_Controls;

  lancet::NavigationDataInReferenceCoordFilter::Pointer m_NDinRefFilter;
  //mitk::NavigationDataToPointSetFilter::Pointer m_NDtoPointSetFilter;
};

#endif // AccuracyTest_h
