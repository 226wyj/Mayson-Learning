# UC Berkeley CS 189 Introduction to Machine Learning by Prof. Jonathan Shewchuk

| LEC# | DATE        | TITLE                                             | READING                                | ASSIGNMENT |
| ---- | ----------- | ------------------------------------------------- | -------------------------------------- | ---------- |
| 10 ✅ | February 27 | Regression: fitting curves to data.               | ISL 4–4.3                              |            |
| 11 ✅ | March 4     | Newton's method and logistic regression.          | ISL 4.4.3, 7.1, 9.3.3; ESL 4.4.1.      | DIS6       |
| 12 ✅ | March 6     | Statistical justifications for regression.        | ESL 2.5, 2.9.                          |            |
| 13 ✅ | March 11    | Ridge regression and LASSO                        | ISL 6–6.1.3, 6.2–6.2.1; ESL 3.4–3.4.3. | DIS7       |
| 14 ✅ | April 1     | Kernels                                           | ISL 9.3.2; ESL 12.3-12.3.1             |            |
| 15   | March 13    | Decision trees                                    | ISL 8-8.1                              | DIS8, HW4  |
| 16   | March 20    | More decision trees                               | ISL 8.2                                | DIS9       |
| 17   | April 3     | Neural networks.                                  | ESL 11.3-11.4                          | HW5        |
| 18   | April 8     | Neuron biology                                    |                                        | DIS10      |
| 19   | April 10    | Heuristics for faster training.                   | ESL 11.5, 11.7                         |            |
| 20   | April 15    | Unsupervised learning.                            | ISL 10-10.2                            | DIS11      |
| 21   | April 17    | SVD and PCA                                       | ISL 10.3                               |            |
| 22   | April 22    | Spectral graph partitioning and graph clustering. | GP 1.2–1.4, 2.1, 2.2, 2.4, 2.5         | DIS12      |
| 23   | April 24    | Graph clustering with multiple eigenvectors.      | GP E.2                                 | HW6        |
| 24   | April 29    | AdaBoost                                          | ESL 10-10.5; ISL 2.2.3                 | DIS 13     |
| 25   | May 1       | K-nearest neighbor queries.                       | -                                      | -          |

---

- Lecture 10 (February 27): Regression: fitting curves to data. The 3-choice menu of regression function + loss function + cost function. Least-squares linear regression as quadratic minimization and as orthogonal projection onto the column space. The design matrix, the normal equations, the pseudoinverse, and the hat matrix (projection matrix). Logistic regression; how to compute it with gradient descent or stochastic gradient descent. 
- Lecture 11 (March 4): Newton's method and its application to logistic regression. LDA vs. logistic regression: advantages and disadvantages. ROC curves. Weighted least-squares regression. Least-squares polynomial regression. 
- Lecture 12 (March 6): Statistical justifications for regression. The empirical distribution and empirical risk. How the principle of maximum likelihood motivates the cost functions for least-squares linear regression and logistic regression. The bias-variance decomposition; its relationship to under-fitting and overfitting; its application to least-squares linear regression. 
- Lecture 13 (March 11): Ridge regression: penalized least-squares regression for reduced overfitting. How the principle of maximum a posteriori (MAP) motivates the penalty term (aka Tikhonov regularization). Subset selection. Lasso: penalized least-squares regression for reduced overfitting and subset selection. 
- Lecture 14 (April 1): Kernels. Kernel ridge regression. The polynomial kernel. Kernel perceptrons. Kernel logistic regression. The Gaussian kernel.
- Lecture 15 (March 13): Decision trees; algorithms for building them. Entropy and information gain. 
- Lecture 16 (March 20): More decision trees: multivariate splits; decision tree regression; stopping early; pruning. Ensemble learning: bagging (bootstrap aggregating), random forests. 
- Lecture 17 (April 3): Neural networks. Gradient descent and the back-propagation algorithm. 
- Lecture 18 (April 8): Neuron biology: axons, dendrites, synapses, action potentials. Differences between traditional computational models and neuronal computational models. Back-propagation with softmax outputs and logistic loss. Unit saturation, aka the vanishing gradient problem, and ways to mitigate it. Heuristics for avoiding bad local minima. 
- Lecture 19 (April 10): Heuristics for faster training. Heuristics for avoiding bad local minima. Heuristics to avoid overfitting. Convolutional neural networks. Neurology of retinal ganglion cells in the eye and simple and complex cells in the V1 visual cortex. 
- Lecture 20 (April 15): Unsupervised learning. Principal components analysis (PCA). Derivations from maximum likelihood estimation, maximizing the variance, and minimizing the sum of squared projection errors. Eigenfaces for face recognition. 
- Lecture 21 (April 17): The singular value decomposition (SVD) and its application to PCA. Clustering: k-means clustering aka Lloyd's algorithm; k-medoids clustering; hierarchical clustering; greedy agglomerative clustering. Dendrograms. 
- Lecture 22 (April 22): Spectral graph partitioning and graph clustering. Relaxing a discrete optimization problem to a continuous one. The Fiedler vector, the sweep cut, and Cheeger's inequality. The vibration analogy. Greedy divisive clustering. The normalized cut and image segmentation. Read my survey of Spectral and Isoperimetric Graph Partitioning, Sections 1.2–1.4, 2.1, 2.2, 2.4, 2.5, Transactions on Pattern Analysis and Machine Intelligence 22(8):888–905, 2000.
- Lecture 23 (April 24): Graph clustering with multiple eigenvectors. Random projection. Latent factor analysis (aka latent semantic indexing). The geometry of high-dimensional spaces.
  - Read the Wikipedia page on latent semantic analysis. 
  - Section E.2 of my survey. For reference: Andrew Y. Ng, Michael I. Jordan, and Yair Weiss, On Spectral Clustering: Analysis and an Algorithm (PostScript format), Advances in Neural Information Processing Systems 14 (Thomas G. Dietterich, Suzanna Becker, and Zoubin Ghahramani, editors), pages 849–856, the MIT Press, September 2002. For reference: Sanjoy Dasgupta and Anupam Gupta, An Elementary Proof of a Theorem of Johnson and Lindenstrauss, Random Structures and Algorithms 22(1)60–65, January 2003.
  - HW 6
- Lecture 24 (April 29): AdaBoost, a boosting method for ensemble learning. Nearest neighbor classification and its relationship to the Bayes risk. 
  - Read ESL, Sections 10–10.5, and ISL, Section 2.2.3. For reference: Yoav Freund and Robert E. Schapire, A Decision-Theoretic Generalization of On-Line Learning and an Application to Boosting, Journal of Computer and System Sciences 55(1):119–139, August 1997.
- Lecture 25 (May 1): The exhaustive algorithm for k-nearest neighbor queries. Speeding up nearest neighbor queries. Voronoi diagrams and point location. k-d trees. Application of nearest neighbor search to the problem of geolocalization: given a query photograph, determine where in the world it was taken. If I like machine learning, what other classes should I take? 
  - For reference: the best paper I know about how to implement a k-d tree is Sunil Arya and David M. Mount, Algorithms for Fast Vector Quantization, Data Compression Conference, pages 381–390, March 1993. For reference: the IM2GPS web page, which includes a link to the paper.

<!-- *@ Lecture 6 (February 11): Decision theory: the Bayes decision rule and optimal risk. Generative and discriminative models. 
[✅] Lecture {1.5h}
[✅] Read ISL, Section 4.4.1. {0.5h}
[✅] Discussion 3 {1h}
*[] HW2 {3h}
[✅] Summary Blog {1.5h}

@ Lecture 7 (February 13): Gaussian discriminant analysis, including quadratic discriminant analysis (QDA) and linear discriminant analysis (LDA). Maximum likelihood estimation (MLE) of the parameters of a statistical model. Fitting an isotropic Gaussian distribution to sample points. 
[✅][✅] Lecture
*[][] Read (selectively) the Wikipedia page on maximum likelihood.
[✅][✅] Discussion 4

@ Lecture 8 (February 20): Eigenvectors, eigenvalues, and the eigendecomposition. The Spectral Theorem for symmetric real matrices. The quadratic form and ellipsoidal isosurfaces as an intuitive way of understanding symmetric matrices. Application to anisotropic normal distributions (aka Gaussians). 
[✅][✅] Lecture


@ Lecture 9 (February 25): Anisotropic normal distributions (aka Gaussians). MLE, QDA, and LDA revisited for anisotropic Gaussians. 
[✅][✅] Lecture
*[] Read Chuong Do's notes on the multivariate Gaussian distribution.
*[] Read ISL, Sections 4.4 and 4.5.
*[] Discussion 5 -->