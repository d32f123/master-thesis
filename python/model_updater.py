import os
import sys
import glob

import numpy as np
from transformer import mfcc_transform, preprocess
from config import *


def main():
    root_path = sys.argv[1]
    patterns_to_update = []
    if len(sys.argv) > 2:
        patterns_to_update = sys.argv[2].split(',')
    patterns_path = os.path.join(root_path, patterns_rel_path)
    models_path = os.path.join(root_path, models_rel_path)
    preprocesseds_path = os.path.join(root_path, preprocessed_rel_path)

    for pattern_pathname in os.listdir(patterns_path):
        pattern_name = pattern_pathname.replace(pattern_path_suffix, "")
        if len(patterns_to_update) > 0 and pattern_name not in patterns_to_update:
            continue

        pattern_path = os.path.join(patterns_path, pattern_pathname)
        model_path = os.path.join(models_path, pattern_name)
        preprocessed_path = os.path.join(preprocesseds_path, pattern_name)
        os.makedirs(model_path, exist_ok=True)
        for x in glob.glob(os.path.join(model_path, '*')):
            os.unlink(x)
        os.makedirs(preprocessed_path, exist_ok=True)
        for x in glob.glob(os.path.join(preprocessed_path, '*')):
            os.unlink(x)

        os.link(os.path.join(pattern_path, active_path), os.path.join(model_path, active_path))
        recordings_pathnames = glob.glob(os.path.join(pattern_path, recordings_glob))
        for recording_path in recordings_pathnames:
            recording_name = recording_path[recording_path.rindex('/') + 1:].replace(recording_extension, '')
            out_recording_path = os.path.join(model_path, recording_name + model_extension)
            preprocessed_recording_path = os.path.join(preprocessed_path, recording_name + recording_extension)
            try:
                preprocess(recording_path, preprocessed_recording_path)
            except ValueError as err:
                print('error while preprocessing', err)
                continue

            with open(preprocessed_recording_path, mode='rb') as preprocessed_file:
                recording_bytes = np.frombuffer(preprocessed_file.read(), dtype=np.float32)
            transformed_bytes, _ = mfcc_transform(recording_bytes, sr, input_dim)
            np.save(out_recording_path, transformed_bytes)


if __name__ == '__main__':
    main()
