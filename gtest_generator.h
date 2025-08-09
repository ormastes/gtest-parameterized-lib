#pragma once
#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <string>
#include <cassert>


namespace gtest_generator {
static constexpr bool GTEST_GENERATOR_LOG = false;
class TestWithGenerator : public ::testing::TestWithParam<int> {
    public:
    virtual void TestBody() = 0;
};

// Forward declaration
class DynamicRangeGenerator;

// Global registry mapping "SuiteName.TestName" -> generator pointer
static std::map<std::string, DynamicRangeGenerator*> g_range_map;

// Thread-local static variable for counting state
static thread_local bool on_counting = false;
static thread_local int current_count = 1;

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
    if ( test_instance) {
        on_counting = true;
        current_count = 1; // Reset count for each test instance
        test_instance->TestBody(); // Call the test body
        on_counting = false;
        end = current_count; // Set end to the current count
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
        // If in counting mode, update the end value
        current_devider = current_count;
        current_count *= values.size(); // Update end to reflect total combinations
        if constexpr (GTEST_GENERATOR_LOG) printf("Generator value for counting mode: %d\n", current_count);
        return *values.begin(); // Return a dummy value
    } else {
        // If not in counting mode, just return the value for the current index
        if constexpr (GTEST_GENERATOR_LOG) printf("Getting generator value for test instance: %p\n", test_instance);
        int paramIndex = test_instance ? test_instance->GetParam() : 0;
        if constexpr (GTEST_GENERATOR_LOG) printf("Generator value for index %d: %d\n", paramIndex, *(values.begin() + (paramIndex % values.size())));
        return *(values.begin() + ((paramIndex / current_devider) % values.size()));
    }
}
}
// Macros
#define USE_GENERATOR() if (gtest_generator::IsCountingMode(*this)) return;
#define GENERATOR(...) gtest_generator::GetGeneratorValue<gtest_generator::make_unique_id(__FILE__, __LINE__)>({__VA_ARGS__}, this)

#define TEST_G(TestClassName, TestName) \
    class TestClassName##__##TestName : public TestClassName {};\
    TEST_P(TestClassName##__##TestName, __)


#define ENABLE_GENERATOR(TestClassName, TestName) \
    static gtest_generator::DynamicRangeGenerator* __gtest_generator__generator_##TestClassName##TestName \
        = new gtest_generator::DynamicRangeGenerator( #TestClassName"."#TestName, new GTEST_TEST_CLASS_NAME_(TestClassName##__##TestName, __)()); \
    INSTANTIATE_TEST_SUITE_P(Generator, TestClassName##__##TestName, testing::internal::ParamGenerator<int>(__gtest_generator__generator_##TestClassName##TestName));

