# AlphaZero
Server baised alpha Zero server and client.

## Download and Build
Build and Install the following libraries according to there installation instructuions provided in there README.md
* [spdlog](https://github.com/gabime/spdlog#Install)
* [sockpp](https://github.com/fpagliughi/sockpp#Building-the-Library)
* Optionally install CUDA 11.3+
* Pytorch will automatically be installed

```
$ git clone https://github.com/JulianWww/AlphaZero && cd AlphaZero/AlphaZeroPytorch && mkdir build && cd build
$ cmake .. & make
```

## Available Programs
### Training the AI
In order to train the AI use the ``train`` excecutable provided by AlphaZeroPytorch
### Run Tests
In order to run the test suite use the ``runTest`` excecutable provided by AlphaZeroPytorch
### Run AI Server
In order to run the test suite use the ``runServer`` excecutable provided by AlphaZeroPytorch

### Build elo Ratings
In order to build the elo ratings of the various agens use the ``eloRaiting`` excecutable provided by AlphaZeroPytorch note that the elo server must be running. It is located in the ``elo`` directory and can be run with ```python3 server.py```
