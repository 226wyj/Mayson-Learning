import numpy as np
import scipy.io
from sklearn.metrics import accuracy_score
import matplotlib.pyplot as plt


class LogisticRegression:
    def __init__(self, d=100, opt='gd', N=10000, lr=1e-7, ls=False):
        # parameters
        self.w = np.zeros([d, 1])

        # hyper-parameters
        self.l2 = 1.0
        self.learning_rate = lr

        # train setting
        self.num_iter = N
        self.optimizer = opt
        self.lr_sched = ls

    def fit(self, X: np.ndarray, y: np.ndarray):
        cost_list = []
        if self.optimizer == 'gd':
            for i in range(self.num_iter):
                dF = self._grad(X, y)
                self.w = self.w - self.learning_rate * dF
                cost_list.append(self.cost(X, y))
        elif self.optimizer == 'sgd':
            for i in range(self.num_iter):
                idx = np.random.randint(X.shape[0], size=1)
                Xi = X[idx, :]
                yi = y[idx, :]
                dF = self._grad(Xi, yi)
                if self.lr_sched:
                    self.w = self.w - self.learning_rate / (i + 1) * dF
                else:
                    self.w = self.w - self.learning_rate * dF
                cost_list.append(self.cost(X, y))
        else:
            exit()
        return cost_list
        # print("#Iteration {}: cost {:.2f}, acc: {:.2f}".format(i, self.cost(X, y), self.score(X, y)))

    def predict(self, X):
        return self.sigmoid(X @ self.w)

    @staticmethod
    def sigmoid(z):
        return 1 / (1 + np.exp(-z))

    def _grad(self, X, y):
        return -X.T @ (y - self.sigmoid(X @ self.w)) + 2 * self.l2 * self.w

    def cost(self, X, y):
        ans = - (np.dot(y.flatten(), np.log(self.sigmoid(X @ self.w))).flatten() +
                 np.dot((1 - y).flatten(), np.log(1 - self.sigmoid(X @ self.w)).flatten())) + \
              self.l2 * (np.dot(self.w.flatten(), self.w.flatten()))
        return ans.item()

    def score(self, X, y):
        y_hat = np.rint(self.predict(X)).astype('int32').flatten()
        y = y.astype('int32').flatten()
        return accuracy_score(y_true=y, y_pred=y_hat)


def plot_cost(y1, y2, y3):
    x1 = list(range(1, len(y1) + 1))
    plt.plot(x1, y1, 'r--', label='part1')
    x2 = list(range(1, len(y2) + 1))
    plt.plot(x2, y2, 'g--', label='part2')
    x3 = list(range(1, len(y3) + 1))
    plt.plot(x3, y3, 'b--', label='part3')
    plt.xlabel('num iterations')
    plt.ylabel('average cost')
    plt.legend()
    plt.show()


if __name__ == "__main__":
    data = scipy.io.loadmat('data.mat')
    X_train = data['X']
    y_train = data['y']
    X_test = data['X_test']

    d = X_train.shape[1]

    model1 = LogisticRegression(d, opt='gd', N=10000)
    cost1 = model1.fit(X_train, y_train)

    model2 = LogisticRegression(d, opt='sgd', N=10000)
    cost2 = model2.fit(X_train, y_train)

    model3 = LogisticRegression(d, opt='sgd', N=10000, ls=True, lr=1e-4)
    cost3 = model3.fit(X_train, y_train)

    plot_cost(cost1, cost2, cost3)
