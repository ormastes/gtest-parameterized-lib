#pragma once
#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>


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
inline thread_local std::vector<int> tl_col_sizes;
inline thread_local int tl_col_ix = 0;

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

        // Start with full count - will be adjusted by USE_GENERATOR at runtime
        end = full_count;
        tl_col_ix = 0;
    }
  }


  // Begin iterator: binds to 'end'
  testing::internal::ParamIteratorInterface<int>* Begin() const override {
    if constexpr (GTEST_GENERATOR_LOG) printf("Creating begin iterator for %s\n", key.c_str());
    return new DynIterator(start, this, /*at_end=*/false);
  }

  // End iterator: marks the end
  testing::internal::ParamIteratorInterface<int>* End() const override {
    return new DynIterator(end, this, /*at_end=*/true);
  }

 private:
  // Iterator yielding a reference to 'end', then marking done
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
        return _generator; // No base generator for begin iterator
    }

    const int* Current() const override {
        if constexpr (GTEST_GENERATOR_LOG) printf("Current value for %s: %d\n", _generator->key.c_str(), _value);
        return &_value; // Return pointer to current value
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
        }
        return *values.begin(); // dummy in counting pass
    }
    
    // RUN PHASE - check what mode this test uses
    const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
    SamplingMode mode = SamplingMode::FULL;
    if (info) {
        std::string key = std::string(info->test_suite_name()) + "." + info->name();
        auto mode_it = g_test_modes.find(key);
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
        if (info) {
            std::string key = std::string(info->test_suite_name()) + "." + info->name();
            auto it = g_colsizes_map.find(key);
            if (it != g_colsizes_map.end()) {
                col_sizes = &(it->second);
            }
        }
        
        // Check if we should skip this test iteration
        int r = test_instance ? test_instance->GetParam() : 0;
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

// ============================================================================
// Private Member Access for Testing
// ============================================================================

// Template function declaration (prevent multiple definition with #ifdef)
// This block can be copied to a common header file for sharing across production and test code
#ifndef GTEST_GENERATOR_ACCESS_PRIVATE_MEMBER_DEFINED
#define GTEST_GENERATOR_ACCESS_PRIVATE_MEMBER_DEFINED

template <typename ID, typename TestCase, typename Target>
auto accessPrivateMember(TestCase* test_case, Target* target = nullptr) -> decltype(auto);

// Macro to make accessPrivateMember a friend of the target class
// Usage: Place inside the target class definition
// In test builds: Grants friend access to test infrastructure
// In production builds: Empty macro (no effect)
#define FRIEND_ACCESS_PRIVATE() \
    template <typename _ID, typename _TC, typename _TG> \
    friend auto accessPrivateMember(_TC*, _TG*) -> decltype(auto)

#endif  // GTEST_GENERATOR_ACCESS_PRIVATE_MEMBER_DEFINED

// Empty macro for production builds - define this in your production headers
// to disable friend access when not testing
#ifndef FRIEND_ACCESS_PRIVATE
#define FRIEND_ACCESS_PRIVATE()
#endif

// Helper macro to concatenate tokens for ID generation
#define CONCAT_IMPL(a, b, c) a##_##b##_##c
#define CONCAT(a, b, c) CONCAT_IMPL(a, b, c)

// Macro to declare the template function with specific types for instance members
// Usage: DECLARE_ACCESS_PRIVATE(ID, TestCase, Target, memberName)
// IMPORTANT: MemberPtr should be just the field name (e.g., privateValue), not &Target::member
// IMPORTANT: To use CONCAT-based ID generation, TestCase must not contain '::'
// Use a 'using' alias to avoid scope qualifiers:
//   using MyTestCase = ::gtest_generator::TestWithGenerator;
//   DECLARE_ACCESS_PRIVATE(ID, MyTestCase, Target, privateField)
// ID parameter is currently ignored - CONCAT(TestCase, Target, MemberPtr) is used instead
#define DECLARE_ACCESS_PRIVATE(ID, TestCase, Target, MemberPtr) \
    struct CONCAT(TestCase, Target, MemberPtr); \
    template <> \
    inline auto accessPrivateMember<CONCAT(TestCase, Target, MemberPtr), TestCase, Target>(TestCase* test_case, Target* target) -> decltype(auto) { \
        return (target->MemberPtr); \
    }

// Macro to declare the template function with specific types for static members
// Usage: Place outside class definition in test file
// ID is automatically generated from TestCase, Target, and Member
#define DECLARE_ACCESS_PRIVATE_STATIC(TestCase, Target, Member) \
    struct CONCAT(TestCase, Target, Member); \
    template <> \
    inline auto accessPrivateMember<CONCAT(TestCase, Target, Member), TestCase, Target>(TestCase* test_case, Target* target) -> decltype(auto) { \
        return Target::Member; \
    }

// Macro to declare the template function without implementation - user appends their own function body
// Usage: DECLARE_ACCESS_PRIVATE_FUNCTION(TestCase, Target, UniqueID) { return custom_expression; }
// ID is automatically generated from TestCase, Target, and UniqueID
#define DECLARE_ACCESS_PRIVATE_FUNCTION(TestCase, Target, UniqueID) \
    struct CONCAT(TestCase, Target, UniqueID); \
    template <> \
    inline auto accessPrivateMember<CONCAT(TestCase, Target, UniqueID), TestCase, Target>(TestCase* test_case, Target* target) -> decltype(auto)

// Macro for calling accessPrivateMember - automatically takes 'this' as first parameter
// Second parameter defaults to nullptr if not provided
// Usage: ACCESS_PRIVATE(TestCase, ID, Target, &obj) or ACCESS_PRIVATE(TestCase, ID, Target)
// TestCase must match the type used in DECLARE_ACCESS_PRIVATE
#define ACCESS_PRIVATE(TestCase, ID, Target, ...) \
    accessPrivateMember<ID, TestCase, Target>(static_cast<TestCase*>(this), ##__VA_ARGS__)

// ============================================================================

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
      \
      /* Adjust generator end value based on mode (only on first call) */ \
      auto gen_it = ::gtest_generator::g_range_map.find(key); \
      auto count_it = ::gtest_generator::g_test_counts.find(key); \
      if (gen_it != ::gtest_generator::g_range_map.end() && \
          count_it != ::gtest_generator::g_test_counts.end()) { \
        auto* gen = gen_it->second; \
        const auto& counts = count_it->second; \
        if (::gtest_generator::tl_mode == ::gtest_generator::SamplingMode::ALIGNED) { \
          gen->end = counts.second; /* Use aligned_max */ \
        } else { \
          gen->end = counts.first; /* Use full_count */ \
        } \
      } \
      \
      if (::gtest_generator::tl_mode == ::gtest_generator::SamplingMode::ALIGNED) { \
        auto it = ::gtest_generator::g_colsizes_map.find(key); \
        if (it != ::gtest_generator::g_colsizes_map.end() && !it->second.empty()) { \
          int max_size = 0; \
          for (int s : it->second) max_size = std::max(max_size, s); \
          if (GetParam() >= max_size) { \
            GTEST_SKIP() << "Skipping iteration " << GetParam() << " for ALIGNED mode (max size: " << max_size << ")"; \
          } \
        } \
      } \
    } \
    if (!::gtest_generator::on_counting) { \
      ::gtest_generator::tl_col_sizes.clear(); \
      ::gtest_generator::tl_col_ix = 0; \
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

