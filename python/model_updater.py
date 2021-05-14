import os
import sys
import glob
import numpy as np
from transformer import mfcc_transform
from config import sr, input_dim

pattern_path_suffix = "_pattern"
recording_suffix = ".raw"
recordings_glob = "*" + recording_suffix
model_suffix = ".model"
active_path = 'active.txt'


def main():
    root_path = sys.argv[1]
    patterns_to_update = []
    if len(sys.argv) > 2:
        patterns_to_update = sys.argv[2].split(',')
    patterns_path = os.path.join(root_path, "patterns")
    models_path = os.path.join(root_path, "model")

    for pattern_pathname in os.listdir(patterns_path):
        pattern_name = pattern_pathname.replace(pattern_path_suffix, "")
        if len(patterns_to_update) > 0 and pattern_name not in patterns_to_update:
            continue

        pattern_path = os.path.join(patterns_path, pattern_pathname)
        model_path = os.path.join(models_path, pattern_name)
        os.makedirs(model_path, exist_ok=True)
        for x in glob.glob(os.path.join(model_path, '*')):
            os.unlink(x)

        os.link(os.path.join(pattern_path, active_path), os.path.join(model_path, active_path))
        recordings_pathnames = glob.glob(os.path.join(pattern_path, recordings_glob))
        for recording_path in recordings_pathnames:
            recording_name = recording_path[recording_path.rindex('/') + 1:].replace(recording_suffix, '')
            out_recording_path = os.path.join(model_path, recording_name + model_suffix)

            with open(recording_path, mode='rb') as recording_file:
                recording_bytes = np.frombuffer(recording_file.read(), dtype=np.float32)
            transformed_bytes, _ = mfcc_transform(recording_bytes, sr, input_dim)
            with open(out_recording_path, mode='wb') as recording_file:
                recording_file.write(transformed_bytes.tobytes())


if __name__ == '__main__':
    main()
