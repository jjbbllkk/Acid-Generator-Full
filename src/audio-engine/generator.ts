import { type Unit } from 'tone';
import { sfc32 } from '../utils';

const scale = Array(7)
  .fill(0)
  .map((_v, i) => i);

type Octave = -1 | 0 | 1;

export interface SequenceStep<T extends Unit.Note | null = Unit.Note | null> {
  note: T extends null ? T : number;
  octave: T extends null ? T : Octave;
  accent: T extends null ? T : boolean;
  slide: T extends null ? T : boolean;
}

interface GeneratorParams {
  patternLength: number;
  density: number;
  spread: number;
  accentsDensity: number;
  slidesDensity: number;
  seed?: number;
}

const randomInt = (min: number, max: number, rng: () => number) => {
  return Math.floor(rng() * (max - min + 1)) + min;
};

const MAX_LEN = 64;
const BAR_LEN = 16;

const generate = ({
  density,
  spread,
  accentsDensity,
  slidesDensity,
  seed = Date.now(),
}: GeneratorParams): SequenceStep[] => {
  const rng = sfc32(seed, seed, seed, seed);

  // --- 1. MUSICAL SPREAD LOGIC ---
  const weightedScale = scale.map((noteIndex) => {
    let weight = rng(); // Use seeded rng
    if (noteIndex === 0) weight += 999; // Always keep Root first
    if (noteIndex === 4) weight += 0.5; // Often the 5th

    return { index: noteIndex, weight };
  });

  weightedScale.sort((a, b) => b.weight - a.weight);
  const sortedScale = weightedScale.map((i) => i.index);

  const spreadCount = Math.max(1, Math.round(scale.length * (spread / 100)));
  const selectedNotes = sortedScale.slice(0, spreadCount);

  // --- 2. DENSITY MASK (RHYTHM) ---
  const barSteps = Array(BAR_LEN)
    .fill(0)
    .map((_, i) => i);
  const weightedBarSteps = barSteps.map((step) => {
    let weight = rng();
    if (step % 4 === 0) weight += 0.5; // Boost downbeats
    if (step === 0) weight += 0.5; // Huge boost for the "One"
    return { step, weight };
  });

  weightedBarSteps.sort((a, b) => b.weight - a.weight);
  const barActivationOrder = weightedBarSteps.map((x) => x.step);

  // --- 3. GENERATE STEP CONTENT ---
  const allSteps = Array(MAX_LEN)
    .fill(0)
    .map((_, i) => i);

  const stepData = allSteps.map((i) => {
    const isDownbeat = i % 4 === 0;
    let noteIndex = 0;

    if (isDownbeat && rng() > 0.3) {
      noteIndex = 0;
    } else {
      noteIndex = randomInt(0, selectedNotes.length - 1, rng);
    }

    return {
      noteIndex: noteIndex,
      octave: randomInt(-1, 1, rng) as Octave,
      accentProb: rng(),
      slideProb: rng(),
    };
  });

  // --- 4. APPLY MASKS ---
  const numStepsToGeneratePerBar = Math.round(BAR_LEN * (density / 100));
  const activeBarStepsSet = new Set(
    barActivationOrder.slice(0, numStepsToGeneratePerBar),
  );

  return allSteps.map((i) => {
    if (!activeBarStepsSet.has(i % BAR_LEN)) {
      return {
        note: null,
        octave: null,
        accent: null,
        slide: null,
      } as SequenceStep<null>;
    }

    const data = stepData[i];

    return {
      note: selectedNotes[data.noteIndex],
      octave: data.octave,
      accent: data.accentProb < accentsDensity / 100,
      slide: data.slideProb < slidesDensity / 100,
    } as SequenceStep<Unit.Note>;
  });
};

export { generate };
