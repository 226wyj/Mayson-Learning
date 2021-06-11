import numpy as np
from numpy import genfromtxt

import scipy.io

from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
from sklearn.tree import DecisionTreeClassifier


class Node:
    def __init__(self, depth=0):
        self.feature_idx: int = -1
        self.threshold: float = 0.0
        self.split_rule = (self.feature_idx, self.threshold)

        self.left: Node = None
        self.right: Node = None
        self.label: int = -1

        self.depth = depth

    def leaf(self):
        return self.label >= 0


class DecisionTree:
    """
    d features and n training points where each feature is real-valued and each label is binary (0/1)
    """

    def __init__(self, features, max_depth=5):
        """
        Initialization of a decision tree
        """
        self.root: Node = None
        self.X = None
        self.y = None
        self.features = features
        self.d = len(self.features)
        self.max_depth = max_depth

    @staticmethod
    def entropy(y):
        """
        TODO: implement a method that calculates the binary entropy given all the labels
        """
        if y.size == 0:
            return 0
        pc = np.where(y == 0)[0].size / y.size
        pd = 1.0 - pc
        if np.abs(pc) < 1e-10 or np.abs(pd) < 1e-10:
            return 0
        return - (pc * np.log(pc) + pd * np.log(pd))

    @staticmethod
    def information_gain(X, y, thresh):
        """
        Calculates information gain given a vector of features and a split threshold
        """
        HS = DecisionTree.entropy(y)
        Sl, Sr = np.where(X < thresh)[0], np.where(X >= thresh)[0]
        if Sl.size == 0 or Sr.size == 0:
            return 0

        ySl, ySr = y[Sl], y[Sr]
        Hafter = (Sl.size * DecisionTree.entropy(ySl) + Sr.size * DecisionTree.entropy(ySr)) / (Sl.size + Sr.size)
        return HS - Hafter

    @staticmethod
    def gini_impurity(y):
        """
        Implement a method that calculates the gini impurity given all the labels
        """
        if y.size == 0:
            return 0
        pc = np.where(y == 0)[0].size / y.size
        pd = 1.0 - pc
        if np.abs(pc) < 1e-10 or np.abs(pd) < 1e-10:
            return 0
        return 1.0 - pc ** 2 - pd ** 2

    @staticmethod
    def gini_purification(X, y, thresh):
        """
        Implement a method that calculates reduction in impurity gain given a vector of features and a split threshold
        """
        IS = DecisionTree.gini_impurity(y)
        Sl, Sr = np.where(X < thresh)[0], np.where(X >= thresh)[0]
        ySl, ySr = y[Sl], y[Sr]
        Iafter = (Sl.size * DecisionTree.gini_impurity(ySl) + Sr.size * DecisionTree.gini_impurity(ySr)) / (Sl.size + Sr.size)
        return IS - Iafter

    def segmenter(self, S):
        """
        Compute entropy gain for all single-dimension splits, return the feature and the threshold for the split that has maximum gain
        """
        feature_idx = 0
        threshold = 0
        max_info_gain_so_far = -1

        XS = self.X[S, :]
        yS = self.y[S]
        for i in range(self.d):
            XSi = XS[:, i]
            p = np.argsort(XSi)
            for j in range(XSi.size - 1):
                v1, v2 = XSi[p][j], XSi[p][j + 1]
                if v1 != v2:  # try splitting between each pair of unequal consecutive values
                    thresh = (v1 + v2) / 2
                    info_gain = self.information_gain(XSi, yS, thresh)
                    if info_gain > max_info_gain_so_far:
                        feature_idx, threshold = i, thresh
                        max_info_gain_so_far = info_gain
        return feature_idx, threshold, max_info_gain_so_far

    def fit(self, X, y):
        """
        Fit the model to a training set.
        Stooping criteria used:
        1 Data not separable
        2 Max Depth reached
        """
        self.X = X
        self.y = y
        S = np.arange(X.shape[0]).astype('int32')
        self.root = self._grow_tree(S, d=0)

    def predict(self, X: np.ndarray):
        """
        Predict the labels for input data
        """
        if self.root is None:
            return None

        ret = []
        for XiT in X:
            node = self.root
            while True:
                if node.leaf():
                    ret.append(node.label)
                    break
                else:
                    node = node.left if XiT[node.feature_idx] < node.threshold else node.right

        return np.array(ret)

    def _grow_tree(self, S: np.ndarray, d):
        XS = self.X[S, :]
        yS = self.y[S]
        if np.all(y == y[0]):  # pure leaf
            return self._leaf_node(label=y[0], depth=d)

        elif d >= self.max_depth:  # reach max depth
            return self._leaf_node(label=self._get_majority_label(yS), depth=d)

        else:  # continue to separate sample points
            j, beta, info_gain = self.segmenter(S)
            if info_gain < 0:  # can not separate anymore
                return self._leaf_node(label=self._get_majority_label(yS), depth=d)  # pick majority vote as label

            Sl, Sr = S[np.where(XS[:, j] < beta)], S[np.where(XS[:, j] >= beta)]

            # if Sl.size == 0 or Sr.size == 0:
            #     return self._leaf_node(label=self._get_majority_label(yS), depth=d)

            return self._internal_node(feature_idx=j, threshold=beta,
                                       left=self._grow_tree(Sl, d + 1), right=self._grow_tree(Sr, d + 1), depth=d)

    @staticmethod
    def _get_majority_label(y: np.ndarray):
        return np.argmax(np.bincount(y))

    @staticmethod
    def _leaf_node(label, depth=0):
        leaf = Node(depth)
        leaf.label = label
        return leaf

    @staticmethod
    def _internal_node(feature_idx, threshold, left, right, depth=0):
        internal = Node(depth)
        internal.feature_idx = feature_idx
        internal.threshold = threshold
        internal.left = left
        internal.right = right
        return internal

    def __repr__(self):
        """
        One way to visualize the decision tree is to write out a __repr__ method
        that returns the string representation of a tree. Think about how to visualize
        a tree structure. You might have seen this before in CS61A.
        """

        def bfs(node, level=0):
            if node.label < 0:
                ret = "\t" * level + "%s < %s" % (self.features[node.feature_idx], node.threshold) + "\n"
                ret += bfs(node.right, level + 1)
                ret += bfs(node.left, level + 1)
                return ret
            else:
                return "\t" * level + 'leave node:' + str(node.label) + "\n"

        if not self.root:
            return ""
        else:
            return bfs(self.root)


if __name__ == "__main__":
    # dataset = "titanic"
    dataset = "spam"

    if dataset == "titanic":
        # Load titanic data
        path_train = 'datasets/titanic/titanic_training.csv'
        data = genfromtxt(path_train, delimiter=',', dtype=None)
        path_test = 'datasets/titanic/titanic_testing_data.csv'
        test_data = genfromtxt(path_test, delimiter=',', dtype=None)
        y = data[1:, 0]  # label = survived
        class_names = ["Died", "Survived"]

        # TODO: preprocess titanic dataset
        # Notes:
        # 1. Some data points are missing their labels
        # 2. Some features are not numerical but categorical
        # 3. Some values are missing for some features

    elif dataset == "spam":
        features = [
            "pain", "private", "bank", "money", "drug", "spam", "prescription",
            "creative", "height", "featured", "differ", "width", "other",
            "energy", "business", "message", "volumes", "revision", "path",
            "meter", "memo", "planning", "pleased", "record", "out",
            "semicolon", "dollar", "sharp", "exclamation", "parenthesis",
            "square_bracket", "ampersand"
        ]
        assert len(features) == 32

        # Load spam data
        path_train = './datasets/spam-dataset/spam_data.mat'
        data = scipy.io.loadmat(path_train)
        X = data['training_data']
        y = np.squeeze(data['training_labels'])
        X_test = data['test_data']
        class_names = ["Ham", "Spam"]

        X_train, X_val, y_train, y_val = train_test_split(X, y, test_size=0.2, random_state=1030)

        classifier = DecisionTree(features, max_depth=50)
        classifier.fit(X_train, y_train)
        y_pred = classifier.predict(X_val)
        acc = accuracy_score(y_val, y_pred)
        print(acc)
        print(classifier)

        base = DecisionTreeClassifier(random_state=0, criterion="entropy")
        base.fit(X_train, y_train)
        acc2 = base.score(X_val, y_val)
        print(acc2)

    elif dataset == "xor":
        X_train = np.array([[0, 0], [0, 1], [1, 0], [1, 1]]).astype('float64')
        y_train = np.array([0, 1, 1, 0]).astype('int32')
        features = ["x1", "x2"]
        classifier = DecisionTree(features)
        classifier.fit(X_train, y_train)
        print(classifier)

    else:
        raise NotImplementedError("Dataset %s not handled" % dataset)

    """
    TODO: train decision tree/random forest on different datasets and perform the tasks
    in the problem
    """
