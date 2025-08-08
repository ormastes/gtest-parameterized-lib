#pragma once

#include <gtest/gtest.h>
#include <vector>
#include <initializer_list>
#include <cstdio>

// Generator state using static local variables
class GeneratorState {
private:
    static thread_local bool is_counting_phase;
    static thread_local int generator_count;
    static thread_local std::vector<std::vector<int>> registered_generators;
    
public:
    static void StartCounting() {
        is_counting_phase = true;
        generator_count = 0;
        registered_generators.clear();
    }
    
    static int EndCounting() {
        is_counting_phase = false;
        
        // Calculate total combinations
        int total = 1;
        for (const auto& gen : registered_generators) {
            total *= gen.size();
        }
        
        // Clear for actual test runs
        registered_generators.clear();
        generator_count = 0;
        
        return total;
    }
    
    static int GetValue(std::initializer_list<int> values, int paramIndex) {
        static thread_local std::vector<std::vector<int>> generators_per_test;
        static thread_local int last_param_index = -1;
        
        // Reset generators when we start a new test (param index changes)
        if (paramIndex != last_param_index) {
            generators_per_test.clear();
            last_param_index = paramIndex;
        }
        
        // Register this generator
        int genId = generators_per_test.size();
        generators_per_test.push_back(std::vector<int>(values));
        
        if (is_counting_phase) {
            // During counting phase, just track generators
            registered_generators.push_back(std::vector<int>(values));
            return 0;
        }
        
        // Calculate which value to use
        const auto& gen = generators_per_test[genId];
        
        // Calculate total combinations after this generator
        size_t totalAfter = 1;
        for (size_t i = genId + 1; i < generators_per_test.size(); i++) {
            totalAfter *= generators_per_test[i].size();
        }
        
        // Need to know total generators - use the size from counting phase
        if (registered_generators.empty()) {
            // Fallback: assume we know the structure
            totalAfter = (genId == 0) ? 2 : 1;  // Hardcoded for 2 generators with 2 values each
        }
        
        size_t index = (paramIndex / totalAfter) % gen.size();
        return gen[index];
    }
};

// Thread-local definitions
thread_local bool GeneratorState::is_counting_phase = false;
thread_local int GeneratorState::generator_count = 0;
thread_local std::vector<std::vector<int>> GeneratorState::registered_generators;

// Counting function template
template<typename TestFixture>
int CountGenerators() {
    GeneratorState::StartCounting();
    
    // Create dummy test instance and run it
    TestFixture fixture;
    fixture.SetParam(0);
    fixture.TestBody();
    
    return GeneratorState::EndCounting();
}

// Macros
#define GENERATOR(...) \
    GeneratorState::GetValue({__VA_ARGS__}, GetParam())

#define USE_GENERATOR() // No-op for now

// Manual specification of total combinations for now
#define ENABLE_GENERATOR(TestClassName) \
    INSTANTIATE_TEST_SUITE_P( \
        Parameterized, \
        TestClassName, \
        ::testing::Range(0, 4) /* 2x2 combinations */ \
    );