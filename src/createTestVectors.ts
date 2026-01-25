import { generate } from './audio-engine/generator';
import { sfc32 } from './utils';

// This script runs the generator with specific inputs and logs the "Correct Answers"
// The hardware engineer will use this to verify their C++ port works exactly like your TypeScript.

const runVectors = () => {
  console.log('Generating Test Vectors...');

  const scenarios = [
    // 1. Baseline
    { seed: 1001, density: 100, spread: 100, accentsDensity: 50, slidesDensity: 50, patternLength: 16 },
    // 2. Low Density (Should mask notes)
    { seed: 1001, density: 25, spread: 100, accentsDensity: 50, slidesDensity: 50, patternLength: 16 },
    // 3. Low Spread (Should restrict pitch range)
    { seed: 1001, density: 100, spread: 10, accentsDensity: 50, slidesDensity: 50, patternLength: 16 },
    // 4. High Accents
    { seed: 5555, density: 100, spread: 100, accentsDensity: 100, slidesDensity: 0, patternLength: 16 },
    // 5. High Slides
    { seed: 5555, density: 100, spread: 100, accentsDensity: 0, slidesDensity: 100, patternLength: 16 },
    // 6. Complex Random Case A
    { seed: 8675309, density: 73, spread: 82, accentsDensity: 33, slidesDensity: 12, patternLength: 64 },
    // 7. Complex Random Case B
    { seed: 42, density: 42, spread: 42, accentsDensity: 42, slidesDensity: 42, patternLength: 32 },
  ];

  const vectorData = scenarios.map((params, index) => {
    // Run the actual generator
    const output = generate(params);
    
    // We only save the first 16 steps to keep the file readable, 
    // unless patternLength is interesting.
    const sliceLen = params.patternLength > 16 ? 32 : 16;

    return {
      id: index + 1,
      description: `Scenario ${index + 1}: Seed ${params.seed}, Den ${params.density}, Spr ${params.spread}`,
      inputs: params,
      // We map the output to a simpler format for the C++ dev to read
      expectedOutput: output.slice(0, sliceLen).map(step => ({
        n: step.note,   // Note Index (0-6)
        o: step.octave, // Octave (-1, 0, 1)
        a: step.accent ? 1 : 0, // 1 = true
        s: step.slide ? 1 : 0   // 1 = true
      }))
    };
  });

  // Also generate raw RNG values to verify the Random Number Generator specifically
  const rng = sfc32(123, 123, 123, 123);
  const rawRngValues = Array(5).fill(0).map(() => rng().toFixed(6));

  const finalOutput = {
    metadata: {
      generator: "Acid Pattern Generator TypeScript Prototype",
      version: "1.0",
      notes: "n=NoteIndex(0-6 or null), o=Octave(-1/0/1), a=Accent(0/1), s=Slide(0/1)"
    },
    rng_check: {
      seed: 123,
      first_5_floats: rawRngValues,
      note: "The C++ sfc32 implementation MUST match these values for seed 123."
    },
    vectors: vectorData
  };

  console.log('----- COPY BELOW THIS LINE -----');
  console.log(JSON.stringify(finalOutput, null, 2));
  console.log('----- COPY ABOVE THIS LINE -----');
};

export default runVectors;
