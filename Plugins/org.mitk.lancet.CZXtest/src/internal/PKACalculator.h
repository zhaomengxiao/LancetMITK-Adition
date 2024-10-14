//#pragma once
//#ifndef PKACALCULATOR
//#define PKACALCULATOR
//#include <mitkDataStorage.h>
//#include <mitkPointSet.h>
//#include <vtkSmartPointer.h>
//#include <vtkMatrix4x4.h>
//
//#include "PKAData.h"
//#include "PKAEnum.h"
//#include "PKAModel.h"
//namespace lancetAlgorithm
//{
//    class PKACalculator
//    {
//    public:
//        PKACalculator() {
//            femurProsMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
//            tibiaTrayMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
//            tibiaInsertMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
//        }
//        PKACalculator(mitk::DataStorage* in, vtkMatrix4x4* femurProsM,
//            vtkMatrix4x4* tibiaTrayM, vtkMatrix4x4* tibiaInsertM);
//        ~PKACalculator();
//
//        virtual void BuildPKAFemur(std::string femurClippedNodeName);
//        virtual void BuildPKATibia(std::string tibiaClippedNodeName);
//        void BuildPKAFemurImplant(std::string femurImplantNodeName);
//        void BuildPKATibiaImplant(std::string tibiaImplantNodeName);
//        void UpdatePKAResult();
//        void BuildSurface(std::string femurClippedNodeName, std::string tibiaClippedNodeName);
//        void BuildImplant(std::string femurImplantNodeName, std::string tibiaImplantNodeName);
//
//        void SetDataStorage(mitk::DataStorage* in);
//        void SetFemurProsMatrix(vtkMatrix4x4* in);
//        void SetTibiaTrayMatrix(vtkMatrix4x4* in);
//        void SetTibiaInsertMatrix(vtkMatrix4x4* in);
//
//    protected:
//        mitk::DataStorage* ds;
//        vtkSmartPointer<vtkMatrix4x4> femurProsMatrix;
//        vtkSmartPointer<vtkMatrix4x4>  tibiaTrayMatrix;
//        vtkSmartPointer<vtkMatrix4x4>  tibiaInsertMatrix;
//    };
//}
//
//#endif // !PKACALCULATOR
