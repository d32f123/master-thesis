from typing import *

import librosa
import numpy as np
from sox.core import sox as sox_runner

from config import sr


def ensure_audio_length(x: np.ndarray, sr: int, length_secs: float) -> np.ndarray:
    length = int(np.ceil(length_secs * sr))
    return np.resize(x, length)


def mfcc_transform(
        X: Union[np.ndarray, List[Any]],
        sr: int,
        input_dim: Union[int, Tuple[int, int]]
) -> Tuple[Union[np.ndarray, List[any]], Tuple[int, int]]:
    n_mfcc = input_dim
    if type(input_dim) is tuple:
        n_mfcc = input_dim[1]
        input_dim = input_dim[0]
    shape = (n_mfcc, input_dim)

    if type(X) is not list and len(X.shape) == 1:
        base_n = X.shape[0]
    else:
        base_n = X[0].shape[0]
    hop = base_n // (input_dim - 1) if base_n % input_dim == 0 else (base_n // input_dim + 1)
    frame = hop * 6

    if type(X) is not list and len(X.shape) == 1:
        return librosa.feature.mfcc(X, sr, n_mfcc=n_mfcc, n_fft=frame, hop_length=hop), shape
    if type(X) is not list:
        return np.apply_along_axis(librosa.feature.mfcc, 1, X, sr, n_mfcc=n_mfcc, n_fft=frame, hop_length=hop), shape
    else:
        return [librosa.feature.mfcc(x, sr, n_mfcc=n_mfcc, n_fft=frame, hop_length=hop) for x in X], shape


def audio_length(filename):
    with open(filename, 'rb') as file:
        y = np.frombuffer(file.read(), dtype=np.float32)
    return librosa.get_duration(y, sr)


def preprocess(in_file, out_file, trigger_level=7, silence_percent=2):
    ok, _, err = sox_runner([
        '-r', str(sr),
        '-e', 'floating-point',
        '-b', '32',
        in_file,
        '-r', str(sr),
        '-e', 'floating-point',
        '-b', '32',
        out_file,
        'vad', '-s', '0.25', '-t', str(trigger_level),
        'silence', '1', '4000', '{}%'.format(silence_percent),
        'reverse', 'silence', '1', '4000', '{}%'.format(silence_percent), 'reverse'
    ])
    if ok != 0:
        raise ValueError(err)

    audio_len = audio_length(out_file)
    if audio_len < 0.2:
        if silence_percent < 2 and trigger_level < 2:
            raise ValueError(out_file + ' shrunk to 0')
        if silence_percent > 1:
            preprocess(in_file, out_file, trigger_level, silence_percent - 1)
        else:
            preprocess(in_file, out_file, trigger_level - 1, silence_percent)
