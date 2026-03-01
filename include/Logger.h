#pragma once

#include"LogSink.h"
#include"ThreadPool.h"
#include"ConsoleSink.h"
#include"FileSink.h"

#define LOG_DEBUG(msg) Logger::getInstance().debug(msg)
#define LOG_INFO(msg)  Logger::getInstance().info(msg)
#define LOG_WARN(msg)  Logger::getInstance().warn(msg)
#define LOG_ERROR(msg) Logger::getInstance().error(msg)

class Logger
{
private:
	std::vector<std::shared_ptr<LogSink>> Sinks;//输出目的地-所有调用
	LogLevel LEVEL{ LogLevel::INFO };
	ThreadPool pool;
	std::mutex mtx_sinks;
	std::mutex mtx_sys;

	Logger(size_t n);
	void log(LogLevel level, const std::string& msg, std::string file,int line);
	void addSink(std::shared_ptr<LogSink> sink);
	
public:
	Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
	static Logger& getInstance(size_t n=4)
	{	
		static Logger instance(n);
		return instance;
	}
	void setLevel(LogLevel level);
	void setFromat(const std::string& fmt);

	void addConsoleSink();
	void addFileSink(const std::string& path) ;
	void addRollingFileSink(const std::string& path,uintmax_t size,int max_num) ;

	void debug(const std::string& msg);
	void info(const std::string& msg);
	void warn(const std::string& msg);
	void error(const std::string& msg);
};
