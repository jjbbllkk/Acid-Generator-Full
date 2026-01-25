import { type FC } from 'react';
import PlayControls, { type Props as PlayControlProps } from './PlayControls';
import SynthControls, { type Props as SynthControlProps } from './SynthControls';
import ChannelSelector, { type Props as ChannelSelectProps } from './ChannelSelector';
import { type SequencerOutput } from '../../types';

import styles from './Controls.module.less';

type Props = SynthControlProps & PlayControlProps & ChannelSelectProps;

const Controls: FC<Props & { output: SequencerOutput | undefined }> = ({
  resonance,
  cutoff,
  envelope,
  decay,
  delay,
  tempo,
  transpose, // New
  onPlayClick,
  onTempoChange,
  onTransposeChange, // New
  playing,
  onCutoffChange,
  onResonanceChange,
  onEnvelopeChange,
  onDecayChange,
  onDelaySendChange,
  output,
  onChannelChange,
}) => {
  return (
    <section className={styles.controls}>
      <PlayControls
        onPlayClick={onPlayClick}
        tempo={tempo}
        onTempoChange={onTempoChange}
        onTransposeChange={onTransposeChange} // Pass
        transpose={transpose} // Pass
        playing={playing}
      />
      <aside>
        {!output ? (
          <SynthControls
            resonance={resonance}
            cutoff={cutoff}
            envelope={envelope}
            decay={decay}
            delay={delay}
            onCutoffChange={onCutoffChange}
            onResonanceChange={onResonanceChange}
            onEnvelopeChange={onEnvelopeChange}
            onDecayChange={onDecayChange}
            onDelaySendChange={onDelaySendChange}
          />
        ) : (
          <ChannelSelector onChannelChange={onChannelChange} output={output} />
        )}
      </aside>
    </section>
  );
};

export { type Props };

export default Controls;
