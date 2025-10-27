// example_common_header.h
// This file can be included in both production and test code
// It provides the friend declaration block that can be shared across your codebase

#pragma once

// ============================================================================
// Private Member Access Support for Testing
// ============================================================================
// Copy this block from gtest_generator.h to enable GTESTG_FRIEND_ACCESS_PRIVATE()
// in production code without requiring the full testing framework

#ifndef GTEST_GENERATOR_ACCESS_PRIVATE_MEMBER_DEFINED
#define GTEST_GENERATOR_ACCESS_PRIVATE_MEMBER_DEFINED

// VirtualAccessor for class-based friend access (TEST_F_FRIEND, TEST_G_FRIEND)
namespace gtestg_detail {
template <class Suite, class Tag>
class VirtualAccessor;
}

// Template function declaration for function-based friend access
template <typename ID, typename TestCase, typename Target>
auto gtestg_private_accessMember(TestCase* test_case, Target* target = nullptr) -> decltype(auto);

// Unified macro that grants friend access for BOTH approaches:
// - Class-based: VirtualAccessor (for TEST_F_FRIEND, TEST_G_FRIEND)
// - Function-based: gtestg_private_accessMember (for GTESTG_PRIVATE_MEMBER macros)
// Usage: Place inside the target class definition
// In test builds: Grants friend access to test infrastructure
// In production builds: Can be redefined as empty (see below)
#define GTESTG_FRIEND_ACCESS_PRIVATE() \
    template <class, class> friend class ::gtestg_detail::VirtualAccessor; \
    template <typename _ID, typename _TC, typename _TG> \
    friend auto ::gtestg_private_accessMember(_TC*, _TG*) -> decltype(auto)

#endif  // GTEST_GENERATOR_ACCESS_PRIVATE_MEMBER_DEFINED

// ============================================================================
// Production Build Configuration (Optional)
// ============================================================================
// Uncomment the following to disable friend access in production builds
// This makes GTESTG_FRIEND_ACCESS_PRIVATE() an empty macro that compiles to nothing

// #ifdef NDEBUG  // Only in release/production builds
// #undef GTESTG_FRIEND_ACCESS_PRIVATE
// #define GTESTG_FRIEND_ACCESS_PRIVATE()  // Empty macro - no friend access
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
    GTESTG_FRIEND_ACCESS_PRIVATE();

    // Regular public methods...
};

// In your test code:
#include "gtest_generator.h"
#include "example_common_header.h"  // Already included above


// Declare accessors - simplified API
GTESTG_PRIVATE_DECLARE_MEMBER(MyProductionClass, sensitiveData);
GTESTG_PRIVATE_DECLARE_MEMBER(MyProductionClass, internalState);

// Example custom function
GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyProductionClass, GetBoth) {
    return std::to_string(target->sensitiveData) + ":" + target->internalState;
}

// Write tests
TEST_G(MyTest, AccessPrivate) {
    USE_GENERATOR();

    MyProductionClass obj(42, "hidden");

    int& data = GTESTG_PRIVATE_MEMBER(MyProductionClass, sensitiveData, &obj);
    EXPECT_EQ(data, 42);
}
*/
