
#ifndef SURFACEBOOLEAN_H
#define SURFACEBOOLEAN_H

#include "MitkLancetGeoUtilExports.h"
#include <itkCommand.h>

class vtkMatrix4x4;

namespace mitk {
  class DataNode;
}

class MITKLANCETGEOUTIL_EXPORT SurfaceBoolean : public itk::Command
{
public:
    /** Smart pointer declaration methods */
    typedef SurfaceBoolean                       Self;
    typedef Command                              Superclass;
    typedef itk::SmartPointer< Self >            Pointer;
    typedef itk::SmartPointer< const Self >      ConstPointer;
    //typedef std::map<std::string, mitk::DataNode*>  NodeMap;
    //typedef std::map<std::string, vtkMatrix4x4*>    MatrixMap;
    itkTypeMacro(SurfaceBoolean, Command)
    itkNewMacro(Self)

    itkGetMacro(IsEnable, bool)
    itkGetMacro(RefNode, mitk::DataNode*)
    itkGetMacro(MoveNode, mitk::DataNode*)
    void Execute(itk::Object *caller, const itk::EventObject &event) override;

    void Execute(const itk::Object *caller, const itk::EventObject &event) override;

    /**
     * \brief Add moving dataNode used to cut or fuse to reference node.
     * \param move_node The node that is bound to the reference node 
     */
    void SetMovingNode(mitk::DataNode * move_node);
    /**
     * \brief Set reference DataNode.
     * \param ref_node A node that other nodes operate on it 
     */
    void SetReferenceNode(mitk::DataNode* ref_node);

    void Update();

    void Disable();

    void Enable();

private:
    mitk::DataNode* m_RefNode = nullptr;
    mitk::DataNode* m_MoveNode = nullptr;

    // std::map<std::string,mitk::DataNode*> m_attachmentList;
    // std::map<std::string,vtkMatrix4x4*> m_matrixList;

    unsigned long m_commandTag{};
    bool m_IsEnable{ false };
};
#endif // NODEBINDER_H
