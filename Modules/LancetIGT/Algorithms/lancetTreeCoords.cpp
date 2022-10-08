#include  "lancetTreeCoords.h"

void NavigationTree::Init(NavigationNode::Pointer root)
{
  this->m_Root = root;
  m_SearchMap.insert_or_assign(root->m_NodeName, root);
}

void NavigationTree::AddChild(NavigationNode::Pointer node, NavigationNode::Pointer parent) {
  parent->m_Children.push_back(node);
  node->m_Parent = parent;
  m_SearchMap.insert_or_assign(node->m_NodeName, node);
}

void NavigationTree::AddChildren(std::vector<NavigationNode::Pointer> nodes, NavigationNode::Pointer parent) {
  for (int i = 0; i < nodes.size(); ++i) {
    AddChild(nodes[i], parent);
  }
}

void NavigationTree::Tranversal() {
  this->Tranversal(this->m_Root);
}

void NavigationTree::Tranversal(NavigationNode::Pointer root) {
  std::vector<NavigationNode::Pointer> nodes = root->m_Children;
  for (int i = 0; i < nodes.size(); ++i) {
    if (nodes[i]->m_Children.size() > 0)
      Tranversal(nodes[i]);
    else
      std::cout << nodes[i]->m_NodeName << ",";
  }
  std::cout << root->m_NodeName << ",";
}

int NavigationTree::GetMaxDepth(NavigationNode::Pointer root, std::vector<NavigationNode::Pointer> nodes) {
  auto iResult = 0;

  return iResult;
}

NavigationNode::Pointer NavigationTree::GetNodeByName(std::string nodeName)
{
  auto item = m_SearchMap.find(nodeName);
  if (item != m_SearchMap.end()) {
    return item->second;
  }
  else {
    return nullptr;
  }
}

void NavigationTree::PrintPathToRoot(NavigationNode::Pointer node)
{
  if (node.IsNull())
  {
    return;
  }
  if (node->m_Parent.IsNotNull())
  {
    std::cout << node->m_NodeName << " , ";
    PrintPathToRoot(node->m_Parent);
  }
  else
  {
    std::cout << node->m_NodeName << std::endl;
  }
}

void NavigationTree::ComputeNdFromRootToNode(NavigationNode::Pointer node, mitk::NavigationData::Pointer& output)
{
  if (node.IsNull())
  {
    return;
  }
  if (node->m_Parent.IsNotNull())
  {
    MITK_DEBUG << node->m_NodeName << " ->";
    output->Compose(node->m_NavigationData);
    ComputeNdFromRootToNode(node->m_Parent,output);
  }
  else
  {
    MITK_DEBUG << node->m_NodeName << std::endl;
    //output->Compose(node->m_NavigationData);
  }
}

mitk::NavigationData::Pointer NavigationTree::GetNavigationData(mitk::NavigationData::Pointer input,
                                                                std::string inputName, std::string outputName)
{
  mitk::NavigationData::Pointer rootToInput = mitk::NavigationData::New();
  ComputeNdFromRootToNode(GetNodeByName(inputName), rootToInput);

  mitk::NavigationData::Pointer rootToOutput = mitk::NavigationData::New();
  ComputeNdFromRootToNode(GetNodeByName(outputName), rootToOutput);

  mitk::NavigationData::Pointer res = input->Clone();
  res->Compose(rootToInput);
  res->Compose(rootToOutput->GetInverse());

  return res;
}


