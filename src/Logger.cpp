#include"Logger.h"
#include"FileSink.h"
#include"ConsoleSink.h"
#include"RollingFileSink.h"
#include"LogLevel.h"
#include"ThreadPool.h"

Logger::Logger(size_t n) :pool(n) {};
	
void Logger::setLevel(LogLevel level)
	{
		LEVEL = level;
		std::string s;
		switch (level)
		{
		case LogLevel::DEBUG://green
			s = "DEBUG";
			break;
		case LogLevel::ERROR://red
			s = "ERROR";
			break;
		case LogLevel::INFO://blue
			s = "INFO";
			break;
		case LogLevel::WARN://yellow
			s = "WARN";
			break;
		}
		{
			std::lock_guard<std::mutex> lk(mtx_sys);
			LOG_DEBUG("[SYSTEM]:SET LEVEL TO " + s);
		}
	}

void Logger::setFromat(const std::string& fmt)//此功能会覆盖子类的格式
{
	auto new_formatter = std::make_shared<LogFormatter>(fmt);
    std::lock_guard<std::mutex> lock(mtx_sinks);
    for (auto& sink : Sinks) {
        sink->setFormatter(new_formatter);
    }
}

void Logger::addSink(std::shared_ptr<LogSink> sink)
	{
		{
			std::lock_guard<std::mutex> lk(mtx_sinks);
			Sinks.emplace_back(sink);
		}
		{
			std::lock_guard<std::mutex> lk(mtx_sys);
			LOG_DEBUG("[SYSTEM]:ADD SINK FINISH");
		}
	}

void Logger::log(LogLevel level, const std::string& msg, std::string file = "",int line =0)
	{
		if (level >= LEVEL)
		{
			// --- Step A: 抓取元数据 (在主线程完成) ---
			LogRecord record;
			record.level = level;
			record.message = msg; // 发生一次 string 拷贝
			record.time = LogSink::getTime();

			// 获取线程ID并转为string (或者直接存 thread::id 交给 Formatter 处理)
			std::stringstream ss;
			ss << std::this_thread::get_id();
			record.thread_id = ss.str();
			record.file = file;
			{
				std::lock_guard<std::mutex> lk(mtx_sinks);
				for (auto& s : Sinks)
				{
					pool.enqueue([s, record]() {s->log(record); });
				}
			}
		}
	}

void Logger::addConsoleSink() 
	{
		std::shared_ptr<LogSink> toconsole = std::make_shared<ConsoleSink>();
		Logger::addSink(toconsole); 
	};
void Logger::addFileSink(const std::string& path) 
	{
		std::shared_ptr<LogSink> tofile = std::make_shared<FileSink>(path);
		Logger::addSink(tofile);
	};
void Logger::addRollingFileSink(const std::string& path,uintmax_t size,int max_num) 
	{
		std::shared_ptr<LogSink> torollfile = std::make_shared<RollingFileSink>(path,size,max_num);
		Logger::addSink(torollfile);
	};

void Logger::debug(const std::string& msg) {log(LogLevel::DEBUG, msg);}
void Logger::info(const std::string& msg) {log(LogLevel::INFO, msg);}
void Logger::warn(const std::string& msg) { log(LogLevel::WARN, msg); }
void Logger::error(const std::string& msg) { log(LogLevel::ERROR, msg); }

