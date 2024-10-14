//#include "ModelInterFactory.h"
//
//// Project includes
//#include "ModelUserControl.h"
//#include "ModelMapper2D.h"
//
//// MITK includes
//#include <mitkBaseRenderer.h>
//#include <mitkDataNode.h>
//#include <mitkProperties.h>
//
//mitk::Mapper::Pointer ModelInterFactory::CreateMapper(mitk::DataNode* node, MapperSlotId id)
//{
//    mitk::Mapper::Pointer newMapper = nullptr;
//
//    if ((dynamic_cast<ModelUserControl*>(node->GetData()) != nullptr))
//    {
//        if (id == mitk::BaseRenderer::Standard2D)
//        {
//            newMapper = ModelMapper2D::New();
//            newMapper->SetDataNode(node);
//        }
//    }
//
//    return newMapper;
//}
//
//void ModelInterFactory::SetDefaultProperties(mitk::DataNode* node)
//{
//    if ((dynamic_cast<ModelUserControl*>(node->GetData()) != nullptr))
//    {
//        ModelMapper2D::SetDefaultProperties(node);
//    }
//}
//
//struct RegisterGizmoObjectFactory
//{
//    RegisterGizmoObjectFactory() : m_Factory(ModelInterFactory::New())
//    {
//        mitk::CoreObjectFactory::GetInstance()->RegisterExtraFactory(m_Factory);
//    }
//    ~RegisterGizmoObjectFactory() { mitk::CoreObjectFactory::GetInstance()->UnRegisterExtraFactory(m_Factory); }
//    ModelInterFactory::Pointer m_Factory;
//};
//
//static RegisterGizmoObjectFactory registerGizmoObjectFactory;