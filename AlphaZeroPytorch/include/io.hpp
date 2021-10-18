#pragma once
#include <jce/load.hpp>
#include <jce/save.hpp>
#include "config.hpp"


namespace AlphaZero
{
    namespace io
    {
        namespace FullState
        {
            class GameSaver
            {
            public: std::list<std::list<std::shared_ptr<Game::GameState>>> states;
            public: void addState(std::shared_ptr<Game::GameState> state);
            public: void addGame();
            public: void clear();
            public: void save(char filename[]);
            public: void load(char filename[]);

            // replay game in console (debug)
            public: void ConsoleReplay(int itx);
            };
        }
        namespace ActionsOnly
        {
            class GameSaver
            {
            public: std::list<std::list<unsigned int>> states;
            public: void addState(int);
            public: void addGame();
            public: void clear();
            public: void save(char filename[]);
            public: void load(char filename[]);

            // replay game in console (debug)
            public: void ConsoleReplay(int idx);
            };
        }
    }
}

inline void AlphaZero::io::FullState::GameSaver::addState(std::shared_ptr<Game::GameState> state)
{
    this->states.back().push_back(state);
}

inline void AlphaZero::io::FullState::GameSaver::addGame()
{
    this->states.push_back(std::list<std::shared_ptr<Game::GameState>>());
}

inline void AlphaZero::io::FullState::GameSaver::clear()
{
    this->states.clear();
}

inline void AlphaZero::io::FullState::GameSaver::save(char filename[])
{
    std::ofstream fout;
    fout.open(filename, std::ios::binary);

    jce::save(fout, this->states);

    fout.close();
}

inline void AlphaZero::io::FullState::GameSaver::load(char filename[])
{
    std::ifstream infile(filename, std::ios::binary);

    jce::load(infile, this->states);
    infile.close();
}

inline void AlphaZero::io::FullState::GameSaver::ConsoleReplay(int idx)
{
    for (auto const& state : *std::next(this->states.begin(), idx))
    {
        state->render();
    }
}


inline void AlphaZero::io::ActionsOnly::GameSaver::addState(int actions)
{
    this->states.back().push_back(actions);
}

inline void AlphaZero::io::ActionsOnly::GameSaver::addGame()
{
    this->states.push_back(std::list<unsigned int>());
}

inline void AlphaZero::io::ActionsOnly::GameSaver::clear()
{
    this->states.clear();
}

inline void AlphaZero::io::ActionsOnly::GameSaver::save(char filename[])
{
    std::ofstream file(filename, std::ios::binary);
    jce::save(file, this->states);
    file.close();
}

inline void AlphaZero::io::ActionsOnly::GameSaver::load(char filename[])
{
    std::ifstream file(filename, std::ios::binary);
    jce::load(file, this->states);
    file.close();
}

inline void AlphaZero::io::ActionsOnly::GameSaver::ConsoleReplay(int idx)
{
    Game::Game game = Game::Game();

    for (int action : *std::next(this->states.begin(), idx))
    {
        game.render();
        game.takeAction(action);
    }
    game.render();
}
