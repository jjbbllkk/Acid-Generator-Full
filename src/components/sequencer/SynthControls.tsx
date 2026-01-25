import { type FC } from 'react';
import Knob from '../Knob';
import { CUTOFF, DEFAULTS, DELAY_SEND, RES, ENV_MOD, DECAY } from '../../constants';

import styles from './SynthControls.module.less';

interface Props {
  resonance: number;
  cutoff: number;
  envelope: number; // New prop
  decay: number;    // New prop
  delay: number;
  onCutoffChange: (v: number) => void;
  onResonanceChange: (v: number) => void;
  onEnvelopeChange: (v: number) => void; // New handler
  onDecayChange: (v: number) => void;    // New handler
  onDelaySendChange: (v: number) => void;
}

const SynthControls: FC<Props> = ({
  cutoff,
  resonance,
  envelope,
  decay,
  delay,
  onCutoffChange,
  onResonanceChange,
  onEnvelopeChange,
  onDecayChange,
  onDelaySendChange,
}) => {
  return (
    <menu className={styles.synthControls}>
      <li>
        <Knob
          onChange={onCutoffChange}
          min={CUTOFF.MIN}
          max={CUTOFF.MAX}
          defaultValue={DEFAULTS.CUTOFF}
          value={cutoff}
          direction={'vertical'}
          step={1}
          label={'CUT'}
        />
      </li>
      <li>
        <Knob
          onChange={onResonanceChange}
          min={RES.MIN}
          max={RES.MAX}
          defaultValue={DEFAULTS.RESONANCE}
          value={resonance}
          direction={'vertical'}
          step={0.1}
          label={'RES'}
        />
      </li>
      <li>
        <Knob
          onChange={onEnvelopeChange}
          min={ENV_MOD.MIN}
          max={ENV_MOD.MAX}
          defaultValue={DEFAULTS.ENV_MOD}
          value={envelope}
          direction={'vertical'}
          step={0.1}
          label={'ENV'}
        />
      </li>
      <li>
        <Knob
          onChange={onDecayChange}
          min={DECAY.MIN}
          max={DECAY.MAX}
          defaultValue={DEFAULTS.DECAY}
          value={decay}
          direction={'vertical'}
          step={0.1}
          label={'DEC'}
        />
      </li>
      <li>
        <Knob
          onChange={onDelaySendChange}
          min={DELAY_SEND.MIN}
          max={DELAY_SEND.MAX}
          defaultValue={DEFAULTS.DELAY_LEVEL}
          value={delay}
          direction={'vertical'}
          step={0.1}
          label={'DLY'}
        />
      </li>
    </menu>
  );
};

export { type Props };

export default SynthControls;
