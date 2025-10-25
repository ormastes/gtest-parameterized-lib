#pragma once
#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include <type_traits>

// ============================================================================
// Private Member Access for Testing
// ============================================================================
#ifndef GTESTG_FRIEND_INFRA_INCLUDED
#define GTESTG_FRIEND_INFRA_INCLUDED

// VirtualAccessor template (universal, works with TEST_FRIEND/TEST_G_FRIEND)
namespace gtestg_detail {
template <typename Suite, typename Tag>
struct VirtualAccessor : public Suite {};
}

// Token concat helper
#ifndef GTESTG_CONCAT
#  define GTESTG_CONCAT_INNER(a, b) a##b
#  define GTESTG_CONCAT(a, b) GTESTG_CONCAT_INNER(a, b)
#endif


// TEST_F style: friend-aware test that derives from a VirtualAccessor.
// IMPORTANT: Must be used at root namespace (not inside any namespace).
// If you need tests in a namespace, use TEST_F with GTESTG_PRIVATE_MEMBER macros instead.
#define TEST_FRIEND(Suite, TestName)                                                      \
    namespace gtestg_detail { \
    struct TestName{};                                                                \
    template<> struct VirtualAccessor<Suite, TestName>                               \
        : public Suite {                \
     public:                                                                              \
      VirtualAccessor<Suite, TestName>  () {}                                \
      void TestBody() override;                                                           \
                                                                                          \
     private:                                                                             \
      static ::testing::TestInfo* const test_info_ GTEST_ATTRIBUTE_UNUSED_;               \
      VirtualAccessor<Suite, TestName>  (                                    \
          VirtualAccessor<Suite, TestName>   const &) = delete;              \
      VirtualAccessor<Suite, TestName>   &operator=(                         \
          VirtualAccessor<Suite, TestName>   const &) = delete;              \
    };                                                                                    \
    ::testing::TestInfo* const VirtualAccessor<Suite, TestName> ::          \
        test_info_ =                                                                      \
        ::testing::internal::MakeAndRegisterTestInfo(                                     \
            #Suite,                                                                       \
            #TestName,                                                                    \
            nullptr, nullptr,                                                             \
            ::testing::internal::CodeLocation(__FILE__, __LINE__),                        \
            (::testing::internal::GetTypeId<                                              \
                ::gtestg_detail::VirtualAccessor<Suite, TestName>>()),              \
            ::testing::internal::SuiteApiResolver<                                        \
                ::gtestg_detail::VirtualAccessor<Suite, TestName>>::                \
                GetSetUpCaseOrSuite(__FILE__, __LINE__),                                  \
            ::testing::internal::SuiteApiResolver<                                        \
                ::gtestg_detail::VirtualAccessor<Suite, TestName>>::                \
                GetTearDownCaseOrSuite(__FILE__, __LINE__),                               \
            new ::testing::internal::TestFactoryImpl<                                     \
                ::gtestg_detail::VirtualAccessor<Suite, TestName>>);                        \
            } \
    void gtestg_detail::VirtualAccessor<Suite, ::gtestg_detail::TestName>::TestBody()

// TEST_G_FRIEND(Suite, TestName) for generator/parameterized tests.
// IMPORTANT: Must be used at root namespace (not inside any namespace).
// If you need tests in a namespace, use TEST_G with GTESTG_PRIVATE_MEMBER macros instead.
#define TEST_G_FRIEND(Suite, TestName)                                                    \
    namespace gtestg_detail { \
    struct TestName{};                                                                \
    template<> struct VirtualAccessor<Suite, TestName>                               \
        : public Suite {                \
     public:                                                                              \
      VirtualAccessor<Suite, TestName>() {}                                \
      void TestBody() override;                                                           \
                                                                                          \
     private:                                                                             \
      static int AddToRegistry() {                                                        \
        ::testing::UnitTest::GetInstance()                                                \
            ->parameterized_test_registry()                                               \
            .GetTestSuitePatternHolder<                                                   \
                ::gtestg_detail::VirtualAccessor<Suite, TestName>>(                 \
                #Suite,                                                                   \
                ::testing::internal::CodeLocation(__FILE__, __LINE__))                    \
            ->AddTestPattern(                                                             \
                #Suite,                                                                   \
                #TestName,                                                                \
                new ::testing::internal::TestMetaFactory<                                 \
                    ::gtestg_detail::VirtualAccessor<Suite, TestName>>(),                   \
                ::testing::internal::CodeLocation(__FILE__, __LINE__));                   \
        return 0;                                                                         \
      }                                                                                   \
      static int gtest_registering_dummy_ GTEST_ATTRIBUTE_UNUSED_;                        \
      VirtualAccessor<Suite, TestName>(                                    \
          VirtualAccessor<Suite, TestName> const &) = delete;              \
      VirtualAccessor<Suite, TestName> &operator=(                         \
          VirtualAccessor<Suite, TestName> const &) = delete;              \
    };                                                                                    \
    int VirtualAccessor<Suite, TestName>::gtest_registering_dummy_ =       \
        VirtualAccessor<Suite, TestName>::AddToRegistry();                 \
    } \
    void gtestg_detail::VirtualAccessor<Suite, ::gtestg_detail::TestName>::TestBody()


// Template function declaration (prevent multiple definition with #ifdef)
// This block can be copied to a common header file for sharing across production and test code
#ifndef GTEST_GENERATOR_ACCESS_PRIVATE_MEMBER_DEFINED
#define GTEST_GENERATOR_ACCESS_PRIVATE_MEMBER_DEFINED

template <typename ID, typename TestCase, typename Target>
auto gtestg_private_accessMember(TestCase* test_case, Target* target = nullptr) -> decltype(auto);


// Unified macro that grants friend access for ALL approaches:
// - VirtualAccessor template (for TEST_FRIEND, TEST_G_FRIEND)
// - Function-based (gtestg_private_accessMember for GTESTG_PRIVATE_MEMBER macros)
#define GTESTG_FRIEND_ACCESS_PRIVATE() \
  template <typename, typename> friend struct ::gtestg_detail::VirtualAccessor; \
  template <typename _ID, typename _TC, typename _TG> \
  friend auto ::gtestg_private_accessMember(_TC*, _TG*) -> decltype(auto)


#endif  // GTEST_GENERATOR_ACCESS_PRIVATE_MEMBER_DEFINED

// Empty macro for production builds - define this in your production headers
// to disable friend access when not testing
#ifndef GTESTG_FRIEND_ACCESS_PRIVATE
#define GTESTG_FRIEND_ACCESS_PRIVATE()
#endif

// Helper macro to concatenate tokens for ID generation
#define GTESTG_PRIVATE_CONCAT_IMPL(a, b) a##_##b
#define GTESTG_PRIVATE_CONCAT(a, b) GTESTG_PRIVATE_CONCAT_IMPL(a, b)

// Dummy test case type for simplified member access
struct gtestg_private_dummy_test;

// Macro to declare access to instance members
// Usage: GTESTG_PRIVATE_DECLARE_MEMBER(Target, MemberName)
// Example: GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateValue)
#define GTESTG_PRIVATE_DECLARE_MEMBER(Target, Member) \
    struct GTESTG_PRIVATE_CONCAT(Target, Member); \
    template <> \
    inline auto gtestg_private_accessMember<GTESTG_PRIVATE_CONCAT(Target, Member), gtestg_private_dummy_test, Target>(gtestg_private_dummy_test* THIS, Target* TARGET) -> decltype(auto) { \
        return (TARGET->Member); \
    }

// Macro to declare access to static members
// Usage: GTESTG_PRIVATE_DECLARE_STATIC(Target, MemberName)
// Example: GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter)
#define GTESTG_PRIVATE_DECLARE_STATIC(Target, Member) \
    struct GTESTG_PRIVATE_CONCAT(Target, Member); \
    template <> \
    inline auto gtestg_private_accessMember<GTESTG_PRIVATE_CONCAT(Target, Member), gtestg_private_dummy_test, Target>(gtestg_private_dummy_test* THIS, Target* TARGET) -> decltype(auto) { \
        return (Target::Member); \
    }

// Macro to declare a custom function for accessing private members
// Usage: GTESTG_PRIVATE_DECLARE_FUNCTION(ThisClass, Target, FunctionName) { return custom_expression; }
// Example: GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum) { return TARGET->field1 + TARGET->field2; }
// ThisClass allows access to test context (e.g., THIS->GetParam())
#define GTESTG_PRIVATE_DECLARE_FUNCTION(ThisClass, Target, FuncName) \
    struct GTESTG_PRIVATE_CONCAT(Target, FuncName); \
    template <> \
    inline auto gtestg_private_accessMember<GTESTG_PRIVATE_CONCAT(Target, FuncName), ThisClass, Target>(ThisClass* THIS, Target* TARGET) -> decltype(auto)

// Macro for accessing instance members
// Usage: GTESTG_PRIVATE_MEMBER(Target, MemberName, &obj)
#define GTESTG_PRIVATE_MEMBER(Target, Member, obj) \
    gtestg_private_accessMember<GTESTG_PRIVATE_CONCAT(Target, Member), gtestg_private_dummy_test, Target>(nullptr, obj)

// Macro for accessing static members
// Usage: GTESTG_PRIVATE_STATIC(Target, MemberName)
#define GTESTG_PRIVATE_STATIC(Target, Member) \
    gtestg_private_accessMember<GTESTG_PRIVATE_CONCAT(Target, Member), gtestg_private_dummy_test, Target>(nullptr, nullptr)

// Macro for calling custom functions with an explicit test case object
// Usage: GTESTG_PRIVATE_CALL(Target, FunctionName, test_obj, &obj)
// The TestCase template parameter is inferred from test_obj's type
#define GTESTG_PRIVATE_CALL(Target, FuncName, test_obj, ...) \
    gtestg_private_accessMember<GTESTG_PRIVATE_CONCAT(Target, FuncName), typename std::decay<decltype(*test_obj)>::type, Target>(test_obj, ##__VA_ARGS__)

// Macro for calling custom functions from within a test (uses 'this')
// Usage: GTESTG_PRIVATE_CALL_ON_TEST(ThisClass, Target, FunctionName, &obj)
// ThisClass should match the type used in GTESTG_PRIVATE_DECLARE_FUNCTION
#define GTESTG_PRIVATE_CALL_ON_TEST(ThisClass, Target, FuncName, ...) \
    gtestg_private_accessMember<GTESTG_PRIVATE_CONCAT(Target, FuncName), ThisClass, Target>(static_cast<ThisClass*>(this), ##__VA_ARGS__)



#endif // GTESTG_FRIEND_INFRA_INCLUDED
// ============================================================================

namespace gtest_generator {
static constexpr bool GTEST_GENERATOR_LOG = false;
class TestWithGenerator : public ::testing::TestWithParam<int> {
    public:
    virtual void TestBody() = 0;
};

// Forward declaration
class DynamicRangeGenerator;

// Global registry mapping "SuiteName.TestName" -> generator pointer
inline std::map<std::string, DynamicRangeGenerator*> g_range_map;

// Thread-local static variable for counting state
inline thread_local bool on_counting = false;
inline thread_local int current_count = 1;

// --- Mode toggle (default FULL = Cartesian) ---
enum class SamplingMode { FULL, ALIGNED };
inline thread_local SamplingMode tl_mode = SamplingMode::FULL;

// Column sizes discovered during counting; cursor during run
// Use inline functions to avoid multiple definition errors with thread_local on Windows/MinGW
inline std::vector<int>& get_tl_col_sizes() {
    static thread_local std::vector<int> instance;
    return instance;
}
inline int& get_tl_col_ix() {
    static thread_local int instance = 0;
    return instance;
}
inline int& get_tl_last_param() {
    static thread_local int instance = -1;
    return instance;
}
inline std::string& get_tl_last_test_key() {
    static thread_local std::string instance;
    return instance;
}

// References for backward compatibility
#define tl_col_sizes get_tl_col_sizes()
#define tl_col_ix get_tl_col_ix()
#define tl_last_param get_tl_last_param()
#define tl_last_test_key get_tl_last_test_key()

// Persist per-test column sizes so run phase can access them
inline std::map<std::string, std::vector<int>> g_colsizes_map;

// Track which tests use ALIGNED mode (set during first USE_GENERATOR call)
inline std::map<std::string, SamplingMode> g_test_modes;

// Store both full count and aligned max size for each test
inline std::map<std::string, std::pair<int, int>> g_test_counts;  // {full_count, aligned_max}

// Custom generator implementing GTest’s ParamGeneratorInterface<int>
class DynamicRangeGenerator : public testing::internal::ParamGeneratorInterface<int> {
 public:
  const std::string key;
  mutable int start = 0;
  mutable int end = 1;
  TestWithGenerator* test_instance;

  explicit DynamicRangeGenerator(const std::string& k, TestWithGenerator* test_case)
      : key(k), test_instance(test_case) {
    g_range_map[key] = this;
    if constexpr (GTEST_GENERATOR_LOG) printf("DynamicRangeGenerator created for %s\n", key.c_str());
    if (test_instance) {
        // First pass with FULL mode to get max count
        on_counting = true;
        current_count = 1;
        tl_col_sizes.clear();
        tl_col_ix = 0;
        tl_mode = SamplingMode::FULL;
        test_instance->TestBody();
        int full_count = current_count;
        SamplingMode detected_mode = tl_mode;  // Capture the mode set by USE_GENERATOR()

        // Second pass with ALIGNED mode to get column sizes
        on_counting = true;
        current_count = 1;
        tl_col_sizes.clear();
        tl_col_ix = 0;
        tl_mode = SamplingMode::ALIGNED;
        test_instance->TestBody();
        on_counting = false;

        // Calculate aligned max size
        int aligned_max = 0;
        for (int s : tl_col_sizes) aligned_max = std::max(aligned_max, s);

        // Store both counts and column sizes
        g_colsizes_map[key] = tl_col_sizes;
        g_test_counts[key] = {full_count, aligned_max};

        // Store the detected mode so it's known at instantiation time
        g_test_modes[key] = detected_mode;

        // Set initial end value based on detected mode
        // This ensures Google Test instantiates the correct number of test cases
        if (detected_mode == SamplingMode::ALIGNED) {
            end = aligned_max;  // ALIGNED mode: only create aligned_max test cases
        } else {
            end = full_count;   // FULL mode: create all test cases
        }
        tl_col_ix = 0;
    }
  }


  // Begin iterator: binds to 'start'
  testing::internal::ParamIteratorInterface<int>* Begin() const override {
    if constexpr (GTEST_GENERATOR_LOG) printf("Creating begin iterator for %s\n", key.c_str());
    return new DynIterator(start, this, /*at_end=*/false);
  }

  // End iterator: binds to 'end' and marks the end
  testing::internal::ParamIteratorInterface<int>* End() const override {
    return new DynIterator(end, this, /*at_end=*/true);
  }

 private:
  // Iterator for parameter generation (used for both begin and end iterators)
  class DynIterator : public testing::internal::ParamIteratorInterface<int> {
   public:
    DynIterator(int& value, const DynamicRangeGenerator* generator, bool at_end)
        : _value(value), _generator(generator), _done(at_end) {
        if constexpr (GTEST_GENERATOR_LOG) printf("DynIterator constructor: value=%d, at_end=%d, addr=%p\n", value, at_end, this);
    }
    
    ~DynIterator() {
        if constexpr (GTEST_GENERATOR_LOG) printf("DynIterator destructor: addr=%p\n", this);
    }

    void Advance() override {
        if constexpr (GTEST_GENERATOR_LOG) printf("Advancing iterator for %s\n", _generator->key.c_str());
        assert(!_done && "Advance() called on already end iterator");
        _value++;
    }

    testing::internal::ParamIteratorInterface<int>* Clone() const override {
        if constexpr (GTEST_GENERATOR_LOG) printf("Cloning iterator for %s\n", _generator->key.c_str());
        return new DynIterator(_value, _generator, _done);
    }

    const testing::internal::ParamGeneratorInterface<int>* BaseGenerator() const override {
        if constexpr (GTEST_GENERATOR_LOG) printf("Base generator for %s\n", _generator->key.c_str());
        return _generator;
    }

    const int* Current() const override {
        if constexpr (GTEST_GENERATOR_LOG) printf("Current value for %s: %d\n", _generator->key.c_str(), _value);
        return &_value;
    }

    bool Equals(const testing::internal::ParamIteratorInterface<int>& other) const override {
        if constexpr (GTEST_GENERATOR_LOG) printf("Comparing iterators for %s\n", _generator->key.c_str());
        if constexpr (GTEST_GENERATOR_LOG) printf("Current value: %d, other value: %d, done: %d\n", _value, other.Current() ? *other.Current() : -1, _done);
    
        if (auto o = dynamic_cast<const DynIterator*>(&other)) {
            if (_value == o->_value) {
                if constexpr (GTEST_GENERATOR_LOG) printf("Iterators are equal for %s\n", _generator->key.c_str());
                return true;
            }
        }
        if constexpr (GTEST_GENERATOR_LOG) printf("Iterators are not equal for %s\n", _generator->key.c_str());
        return false;
    }

   private:
    int& _value;
    bool _done;
    const DynamicRangeGenerator* _generator;
  };
};

// Utility: return reference to the 'end' variable for the current test instance
inline bool IsCountingMode(const ::testing::Test&) {
    /*printf("Getting dynamic range end for current test\n");
    static int dummy_end = 0; // Dummy variable to avoid dangling reference
    const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
    std::string key = std::string(info->test_suite_name()) + "." + info->name();
    printf("Key for dynamic range end: %s\n", key.c_str());
    // check if key contains multiple '/'. count '/' in key
    int key_slash_count = std::count(key.begin(), key.end(), '/');
    printf("Key slash count: %d\n", key_slash_count);
    assert(key_slash_count >= 2 && "Key should not contain multiple '/'");
    // split key by '/' and get second part. 
    size_t pos = key.find('/');
    key = key.substr(pos + 1);
    pos = key.find('/');
    key = key.substr(0, pos);
    for (const auto& pair : g_range_map) {
        printf("Checking generator for %s\n", pair.first.c_str());
    }*/
    return on_counting;
}
// Helper function to create unique ID from file and line
constexpr size_t hash_string(const char* str, size_t hash = 5381) {
    return (*str == 0) ? hash : hash_string(str + 1, ((hash << 5) + hash) + *str);
}

constexpr size_t make_unique_id(const char* file, int line) {
    return hash_string(file) ^ static_cast<size_t>(line);
}

template <size_t UniqueId, typename T>
inline const T& GetGeneratorValue(std::initializer_list<T> values, ::gtest_generator::TestWithGenerator* test_instance) {
    static int current_devider = 1; // Static variable to hold the current devider
    
    if (on_counting) {
        // Record this column's size in declaration order
        tl_col_sizes.push_back((int)values.size());

        if (tl_mode == SamplingMode::FULL) {
            current_devider = current_count;
            current_count *= values.size(); // Cartesian only
            return *values.begin(); // Return first value for FULL mode counting
        } else {
            // ALIGNED mode counting: return second value if available
            // This ensures the test body runs with different values during counting
            if (values.size() > 1) {
                return *(values.begin() + 1); // Return second value
            }
            return *values.begin(); // Only one value available
        }
    }
    
    // RUN PHASE - check what mode this test uses
    const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
    SamplingMode mode = SamplingMode::FULL;
    std::string key_for_mode;
    if (info) {
        // Parse GTest name format: "Generator/SimpleTest__TwoValues.__/0"
        // Extract to simple format: "SimpleTest.TwoValues"
        std::string gtest_suite = info->test_suite_name();
        std::string simple_key;
        size_t slash_pos = gtest_suite.find('/');
        if (slash_pos != std::string::npos) {
            std::string suite_part = gtest_suite.substr(slash_pos + 1);
            size_t dunder_pos = suite_part.find("__");
            if (dunder_pos != std::string::npos) {
                std::string test_class = suite_part.substr(0, dunder_pos);
                std::string test_name = suite_part.substr(dunder_pos + 2);
                simple_key = test_class + "." + test_name;
            }
        }
        if (simple_key.empty()) simple_key = std::string(info->test_suite_name()) + "." + info->name();

        key_for_mode = simple_key;
        auto mode_it = g_test_modes.find(key_for_mode);
        if (mode_it != g_test_modes.end()) {
            mode = mode_it->second;
        }
    }

    if (mode == SamplingMode::FULL) {
        int paramIndex = test_instance ? test_instance->GetParam() : 0;
        return *(values.begin() + ((paramIndex / current_devider) % values.size()));
    } else {
        // ALIGNED: keep column order; round-robin each column's values
        std::vector<int>* col_sizes = nullptr;
        std::string current_test_key = key_for_mode;  // Use the same parsed key
        if (!current_test_key.empty()) {
            auto it = g_colsizes_map.find(current_test_key);
            if (it != g_colsizes_map.end()) {
                col_sizes = &(it->second);
            }
        }

        // Check if we should skip this test iteration
        int r = test_instance ? test_instance->GetParam() : 0;

        // Reset column index when we start a new test OR new parameter
        if (current_test_key != tl_last_test_key || r != tl_last_param) {
            tl_col_ix = 0;
            tl_last_param = r;
            tl_last_test_key = current_test_key;
        }

        if (col_sizes && !col_sizes->empty()) {
            int max_size = 0;
            for (int s : *col_sizes) max_size = std::max(max_size, s);
            if (r >= max_size) {
                // This iteration should be skipped for ALIGNED mode
                // Return first value as dummy
                return *values.begin();
            }

            int col = tl_col_ix++;
            if (tl_col_ix >= (int)col_sizes->size()) tl_col_ix = 0;

            int s_i = (*col_sizes)[col];
            int idx = (s_i == 0) ? 0 : (r % s_i);
            return *(values.begin() + idx);
        }

        // Fallback
        int paramIndex = test_instance ? test_instance->GetParam() : 0;
        return *(values.begin() + (paramIndex % values.size()));
    }
}

// Helper template for lazy initialization
template<typename TestClass>
inline DynamicRangeGenerator* CreateGenerator(const std::string& name) {
    static DynamicRangeGenerator* generator = 
        new DynamicRangeGenerator(name, new TestClass());
    return generator;
}

}  // namespace gtest_generator

// Macros
// USE_GENERATOR with optional mode parameter (defaults to FULL for backward compatibility)
#define USE_GENERATOR(...) \
  do { \
    ::gtest_generator::tl_mode = ::gtest_generator::SamplingMode::FULL; \
    __VA_OPT__(::gtest_generator::tl_mode = ::gtest_generator::SamplingMode::__VA_ARGS__;) \
    const auto* info = ::testing::UnitTest::GetInstance()->current_test_info(); \
    if (info) { \
      /* Parse GTest name format: "Generator/AlignedModeTest__RunCountAligned.__/0" */ \
      /* Extract to simple format: "AlignedModeTest.RunCountAligned" */ \
      std::string gtest_suite = info->test_suite_name(); /* "Generator/AlignedModeTest__RunCountAligned" */ \
      std::string simple_key; \
      size_t slash_pos = gtest_suite.find('/'); \
      if (slash_pos != std::string::npos) { \
        std::string suite_part = gtest_suite.substr(slash_pos + 1); /* "AlignedModeTest__RunCountAligned" */ \
        size_t dunder_pos = suite_part.find("__"); \
        if (dunder_pos != std::string::npos) { \
          std::string test_class = suite_part.substr(0, dunder_pos); /* "AlignedModeTest" */ \
          std::string test_name = suite_part.substr(dunder_pos + 2); /* "RunCountAligned" */ \
          simple_key = test_class + "." + test_name; \
        } \
      } \
      if (simple_key.empty()) simple_key = std::string(info->test_suite_name()) + "." + info->name(); \
      \
      std::string key = simple_key; \
      ::gtest_generator::g_test_modes[key] = ::gtest_generator::tl_mode; \
    } \
    if (!::gtest_generator::on_counting) { \
      ::gtest_generator::tl_col_sizes.clear(); \
      ::gtest_generator::tl_col_ix = 0; \
      ::gtest_generator::tl_last_param = -1; \
    } \
  } while(0); \
  if (gtest_generator::IsCountingMode(*this)) return;

#define GENERATOR(...) gtest_generator::GetGeneratorValue<gtest_generator::make_unique_id(__FILE__, __LINE__)>({__VA_ARGS__}, this)

#define TEST_G(TestClassName, TestName) \
    class TestClassName##__##TestName : public TestClassName {};\
    class GTEST_TEST_CLASS_NAME_(TestClassName##__##TestName, __); \
    inline gtest_generator::DynamicRangeGenerator* __gtest_generator__get_generator_##TestClassName##TestName() { \
        return gtest_generator::CreateGenerator<GTEST_TEST_CLASS_NAME_(TestClassName##__##TestName, __)>( \
            #TestClassName"."#TestName); \
    } \
    INSTANTIATE_TEST_SUITE_P(Generator, TestClassName##__##TestName, \
        testing::internal::ParamGenerator<int>(__gtest_generator__get_generator_##TestClassName##TestName())); \
    TEST_P(TestClassName##__##TestName, __)

// ============================================================================
// Array Comparison Macros
// ============================================================================

// Compare two arrays element by element (non-fatal)
// Usage: EXPECT_ARRAY_EQ(expected, actual, size)
#define EXPECT_ARRAY_EQ(expected, actual, size) \
  do { \
    bool all_equal = true; \
    for (size_t i = 0; i < (size); ++i) { \
      if ((expected)[i] != (actual)[i]) { \
        all_equal = false; \
        EXPECT_EQ((expected)[i], (actual)[i]) << "Arrays differ at index " << i; \
      } \
    } \
    if (all_equal) { \
      SUCCEED() << "Arrays are equal (size: " << (size) << ")"; \
    } \
  } while(0)

// Compare two arrays element by element (fatal)
// Usage: ASSERT_ARRAY_EQ(expected, actual, size)
#define ASSERT_ARRAY_EQ(expected, actual, size) \
  do { \
    for (size_t i = 0; i < (size); ++i) { \
      ASSERT_EQ((expected)[i], (actual)[i]) << "Arrays differ at index " << i; \
    } \
  } while(0)

// Compare two floating-point arrays with tolerance (non-fatal)
// Usage: EXPECT_ARRAY_NEAR(expected, actual, size, abs_error)
#define EXPECT_ARRAY_NEAR(expected, actual, size, abs_error) \
  do { \
    bool all_near = true; \
    for (size_t i = 0; i < (size); ++i) { \
      if (std::abs((expected)[i] - (actual)[i]) > (abs_error)) { \
        all_near = false; \
        EXPECT_NEAR((expected)[i], (actual)[i], (abs_error)) << "Arrays differ at index " << i; \
      } \
    } \
    if (all_near) { \
      SUCCEED() << "Arrays are near (size: " << (size) << ", tolerance: " << (abs_error) << ")"; \
    } \
  } while(0)

// Compare two floating-point arrays with tolerance (fatal)
// Usage: ASSERT_ARRAY_NEAR(expected, actual, size, abs_error)
#define ASSERT_ARRAY_NEAR(expected, actual, size, abs_error) \
  do { \
    for (size_t i = 0; i < (size); ++i) { \
      ASSERT_NEAR((expected)[i], (actual)[i], (abs_error)) << "Arrays differ at index " << i; \
    } \
  } while(0)

// Compare two double arrays with default tolerance (non-fatal)
// Usage: EXPECT_ARRAY_DOUBLE_EQ(expected, actual, size)
#define EXPECT_ARRAY_DOUBLE_EQ(expected, actual, size) \
  do { \
    bool all_equal = true; \
    for (size_t i = 0; i < (size); ++i) { \
      if ((expected)[i] != (actual)[i]) { \
        all_equal = false; \
        EXPECT_DOUBLE_EQ((expected)[i], (actual)[i]) << "Arrays differ at index " << i; \
      } \
    } \
    if (all_equal) { \
      SUCCEED() << "Double arrays are equal (size: " << (size) << ")"; \
    } \
  } while(0)

// Compare two float arrays with default tolerance (non-fatal)
// Usage: EXPECT_ARRAY_FLOAT_EQ(expected, actual, size)
#define EXPECT_ARRAY_FLOAT_EQ(expected, actual, size) \
  do { \
    bool all_equal = true; \
    for (size_t i = 0; i < (size); ++i) { \
      if ((expected)[i] != (actual)[i]) { \
        all_equal = false; \
        EXPECT_FLOAT_EQ((expected)[i], (actual)[i]) << "Arrays differ at index " << i; \
      } \
    } \
    if (all_equal) { \
      SUCCEED() << "Float arrays are equal (size: " << (size) << ")"; \
    } \
  } while(0)

