#include "gtest/gtest.h"
#include "time.h"
#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace testing;

class GObjectTest: public testing::Test
{
public:
    GObjectTest()
    {
    }

    virtual ~GObjectTest() { }

    virtual void SetUp() {
        std::string strClassName(::testing::UnitTest::GetInstance()->current_test_info()->test_case_name());
        std::string strFuncName(::testing::UnitTest::GetInstance()->current_test_info()->name());
        m_strTestName = strClassName + "." + strFuncName;

        

    }
    virtual void TearDown()  {

        

    }


protected:
    std::string m_strTestName;
};


TEST_F(GObjectTest, process)
{
    std::cout << "photo shooting..." << std::endl;

}

