/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

//Poco headers
#include <Poco/Zip/Compress.h>
#include <Poco/Path.h>

//mitk headers
#include "lancetNavigationObjectWriter.h"
#include <mitkStandaloneDataStorage.h>
#include <mitkProperties.h>
#include <mitkSceneIO.h>
#include <mitkPointSet.h>
#include <mitkIOUtil.h>
#include <mitkMatrixConvert.h>

//std headers
#include <cstdio>

#include "lancetNavigationObject.h"

lancet::NavigationObjectWriter::NavigationObjectWriter()
{
}

lancet::NavigationObjectWriter::~NavigationObjectWriter()
{
}

bool lancet::NavigationObjectWriter::DoWrite(std::string FileName, NavigationObject::Pointer Object)
{
  //some initial validation checks...
  if (Object.IsNull())
  {
    m_ErrorMessage = "Cannot write a navigation object containing invalid object data, aborting!";
    MITK_ERROR << m_ErrorMessage;
    return false;
  }


  // Workaround for a problem: the geometry might be modified if the tool is tracked. If this
  // modified geometry is saved the surface representation is moved by this offset. To avoid
  // this bug, the geometry is set to identity for the saving progress and restored later.
  mitk::BaseGeometry::Pointer geometryBackup;
  if (Object->GetDataNode().IsNotNull()
    && (Object->GetDataNode()->GetData() != nullptr)
    && (Object->GetDataNode()->GetData()->GetGeometry() != nullptr)
  )
  {
    geometryBackup = Object->GetDataNode()->GetData()->GetGeometry()->Clone();
    Object->GetDataNode()->GetData()->GetGeometry()->SetIdentity();
  }
  else { MITK_WARN << "Saving a object with invalid data node, proceeding but errors might occure!"; }

  //convert whole data to a mitk::DataStorage
  mitk::StandaloneDataStorage::Pointer saveStorage = mitk::StandaloneDataStorage::New();
  mitk::DataNode::Pointer thisTool = ConvertToDataNode(Object);
  saveStorage->Add(thisTool);

  //if object contain image data,add it to storage too.
  mitk::Image::Pointer image = Object->GetImage();
  if (image.IsNotNull())
  {
    mitk::DataNode::Pointer imageNode = mitk::DataNode::New();
    imageNode->SetData(image);
    saveStorage->Add(imageNode);
  }

  //use SceneSerialization to save the DataStorage
  std::string DataStorageFileName = mitk::IOUtil::CreateTemporaryDirectory() + Poco::Path::separator() +
    GetFileWithoutPath(FileName) + ".storage";
  mitk::SceneIO::Pointer mySceneIO = mitk::SceneIO::New();
  mySceneIO->SaveScene(saveStorage->GetAll(), saveStorage, DataStorageFileName);

  //now put the DataStorage and the Toolfile in a ZIP-file
  // std::ofstream file( FileName.c_str(), std::ios::binary | std::ios::out);
  // if (!file.good())
  //   {
  //   m_ErrorMessage = "Could not open a zip file for writing: '" + FileName + "'";
  //   MITK_ERROR << m_ErrorMessage;
  //   return false;
  //   }
  // else
  //   {
  //   Poco::Zip::Compress zipper( file, true );
  //   zipper.addFile(DataStorageFileName,GetFileWithoutPath(DataStorageFileName));
  //   if (Object->GetCalibrationFile()!="none") zipper.addFile(Object->GetCalibrationFile(),GetFileWithoutPath(Object->GetCalibrationFile()));
  //   zipper.close();
  //   }

  //delete the data storage
  std::remove(DataStorageFileName.c_str());

  //restore original geometry
  if (geometryBackup.IsNotNull()) { Object->GetDataNode()->GetData()->SetGeometry(geometryBackup); }

  return true;
}

mitk::DataNode::Pointer lancet::NavigationObjectWriter::ConvertToDataNode(NavigationObject::Pointer Object)
{
  mitk::DataNode::Pointer thisTool = Object->GetDataNode();
  //Name
  if (Object->GetDataNode().IsNull())
  {
    thisTool = mitk::DataNode::New();
    thisTool->SetName("none");
  }

  //Object Landmarks
  thisTool->AddProperty("ObjectRegistrationLandmarks",
                        mitk::StringProperty::New(ConvertPointSetToString(Object->GetLandmarks())), nullptr, true);
  thisTool->AddProperty("ObjectRegistrationLandmarks_Probe",
    mitk::StringProperty::New(ConvertPointSetToString(Object->GetLandmarks_probe())), nullptr, true);
  //Object ICPs
  thisTool->AddProperty("ObjectRegistrationIcpPoints",
                        mitk::StringProperty::New(ConvertPointSetToString(Object->GetIcpPoints())), nullptr,
                        true);
  thisTool->AddProperty("ObjectRegistrationIcpPoints_Probe",
    mitk::StringProperty::New(ConvertPointSetToString(Object->GetIcpPoints_probe())), nullptr,
    true);
  //Object Registration Matrix
  thisTool->AddProperty("ObjectRegistrationMatrix",
                        mitk::StringProperty::New(ConvertVtkMatrix4x4ToToString(Object->GetT_Object2ReferenceFrame())),
                        nullptr,
                        true);


  //Material is not needed, to avoid errors in scene serialization we have to do this:
  thisTool->RemoveProperty("material");

  return thisTool;
}

std::string lancet::NavigationObjectWriter::GetFileWithoutPath(std::string FileWithPath)
{
  Poco::Path myFile(FileWithPath.c_str());
  return myFile.getFileName();
}

std::string lancet::NavigationObjectWriter::ConvertPointSetToString(mitk::PointSet::Pointer pointSet)
{
  std::stringstream returnValue;
  mitk::PointSet::PointDataIterator it;
  for (it = pointSet->GetPointSet()->GetPointData()->Begin(); it != pointSet->GetPointSet()->GetPointData()->End(); it
       ++)
  {
    mitk::Point3D thisPoint = pointSet->GetPoint(it->Index());
    returnValue << it->Index() << ";" << ConvertPointToString(thisPoint) << "|";
  }
  return returnValue.str();
}

std::string lancet::NavigationObjectWriter::ConvertPointToString(mitk::Point3D point)
{
  std::stringstream returnValue;
  returnValue << point[0] << ";" << point[1] << ";" << point[2];
  return returnValue.str();
}

std::string lancet::NavigationObjectWriter::ConvertQuaternionToString(mitk::Quaternion quat)
{
  std::stringstream returnValue;
  returnValue << quat.x() << ";" << quat.y() << ";" << quat.z() << ";" << quat.r();
  return returnValue.str();
}

std::string lancet::NavigationObjectWriter::ConvertAffineTransformToString(mitk::AffineTransform3D::Pointer affine)
{
  std::stringstream returnValue;
  auto mat = affine->GetMatrix();
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      returnValue << mat[i][j] << ";";
  auto offset = affine->GetOffset();
  returnValue << offset[0] << ";" << offset[1] << ";" << offset[2];
  return returnValue.str();
}

std::string lancet::NavigationObjectWriter::ConvertVtkMatrix4x4ToToString(vtkSmartPointer<vtkMatrix4x4> matrix)
{
  std::stringstream returnValue;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      returnValue << matrix->GetElement(i,j)<< ";";
  return returnValue.str();
}
