#include "log.hpp"
#include <stdio.h>
#include <jce/save.hpp>
#include <jce/load.hpp>



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
#if LossLogger
debug::log::lossLogger debug::log::_lossLogger = debug::log::lossLogger();
#endif

debug::log::lossLogger::lossLogger(const char file[])
{
	std::ifstream in(file, std::ios::binary);
	jce::load(in, this->vals);
}

void debug::log::lossLogger::save(const char file[])
{
	std::ofstream out (file, std::ios::binary);
	if (out.is_open())
	{
		jce::save(out, this->vals);
	}
	else
	{
		std::cout << "\33[31;1mFailed to save lossLogger to " << file << "\33[0m" << std::endl;
	}
}

bool debug::log::lossLogger::operator==(const lossLogger& other)
{
	if (other.vals.size() == this->vals.size())
	{
		for (size_t idx = 0; idx < other.vals.size(); idx++)
		{
			if (other[idx] != (*this)[idx])
			{
				return false;
			}
		}
		return true;
	}
	return false;
}