#pragma once

#include"LogRecord.h"

#include<string>
#include<vector>
#include<functional>

class LogFormatter {
private:
	std::string pattern; // 例如 "[%d] [%t] [%l] %m%n"
	std::vector<std::function<std::string(const LogRecord& record)>> items;
public:
	explicit LogFormatter(const std::string& pattern_);
	std::string format(const LogRecord& record,bool color);
};