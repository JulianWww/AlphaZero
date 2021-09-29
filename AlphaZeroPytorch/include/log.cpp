#include "log.hpp"
#include <stdio.h>



#if MainLogger
std::shared_ptr<spdlog::logger> debug::log::mainLogger = debug::log::createLogger("mainLogger", "logs/c++/mainLogger.log");
#endif
#if MCTSLogger
std::shared_ptr<spdlog::logger> debug::log::MCTS_Logger = debug::log::createLogger("MCTS_Logger", "logs/c++/MCTS_Logger.log");
#endif
#if MemoryLogger
std::shared_ptr<spdlog::logger> debug::log::memoryLogger = debug::log::createLogger("memoryLogger", "logs/c++/memoryLogger.log");
#endif
#if ProfileLogger
std::shared_ptr<spdlog::logger> debug::log::profileLogger = debug::log::createLogger("profileLogger", "logs/c++/profileLogger.log");

debug::Profiler::MCTSProfiler debug::Profiler::profiler = debug::Profiler::MCTSProfiler();
#endif
#if ModelLogger
std::shared_ptr<spdlog::logger> debug::log::modelLogger = debug::log::createLogger("ModelLogger", "logs/c++/ModelLogger.log");
#endif
