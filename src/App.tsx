import { type FC, useCallback, useEffect } from 'react';
import { useDispatch, useSelector } from 'react-redux';
import {
  changeCutoff,
  changeDelaySend,
  changeResonance,
  changeEnvelope,
  changeDecay,
  changeTempo,
  downloadPattern,
  generatePattern,
  stopInternalSynth,
  toggleTransport,
} from './audio-engine/controls';
import { type SCALE } from './audio-engine/scales';
import { BASE_NOTE } from './constants'; // Import BASE_NOTE
import GeneratorControls from './components/GeneratorControls';
import Sequencer from './components/Sequencer';
import About from './components/About';
import StoredPatterns from './components/StoredPatterns';
import {
  setAccentDensity,
  setDensity,
  setPatternLength,
  setSlidesDensity,
  setSpread,
} from './store/generator';
import {
  deletePattern,
  loadPattern,
  selectOutput,
  setMidiChannel,
  setScale,
  shiftPattern,
  storePattern,
  setBaseNote, // Import new action
} from './store/sequencer';
import { type State } from './store';
import { DIRECTION } from './types';
import {
  getMidiAccess,
  midiStateChangeEventListener,
} from './audio-engine/midi-output.ts';

import styles from './App.module.less';

// eslint-disable-next-line import/no-unresolved
import about from '../README.md?raw';

const App: FC = () => {
  const dispatch = useDispatch();

  const {
    transport: { currentStep, tempo, playing },
    sequencer: {
      pattern,
      scale,
      name,
      storedPatterns,
      options: {
        output: { outputs, midi },
        baseNote, // Destructure baseNote
      },
    },
    generator: {
      dispatchGenerate,
      density,
      spread,
      accentsDensity,
      patternLength,
      slidesDensity,
    },
    synth: { cutoff, resonance, envelope, decay, delaySend },
  } = useSelector((state: State) => {
    return state;
  });

  useEffect(() => {
    void getMidiAccess();
  }, []);

  useEffect(() => {
    midi?.addEventListener('statechange', midiStateChangeEventListener);
    return () => {
      console.info('remove listener');
      midi?.removeEventListener('statechange', midiStateChangeEventListener);
    };
  }, [midi]);

  const handleGenerateClick = useCallback(() => {
    generatePattern();
  }, []);

  const handleScaleChange = useCallback(
    (newScale: SCALE) => {
      dispatch(setScale(newScale));
    },
    [dispatch],
  );

  const handlePatternLengthChange = useCallback(
    (v: number) => {
      dispatch(setPatternLength(v));
    },
    [dispatch],
  );
  const handleSpreadChange = useCallback(
    (v: number) => {
      dispatch(setSpread(v));
      generatePattern(true, true);
    },
    [dispatch],
  );
  const handleDensityChange = useCallback(
    (v: number) => {
      dispatch(setDensity(v));
      generatePattern(true, true);
    },
    [dispatch],
  );
  const handleAccentsDensityChange = useCallback(
    (v: number) => {
      dispatch(setAccentDensity(v));
      generatePattern(true, true);
    },
    [dispatch],
  );
  const handleSlidesDensityChange = useCallback(
    (v: number) => {
      dispatch(setSlidesDensity(v));
      generatePattern(true, true);
    },
    [dispatch],
  );

  const togglePlay = useCallback(() => {
    void toggleTransport();
  }, []);

  const handleTempoChange = useCallback((bpm: number) => changeTempo(bpm), []);

  const handleTransposeChange = useCallback(
    (val: number) => {
      dispatch(setBaseNote(BASE_NOTE + val));
    },
    [dispatch],
  );

  const handleShiftLeftClick = useCallback(() => {
    dispatch(shiftPattern(DIRECTION.LEFT));
  }, [dispatch]);

  const handleShiftRightClick = useCallback(() => {
    dispatch(shiftPattern(DIRECTION.RIGHT));
  }, [dispatch]);

  const handlePatternStoreClick = useCallback(() => {
    dispatch(storePattern({ pattern, name, scale }));
  }, [dispatch, pattern, name, scale]);

  const handleDownloadSequencerPatternb = useCallback(() => {
    downloadPattern({ pattern, name, scale });
  }, [name, scale, pattern]);

  const handleLoadPattern = useCallback(
    (i: number) => {
      dispatch(loadPattern(i));
    },
    [dispatch],
  );

  const handleDeletePattern = useCallback(
    (i: number) => {
      dispatch(deletePattern(i));
    },
    [dispatch],
  );

  const handleDownloadPattern = useCallback(
    (i: number) => {
      downloadPattern(storedPatterns[i]);
    },
    [storedPatterns],
  );

  const handleOutputChange = useCallback(
    (id: string | undefined) => {
      stopInternalSynth();
      dispatch(selectOutput(id));
    },
    [dispatch],
  );

  const handleChannelChange = useCallback(
    (channel: number, id: string) => {
      dispatch(setMidiChannel({ channel, id }));
    },
    [dispatch],
  );

  return (
    <>
      <main className={`mainPart ${styles.main}`}>
        <GeneratorControls
          onGenerateClick={handleGenerateClick}
          waiting={dispatchGenerate}
          patternLength={patternLength}
          density={density}
          spread={spread}
          accentsDensity={accentsDensity}
          slidesDensity={slidesDensity}
          onPatternLengthChange={handlePatternLengthChange}
          onSpreadChange={handleSpreadChange}
          onDensityChange={handleDensityChange}
          onAccentsChange={handleAccentsDensityChange}
          onSlidesChange={handleSlidesDensityChange}
        />
        <Sequencer
          onDownloadClick={handleDownloadSequencerPatternb}
          name={name}
          pattern={pattern}
          currentStep={currentStep}
          scaleName={scale}
          patternLength={patternLength}
          onScaleChange={handleScaleChange}
          resonance={resonance}
          cutoff={cutoff}
          envelope={envelope}
          decay={decay}
          delay={delaySend}
          transpose={baseNote - BASE_NOTE} // Calculate offset for knob
          onCutoffChange={changeCutoff}
          onResonanceChange={changeResonance}
          onEnvelopeChange={changeEnvelope}
          onDecayChange={changeDecay}
          onDelaySendChange={changeDelaySend}
          onTransposeChange={handleTransposeChange}
          onPlayClick={togglePlay}
          onTempoChange={handleTempoChange}
          tempo={tempo}
          playing={playing}
          onShiftLeftClick={handleShiftLeftClick}
          onShiftRightClick={handleShiftRightClick}
          onPatternStoreClick={handlePatternStoreClick}
          outputs={outputs}
          onOutputChange={handleOutputChange}
          onChannelChange={handleChannelChange}
        />
        <About content={about} />
      </main>
      <aside className="storedPatterns">
        <StoredPatterns
          patterns={storedPatterns}
          onDownloadClick={handleDownloadPattern}
          onDeleteClick={handleDeletePattern}
          onLoadClick={handleLoadPattern}
        />
      </aside>
    </>
  );
};

export default App;
