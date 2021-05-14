import os
import zmq
import time
import pyaudio
import random
from multiprocessing import Process, Pipe, Value
from threading import Thread
from multiprocessing.connection import Connection

from zmq import Socket
import numpy as np

IPC_ENDPOINT = 'ipc://recogsock'
DONE_ENDPOINT = 'ipc://recogsockdone'

result_queue = []
found_sleep = False


class AudioHandler(object):
    def __init__(self, callback):
        self.reader, self.writer = Pipe(False)
        self.exiter = Value('b', False)
        self.p: Process = None
        self.t: Thread = None
        self.buf = np.empty((0,), dtype=np.float32)
        self.callback = callback

        self.FORMAT = pyaudio.paFloat32
        self.CHANNELS = 1
        self.RATE = 22050
        self.CHUNK = 1024
        self.SAMPLE_LEN = 24000
        self.sleepy = (self.CHUNK / self.RATE) * 0.9
        self.skippy = 0

    def start(self):
        self.p = Process(
            target=AudioHandler._start_process,
            args=(self.FORMAT, self.CHANNELS, self.RATE, self.CHUNK, self.writer, self.exiter)
        )
        self.p.start()
        self.t = Thread(
            target=self._start_thread
        )
        self.t.start()

    def _start_thread(self):
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
            while self.buf.shape[0] > self.SAMPLE_LEN:
                if skipping:
                    self.callback(False)
                    skipping = False
                if random.randint(0, 300) == 0:
                    self.callback(True)
                    skipping = True
                    self.skippy = self.SAMPLE_LEN

                self.buf = self.buf[self.CHUNK:]

            time.sleep(self.sleepy)

    @staticmethod
    def _start_process(fmt, channels, rate, chunk, conn: Connection, should_exit):
        def cb(in_data, frame_count, time_info, flag):
            conn.send_bytes(in_data)
            return None, pyaudio.paContinue

        p = pyaudio.PyAudio()
        stream = p.open(format=fmt,
                        channels=channels,
                        rate=rate,
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


def main():
    context = zmq.Context()
    socket: Socket = context.socket(zmq.PUB)

    done: Socket = context.socket(zmq.SUB)
    done.subscribe('')

    socket.connect(IPC_ENDPOINT)
    done.connect(DONE_ENDPOINT)

    def cb(found):
        socket.send_string('true' if found else 'false')
        print('found!', found)
    audio_handler = AudioHandler(cb)
    audio_handler.start()
    while True:
        try:
            string = done.recv_string()
            if string == 'done':
                break
        except zmq.ZMQError as err:
            print('Error while receiving: ', err)
        time.sleep(1)

    audio_handler.stop()
    socket.close()
    done.close()


if __name__ == '__main__':
    main()
