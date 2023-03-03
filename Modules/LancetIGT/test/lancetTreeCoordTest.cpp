/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

// Testing
#include "mitkTestFixture.h"
#include "mitkTestingMacros.h"

// MITK includes
#include "lancetTreeCoords.h"

class lancetTreeCoordTestSuite : public mitk::TestFixture
{
  CPPUNIT_TEST_SUITE(lancetTreeCoordTestSuite);

    // Test the append method
    MITK_TEST(ComputeNdFromRootToNode_RootNode_Identity);
    MITK_TEST(ComputeNdFromRootToNode_SecondLayerChildNode_SameAsNavigationDataOfChildNode);
    MITK_TEST(ComputeNdFromRootToNode_ThirdLayerChildNode_SameAsNavigationDataComp);
    MITK_TEST(GetNavigationData_SameBranch);
    MITK_TEST(GetNavigationData_DiffBranch);
    MITK_TEST(GetNavigationData_FromRootToChild);
  CPPUNIT_TEST_SUITE_END();

private:
  //     A
  //    / \
  //   B   C
  //        \
  //         D
  NavigationTree::Pointer m_NavigationTree;
  NavigationNode::Pointer m_NodeA;
  NavigationNode::Pointer m_NodeB;
  NavigationNode::Pointer m_NodeC;
  NavigationNode::Pointer m_NodeD;
  mitk::NavigationData::Pointer m_ndA;
  mitk::NavigationData::Pointer m_ndB;
  mitk::NavigationData::Pointer m_ndC;
  mitk::NavigationData::Pointer m_ndD;
  mitk::NavigationData::Pointer m_ndInput;
public:
  /**
  * @brief Setup Always call this method before each Test-case to ensure correct and new intialization of the used
  * members for a new test case. (If the members are not used in a test, the method does not need to be called).
  */
  void setUp() override
  {
    //hard coded navigation data

    double axisx[3]{1, 0, 0};
    double axisy[3]{0, 1, 0};
    double axisz[3]{0, 0, 1};

    double trans1[3]{0, 0, 50};
    double trans2[3]{0, 50, 0};
    double trans3[3]{50, 0, 0};
    double trans4[3]{0, 0, -50};

    mitk::AffineTransform3D::Pointer tmp = mitk::AffineTransform3D::New();
    tmp->Rotate3D(axisz, 0.174);
    tmp->Translate(trans1);
    m_ndA = mitk::NavigationData::New(tmp);

    tmp->Rotate3D(axisx, -0.174);
    tmp->Translate(trans2);
    m_ndB = mitk::NavigationData::New(tmp);

    tmp->Rotate3D(axisy, -0.174);
    tmp->Translate(trans3);
    m_ndC = mitk::NavigationData::New(tmp);

    tmp->Rotate3D(axisz, -0.174);
    tmp->Translate(trans4);
    m_ndD = mitk::NavigationData::New(tmp);

    tmp->Rotate3D(axisy, 0.174);
    tmp->Translate(trans1);
    m_ndInput = mitk::NavigationData::New(tmp);

    //Create the Navigation Tree
    //     A
    //    / \
    //   B   C
    //        \
    //         D
    m_NavigationTree = NavigationTree::New();

    m_NodeA = NavigationNode::New();
    m_NodeA->SetNodeName("A");
    m_NodeA->SetNavigationData(m_ndA);

    m_NavigationTree->Init(m_NodeA);

    m_NodeB = NavigationNode::New();
    m_NodeB->SetNodeName("B");
    m_NodeB->SetNavigationData(m_ndB);
    m_NavigationTree->AddChild(m_NodeB, m_NodeA);

    m_NodeC = NavigationNode::New();
    m_NodeC->SetNodeName("C");
    m_NodeC->SetNavigationData(m_ndC);
    m_NavigationTree->AddChild(m_NodeC, m_NodeA);

    m_NodeD = NavigationNode::New();
    m_NodeD->SetNodeName("D");
    m_NodeD->SetNavigationData(m_ndD);
    m_NavigationTree->AddChild(m_NodeD, m_NodeC);
  }

  void tearDown() override
  {
    m_NavigationTree = nullptr;
  }

  void ComputeNdFromRootToNode_RootNode_Identity()
  {
    MITK_TEST_OUTPUT(<< "---- Testing method ComputeNdFromRootToNode() ----");
    //A
    //Use Tree
    mitk::NavigationData::Pointer res = mitk::NavigationData::New();
    m_NavigationTree->ComputeNdFromRootToNode(m_NodeA, res);
    //by hand should be I
    mitk::NavigationData::Pointer nd_I = mitk::NavigationData::New();

    CPPUNIT_ASSERT_MESSAGE("Orientation not equal",
                           mitk::Equal(res->GetOrientation(), nd_I->
                             GetOrientation())
    );
    CPPUNIT_ASSERT_MESSAGE("Position not equal",
                           mitk::Equal(res->GetPosition(), nd_I->
                             GetPosition()));
  }

  void ComputeNdFromRootToNode_SecondLayerChildNode_SameAsNavigationDataOfChildNode()
  {
    MITK_TEST_OUTPUT(<< "---- Testing method ComputeNdFromRootToNode() ----");
    //A->B
    //Use Tree
    mitk::NavigationData::Pointer res = mitk::NavigationData::New();
    m_NavigationTree->ComputeNdFromRootToNode(m_NodeB, res);
    //by hand m_ndB
    CPPUNIT_ASSERT_MESSAGE("Orientation not equal", mitk::Equal(res->GetOrientation(), m_ndB->GetOrientation()));
    CPPUNIT_ASSERT_MESSAGE("Position not equal", mitk::Equal(res->GetPosition(), m_ndB->GetPosition()));

    MITK_TEST_OUTPUT(<< res);
    MITK_TEST_OUTPUT(<< m_ndB);
  }

  void ComputeNdFromRootToNode_ThirdLayerChildNode_SameAsNavigationDataComp()
  {
    MITK_TEST_OUTPUT(<< "---- Testing method ComputeNdFromRootToNode() ----");
    //A->C->D
    //Use Tree
    mitk::NavigationData::Pointer res = mitk::NavigationData::New();
    m_NavigationTree->ComputeNdFromRootToNode(m_NodeD, res);
    //by hand m_ndD compose m_ndC
    mitk::NavigationData::Pointer correct = m_ndD->Clone();
    correct->Compose(m_ndC);
    CPPUNIT_ASSERT_MESSAGE("Orientation not equal", mitk::Equal(res->GetOrientation(), correct->GetOrientation()));
    CPPUNIT_ASSERT_MESSAGE("Position not equal", mitk::Equal(res->GetPosition(), correct->GetPosition()));

    MITK_TEST_OUTPUT(<< res);
    MITK_TEST_OUTPUT(<< correct);
  }

  void GetNavigationData_SameBranch()
  {
    MITK_TEST_OUTPUT(<< "---- Testing method GetNavigationData() ----");
    //Td2e = GetNavigationData(Tc2e,"C","D")
    //Use Tree
    mitk::NavigationData::Pointer res = mitk::NavigationData::New();
    res = m_NavigationTree->GetNavigationData(m_ndInput, "C","D");
    //by hand m_ndD compose m_ndC
    mitk::NavigationData::Pointer correct = m_ndInput->Clone();
    correct->Compose(m_ndD->GetInverse());
    CPPUNIT_ASSERT_MESSAGE("Orientation not equal", mitk::Equal(res->GetOrientation(), correct->GetOrientation()));
    CPPUNIT_ASSERT_MESSAGE("Position not equal", mitk::Equal(res->GetPosition(), correct->GetPosition()));

    MITK_TEST_OUTPUT(<< res);
    MITK_TEST_OUTPUT(<< correct);
  }
  void GetNavigationData_DiffBranch()
  {
    MITK_TEST_OUTPUT(<< "---- Testing method GetNavigationData() ----");
    //Td2e = GetNavigationData(Tb2e,"B","D")
    //Use Tree
    mitk::NavigationData::Pointer res = mitk::NavigationData::New();
    res = m_NavigationTree->GetNavigationData(m_ndInput, "B", "D");
    //by hand:
    //Td2e = Td2c*Tc2a*Ta2b*Tb2e
    //  = Tc2d^-1 * Ta2c^-1 *Ta2b*Tb2e
    //  = m_ndD^-1 * m_ndC^-1 *m_ndB*m_ndInput
    mitk::NavigationData::Pointer correct = m_ndInput->Clone();
    correct->Compose(m_ndB);
    correct->Compose(m_ndC->GetInverse());
    correct->Compose(m_ndD->GetInverse());
    CPPUNIT_ASSERT_MESSAGE("Orientation not equal", mitk::Equal(res->GetOrientation(), correct->GetOrientation()));
    CPPUNIT_ASSERT_MESSAGE("Position not equal", mitk::Equal(res->GetPosition(), correct->GetPosition()));

    MITK_TEST_OUTPUT(<< res);
    MITK_TEST_OUTPUT(<< correct);
  }

  void GetNavigationData_FromRootToChild()
  {
    MITK_TEST_OUTPUT(<< "---- Testing method GetNavigationData() ----");
    //Td2e = GetNavigationData(Ta2e,"A","D")
    //Use Tree
    mitk::NavigationData::Pointer res = m_NavigationTree->GetNavigationData(m_ndInput, "A", "D");
    //by hand:
    //Td2e = Td2c*Tc2a*Ta2e
    //  = Tc2d^-1 * Ta2c^-1 *Ta2e
    //  = m_ndD^-1 * m_ndC^-1 *m_ndInput
    mitk::NavigationData::Pointer correct = m_ndInput->Clone();
    correct->Compose(m_ndC->GetInverse());
    correct->Compose(m_ndD->GetInverse());
    CPPUNIT_ASSERT_MESSAGE("Orientation not equal", mitk::Equal(res->GetOrientation(), correct->GetOrientation()));
    CPPUNIT_ASSERT_MESSAGE("Position not equal", mitk::Equal(res->GetPosition(), correct->GetPosition()));

    MITK_TEST_OUTPUT(<< res);
    MITK_TEST_OUTPUT(<< correct);
  }
};

MITK_TEST_SUITE_REGISTRATION(lancetTreeCoord)
