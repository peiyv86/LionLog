#pragma once

#include"LogLevel.h"
#include<string>

struct LogRecord {
	LogLevel level;
	std::string message;
	std::string thread_id; 
	std::string time; 
	std::string file;
	int line;

	// 辅助函数：转字符串
	std::string getLevelStr() const {
		switch (level) {
		case LogLevel::DEBUG: return "DEBUG";
		case LogLevel::INFO:  return "INFO";
		case LogLevel::WARN:  return "WARN";
		case LogLevel::ERROR: return "ERROR";
		default: return "UNKNOWN";
		}
	}
};