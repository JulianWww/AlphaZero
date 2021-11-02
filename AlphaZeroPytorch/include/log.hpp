#pragma once
#define threads 0

//logging
#define MainLogger true
#define MCTSLogger false
#define MemoryLogger false
#define ProfileLogger false
#define ModelLogger false
#define LossLogger true


#include <unordered_map>
#include <spdlog/sinks/basic_file_sink.h>
//#include <memory>
#if ProfileLogger
#include "timer.hpp"
#endif
#include <stdio.h>
#include <chrono>
#if (MainLogger || MCTSLogger || MemoryLogger || ProfileLogger || ModelLogger || LossLogger)



namespace debug {
#if ProfileLogger
	extern double totalModelTime;
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
		void logVector(std::shared_ptr<spdlog::logger> logger, std::vector<T>, char*);
		void logVector(std::shared_ptr<spdlog::logger> logger, std::vector<int>);
		void logVector(std::shared_ptr<spdlog::logger> logger, std::vector<float>);

		class lossLogger
		{
		public: lossLogger();
		public: lossLogger(const char file[]);

		protected: std::vector<std::pair<float, float>> vals;

		public: void addValue(const float val, const float poly);
		public: void addValue(const std::pair<float, float>& val);
		public: void save(const char file[]);

		public: std::pair<float, float> operator[](size_t idx) const;
		public: bool operator==(const lossLogger&);
		};

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
#if LossLogger
		extern lossLogger _lossLogger;
#endif
	}
}

inline std::shared_ptr<spdlog::logger> debug::log::createLogger(const char* name, const char* file) {
	// Create a daily logger - a new file is created every day on 2:30am
	int success = remove(file);
	auto logger = spdlog::basic_logger_mt(name, file);
	return logger;
}

inline debug::log::lossLogger::lossLogger()
{
}

inline void debug::log::lossLogger::addValue(const float a, const float b)
{
	std::pair<float, float> data = { a, b };
	this->addValue(data);
}

inline void debug::log::lossLogger::addValue(const std::pair<float, float>& val)
{
	this->vals.push_back(val);
}

inline std::pair<float, float> debug::log::lossLogger::operator[](size_t idx) const
{
	return this->vals[idx];
}

template<typename T>
inline void debug::log::logVector(std::shared_ptr<spdlog::logger> logger, std::vector<T> vec, char* out)
{
	logger->info(out, (vec[0]),  (vec[1]),  (vec[2]),  (vec[3]),  (vec[4]),  (vec[5]),  (vec[6]));
	logger->info(out, (vec[7]),  (vec[8]),  (vec[9]),  (vec[10]), (vec[11]), (vec[12]), (vec[13]));
	logger->info(out, (vec[14]), (vec[15]), (vec[16]), (vec[17]), (vec[18]), (vec[19]), (vec[20]));
	logger->info(out, (vec[21]), (vec[22]), (vec[23]), (vec[24]), (vec[25]), (vec[26]), (vec[27]));
	logger->info(out, (vec[28]), (vec[29]), (vec[30]), (vec[31]), (vec[32]), (vec[33]), (vec[34]));
	logger->info(out, (vec[35]), (vec[36]), (vec[37]), (vec[38]), (vec[39]), (vec[40]), (vec[41]));
}

inline void debug::log::logVector(std::shared_ptr<spdlog::logger> logger, std::vector<int> vec)
{
	char out[] = "Action vals are: {0:3d}, {1:3d}, {2:3d}, {3:3d}, {4:3d}, {5:3d}, {6:3d}";
	logVector(logger, vec, out);
}

inline void debug::log::logVector(std::shared_ptr<spdlog::logger> logger, std::vector<float> vec)
{
	char out[] = "Action vals are: {:1.2f}, {:1.2f}, {:1.2f}, {:1.2f}, {:1.2f}, {:1.2f}, {:1.2f}";
	logVector(logger, vec, out);
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
	logger->info("CPU: 8x Xenon Cpu @2.2 GHz");
	logger->info("GPU: Nvidia P4 cuda 11.4");
	logger->info("memory: 7.8 Gb");
#endif

	logger->info("================================================================================");
	for (auto const& pair : this->times) {
		logger->info("Profiler time id {} took {} s", pair.first, pair.second);
	}
	logger->info("Profiler time Model took {} s", totalModelTime);
	logger->info("everything else took: {}", this->rest);

	logger->info("");
	logger->info("0 : MCTS and NN forward");
	logger->info("3 : Game Stuff");
	logger->info("4 : Memory shuffeling");
	logger->info("5 : NN Backward");
}
#endif
#endif

