import { SCALE } from './audio-engine/scales.ts';
import { type InternalSynth } from './types.ts';

type Range = {
  MIN: number;
  MAX: number;
};

export const BASE_NOTE = 48; // C3

export const DEFAULTS = {
  BPM: 120,
  SCALE: SCALE.PHRYGIAN,
  SEQ_LENGTH: 16,
  DELAY_LEVEL: -18,
  CUTOFF: 220,
  RESONANCE: 3,
  ENV_MOD: 3,
  DECAY: 0.5,
  TRANSPOSE: 0, // Default 0 offset
};

export const CUTOFF: Range = {
  MIN: 110,
  MAX: 880,
};

export const RES: Range = {
  MIN: 0,
  MAX: 9,
};

export const BPM: Range = {
  MIN: 30,
  MAX: 240,
};

export const DELAY_SEND: Range = {
  MIN: -80,
  MAX: -1,
};

export const ENV_MOD: Range = {
  MIN: 0,
  MAX: 10,
};

export const DECAY: Range = {
  MIN: 0.1,
  MAX: 2.0,
};

export const TRANSPOSE: Range = {
  MIN: -12,
  MAX: 12,
};

export const internalSynth: InternalSynth = 'internal';
