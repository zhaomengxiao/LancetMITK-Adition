#ifndef LANCETTREECOORDS_H
#define LANCETTREECOORDS_H
#include <vector>

#include "MitkLancetIGTExports.h"
#include "mitkNavigationData.h"
#include "itkObject.h"

class MITKLANCETIGT_EXPORT NavigationNode : public itk::Object
{
public:
  mitkClassMacroItkParent(NavigationNode, itk::Object);
  itkFactorylessNewMacro(Self); //New() and CreateAnother()

  //itkGetMacro(NodeName, std::string);
  itkSetMacro(NodeName, std::string);

  //itkGetMacro(Children, std::vector<NavigationNode::Pointer>);
  //itkSetMacro(Children, std::vector<NavigationNode::Pointer>);

  //itkGetMacro(Parent, NavigationNode::Pointer);
  itkSetMacro(Parent, NavigationNode::Pointer);

  itkSetMacro(NavigationData, mitk::NavigationData::Pointer);

  std::string m_NodeName{"undefined"};
  std::vector<NavigationNode::Pointer> m_Children{};
  NavigationNode::Pointer m_Parent{nullptr};
  mitk::NavigationData::Pointer m_NavigationData{mitk::NavigationData::New()};
};

class MITKLANCETIGT_EXPORT NavigationTree : public itk::Object
{
public:
  mitkClassMacroItkParent(NavigationTree, itk::Object);
  itkFactorylessNewMacro(Self); //New() and CreateAnother()
  //itkCloneMacro(Self); Clone()
  void Init(NavigationNode::Pointer root);
  void AddChild(NavigationNode::Pointer node, NavigationNode::Pointer parent);
  void AddChildren(std::vector<NavigationNode::Pointer> nodes, NavigationNode::Pointer parent);
  void Tranversal(NavigationNode::Pointer root);
  void Tranversal();
  int  GetMaxDepth(NavigationNode::Pointer root, std::vector<NavigationNode::Pointer> nodes);

  NavigationNode::Pointer GetNodeByName(std::string nodeName);
  void PrintPathToRoot(NavigationNode::Pointer node);
  void ComputeNdFromRootToNode(NavigationNode::Pointer node,mitk::NavigationData::Pointer& output);
  mitk::NavigationData::Pointer GetNavigationData(mitk::NavigationData::Pointer input, std::string inputName, std::string outputName);
private:
  NavigationNode::Pointer m_Root{};
  std::map<std::string, NavigationNode::Pointer> m_SearchMap;
};

#endif // LANCETTREECOORDS_H
