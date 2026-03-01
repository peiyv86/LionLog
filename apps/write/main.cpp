#include"Logger.h"
#include"LogLevel.h"

#include<iostream>
#include<fstream>
#include<string>
#include<thread>
#include<chrono>
#include<csignal>
#include<atomic>
#include<filesystem>
#include<mutex>

LogLevel parse_level(std::string str) 
    {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        if (str == "DEBUG") return LogLevel::DEBUG;
        if (str == "INFO")  return LogLevel::INFO;
        if (str == "WARN")  return LogLevel::WARN;
        if (str == "ERROR") return LogLevel::ERROR;
        return LogLevel::INFO; 
    }

std::string getLevelStr(LogLevel level)
    {
		switch (level) {
		case LogLevel::DEBUG: return "DEBUG";
		case LogLevel::INFO:  return "INFO";
		case LogLevel::WARN:  return "WARN";
		case LogLevel::ERROR: return "ERROR";
		default: return "UNKNOWN";
		}
	}


int main(int argc, char* argv[])
{
    if (argc < 5 || std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")
    {   std::cout << "\033[32m";
        std::cout << "Usage: " << argv[0] << " <Level> <Format> <Message> <Path1> [Path2...]\n";
        std::cout << "Example: " << argv[0] << " INFO \"[%d][%l]%m\" \"System Check\" \"out.log\"\n";
        std::cout << "\033[0m";
        return 0;
    }

    LogLevel level  = parse_level(argv[1]);
    std::string fmt = argv[2];
    std::string msg = argv[3];

    auto& logger = Logger::getInstance();
    
    for (int i = 4; i < argc; i++)
    {
        logger.addFileSink(argv[i]);
    }
    
    logger.addConsoleSink();
    logger.setLevel(level);
    logger.setFromat(fmt);
    std::cout << "\033[34m";
    std::cout << "[SYSTEM]:LionLog Write Service Started.\n";
    std::cout << "\033[0m";

    switch (level)
    {
        case LogLevel::DEBUG: LOG_DEBUG(msg); break;
        case LogLevel::INFO:  LOG_INFO(msg);  break;
        case LogLevel::WARN:  LOG_WARN(msg);  break;
        case LogLevel::ERROR: LOG_ERROR(msg); break;
    } 

    std::cout << "\033[34m";
    std::cout << "[SYSTEM]:LionLog Write Finished.\n";
    std::cout << "\033[0m";

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
