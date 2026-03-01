#include"Logger.h"

#include<iostream>
#include<fstream>
#include<string>
#include<thread>
#include<chrono>
#include<csignal>
#include<atomic>
#include<filesystem>

std::atomic<bool> start{true};

void signal_handler(int sig)
{
    LOG_WARN("[Collector] Received signal "+std::to_string(sig) +".Stop.");
    start=false;
}

class SysLogWatcher
{
private:
    std::string path;
public:
    SysLogWatcher(const std::string& p):path(p){};
    void run()
    {
        std::ifstream file(path);
        if(!file.is_open())
        {
            LOG_ERROR("Failed to open system log: " + path);
            std::cout<<"\033[31m[ERROR]: Cannot open "<<path<<" (Permission denied)\n";
            return;
        }
        file.seekg(0,std::ios::end);
        LOG_DEBUG("[Collector]:Watching file: "+ path);
        std::string line;
        while(start)
        {
            if(std::getline(file,line))
            {
                LOG_INFO(line);
            }
            else
            {
                if(file.eof())//末尾
                {
                    file.clear();
                    std::error_code ec;
                    auto path_size = std::filesystem::file_size(path, ec);
                    // 核心判断：
                    // 1. 如果获取大小失败（文件可能被删了）
                    // 2. 或者 磁盘上的文件大小 < 我读到的位置 (说明发生了截断 或 轮转出了新文件)
                    if (ec || path_size < (std::uintmax_t)file.tellg()) 
                    {//可能会重开失效（大小刚好为之前的大小）
                        LOG_WARN("File truncation or rotation detected. Reopening...");
                        file.close(); 
                        // 下次循环会进入 "else { file.open... }" 分支或者你需要在这里立刻 open
                        // 建议：直接重置流，让下一次循环处理打开
                    }
                }
                else
                {
                    LOG_WARN("File stream error, trying to reopen...");
                    file.close();
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    file.open(path);
                    file.seekg(0, std::ios::end); // 重新打开后指向末尾
                };
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            };
        };
        LOG_DEBUG("[Collector] loop finished.");
    };
};

LogLevel parse_level(std::string str) 
    {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        if (str == "DEBUG") return LogLevel::DEBUG;
        if (str == "INFO")  return LogLevel::INFO;
        if (str == "WARN")  return LogLevel::WARN;
        if (str == "ERROR") return LogLevel::ERROR;
        return LogLevel::INFO; 
    }

void print_usage(const char* prog_name) 
    {
        std::cout<< "\033[32m"
                << "Usage: " << prog_name << " [options] <logfile_path>\n"
                << "Options:\n"
                << "  --level <LEVEL>    Set log level (DEBUG, INFO, WARN, ERROR). Default: DEBUG\n"
                << "  --format <PATTERN> Set log format. Default: \"[%d][%l]%m\"\n"
                << "                     %d=Date, %l=Level, %m=Message, %f=File, %n=Line\n"
                << "Example:\n"
                << "  " << prog_name << " --level WARN --format \"[%d] %m\" /var/log/syslog\n"
                <<"\033[0m";
    }

void CmdMap(int argc, char* argv[], std::string& target_file, std::string& log_format, LogLevel& log_level) 
    {
        for (int i = 1; i < argc; ++i) 
        {
            std::string arg = argv[i];
            if (arg == "--help" || arg == "-h") 
            {
                print_usage(argv[0]);
                std::exit(0);
            } 
            else if (arg == "--level" && i + 1 < argc) 
            {
                log_level = parse_level(argv[++i]);
            } 
            else if (arg == "--format" && i + 1 < argc) 
            {
                log_format = argv[++i];
            } 
            else if (arg[0] == '-') 
            {
                // 處理無效的選項 (例如輸入了 --xyz)
                std::cerr << "Unknown option: " << arg << "\n";
                print_usage(argv[0]);
                std::exit(1);
            } 
            else 
            {
                target_file = arg;
            }
        }
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

int main(int argc ,char* argv[])
{
    std::signal(SIGINT,signal_handler);
    std::signal(SIGTERM,signal_handler);

    std::string target_file = "/var/log/syslog";
    std::string log_format = "[%d][%l]%m";
    LogLevel log_level = LogLevel::DEBUG;

    CmdMap(argc,argv,target_file,log_format,log_level);

    Logger::getInstance().addRollingFileSink("collector.log", 1024 * 1024 * 10, 5);
    Logger::getInstance().addConsoleSink();

    Logger::getInstance().setLevel(log_level);
    Logger::getInstance().setFromat(log_format);

    std::cout << "\033[32m";
    std::cout<<"[SYSTEM]:LionLog Collector Service Started.\n";
    std::cout << "[Config]:Monitoring: " + target_file;
    std::cout << "[Config]:Level: " + getLevelStr(log_level);
    std::cout << "[Config]:Format: " + log_format;
    std::cout << "\033[0m";

    //监视器
    SysLogWatcher watcher(target_file);
    watcher.run();

    std::cout << "\033[32m";
    std::cout<<"[SYSTEM]:LionLog Collector Service Stopped.\n";
    std::cout << "\033[0m";
    //LOG_WARN("LionLog Collector Service Stopped.");

    return 0;
}