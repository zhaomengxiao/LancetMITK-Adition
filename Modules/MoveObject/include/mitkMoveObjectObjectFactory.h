/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef mitkMoveObjectObjectFactory_h
#define mitkMoveObjectObjectFactory_h

#include <MitkMoveObjectExports.h>
#include <mitkCoreObjectFactoryBase.h>

namespace mitk
{
  class MITKMOVEOBJECT_EXPORT MoveObjectObjectFactory : public CoreObjectFactoryBase
  {
  public:
    mitkClassMacro(MoveObjectObjectFactory, CoreObjectFactoryBase);
    itkFactorylessNewMacro(Self);
    itkCloneMacro(Self);

      Mapper::Pointer CreateMapper(DataNode *node, MapperSlotId slotId) override;
    void SetDefaultProperties(DataNode *node) override;
    std::string GetFileExtensions() override;
    CoreObjectFactoryBase::MultimapType GetFileExtensionsMap() override;
    std::string GetSaveFileExtensions() override;
    CoreObjectFactoryBase::MultimapType GetSaveFileExtensionsMap() override;
    const char *GetDescription() const override;

  protected:
    MoveObjectObjectFactory();
    ~MoveObjectObjectFactory() override;

    void CreateFileExtensionsMap();

  private:
    MultimapType m_FileExtensionsMap;
    MultimapType m_SaveFileExtensionsMap;
  };

  MITKMOVEOBJECT_EXPORT void RegisterMoveObjectObjectFactory();
}

#endif
