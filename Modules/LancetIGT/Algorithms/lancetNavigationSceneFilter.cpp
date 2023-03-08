/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "lancetNavigationSceneFilter.h"


void lancet::NavigationSceneFilter::SetTrackingDevice(mitk::TrackingDeviceSource::Pointer trackingDeviceSource)
{
  CreateNavigationSceneFromTrackingDeviceSource(trackingDeviceSource, m_NavigationScene);

  //create tracking device itself as input,set input will create output.
  this->SetInput(0, m_NavigationScene->GetNodeByName(trackingDeviceSource->GetName())->GetNavigationData());
  //append and connect other inputs from trackingDeviceSource with 1 index offset.
  for (DataObjectPointerArraySizeType i = 0; i < trackingDeviceSource->GetNumberOfOutputs(); i++)
  {
    this->SetInput(1 + i, trackingDeviceSource->GetOutput(i));
  }
}

void lancet::NavigationSceneFilter::AddTrackingDevice(mitk::TrackingDeviceSource::Pointer trackingDeviceSource,
                                                      std::string fatherSceneNodeName)
{
  AppendNavigationSceneFromTrackingDeviceSource(trackingDeviceSource, m_NavigationScene, fatherSceneNodeName);

  //create tracking device itself as input,set input will create output,offset index by NumberOfIndexedInputs
  auto offset = this->GetNumberOfIndexedInputs();
  this->SetInput(offset, m_NavigationScene->GetNodeByName(trackingDeviceSource->GetName())->GetNavigationData());
  //append and connect other inputs from trackingDeviceSource with offset+1 index offset.
  for (DataObjectPointerArraySizeType i = 0; i < trackingDeviceSource->GetNumberOfOutputs(); i++)
  {
    this->SetInput(offset + 1 + i, trackingDeviceSource->GetOutput(i));
  }
}

lancet::NavigationSceneFilter::NavigationSceneFilter() : mitk::NavigationDataToNavigationDataFilter()
{
  m_NavigationScene = NavigationScene::New();
}


lancet::NavigationSceneFilter::~NavigationSceneFilter()
{
}

void lancet::NavigationSceneFilter::GenerateData()
{
  DataObjectPointerArraySizeType numberOfInputs = this->GetNumberOfInputs();

  if (numberOfInputs == 0)
    return;

  this->CreateOutputsForAllInputs();
 
  // generate output
  for (unsigned int i = 0; i < numberOfInputs; ++i)
  {
    const mitk::NavigationData *nd = this->GetInput(i);
    assert(nd);

    mitk::NavigationData *output = this->GetOutput(i);
    assert(output);

    //cal navigation data in reference node coordinates.
    mitk::NavigationData::Pointer nd_input = mitk::NavigationData::New();
    nd_input->Graft(nd);
    auto nd_inRef = m_NavigationScene->GetNavigationData(nd_input, nd->GetName(), m_ReferenceName);

    output->Graft(nd); // copy all information from input to output
    output->SetPosition(nd_inRef->GetPosition());
    output->SetOrientation(nd_inRef->GetOrientation());
    output->SetDataValid(nd->IsDataValid());
  }
}

void lancet::NavigationSceneFilter::CreateNavigationSceneFromTrackingDeviceSource(
  mitk::TrackingDeviceSource::Pointer trackingDeviceSource, NavigationScene::Pointer tree)
{
  //init navigation scene by node represent the tracking Device it self.
  mitk::NavigationData::Pointer nd_trackingDevice = mitk::NavigationData::New();
  nd_trackingDevice->SetName(trackingDeviceSource->GetName());
  nd_trackingDevice->SetDataValid(true);

  auto parentNode = SceneNode::New();
  parentNode->SetNodeName(trackingDeviceSource->GetName());
  parentNode->SetNavigationData(nd_trackingDevice);
  tree->Init(parentNode);
  auto outputs = trackingDeviceSource->GetOutputs();
  for (int i = 0; i < trackingDeviceSource->GetNumberOfOutputs(); i++)
  {
    auto node = SceneNode::New();
    node->SetNodeName(trackingDeviceSource->GetOutput(i)->GetName());
    node->SetNavigationData(trackingDeviceSource->GetOutput(i));
    tree->AddChild(node, parentNode);
  }
}

void lancet::NavigationSceneFilter::AppendNavigationSceneFromTrackingDeviceSource(
  mitk::TrackingDeviceSource::Pointer trackingDeviceSource, NavigationScene::Pointer tree, std::string fatherNodeName)
{
  //create a node represent the tracking Device it self and add it under fatherNode
  mitk::NavigationData::Pointer nd_trackingDevice = mitk::NavigationData::New();
  nd_trackingDevice->SetName(trackingDeviceSource->GetName());
  nd_trackingDevice->SetDataValid(true);

  auto parentNode = SceneNode::New();
  parentNode->SetNodeName(trackingDeviceSource->GetName());
  parentNode->SetNavigationData(nd_trackingDevice);
  tree->AddChild(parentNode,tree->GetNodeByName(fatherNodeName));

  //add trackingDeviceSource nodes under tracking device node.
  for (int i = 0; i < trackingDeviceSource->GetNumberOfOutputs(); i++)
  {
    auto node = SceneNode::New();
    node->SetNodeName(trackingDeviceSource->GetOutput(i)->GetName());
    node->SetNavigationData(trackingDeviceSource->GetOutput(i));
    tree->AddChild(node, parentNode);
  }
}


