#pragma once
#include <log.hpp>
#include <bitset>
#include <mutex>

#ifdef unix
#define UNIX
#endif

#ifdef UNIX
#define DEVICES "cuda:0"
#endif
#ifndef UNIX
#define DEVICES "cpu"
#endif

#define OPSMode 1

extern std::mutex console_mutex;
extern std::mutex rand_mutex;

/*
+---------+-------------+
| OPSMode | Description |
+---------+-------------+
| 1       | Run Server  |
+---------+-------------+
| 2       | Run Tester  |
+---------+-------------+
*/

#define GameChecksLegalMoved true // the game will check if a move is legal not neded for training
#define stateSize 84
#define Training true
#define DEBUG false

#define U_computation(edge) (this->cpuct * edge.P * std::sqrt((float)Nb) / (float)(1 + edge.N))


// runn setting
#define runVersion 2
#define loadVersion 57

// Net settings
#define MaxQuDefault -99999
#define reg_const 0.0001
#define learningRage 0.1
#define Momentum 0.9

// simulation setting
#define MCTSSimulations 50
#define cpuct_ 2.0f
#define ProbabiliticMoves 10
#define Alpha 0.9
#define EPSILON 0.2f

// memory setting
#define memory_size 30000

// self play
#define EPOCHS 1
#define GEN_THREADS 60
#define probabilitic_moves 10 // how manny moves are prabilistic in the begining of the game to aid in exploration

// training
#define Training_loops 20
#define Training_batch 256
#define Training_epochs 5

// turney
#define Turnement_probabiliticMoves 2
#define TurneyEpochs 1
#define TurneyThreads 20
#define scoringThreshold 1.3

// console
#define RenderTrainingProgress false
#define RenderGenAndTurneyProgress false
//#define RenderGameProgress true;

// Saving
#define SaverType 0
/*	+-----------+-------------------------------------------------------------------------+
	| SaverType | Description                                                             |
	+-----------+-------------------------------------------------------------------------+
	| 0         | no Saver                                                                |
	+-----------+-------------------------------------------------------------------------+
	| 1         | save full state to file                                                 |
	+-----------+-------------------------------------------------------------------------+
	| 2         | Save taken Actions to file (int size is saved size of the int in bytes) |
	+-----------+-------------------------------------------------------------------------+*/
#define SaverIntSize 1


typedef std::bitset<stateSize> IDType;