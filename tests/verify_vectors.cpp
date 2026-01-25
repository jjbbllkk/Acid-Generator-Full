/**
 * Test Vector Verification for Acid Generator C++ Port
 *
 * This program runs the exact same scenarios as src/createTestVectors.ts
 * and outputs JSON that can be diff'd against the TypeScript output.
 *
 * Compile:
 *   g++ -std=c++17 -I../src -o verify_vectors verify_vectors.cpp
 *
 * Run:
 *   ./verify_vectors > cpp_output.json
 *
 * Compare with TypeScript output:
 *   npm run test:vectors > ts_output.json   (or however you run createTestVectors)
 *   diff cpp_output.json ts_output.json
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdio>

// Include the generator (header-only)
#include "Generator.hpp"

using namespace AcidGenerator;

//-----------------------------------------------------------------------------
// Test Scenario Definition
//-----------------------------------------------------------------------------

struct TestScenario {
    uint32_t seed;
    float density;
    float spread;
    float accentsDensity;
    float slidesDensity;
    int patternLength;
    std::string description;
};

//-----------------------------------------------------------------------------
// JSON Output Helpers
//-----------------------------------------------------------------------------

void printIndent(int level) {
    for (int i = 0; i < level * 2; i++) std::cout << ' ';
}

void printRngCheck() {
    // Verify RNG produces same values as TypeScript for seed 123
    SFC32 rng(123);

    std::cout << "  \"rng_check\": {\n";
    std::cout << "    \"seed\": 123,\n";
    std::cout << "    \"first_5_floats\": [\n";

    for (int i = 0; i < 5; i++) {
        float val = rng.next();
        std::cout << "      \"" << std::fixed << std::setprecision(6) << val << "\"";
        if (i < 4) std::cout << ",";
        std::cout << "\n";
    }

    std::cout << "    ],\n";
    std::cout << "    \"note\": \"The C++ sfc32 implementation MUST match these values for seed 123.\"\n";
    std::cout << "  },\n";
}

void printStep(const SequenceStep& step, bool isLast) {
    // Output in same format as TypeScript: {n, o, a, s}
    // n = note index (or null/-1 for rest)
    // o = octave (-1, 0, 1)
    // a = accent (0 or 1)
    // s = slide (0 or 1)

    std::cout << "        { ";

    if (step.isRest()) {
        std::cout << "\"n\": null, ";
        std::cout << "\"o\": null, ";
        std::cout << "\"a\": null, ";
        std::cout << "\"s\": null";
    } else {
        std::cout << "\"n\": " << step.note << ", ";
        std::cout << "\"o\": " << step.octave << ", ";
        std::cout << "\"a\": " << (step.accent ? 1 : 0) << ", ";
        std::cout << "\"s\": " << (step.slide ? 1 : 0);
    }

    std::cout << " }";
    if (!isLast) std::cout << ",";
    std::cout << "\n";
}

void printScenario(int id, const TestScenario& scenario, const Pattern& pattern, bool isLast) {
    // Determine slice length (same logic as TypeScript)
    int sliceLen = scenario.patternLength > 16 ? 32 : 16;

    std::cout << "    {\n";
    std::cout << "      \"id\": " << id << ",\n";
    std::cout << "      \"description\": \"" << scenario.description << "\",\n";

    // Inputs
    std::cout << "      \"inputs\": {\n";
    std::cout << "        \"seed\": " << scenario.seed << ",\n";
    std::cout << "        \"density\": " << static_cast<int>(scenario.density) << ",\n";
    std::cout << "        \"spread\": " << static_cast<int>(scenario.spread) << ",\n";
    std::cout << "        \"accentsDensity\": " << static_cast<int>(scenario.accentsDensity) << ",\n";
    std::cout << "        \"slidesDensity\": " << static_cast<int>(scenario.slidesDensity) << ",\n";
    std::cout << "        \"patternLength\": " << scenario.patternLength << "\n";
    std::cout << "      },\n";

    // Expected output
    std::cout << "      \"expectedOutput\": [\n";
    for (int i = 0; i < sliceLen; i++) {
        printStep(pattern.steps[i], i == sliceLen - 1);
    }
    std::cout << "      ]\n";

    std::cout << "    }";
    if (!isLast) std::cout << ",";
    std::cout << "\n";
}

//-----------------------------------------------------------------------------
// Main Test Runner
//-----------------------------------------------------------------------------

int main() {
    // Define all test scenarios (same as createTestVectors.ts)
    std::vector<TestScenario> scenarios = {
        // 1. Baseline
        { 1001, 100, 100, 50, 50, 16,
          "Scenario 1: Seed 1001, Den 100, Spr 100" },

        // 2. Low Density (Should mask notes)
        { 1001, 25, 100, 50, 50, 16,
          "Scenario 2: Seed 1001, Den 25, Spr 100" },

        // 3. Low Spread (Should restrict pitch range)
        { 1001, 100, 10, 50, 50, 16,
          "Scenario 3: Seed 1001, Den 100, Spr 10" },

        // 4. High Accents
        { 5555, 100, 100, 100, 0, 16,
          "Scenario 4: Seed 5555, Den 100, Spr 100" },

        // 5. High Slides
        { 5555, 100, 100, 0, 100, 16,
          "Scenario 5: Seed 5555, Den 100, Spr 100" },

        // 6. Complex Random Case A
        { 8675309, 73, 82, 33, 12, 64,
          "Scenario 6: Seed 8675309, Den 73, Spr 82" },

        // 7. Complex Random Case B
        { 42, 42, 42, 42, 42, 32,
          "Scenario 7: Seed 42, Den 42, Spr 42" },
    };

    // Output JSON
    std::cout << "{\n";

    // Metadata
    std::cout << "  \"metadata\": {\n";
    std::cout << "    \"generator\": \"Acid Pattern Generator C++ Port\",\n";
    std::cout << "    \"version\": \"1.0\",\n";
    std::cout << "    \"notes\": \"n=NoteIndex(0-6 or null), o=Octave(-1/0/1), a=Accent(0/1), s=Slide(0/1)\"\n";
    std::cout << "  },\n";

    // RNG verification
    printRngCheck();

    // Test vectors
    std::cout << "  \"vectors\": [\n";

    for (size_t i = 0; i < scenarios.size(); i++) {
        const TestScenario& scenario = scenarios[i];

        // Run generator with these params
        GeneratorParams params;
        params.patternLength = scenario.patternLength;
        params.density = scenario.density;
        params.spread = scenario.spread;
        params.accentsDensity = scenario.accentsDensity;
        params.slidesDensity = scenario.slidesDensity;
        params.seed = scenario.seed;

        Pattern pattern;
        generate(params, pattern);

        printScenario(static_cast<int>(i + 1), scenario, pattern, i == scenarios.size() - 1);
    }

    std::cout << "  ]\n";
    std::cout << "}\n";

    return 0;
}
