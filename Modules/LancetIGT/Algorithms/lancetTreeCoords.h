#ifndef LANCETTREECOORDS_H
#define LANCETTREECOORDS_H
#include <vector>

#include "MitkLancetIGTExports.h"
#include "mitkNavigationData.h"
#include "itkObject.h"

class MITKLANCETIGT_EXPORT SceneNode : public itk::Object
{
public:
  mitkClassMacroItkParent(SceneNode, itk::Object);
  itkFactorylessNewMacro(Self); //New() and CreateAnother()

  //itkGetMacro(NodeName, std::string);
  itkSetMacro(NodeName, std::string);

  //itkGetMacro(Children, std::vector<SceneNode::Pointer>);
  //itkSetMacro(Children, std::vector<SceneNode::Pointer>);

  //itkGetMacro(Parent, SceneNode::Pointer);
  itkSetMacro(Parent, SceneNode::Pointer);

  itkSetMacro(NavigationData, mitk::NavigationData::Pointer);
  itkGetMacro(NavigationData, mitk::NavigationData::Pointer);
  std::string m_NodeName{"undefined"};
  std::vector<SceneNode::Pointer> m_Children{};
  SceneNode::Pointer m_Parent{nullptr};
  mitk::NavigationData::Pointer m_NavigationData{mitk::NavigationData::New()};
};

class MITKLANCETIGT_EXPORT NavigationScene : public itk::Object
{
public:
  mitkClassMacroItkParent(NavigationScene, itk::Object);
  itkFactorylessNewMacro(Self); //New() and CreateAnother()
  //itkCloneMacro(Self); Clone()
  void Init(SceneNode::Pointer root);
  void AddChild(SceneNode::Pointer node, SceneNode::Pointer parent);
  void AddChildren(std::vector<SceneNode::Pointer> nodes, SceneNode::Pointer parent);
  void Tranversal(SceneNode::Pointer root);
  void Tranversal();
  int  GetMaxDepth(SceneNode::Pointer root, std::vector<SceneNode::Pointer> nodes);

  SceneNode::Pointer GetNodeByName(std::string nodeName);
  void PrintPathToRoot(SceneNode::Pointer node);
  void ComputeNdFromRootToNode(SceneNode::Pointer node,mitk::NavigationData::Pointer& output);
  mitk::NavigationData::Pointer GetNavigationData(mitk::NavigationData::Pointer input, std::string inputName, std::string outputName);
private:
  SceneNode::Pointer m_Root{};
  std::map<std::string, SceneNode::Pointer> m_SearchMap;
};

#endif // LANCETTREECOORDS_H
