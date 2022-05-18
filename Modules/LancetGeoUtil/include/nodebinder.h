
#ifndef NODEBINDER_H
#define NODEBINDER_H

#include "MitkLancetGeoUtilExports.h"
#include <itkCommand.h>

class vtkMatrix4x4;

namespace mitk {
  class DataNode;
}

class MITKLANCETGEOUTIL_EXPORT NodeBinder : public itk::Command
{
public:
    /** Smart pointer declaration methods */
    typedef NodeBinder                       Self;
    typedef Command                              Superclass;
    typedef itk::SmartPointer< Self >            Pointer;
    typedef itk::SmartPointer< const Self >      ConstPointer;
    typedef std::map<std::string, mitk::DataNode*>  NodeMap;
    typedef std::map<std::string, vtkMatrix4x4*>    MatrixMap;
    itkTypeMacro(NodeBinder, Command);
    itkNewMacro(Self);

    itkGetMacro(isBind, bool);

    void Execute(itk::Object *caller, const itk::EventObject &event) override;

    void Execute(const itk::Object *caller, const itk::EventObject &event) override;

    /**
     * \brief Add attachment dataNode one by one.
     * \param attachment The node that is bound to the reference node 
     */
    void AddBindingNode(mitk::DataNode *attachment);
    /**
     * \brief Set reference DataNode.
     * \param ref_node A node that moves other nodes with it 
     */
    void SetReferenceNode(mitk::DataNode* ref_node);

    void UpdateBinding();

    void DisableBind();

    void EnableBind();

private:
    mitk::DataNode* m_refNode = nullptr;

    std::map<std::string,mitk::DataNode*> m_attachmentList;
    std::map<std::string,vtkMatrix4x4*> m_matrixList;

    unsigned long m_commandTag{};
    bool m_isBind{ false };
};
#endif // NODEBINDER_H
