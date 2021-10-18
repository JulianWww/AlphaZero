#pragma once
#include <log.hpp>
#include <bitset>

#define OPSMode 0

/*
+---------+-------------+
| OPSMode | Description |
+---------+-------------+
| 0       | Train an AI |
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

#define U_computation(edge) (this->cpuct * edge->P * std::sqrt(Nb) / (1 + edge->N))


// runn setting
#define runVersion 1
#define loadVersion -1

// Net settings
#define MaxQuDefault -99999
#define reg_const 0.0001
#define Alpha 0.8
#define learningRage 0.1

// simulation setting
#define MCTSSimulations 5
#define cpuct_ 2.0f
#define ProbabiliticMoves 15

// memory setting
#define memory_size 30000

// self play
#define EPOCHS 30
#define probabilitic_moves 2 // how manny moves are prabilistic in the begining of the game to aid in exploration

// training
#define Training_loops 15
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
#define UNIX
#endif