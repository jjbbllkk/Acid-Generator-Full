import { type FC } from 'react';
import { PauseIcon, PlayIcon } from '../Icons';
import { BPM, DEFAULTS, TRANSPOSE } from '../../constants';
import Knob from '../Knob';
import Button from '../Button';

import styles from './PlayControls.module.less';

interface Props {
  onPlayClick: () => void;
  onTempoChange: (v: number) => void;
  onTransposeChange: (v: number) => void; // New
  transpose: number; // New
  tempo: number;
  playing: boolean;
}

const PlayControls: FC<Props> = ({
  onPlayClick,
  onTempoChange,
  onTransposeChange,
  tempo,
  transpose,
  playing,
}) => {
  return (
    <nav className={styles.playControls}>
      <Button onClick={onPlayClick} bindKey="Space">
        {playing ? <PauseIcon /> : <PlayIcon />}
      </Button>
      <Knob
        onChange={onTempoChange}
        min={BPM.MIN}
        max={BPM.MAX}
        defaultValue={DEFAULTS.BPM}
        value={tempo}
        direction={'vertical'}
        step={1}
        label={'TEMPO'}
      />
      <Knob
        onChange={onTransposeChange}
        min={TRANSPOSE.MIN}
        max={TRANSPOSE.MAX}
        defaultValue={DEFAULTS.TRANSPOSE}
        value={transpose}
        direction={'vertical'}
        step={1}
        label={'TRANSPOSE'}
      />
    </nav>
  );
};

export { type Props };

export default PlayControls;
