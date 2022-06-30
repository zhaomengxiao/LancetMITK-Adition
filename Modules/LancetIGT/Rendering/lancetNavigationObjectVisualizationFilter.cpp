/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/
#include "lancetNavigationObjectVisualizationFilter.h"

#include "mitkDataStorage.h"
#include <vector>

lancet::NavigationObjectVisualizationFilter::NavigationObjectVisualizationFilter()
  : NavigationDataToNavigationDataFilter(),
    m_RepresentationVectorMap(),
    m_TransformPosition(),
    m_TransformOrientation(),
    m_RotationMode(RotationStandard)
{
}

lancet::NavigationObjectVisualizationFilter::~NavigationObjectVisualizationFilter()
{
  m_RepresentationVectorMap.clear();
  m_NavigationObjectVectorMap.clear();
  m_OffsetList.clear();
  m_ObjRegList.clear();
}

mitk::BaseData::Pointer lancet::NavigationObjectVisualizationFilter::GetRepresentationObject(unsigned int idx) const
{  
  auto iter = m_RepresentationVectorMap.find(idx);
  if (iter != m_RepresentationVectorMap.end())
    return iter->second.at(0);

  return nullptr;
}

mitk::BaseData::Pointer lancet::NavigationObjectVisualizationFilter::GetNavigationObject(unsigned int idx) const
{
  auto iter = m_NavigationObjectVectorMap.find(idx);
  if (iter != m_NavigationObjectVectorMap.end())
    return iter->second.at(0);

  return nullptr;
}

std::vector<mitk::BaseData::Pointer> lancet::NavigationObjectVisualizationFilter::GetAllRepresentationObjects(unsigned int idx) const
{
  RepresentationVectorPointerMap::const_iterator iter = m_RepresentationVectorMap.find(idx);
  if (iter != m_RepresentationVectorMap.end())
    return iter->second;

  std::vector<RepresentationPointer> empty;
  return empty;
}

std::vector<mitk::BaseData::Pointer> lancet::NavigationObjectVisualizationFilter::GetAllNavigationObjects(unsigned int idx) const
{
  RepresentationVectorPointerMap::const_iterator iter = m_NavigationObjectVectorMap.find(idx);
  if (iter != m_NavigationObjectVectorMap.end())
    return iter->second;

  std::vector<RepresentationPointer> empty;
  return empty;
}

mitk::AffineTransform3D::Pointer lancet::NavigationObjectVisualizationFilter::GetOffset(int index)
{
  OffsetPointerMap::const_iterator iter = m_OffsetList.find(index);
  if (iter != m_OffsetList.end())
    return iter->second;
  return nullptr;
}

mitk::AffineTransform3D::Pointer lancet::NavigationObjectVisualizationFilter::GetObjectRegistrationMatrix(int index)
{
  OffsetPointerMap::const_iterator iter = m_ObjRegList.find(index);
  if (iter != m_ObjRegList.end())
    return iter->second;
  return nullptr;
}

void lancet::NavigationObjectVisualizationFilter::SetRepresentationObject(unsigned int idx, mitk::BaseData::Pointer data)
{
  std::vector<mitk::BaseData::Pointer> dataVector;
  dataVector.push_back(data);
  SetRepresentationObjects(idx, dataVector);
}

void lancet::NavigationObjectVisualizationFilter::SetRepresentationObjects(unsigned int idx, const std::vector<mitk::BaseData::Pointer> &data)
{
  m_RepresentationVectorMap[idx] = data;
}

void lancet::NavigationObjectVisualizationFilter::SetNavigationObject(unsigned int idx, mitk::BaseData::Pointer data)
{
  std::vector<mitk::BaseData::Pointer> dataVector;
  dataVector.push_back(data);
  SetNavigationObjects(idx, dataVector);
}

void lancet::NavigationObjectVisualizationFilter::SetNavigationObjects(unsigned int idx, const std::vector<mitk::BaseData::Pointer>& data)
{
  m_NavigationObjectVectorMap[idx] = data;
}

void lancet::NavigationObjectVisualizationFilter::SetOffset(int index, mitk::AffineTransform3D::Pointer offset)
{
  m_OffsetList[index] = offset;
}

void lancet::NavigationObjectVisualizationFilter::SetObjectRegistrationMatrix(int index,
  mitk::AffineTransform3D::Pointer matrix)
{
  m_ObjRegList[index] = matrix;
}

void lancet::NavigationObjectVisualizationFilter::SetRotationMode(RotationMode r)
{
  m_RotationMode = r;
}

void lancet::NavigationObjectVisualizationFilter::GenerateData()
{
  /*get each input, lookup the associated BaseData and transfer the data*/
  DataObjectPointerArray inputs = this->GetInputs(); // get all inputs
  for (unsigned int index = 0; index < inputs.size(); index++)
  {
    // get the needed variables
    const mitk::NavigationData *nd = this->GetInput(index);
    assert(nd);

    mitk::NavigationData *output = this->GetOutput(index);
    assert(output);

    // check if the data is valid
    if (!nd->IsDataValid())
    {
      output->SetDataValid(false);
      continue;
    }
    output->Graft(nd); // copy all information from input to output

    //show navigation tool 
    const std::vector<RepresentationPointer> data =
      this->GetAllRepresentationObjects(index);

    for (unsigned int dataIdx = 0; dataIdx < data.size(); dataIdx++)
    {
      if (data.at(dataIdx) == nullptr)
      {
        MITK_WARN << "No BaseData associated with input " << index;
        continue;
      }

      // get the transform from data
      mitk::AffineTransform3D::Pointer affineTransform = data.at(dataIdx)->GetGeometry()->GetIndexToWorldTransform();
      if (affineTransform.IsNull())
      {
        MITK_WARN << "AffineTransform IndexToWorldTransform not initialized!";
        continue;
      }

      // check for offset
      mitk::AffineTransform3D::Pointer offset = this->GetOffset(index);

      // store the current scaling to set it after transformation
      mitk::Vector3D spacing = data.at(dataIdx)->GetGeometry()->GetSpacing();
      // clear spacing of data to be able to set it again afterwards
      mitk::ScalarType scale[] = {1.0, 1.0, 1.0};
      data.at(dataIdx)->GetGeometry()->SetSpacing(scale);

      /*now bring quaternion to affineTransform by using vnl_Quaternion*/
      affineTransform->SetIdentity();

      if (this->GetTransformOrientation(index) == true)
      {
        mitk::NavigationData::OrientationType orientation = nd->GetOrientation();

        /* because of an itk bug, the transform can not be calculated with float data type.
        To use it in the mitk geometry classes, it has to be transfered to mitk::ScalarType which is float */
        static mitk::AffineTransform3D::MatrixType m;

        // convert quaternion to rotation matrix depending on the rotation mode
        if (m_RotationMode == RotationStandard)
        {
          // calculate the transform from the quaternions
          static itk::QuaternionRigidTransform<double>::Pointer quatTransform =
            itk::QuaternionRigidTransform<double>::New();
          // convert mitk::ScalarType quaternion to double quaternion because of itk bug
          vnl_quaternion<double> doubleQuaternion(orientation.x(), orientation.y(), orientation.z(), orientation.r());
          quatTransform->SetIdentity();
          quatTransform->SetRotation(doubleQuaternion);
          quatTransform->Modified();
          mitk::TransferMatrix(quatTransform->GetMatrix(), m);
        }

        else if (m_RotationMode == RotationTransposed)
        {
          vnl_matrix_fixed<mitk::ScalarType, 3, 3> rot = orientation.rotation_matrix_transpose();
          for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
              m[i][j] = rot[i][j];
        }
        affineTransform->SetMatrix(m);
      }
      if (this->GetTransformPosition(index) == true)
      {
        ///*set the offset by convert from itkPoint to itkVector and setting offset of transform*/
        mitk::Vector3D pos;
        pos.SetVnlVector(nd->GetPosition().GetVnlVector());
        affineTransform->SetOffset(pos);
      }
      affineTransform->Modified();

      // set the transform to data
      if (offset.IsNotNull()) // first use offset if there is one.
      {
        mitk::AffineTransform3D::Pointer overallTransform = mitk::AffineTransform3D::New();
        overallTransform->SetIdentity();
        overallTransform->Compose(offset);
        overallTransform->Compose(affineTransform);
        data.at(dataIdx)->GetGeometry()->SetIndexToWorldTransform(overallTransform);
      }
      else
      {
        data.at(dataIdx)->GetGeometry()->SetIndexToWorldTransform(affineTransform);
      }

      // set the original spacing to keep scaling of the geometrical object
      data.at(dataIdx)->GetGeometry()->SetSpacing(spacing);
      data.at(dataIdx)->GetGeometry()->Modified();
      data.at(dataIdx)->Modified();
      output->SetDataValid(true); // operation was successful, therefore data of output is valid.
    }

    //show navigation object
    const std::vector<RepresentationPointer> navigationObject =
      this->GetAllNavigationObjects(index);
    for (unsigned int dataIdx = 0; dataIdx < navigationObject.size(); dataIdx++)
    {
      if (navigationObject.at(dataIdx) == nullptr)
      {
        MITK_WARN << "No BaseData associated with input " << index;
        continue;
      }

      // get the transform from data
      mitk::AffineTransform3D::Pointer affineTransform = navigationObject.at(dataIdx)->GetGeometry()->GetIndexToWorldTransform();
      if (affineTransform.IsNull())
      {
        MITK_WARN << "AffineTransform IndexToWorldTransform not initialized!";
        continue;
      }

      // check for registration matrix
      mitk::AffineTransform3D::Pointer registrationMatrix = this->GetObjectRegistrationMatrix(index);

      // store the current scaling to set it after transformation
      mitk::Vector3D spacing = navigationObject.at(dataIdx)->GetGeometry()->GetSpacing();
      // clear spacing of data to be able to set it again afterwards
      mitk::ScalarType scale[] = { 1.0, 1.0, 1.0 };
      navigationObject.at(dataIdx)->GetGeometry()->SetSpacing(scale);

      /*now bring quaternion to affineTransform by using vnl_Quaternion*/
      affineTransform->SetIdentity();

      if (this->GetTransformOrientation(index) == true)
      {
        mitk::NavigationData::OrientationType orientation = nd->GetOrientation();

        /* because of an itk bug, the transform can not be calculated with float data type.
        To use it in the mitk geometry classes, it has to be transfered to mitk::ScalarType which is float */
        static mitk::AffineTransform3D::MatrixType m;

        // convert quaternion to rotation matrix depending on the rotation mode
        if (m_RotationMode == RotationStandard)
        {
          // calculate the transform from the quaternions
          static itk::QuaternionRigidTransform<double>::Pointer quatTransform =
            itk::QuaternionRigidTransform<double>::New();
          // convert mitk::ScalarType quaternion to double quaternion because of itk bug
          vnl_quaternion<double> doubleQuaternion(orientation.x(), orientation.y(), orientation.z(), orientation.r());
          quatTransform->SetIdentity();
          quatTransform->SetRotation(doubleQuaternion);
          quatTransform->Modified();
          mitk::TransferMatrix(quatTransform->GetMatrix(), m);
        }

        else if (m_RotationMode == RotationTransposed)
        {
          vnl_matrix_fixed<mitk::ScalarType, 3, 3> rot = orientation.rotation_matrix_transpose();
          for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
              m[i][j] = rot[i][j];
        }
        affineTransform->SetMatrix(m);
      }
      if (this->GetTransformPosition(index) == true)
      {
        ///*set the offset by convert from itkPoint to itkVector and setting offset of transform*/
        mitk::Vector3D pos;
        pos.SetVnlVector(nd->GetPosition().GetVnlVector());
        affineTransform->SetOffset(pos);
      }
      affineTransform->Modified();

      // set the transform to data
      if (registrationMatrix.IsNotNull()) // first use offset if there is one.
      {
        mitk::AffineTransform3D::Pointer overallTransform = mitk::AffineTransform3D::New();
        overallTransform->SetIdentity();
        overallTransform->Compose(registrationMatrix);
        overallTransform->Compose(affineTransform);
        navigationObject.at(dataIdx)->GetGeometry()->SetIndexToWorldTransform(overallTransform);
      }
      else
      {
        navigationObject.at(dataIdx)->GetGeometry()->SetIndexToWorldTransform(affineTransform);
      }

      // set the original spacing to keep scaling of the geometrical object
      navigationObject.at(dataIdx)->GetGeometry()->SetSpacing(spacing);
      navigationObject.at(dataIdx)->GetGeometry()->Modified();
      navigationObject.at(dataIdx)->Modified();
      output->SetDataValid(true); // operation was successful, therefore data of output is valid.
    }
  }
}

void lancet::NavigationObjectVisualizationFilter::SetTransformPosition(unsigned int index, bool applyTransform)
{
  itkDebugMacro("setting TransformPosition for index " << index << " to " << applyTransform);
  BooleanInputMap::const_iterator it = this->m_TransformPosition.find(index);
  if ((it != this->m_TransformPosition.end()) && (it->second == applyTransform))
    return;

  this->m_TransformPosition[index] = applyTransform;
  this->Modified();
}

bool lancet::NavigationObjectVisualizationFilter::GetTransformPosition(unsigned int index) const
{
  itkDebugMacro("returning TransformPosition for index " << index);
  BooleanInputMap::const_iterator it = this->m_TransformPosition.find(index);
  if (it != this->m_TransformPosition.end())
    return it->second;
  else
    return true; // default to true
}

void lancet::NavigationObjectVisualizationFilter::TransformPositionOn(unsigned int index)
{
  this->SetTransformPosition(index, true);
}

void lancet::NavigationObjectVisualizationFilter::TransformPositionOff(unsigned int index)
{
  this->SetTransformPosition(index, false);
}

void lancet::NavigationObjectVisualizationFilter::SetTransformOrientation(unsigned int index, bool applyTransform)
{
  itkDebugMacro("setting TransformOrientation for index " << index << " to " << applyTransform);
  BooleanInputMap::const_iterator it = this->m_TransformOrientation.find(index);
  if ((it != this->m_TransformOrientation.end()) && (it->second == applyTransform))
    return;

  this->m_TransformOrientation[index] = applyTransform;
  this->Modified();
}

bool lancet::NavigationObjectVisualizationFilter::GetTransformOrientation(unsigned int index) const
{
  itkDebugMacro("returning TransformOrientation for index " << index);
  BooleanInputMap::const_iterator it = this->m_TransformOrientation.find(index);
  if (it != this->m_TransformOrientation.end())
    return it->second;
  else
    return true; // default to true
}

void lancet::NavigationObjectVisualizationFilter::TransformOrientationOn(unsigned int index)
{
  this->SetTransformOrientation(index, true);
}

void lancet::NavigationObjectVisualizationFilter::TransformOrientationOff(unsigned int index)
{
  this->SetTransformOrientation(index, false);
}
