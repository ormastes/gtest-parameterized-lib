#include "gtest_generator.h"

class SimpleTest : public gtest_generator::TestWithGenerator {};

TEST_G(SimpleTest, TwoValues) {
    auto a = GENERATOR(1, 2);
    auto b = GENERATOR(10, 20);
    USE_GENERATOR(ALIGNED);
    
    printf("GetParam=%d: a=%d (expected %d), b=%d (expected %d)\n", 
           GetParam(), a, GetParam()==0?1:2, b, GetParam()==0?10:20);
    
    if (GetParam() == 0) {
        EXPECT_EQ(a, 1);
        EXPECT_EQ(b, 10);
    } else if (GetParam() == 1) {
        EXPECT_EQ(a, 2);
        EXPECT_EQ(b, 20);
    }
}
