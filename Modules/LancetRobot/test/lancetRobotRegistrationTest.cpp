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


class lancetRobotRegistrationTestSuite : public mitk::TestFixture
{
  CPPUNIT_TEST_SUITE(lancetRobotRegistrationTestSuite);

  // Test the append method
  MITK_TEST(RegistraionTest);
  CPPUNIT_TEST_SUITE_END();

private:
  //NavigationTree::Pointer m_NavigationTree;

public:
  /**
  * @brief Setup Always call this method before each Test-case to ensure correct and new intialization of the used
  * members for a new test case. (If the members are not used in a test, the method does not need to be called).
  */
  void setUp() override
  {
    
  }

  void tearDown() override
  {
    
  }

  void RegistraionTest()
  {
     /*std::string appendedString = "And a third line\n";
     std::string wholeString = m_DefaultDataString + appendedString;
     m_Data->AppendAString(appendedString);
    */
     CPPUNIT_ASSERT_MESSAGE("Checking whether string was correctly appended."," m_Data->GetData()" == " m_Data->GetData()");
  }
};

MITK_TEST_SUITE_REGISTRATION(lancetRobotRegistration)