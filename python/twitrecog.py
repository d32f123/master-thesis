import signal
import time
import sys
from typing import *
from multiprocessing import Process, Pipe, Value
from multiprocessing.connection import Connection
from threading import Thread

import zmq
import pyaudio
import numpy as np

from config import sr, input_dim, dtw_k, dtw_cost, channels
from transformer import mfcc_transform, audio_length
from dtw import DTW
from predictor import Predictor

DATA_ENDPOINT = 'ipc://twitrecog_data'
COMMAND_ENDPOINT = 'ipc://twitrecog_command'

result_queue = []
found_sleep = False


class AudioHandler(object):
    def __init__(self, callback, predictors: List[Predictor]):
        self.reader, self.writer = Pipe(False)
        self.exiter = Value('b', False)
        self.p: Process = None
        self.t: Thread = None
        self.buf = np.empty((0,), dtype=np.float32)
        self.callback = callback
        self.predictors = predictors

        self.CHUNK = 1536
        self.sleepy = (self.CHUNK / sr / 2)
        self.skippy = 0

    def start(self):
        self.p = Process(
            target=AudioHandler._start_process,
            args=(self.CHUNK, self.writer, self.exiter)
        )
        self.p.start()
        self.t = Thread(
            target=self._start_thread
        )
        self.t.start()

    def _predict(self) -> Tuple[bool, int]:
        for predictor in self.predictors:
            sample_len = predictor.get_sample_len()
            if sample_len > self.buf.shape[0]:
                continue
            x = self.buf[:sample_len]
            x_transformed, _ = mfcc_transform(x, sr, input_dim)
            if predictor.predict(x_transformed):
                return True, predictor.get_sample_len()
        return False, 0

    def _start_thread(self):
        min_sample_len = np.min([x.get_sample_len() for x in self.predictors])
        skipping = False
        while not self.exiter.value:
            recv = np.frombuffer(self.reader.recv_bytes(), np.float32)
            self.buf = np.append(self.buf, recv)

            if self.skippy > 0:
                skipped = self.skippy
                if self.buf.shape[0] < skipped:
                    skipped = self.buf.shape[0]
                self.skippy -= skipped
                self.buf = self.buf[skipped:]
            while self.skippy == 0 and self.buf.shape[0] > min_sample_len:
                if skipping:
                    self.callback(False)
                    skipping = False
                skipping, self.skippy = self._predict()
                if skipping:
                    self.callback(True)

                self.buf = self.buf[self.CHUNK:]

            time.sleep(self.sleepy)

    @staticmethod
    def _start_process(chunk, conn: Connection, should_exit):
        def cb(in_data, frame_count, time_info, flag):
            conn.send_bytes(in_data)
            return None, pyaudio.paContinue

        def signal_cb(_, __):
            should_exit.value = True

        signal.signal(signal.SIGINT, signal_cb)
        p = pyaudio.PyAudio()
        print('process started')
        stream = p.open(format=pyaudio.paFloat32,
                        channels=channels,
                        rate=sr,
                        input=True,
                        output=False,
                        stream_callback=cb,
                        frames_per_buffer=chunk)
        while not should_exit.value:
            time.sleep(0.1)
        stream.close()
        p.terminate()

    def stop(self):
        self.exiter.value = True
        self.p.join()
        self.t.join()


def get_models(root: str) -> Tuple[List[List[Tuple[np.ndarray, int]]], List[Tuple[np.ndarray, int]]]:
    import os
    import glob
    from config import models_glob, models_rel_path, active_path, preprocessed_rel_path, model_extension
    from config import recording_extension

    ret = []
    false_ret = []
    path = os.path.join(root, models_rel_path)
    for pattern_name in os.listdir(path):
        pattern_path = os.path.join(path, pattern_name)
        with open(os.path.join(pattern_path, active_path), 'r') as active_file:
            if active_file.read() == 'false':
                continue
        if pattern_name == 'false':
            target = false_ret
        else:
            ret.append([])
            target = ret[-1]

        pattern_models_paths = [os.path.join(pattern_path, pathname)
                                for pathname in glob.glob(os.path.join(pattern_path, models_glob))]
        for pattern_model_path in pattern_models_paths:
            pattern_model_idx = pattern_model_path[pattern_model_path.rindex("/") + 1:].replace(model_extension, '')
            audio_len = int(np.ceil(audio_length(os.path.join(
                root, preprocessed_rel_path,
                pattern_name,
                pattern_model_idx + recording_extension
            )) * sr))
            target.append((np.load(pattern_model_path), audio_len))

    return ret, false_ret


def main():
    context = zmq.Context()
    socket: zmq.Socket = context.socket(zmq.PUB)

    done: zmq.Socket = context.socket(zmq.SUB)
    done.subscribe('')
    done.setsockopt(zmq.RCVTIMEO, 2000)

    socket.connect(DATA_ENDPOINT)
    done.connect(COMMAND_ENDPOINT)

    predictors = []
    models, false_model = get_models(sys.argv[1])
    for model in models:
        x = [recording for recording, _ in model]
        sample_len = int(np.ceil(np.average([recording_len for _, recording_len in model])))
        predictor = DTW(dtw_cost, sample_len)
        predictor.compile(dtw_k)
        predictor.train(x, [[0.0, 1.0] for _ in range(len(model))])
        predictors.append(predictor)

    def cb(found):
        socket.send_string('true' if found else 'false')
        print('found!', found)

    audio_handler = AudioHandler(cb, predictors)
    audio_handler.start()

    signal_received = False

    def signal_cb(_, __):
        nonlocal signal_received
        signal_received = True

    signal.signal(signal.SIGINT, signal_cb)
    print('started')
    socket.send_string('started')
    while not signal_received:
        try:
            string = done.recv_string()
            if string == 'done':
                signal_received = True
                break
        except zmq.ZMQError as err:
            print('Error while receiving: ', err)
        time.sleep(1)
    audio_handler.stop()
    socket.close()
    done.close()


if __name__ == '__main__':
    main()
