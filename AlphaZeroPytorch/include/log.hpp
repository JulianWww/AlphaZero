#pragma once
#define threads 15

//logging
#define MainLogger true
#define MCTSLogger false
#define MemoryLogger false
#define ProfileLogger false
#define ModelLogger false


#if (MainLogger || MCTSLogger || MemoryLogger || ProfileLogger || ModelLogger)
#include <spdlog/sinks/basic_file_sink.h>
//#include <memory>
#include <unordered_map>
#if ProfileLogger
#include "timer.hpp"
#endif
#include <stdio.h>
#include <chrono>




namespace debug {
#if ProfileLogger
	namespace Profiler {
		class MCTSProfiler {
		private: utils::Timer timer;
		public: std::unordered_map<unsigned int, double> times;
		private: double rest;

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
	logger->info("Action vals are: {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}", Mround(vec[0]),  Mround(vec[1]),  Mround(vec[2]),  Mround(vec[3]),  Mround(vec[4]),  Mround(vec[5]),  Mround(vec[6]));
	logger->info("Action vals are: {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}", Mround(vec[7]),  Mround(vec[8]),  Mround(vec[9]),  Mround(vec[10]), Mround(vec[11]), Mround(vec[12]), Mround(vec[13]));
	logger->info("Action vals are: {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}", Mround(vec[14]), Mround(vec[15]), Mround(vec[16]), Mround(vec[17]), Mround(vec[18]), Mround(vec[19]), Mround(vec[20]));
	logger->info("Action vals are: {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}", Mround(vec[21]), Mround(vec[22]), Mround(vec[23]), Mround(vec[24]), Mround(vec[25]), Mround(vec[26]), Mround(vec[27]));
	logger->info("Action vals are: {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}", Mround(vec[28]), Mround(vec[29]), Mround(vec[30]), Mround(vec[31]), Mround(vec[32]), Mround(vec[33]), Mround(vec[34]));
	logger->info("Action vals are: {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}, {:03.2f}", Mround(vec[35]), Mround(vec[36]), Mround(vec[37]), Mround(vec[38]), Mround(vec[39]), Mround(vec[40]), Mround(vec[41]));
}
#if ProfileLogger
inline void debug::Profiler::MCTSProfiler::switchOperation(unsigned int id)
{
	this->stop();
	this->first = false;
	this->toRest = false;
	this->timer.reset();
	this->currentTime = id;
}

inline void debug::Profiler::MCTSProfiler::stop()
{
	
	if (!this->first) {
		if (toRest) {
			this->rest = this->rest + this->timer.elapsed();
		}
		double currentNum;
		if (this->times.count(currentTime) == 0) {
			currentNum = 0.0f;
		}
		else {
			currentNum = this->times.at(this->currentTime);
		}
		double res = currentNum + this->timer.elapsed();
		this->times.insert_or_assign( currentTime, res);
	}
	this->toRest = true;
}

inline void debug::Profiler::MCTSProfiler::log() {
	this->log(debug::log::profileLogger);
}

inline void debug::Profiler::MCTSProfiler::log(std::shared_ptr<spdlog::logger> logger)
{
	logger->info("using {} threads", threads);
	logger->info("run Info:");
	logger->info("");

	// this is only true on the computers im using as im just entering the infomation here to save time
#ifdef WIN32
	logger->info("os: Windows 10");
	logger->info("CPU: Intel(R) Core(TM) i5-8350U CPU @ 1.70 GHz ");
	logger->info("GPU: None");
	logger->info("memory: 8 GB");
#else
	logger->info("OS: Ubuntu 18.04");
	logger->info("CPU: ??");
	logger->info("GPU: Nvidia P4 cuda 11.4");
	logger->info("memory: 7.8 Gb");
#endif

	logger->info("================================================================================");
	for (auto const& pair : this->times) {
		logger->info("Profiler time id {} took {} s", pair.first, pair.second);
	}
	logger->info("everything else took: {}", this->rest);

	logger->info("");
	logger->info("0 : MCTS and NN forward");
	logger->info("3 : Game Stuff");
	logger->info("4 : Memory shuffeling");
	logger->info("5 : NN Backward");
}
#endif
#endif

