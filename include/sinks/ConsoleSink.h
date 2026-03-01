#pragma once

#include"LogSink.h"
#include"LogLevel.h"

class ConsoleSink:public LogSink
{
public:
	void log(const LogRecord& record) override;
	~ConsoleSink() override = default;  // 添加虚析构函数
};