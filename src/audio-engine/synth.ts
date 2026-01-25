import { MonoSynth, PingPongDelay, Split, Volume } from 'tone';
import { store } from '../store';

const {
  synth: { cutoff, resonance, delaySend, envelope, decay },
} = store.getState();

const split = new Split(2);
const pingPong = new PingPongDelay('8n.', 0.6).toDestination();
const vol = new Volume(delaySend).connect(pingPong);

split.connect(vol);

const tb303 = new MonoSynth({
  oscillator: {
    type: 'sawtooth',
  },
  envelope: {
    attackCurve: 'exponential',
    releaseCurve: 'exponential',
    attack: 0.01,
    decay: decay,
    sustain: 0.1,
    release: 0.2,
  },
  filterEnvelope: {
    attackCurve: 'exponential',
    releaseCurve: 'exponential',
    attack: 0.01,
    decay: decay,
    sustain: 0.1,
    release: 1,
    baseFrequency: cutoff,
    octaves: envelope,
    exponent: 5,
  },
  filter: {
    frequency: cutoff,
    rolloff: -24,
    Q: resonance,
    type: 'lowpass',
  },
  portamento: 0.05, // Increased from 0.02 to 0.05 for more "drag"
})
  .connect(split)
  .toDestination();

export { tb303, vol as delaySend };
