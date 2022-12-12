# AlphaZero
Server baised alpha Zero server and client.

## Download and Build
Build and Install the following libraries according to there installation instructuions provided in there README.md
* [spdlog](https://github.com/gabime/spdlog#Install)
* [sockpp](https://github.com/fpagliughi/sockpp#Building-the-Library)
* Optionally install CUDA 11.3+ (if installed cuDNN is also required)
* download and extract pytorch from https://pytorch.org/

Then compile as follows

* Clone this reposetory and switch to the working branch as main is currently broken.
* Compile code in ``AlphaZeroPytorch`` using ``cmake``. like so
```
cmake /your/AlphaZeroPytorch/Path -DCMAKE_PREFIX_PATH=your/pytorch/path
```
* build with cmake

## Available Programs
### Training the AI
In order to train the AI use the ``train`` excecutable provided by AlphaZeroPytorch
### Run Tests
In order to run the test suite use the ``runTest`` excecutable provided by AlphaZeroPytorch
### Run AI Server
In order to run the test suite use the ``runServer`` excecutable provided by AlphaZeroPytorch

### Build elo Ratings
In order to build the elo ratings of the various agens use the ``eloRaiting`` excecutable provided by AlphaZeroPytorch note that the elo server must be running. It is located in the ``elo`` directory and can be run with ```python3 server.py```
