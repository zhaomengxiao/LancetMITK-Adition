/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef NAVIGATIONOBJECTWRITER_H_INCLUDED
#define NAVIGATIONOBJECTWRITER_H_INCLUDED

//itk headers
#include <itkObjectFactory.h>

//mitk headers
#include <mitkCommon.h>
// #include "mitkNavigationToolStorageSerializer.h"
#include <MitkLancetIGTExports.h>

#include "lancetNavigationObject.h"
#include "mitkDataNode.h"


namespace lancet
{
  /**Documentation
  * \brief This class offers methods to write objects of the class navigation tool permanently
  *        to the harddisk. The objects are saved in a special fileformat which can be read
  *        by the class NavigationToolReader to restore the object.
  *
  * \ingroup IGT
  */
  class MITKLANCETIGT_EXPORT NavigationObjectWriter : public itk::Object
  {
     //friend class mitk::NavigationToolStorageSerializer;

  public:
    mitkClassMacroItkParent(NavigationObjectWriter,itk::Object);
    itkFactorylessNewMacro(Self);
    itkCloneMacro(Self);

    /**
     * @brief           Writes a navigation tool to a file.
     * @param  FileName The filename (complete, with path, C:/temp/myTool.igtTool for example)
     * @param  Tool     The tool which should be written to the file.
     * @return          Returns true if the file was written successfully, false if not. In the second
     *                  case you can get the error message by using the method GetErrorMessage().
     */
    bool DoWrite(std::string FileName,NavigationObject::Pointer no);

    itkGetMacro(ErrorMessage,std::string);

  protected:
    NavigationObjectWriter();
    ~NavigationObjectWriter() override;
    std::string m_ErrorMessage;
    mitk::DataNode::Pointer ConvertToDataNode(NavigationObject::Pointer no);
    std::string GetFileWithoutPath(std::string FileWithPath);
    std::string ConvertPointSetToString(mitk::PointSet::Pointer pointSet);
    std::string ConvertPointToString(mitk::Point3D point);
    std::string ConvertQuaternionToString(mitk::Quaternion quat);
    std::string ConvertAffineTransformToString(mitk::AffineTransform3D::Pointer affine);
    std::string ConvertVtkMatrix4x4ToToString(vtkSmartPointer<vtkMatrix4x4> matrix);
  };
} // namespace mitk
#endif //NAVIGATIONOBJECTWRITER
