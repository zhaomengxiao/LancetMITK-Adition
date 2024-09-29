//#pragma once
//#include <mitkCoreObjectFactory.h>
//
//class ModelInterFactory : public mitk::CoreObjectFactoryBase
//{
//public:
//    mitkClassMacro(ModelInterFactory, CoreObjectFactoryBase);
//    itkFactorylessNewMacro(Self);
//    itkCloneMacro(Self);
//
//    // Create a mapper for given node
//    mitk::Mapper::Pointer CreateMapper(mitk::DataNode* node, MapperSlotId slotId) override;
//
//    // Assign default properties to given node
//    void SetDefaultProperties(mitk::DataNode* node) override;
//
//    // Deprecated but required!
//    std::string GetFileExtensions() override { return ""; }
//    // Deprecated but required!
//    mitk::CoreObjectFactoryBase::MultimapType GetFileExtensionsMap() override
//    {
//        return CoreObjectFactoryBase::MultimapType();
//    }
//
//    // Deprecated but required!
//    std::string GetSaveFileExtensions() override { return ""; }
//    // Deprecated but required!
//    mitk::CoreObjectFactoryBase::MultimapType GetSaveFileExtensionsMap() override
//    {
//        return CoreObjectFactoryBase::MultimapType();
//    }
//};
//
