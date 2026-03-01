#pragma once
#include"LogLevel.h"
#include"LogRecord.h"
#include"LogFormatter.h"

#include<iostream>
#include<vector>
#include<string>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<memory>
#include<ctime>
#include<fstream>

class LogSink
{
public:
	std::shared_ptr<LogFormatter> format;
    std::mutex mtx_out; 
    std::mutex mtx_file;
    LogSink() 
    {
        format = std::make_shared<LogFormatter>("[%d][%l]%m");
    }
    virtual ~LogSink() = default;
    virtual void log(const LogRecord& record) = 0;
    void setFormatter(std::shared_ptr<LogFormatter> fmt) 
    {
        format = fmt;
    }
    static std::string getTime()
    {
        std::time_t now = std::time(0);
        char time_buf[20];//算上\0
        std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(time_buf);
    }
};
