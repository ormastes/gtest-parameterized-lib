// example_common_header.h
// This file can be included in both production and test code
// It provides the friend declaration block that can be shared across your codebase

#pragma once

// ============================================================================
// Private Member Access Support for Testing
// ============================================================================
// Copy this block from gtest_generator.h to enable FRIEND_ACCESS_PRIVATE()
// in production code without requiring the full testing framework

#ifndef GTEST_GENERATOR_ACCESS_PRIVATE_MEMBER_DEFINED
#define GTEST_GENERATOR_ACCESS_PRIVATE_MEMBER_DEFINED

// Template function declaration
template <typename ID, typename TestCase, typename Target>
auto accessPrivateMember(TestCase* test_case, Target* target = nullptr) -> decltype(auto);

// Macro to make accessPrivateMember a friend of the target class
// Usage: Place inside the target class definition
// In test builds: Grants friend access to test infrastructure
// In production builds: Can be redefined as empty (see below)
#define FRIEND_ACCESS_PRIVATE() \
    template <typename _ID, typename _TC, typename _TG> \
    friend auto accessPrivateMember(_TC*, _TG*) -> decltype(auto)

#endif  // GTEST_GENERATOR_ACCESS_PRIVATE_MEMBER_DEFINED

// ============================================================================
// Production Build Configuration (Optional)
// ============================================================================
// Uncomment the following to disable friend access in production builds
// This makes FRIEND_ACCESS_PRIVATE() an empty macro that compiles to nothing

// #ifdef NDEBUG  // Only in release/production builds
// #undef FRIEND_ACCESS_PRIVATE
// #define FRIEND_ACCESS_PRIVATE()  // Empty macro - no friend access
// #endif

// ============================================================================
// Example Usage
// ============================================================================

/*
// In your production code:
#include "example_common_header.h"

class MyProductionClass {
private:
    int sensitiveData;
    std::string internalState;

public:
    MyProductionClass(int d, const std::string& s)
        : sensitiveData(d), internalState(s) {}

    // Grant friend access for testing
    FRIEND_ACCESS_PRIVATE();

    // Regular public methods...
};

// In your test code:
#include "gtest_generator.h"
#include "example_common_header.h"  // Already included above

using TestBase = gtest_generator::TestWithGenerator;

// Declare accessors
DECLARE_ACCESS_PRIVATE(id1, TestBase, MyProductionClass, sensitiveData);
DECLARE_ACCESS_PRIVATE(id2, TestBase, MyProductionClass, internalState);

// Write tests
TEST_G(MyTest, AccessPrivate) {
    USE_GENERATOR();

    MyProductionClass obj(42, "hidden");

    int& data = ACCESS_PRIVATE(TestBase, TestBase_MyProductionClass_sensitiveData,
                                MyProductionClass, &obj);
    EXPECT_EQ(data, 42);
}
*/
