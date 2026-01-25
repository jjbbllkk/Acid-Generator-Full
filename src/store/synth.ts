import {
  type CaseReducer,
  createSlice,
  type PayloadAction,
  type SliceCaseReducers,
} from '@reduxjs/toolkit';
import { DEFAULTS } from '../constants';
import { storage } from '../localStorage';

const { synth } = storage;

const { CUTOFF, RESONANCE, DELAY_LEVEL, ENV_MOD, DECAY } = DEFAULTS;

interface State {
  cutoff: number;
  resonance: number;
  delaySend: number;
  envelope: number; // New
  decay: number;    // New
}

interface Reducers extends SliceCaseReducers<State> {
  setCutoff: CaseReducer<State, PayloadAction<number>>;
  setResonance: CaseReducer<State, PayloadAction<number>>;
  setDelaySend: CaseReducer<State, PayloadAction<number>>;
  setEnvelope: CaseReducer<State, PayloadAction<number>>; // New
  setDecay: CaseReducer<State, PayloadAction<number>>;    // New
}

const initialState: State = {
  cutoff: CUTOFF,
  resonance: RESONANCE,
  delaySend: DELAY_LEVEL,
  envelope: ENV_MOD,
  decay: DECAY,
};

const slice = createSlice<State, Reducers>({
  name: 'generateor',
  initialState: {
    ...initialState,
    ...synth,
  },
  reducers: {
    setCutoff: (state, { payload }) => {
      return {
        ...state,
        cutoff: payload,
      };
    },
    setResonance: (state, { payload }) => {
      return {
        ...state,
        resonance: payload,
      };
    },
    setDelaySend: (state, { payload }) => {
      return {
        ...state,
        delaySend: payload,
      };
    },
    setEnvelope: (state, { payload }) => {
      return {
        ...state,
        envelope: payload,
      };
    },
    setDecay: (state, { payload }) => {
      return {
        ...state,
        decay: payload,
      };
    },
  },
});

const {
  actions: { setCutoff, setResonance, setDelaySend, setEnvelope, setDecay },
  reducer,
} = slice;

export { setCutoff, setResonance, setDelaySend, setEnvelope, setDecay };
export type { State as SynthState };
export default reducer;
