#pragma once
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>
#include <stdio.h>
#include <chrono>

//logging
#define MainLogger false
#define MCTSLogger false
#define MemoryLogger false
#define ProfileLogger true
#define ModelLogger true

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::microseconds;

namespace debug {
#if ProfileLogger
	namespace Profiler {
		class MCTSProfiler {
		public: std::unordered_map<unsigned int, unsigned long long > times;
		private: unsigned long long rest;

		private: std::chrono::steady_clock::time_point lastTime;
		private: bool first = true;
		private: bool toRest = false;
		private: unsigned int currentTime;

		public: void switchOperation(unsigned int id);
		public: void stop();
		public: void log();
		public: void log(std::shared_ptr<spdlog::logger> logger);
		};
		extern debug::Profiler::MCTSProfiler profiler;
	};
#endif
	namespace log {
		std::shared_ptr<spdlog::logger> createLogger(const char* name, const char* file);
		template<typename T>
		void logVector(std::shared_ptr<spdlog::logger> logger, std::vector<T>);

#if MainLogger
		extern std::shared_ptr<spdlog::logger> mainLogger;
#endif
#if MCTSLogger
		extern std::shared_ptr<spdlog::logger> MCTS_Logger;
#endif
#if MemoryLogger
		extern std::shared_ptr<spdlog::logger> memoryLogger;
#endif
#if ProfileLogger
		extern std::shared_ptr<spdlog::logger> profileLogger;
#endif
#if ModelLogger
		extern std::shared_ptr<spdlog::logger> modelLogger;
#endif
	}
}

inline std::shared_ptr<spdlog::logger> debug::log::createLogger(const char* name, const char* file) {
	// Create a daily logger - a new file is created every day on 2:30am
	int success = remove(file);
	auto logger = spdlog::basic_logger_mt(name, file);
	return logger;
}

template<typename T>
inline void debug::log::logVector(std::shared_ptr<spdlog::logger> logger, std::vector<T> vec)
{
	logger->info("Action vals are: {}, {}, {}, {}, {}, {}, {}", vec[0],  vec[1],  vec[2],  vec[3],  vec[4],  vec[5],  vec[6]);
	logger->info("Action vals are: {}, {}, {}, {}, {}, {}, {}", vec[7],  vec[8],  vec[9],  vec[10], vec[11], vec[12], vec[13]);
	logger->info("Action vals are: {}, {}, {}, {}, {}, {}, {}", vec[14], vec[15], vec[16], vec[17], vec[18], vec[19], vec[20]);
	logger->info("Action vals are: {}, {}, {}, {}, {}, {}, {}", vec[21], vec[22], vec[23], vec[24], vec[25], vec[26], vec[27]);
	logger->info("Action vals are: {}, {}, {}, {}, {}, {}, {}", vec[28], vec[29], vec[30], vec[31], vec[32], vec[33], vec[34]);
	logger->info("Action vals are: {}, {}, {}, {}, {}, {}, {}", vec[35], vec[36], vec[37], vec[38], vec[39], vec[40], vec[41]);
}
#if ProfileLogger
inline void debug::Profiler::MCTSProfiler::switchOperation(unsigned int id)
{
	this->stop();
	this->first = false;
	this->toRest = false;
	this->lastTime = std::chrono::high_resolution_clock::now();
	this->currentTime = id;
}

inline void debug::Profiler::MCTSProfiler::stop()
{
	auto time_now = std::chrono::high_resolution_clock::now();
	if (!this->first) {
		if (toRest) {
			this->rest = this->rest + duration_cast<microseconds>(time_now - this->lastTime).count();
		}
		unsigned long long currentNum;
		if (this->times.count(currentTime) == 0) {
			currentNum = 0.0f;
		}
		else {
			currentNum = this->times.at(this->currentTime);
		}
		unsigned long long res = currentNum + duration_cast<microseconds>(time_now - this->lastTime).count();
		this->times.insert_or_assign( currentTime, res);
	}
	this->toRest = true;
}

inline void debug::Profiler::MCTSProfiler::log() {
	this->log(debug::log::profileLogger);
}

inline void debug::Profiler::MCTSProfiler::log(std::shared_ptr<spdlog::logger> logger)
{
	logger->info("================================================================================");
	for (auto const& pair : this->times) {
		logger->info("Profiler time id {} took {} ms", pair.first, pair.second);
	}
	logger->info("everything else took: {}", this->rest);
}
#endif

