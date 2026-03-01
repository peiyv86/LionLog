#pragma once

#include"LogRecord.h"
#include"LogSink.h"

#include<filesystem>
#include<fstream>
#include<thread>

class RollingFileSink : public LogSink {
private:
    const std::string base_path;    // 永遠是 app.log
    std::ofstream outfile;
    std::uintmax_t max_size;
    int max_num;                    // 最大保留備份數
    int cur_num = 0;                // 記錄當前備份到第幾個
    std::mutex mtx_file;

public:
    RollingFileSink(const std::string& s, uintmax_t size, int num);
    void log(const LogRecord& record) override;
private:
    void openLogFile();
    void OpenNewFiles();
};
