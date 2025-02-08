/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef mitkMoveObjectVtkMapper3D_h
#define mitkMoveObjectVtkMapper3D_h

#include <MitkMoveObjectExports.h>

#include <mitkVtkMapper.h>

#include <vtkPropAssembly.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace mitk
{
  class MITKMOVEOBJECT_EXPORT MoveObjectVtkMapper3D : public VtkMapper
  {
  public:
    static void SetDefaultProperties(DataNode *node, BaseRenderer *renderer = nullptr, bool overwrite = false);

    mitkClassMacro(MoveObjectVtkMapper3D, VtkMapper);

    itkFactorylessNewMacro(Self);

    itkCloneMacro(Self);

      void ApplyColorAndOpacityProperties(BaseRenderer *, vtkActor *) override;
    void ApplyMoveObjectProperties(BaseRenderer *renderer, vtkActor *);
    vtkProp *GetVtkProp(BaseRenderer *renderer) override;
    //   virtual void UpdateVtkTransform(mitk::BaseRenderer* renderer) override;
  protected:
    void GenerateDataForRenderer(BaseRenderer *renderer) override;

  private:
    MoveObjectVtkMapper3D();
    ~MoveObjectVtkMapper3D() override;

    MoveObjectVtkMapper3D(const Self &);
    Self &operator=(const Self &);

    class Impl;
    Impl *m_Impl;
  };
}

#endif
