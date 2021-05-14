import numpy as np
from typing import *
import librosa


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
