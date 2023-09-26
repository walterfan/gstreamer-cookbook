#include <gtest/gtest.h>

using namespace std;
using namespace testing;
//using ::testing::AtLeast;

int main(int argc, char** argv) {
    printf("--- unit test ---\n");
    testing::GTEST_FLAG(output)="xml:unit_test_result.xml";
    testing::InitGoogleTest(&argc, argv);
    //InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
