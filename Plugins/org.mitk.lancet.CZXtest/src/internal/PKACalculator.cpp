//#include "PKACalculator.h"
//
//lancetAlgorithm::PKACalculator::PKACalculator(mitk::DataStorage* in, vtkMatrix4x4* femurProsM, vtkMatrix4x4* tibiaTrayM, vtkMatrix4x4* tibiaInsertM)
//{
//    femurProsMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
//    tibiaTrayMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
//    tibiaInsertMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
//
//    femurProsMatrix->DeepCopy(femurProsM);
//    tibiaTrayMatrix->DeepCopy(tibiaTrayM);
//    tibiaInsertMatrix->DeepCopy(tibiaInsertM);
//
//    PKA_Model.SetOperationSide(PKAData::m_SurgicalSide);
//}
//
//lancetAlgorithm::PKACalculator::~PKACalculator()
//{
//    femurProsMatrix->Delete();
//    tibiaTrayMatrix->Delete();
//    tibiaInsertMatrix->Delete();
//}
//
//void lancetAlgorithm::PKACalculator::BuildPKAFemur(std::string femurClippedNodeName)
//{
//    auto Femur = dynamic_cast<mitk::Surface*>
//        (ds->GetNamedNode(PKAData::m_FemurClippedSurfaceNodeName.toStdString())->GetData());//固定不需要乘以VTK矩阵来变换
//    auto Epicondyle = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_FemurEpicondyleNodeName.toStdString())->GetData());//固定
//
//    auto Post = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_FemurPosteriorNodeName.toStdString())->GetData());//固定
//
//    auto Distal = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_FemurDistalNodeName.toStdString())->GetData());//固定
//
//    auto Hip = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_HipCenterNodeName.toStdString())->GetData());//固定
//
//    auto Knee = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_FemurKneeCenterNodeName.toStdString())->GetData());//固定
//
//    if (Femur == nullptr || Epicondyle == nullptr || Post == nullptr || Distal == nullptr
//        || Hip == nullptr || Knee == nullptr)
//    {
//        MITK_INFO << "femur data incomplete";
//        return;
//    }
//
//    mitk::DataNode::Pointer tempFemurNode = nullptr;
//
//    
//    tempFemurNode = ds->GetNamedNode(femurClippedNodeName);
//    
//
//    auto femurM = tempFemurNode->GetData()->GetGeometry()->GetVtkMatrix();
//    if (femurM == nullptr)
//        return;
//
//    auto Epicondyle1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(Epicondyle, femurM, Epicondyle1);
//    auto Post1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(Post, femurM, Post1);
//    auto Distal1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(Distal, femurM, Distal1);
//    auto Hip1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(Hip, femurM, Hip1);
//    auto Knee1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(Knee, femurM, Knee1);
//
//    PKA_Model.BuildFemur(10, Epicondyle1->GetPoint(0).GetDataPointer(), Epicondyle1->GetPoint(1).GetDataPointer(),
//        Post1->GetPoint(0).GetDataPointer(), Post1->GetPoint(1).GetDataPointer(), Distal1->GetPoint(0).GetDataPointer()
//        , Distal1->GetPoint(1).GetDataPointer(), Post1->GetPoint(0).GetDataPointer(), Post1->GetPoint(1).GetDataPointer(),
//        Hip1->GetPoint(0).GetDataPointer(), Knee1->GetPoint(0).GetDataPointer());
//}
//
//void lancetAlgorithm::PKACalculator::BuildPKATibia(std::string femurClippedNodeName)
//{
//    auto Tibia = dynamic_cast<mitk::Surface*>
//        (ds->GetNamedNode(PKAData::m_TibiaClippedSurfaceNodeName.toStdString())->GetData());//固定
//    auto PCL = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_PCLCenterNodeName.toStdString())->GetData());//固定
////    PCL->GetGeometry()->SetIdentity();
//    auto Knee = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_TibiaKneeCenterNodeName.toStdString())->GetData());//固定
////    Knee->GetGeometry()->SetIdentity();
//    auto Tub = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_TubercleMedialNodeName.toStdString())->GetData());//固定
////    Tub->GetGeometry()->SetIdentity();
//    auto Mall = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_MalleolusCenterNodeName.toStdString())->GetData());//固定
////    Mall->GetGeometry()->SetIdentity();
//    auto Proximal = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_TibiaProximalNodeName.toStdString())->GetData());//固定
////    Proximal->GetGeometry()->SetIdentity();
//
//
//    if (Tibia == nullptr || PCL == nullptr || Knee == nullptr || Tub == nullptr
//        || Mall == nullptr || Proximal == nullptr)
//    {
//        MITK_INFO << "tibia data incomplete";
//        return;
//    }
//
//    mitk::DataNode::Pointer tempTibiaNode = nullptr;
//
//    tempTibiaNode = ds->GetNamedNode(femurClippedNodeName);
//
//    auto tibiaM = tempTibiaNode->GetData()->GetGeometry()->GetVtkMatrix();
//    if (tibiaM == nullptr)
//        return;
//
//    auto PCL1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(PCL, tibiaM, PCL1);
//    auto Knee1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(Knee, tibiaM, Knee1);
//    auto Tub1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(Tub, tibiaM, Tub1);
//    auto Mall1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(Mall, tibiaM, Mall1);
//    auto Proximal1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(Proximal, tibiaM, Proximal1);
//
//    PKA_Model.BuildTibia(6, PCL1->GetPoint(0).GetDataPointer(), Knee1->GetPoint(0).GetDataPointer(),
//        Mall1->GetPoint(2).GetDataPointer(), Tub1->GetPoint(0).GetDataPointer(),
//        Proximal1->GetPoint(0).GetDataPointer(),
//        Proximal1->GetPoint(1).GetDataPointer());
//}
//
//void lancetAlgorithm::PKACalculator::BuildPKAFemurImplant(std::string femurProsNodeName)
//{
//    auto Implant = dynamic_cast<mitk::Surface*>
//        (ds->GetNamedNode(PKAData::m_FemurImplantNodeName.toStdString())->GetData());
//    auto AnterCham = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_AnteriorChamferCutNodeName.toStdString())->GetData());
//    AnterCham->GetGeometry()->SetIdentity();
//    auto Distal = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_DistalCutNodeName.toStdString())->GetData());
//    Distal->GetGeometry()->SetIdentity();
//
//    auto Post = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_PosteriorCutNodeName.toStdString())->GetData());
//    Post->GetGeometry()->SetIdentity();
//    //auto Resection = dynamic_cast<mitk::PointSet*>
//    //    (ds->GetNamedNode(PKAData::m_ChunliProsthesisRegistrationPointNodeName.toStdString())->GetData());
//    //Resection->GetGeometry()->SetIdentity();
//
//    if (Implant == nullptr  || AnterCham == nullptr || Distal == nullptr
//         || Post == nullptr /*|| Resection == nullptr*/)
//    {
//        MITK_INFO << "femur implant data incomplete\n";
//        return;
//    }
//
//    auto femurProsNode = ds->GetNamedNode
//    (femurProsNodeName);
//    if (femurProsNode == nullptr)
//        return;
//    auto femurProsM = femurProsNode->GetData()->GetGeometry()->GetVtkMatrix();
//
//    auto AnterCham1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(AnterCham, femurProsM, AnterCham1);
//    auto Distal1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(Distal, femurProsM, Distal1);
//    auto Post1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(Post, femurProsM, Post1);
//    //auto Resection1 = mitk::PointSet::New();
//    //CalculationHelper::TransformPoints(Resection, femurProsM, Resection1);
//
//    PKA_Model.BuildFemurImplant(12, AnterCham1->GetPoint(0).GetDataPointer(), AnterCham1->GetPoint(1).GetDataPointer(),
//        Distal1->GetPoint(0).GetDataPointer(), Distal1->GetPoint(1).GetDataPointer(),
//        Post1->GetPoint(0).GetDataPointer(), Post1->GetPoint(1).GetDataPointer()/* ,
//       Resection1->GetPoint(0).GetDataPointer(), Resection1->GetPoint(1).GetDataPointer()*/);
//}
//
//void lancetAlgorithm::PKACalculator::BuildPKATibiaImplant(std::string tibiaTrayNodeName)
//{
//    auto Implant = dynamic_cast<mitk::Surface*>
//        (ds->GetNamedNode(PKAData::m_TibiaTrayNodeName.toStdString())->GetData());
//    auto Proximal = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_TibiaProsthesisProximalNodeName.toStdString())->GetData());
//    Proximal->GetGeometry()->SetIdentity();
//    auto Symmetry = dynamic_cast<mitk::PointSet*>
//        (ds->GetNamedNode(PKAData::m_TibiaProsthesisSymmetryNodeName.toStdString())->GetData());
//    Symmetry->GetGeometry()->SetIdentity();
//
//    if (Implant == nullptr || Proximal == nullptr || Symmetry == nullptr)
//    {
//        MITK_INFO << "tibia implant data incomplete";
//        return;
//    }
//
//    auto tibiaTrayNode = ds->GetNamedNode(tibiaTrayNodeName);
//    if (tibiaTrayNode == nullptr)
//        return;
//    auto tibiaTray = tibiaTrayNode->GetData();
//    auto tibiaM = tibiaTray->GetGeometry()->GetVtkMatrix();
//
//    auto Proximal1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(Proximal, tibiaM, Proximal1);
//    auto Symmetry1 = mitk::PointSet::New();
//    CalculationHelper::TransformPoints(Symmetry, tibiaM, Symmetry1);
//
//    PKA_Model.BuildTibiaImplant(4, Proximal1->GetPoint(0).GetDataPointer(), Proximal1->GetPoint(1).GetDataPointer(),
//        Symmetry1->GetPoint(0).GetDataPointer(), Symmetry1->GetPoint(1).GetDataPointer());
//}
//
//void lancetAlgorithm::PKACalculator::UpdatePKAResult()
//{
//    PKA_Model.CalPrePlanning();
//    PKA_Model.CalIntraPlanning();
//}
//
//void lancetAlgorithm::PKACalculator::BuildSurface(std::string femurClippedNodeName, std::string tibiaClippedNodeName)
//{
//    BuildPKAFemur(femurClippedNodeName);
//    BuildPKATibia(tibiaClippedNodeName);
//}
//
//void lancetAlgorithm::PKACalculator::BuildImplant(std::string femurImplantNodeName, std::string tibiaImplantNodeName)
//{
//    BuildPKAFemurImplant(femurImplantNodeName);
//    BuildPKATibiaImplant(tibiaImplantNodeName);
//}
//
//void lancetAlgorithm::PKACalculator::SetDataStorage(mitk::DataStorage* in)
//{
//    ds = in;
//}
//
//void lancetAlgorithm::PKACalculator::SetFemurProsMatrix(vtkMatrix4x4* in)
//{
//    femurProsMatrix->DeepCopy(in);
//}
//
//void lancetAlgorithm::PKACalculator::SetTibiaTrayMatrix(vtkMatrix4x4* in)
//{
//    tibiaTrayMatrix->DeepCopy(in);
//}
//
//void lancetAlgorithm::PKACalculator::SetTibiaInsertMatrix(vtkMatrix4x4* in)
//{
//    tibiaInsertMatrix->DeepCopy(in);
//}
