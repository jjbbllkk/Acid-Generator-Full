import { type SCALE, SCALES } from './audio-engine/scales';
import { type SequencerOutput } from './types';

// --- ORIGINAL UTILITIES (RESTORED) ---

// A "Seeded" Random Number Generator. 
// If you give it the same seed (e.g. 123), it always produces the same sequence of numbers.
export const sfc32 = (a: number, b: number, c: number, d: number) => {
  return function () {
    a >>>= 0;
    b >>>= 0;
    c >>>= 0;
    d >>>= 0;
    let t = (a + b) | 0;
    a = b ^ (b >>> 9);
    b = (c + (c << 3)) | 0;
    c = (c << 21) | (c >>> 11);
    d = (d + 1) | 0;
    t = (t + d) | 0;
    c = (c + t) | 0;
    return (t >>> 0) / 4294967296;
  };
};

export const arrayRand = (arr: number[], l: number, rng: () => number = Math.random): number[] => {
  // We use the 'rng' (random number generator) passed in, instead of standard Math.random
  return [...arr].sort(() => 0.5 - rng()).slice(0, l);
};

export const mapRange = (
  value: number,
  inMin: number,
  inMax: number,
  outMin: number,
  outMax: number,
) => {
  if (value <= inMin) return outMin;
  if (value >= inMax) return outMax;
  return ((value - inMin) * (outMax - outMin)) / (inMax - inMin) + outMin;
};

type NullOrNumber<T> = T extends number ? number : null;

// --- NEW UPDATED LOGIC ---

export const getNoteInScale = <T extends number | null>(
  note: T,
  scaleName: SCALE,
  root = 0,
  octave = 0,
): NullOrNumber<T> => {
  if (note === null) {
    return null as NullOrNumber<T>;
  }

  const scale = SCALES[scaleName];
  const len = scale.length;

  // Wrap the index to fit the scale length
  const wrappedIndex = note % len;
  
  // Calculate extra octaves if the index exceeded the scale length
  const octaveOffset = Math.floor(note / len);

  // Formula: Note in Scale + Root + (User Octave * 12) + (Wrapped Octave * 12)
  const finalNote = scale[wrappedIndex] + root + 12 * (octave + octaveOffset);

  return finalNote as NullOrNumber<T>;
};

export const getOutput = (outputs: SequencerOutput[]): SequencerOutput | undefined =>
  outputs.find(({ selected }) => selected);
