import librosa
import scipy.spatial
import numpy as np
from predictor import Predictor


class DTW(Predictor):
    def __init__(self, cost_function, sample_len):
        self.cost_function = cost_function
        self.coeff = 1.1
        self.train_data = []
        self.threshold = 0
        self.cost = 0
        self.sample_len = sample_len

    def get_sample_len(self) -> int:
        return self.sample_len

    def compile(self, k):
        self.coeff = k

    @staticmethod
    def _dtw(X, Y, C):
        return librosa.sequence.dtw(C=C, backtrack=False)[-1, -1] / (X.shape[1] + Y.shape[1])

    def _cost(self, X, Y):
        return scipy.spatial.distance.cdist(X.T, Y.T, metric=self.cost_function)

    def _test_samples(self, X, Y):
        C = self._cost(X, Y)
        return DTW._dtw(X, Y, C)

    def train(self, X, Y):
        # Leave only xs which are labeled with 1
        X = [x for x, y in zip(X, Y) if y[1] == 1.0]

        # X_len = [x.shape[0] for x in X]
        # self.average_len = int(np.ceil(np.average(X_len)))
        self.train_data = X

        train_costs = [np.min([self._test_samples(x, y) for i, x in enumerate(X) if i != j]) for j, y in enumerate(X)]
        average_cost = np.average(train_costs)
        self.cost = average_cost
        self.threshold = self.cost * self.coeff

    def predict(self, X):
        cost = np.min([self._test_samples(X, y) for y in self.train_data])
        return cost < self.threshold
