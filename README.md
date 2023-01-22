# DSRRANS

This work introduces a novel data-driven framework to formulate explicit algebraic Reynolds-averaged Navier-Stokes
(RANS) turbulence closures. Recent years have witnessed a blossom in applying machine learning (ML) methods to
revolutionize the paradigm of turbulence modeling. However, due to the black-box essence of most ML methods, it is
currently hard to extract interpretable information and knowledge from data-driven models. To address this critical
limitation, this work leverages deep learning with symbolic regression methods to discover hidden governing equations of
Reynolds stress models. Specifically, the Reynolds stress tensor is decomposed into linear and non-linear parts. While
the linear part is taken as the regular linear eddy viscosity model, a long short-term memory neural network is employed
to generate symbolic terms on which tractable mathematical expressions for the non-linear counterpart are built. A novel
reinforcement learning algorithm is employed to train the neural network to produce best-fitted symbolic expressions.
Within the proposed framework, the Reynolds stress closure is explicitly expressed in algebraic forms, thus allowing for
direct functional inference. On the other hand, the Galilean and rotational invariance are craftily respected by
constructing the training feature space with independent invariants and tensor basis functions. The performance of the
present methodology is validated through numerical simulations of three different canonical flows that deviate in
geometrical configurations. The results demonstrate promising accuracy improvements over traditional RANS models,
showing the generalization ability of the proposed method. Moreover, with the given explicit model equations, it can be
easier to interpret the influence of input features on generated models.

# Installation

### Install dso package

To install all dependencies for performing training, run `pip install -e ./dso[all]`


### Install OpenFOAM package

To install turbulence model module, run the following command in `TurbulenceModels` folder `wclean && wmake`

# Getting started

### Run the training

To start a training, run `python -m dso.run dsoTurbulenceConfig.json` in `dso/dso`. Change the dataset path to use your own data.

### Run the simulation

You can use the `runSimulation.sh` script to start the simulation after finishing the compilation of turbulence model.

### Configuring runs



