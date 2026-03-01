#pragma once

#include"LogSink.h"
#include"LogLevel.h"

class FileSink :public LogSink
{
private:
	std::string path;
	std::mutex mtx_file;
	std::ofstream outfile;
public:
	FileSink(const std::string& s);
	void log(const LogRecord& record) override;
	~FileSink();
};