#pragma once
#include <log.hpp>
#include <bitset>

#define GameChecksLegalMoved true // the game will check if a move is legal not neded for training
#define stateSize 84
#define Training true
#define DEBUG false

#define U_computation(edge) (this->cpuct * ((1-doRand) * edge->P + doRand * nu[idx] ) * std::sqrt(Nb) / (1 + edge->N))


// runn setting
#define runVersion 1
#define loadVersion -1

// Net settings
#define MaxQuDefault -99999
#define reg_const 0.000001
#define Alpha 0.9
#define learningRage 0.1

// simulation setting
#define MCTSSimulations 50
#define cpuct_ 0.4f
#define ProbabiliticMoves 3
#define threads 0				// replace the predict method with C++ tensorflow predict or find a way to allow for multithreading

// memory setting
#define memory_size 1

// self play
#define EPOCHS 30
#define probabilitic_moves 2 // how manny moves are prabilistic in the begining of the game to aid in exploration

// training
#define Training_loops 5
#define Training_batch 256
#define Training_epochs 10

// turney
#define Turnement_probabiliticMoves 2
#define TurneyEpochs 20
#define scoringThreshold 1.3

// console
#define RenderTrainingProgress true
#define RenderGenAndTurneyProgress true
//#define RenderGameProgress true;


typedef std::bitset<stateSize> IDType;
