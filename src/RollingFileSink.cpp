#include"RollingFileSink.h"
#include"LogSink.h"
#include"GzipUtil.h"

#include<filesystem>
#include<fstream>
#include<thread>
#include <cstdlib>

RollingFileSink::RollingFileSink(const std::string& s, uintmax_t size, int num)
        : base_path(s), max_size(size), max_num(num) 
    {
        openLogFile();
    }

void RollingFileSink::log(const LogRecord& record)
    {
        std::string str = format->format(record, false);
        {
            std::lock_guard<std::mutex> lk(mtx_file);
            if (!outfile.is_open()) return;

            std::error_code ec;
            auto cur_size = std::filesystem::file_size(base_path, ec);

            if (!ec && cur_size >= max_size) 
            {
                OpenNewFiles();
            }
            outfile << str << '\n';
        }
    }

void RollingFileSink::openLogFile() 
    {
        if (outfile.is_open()) outfile.close();
        outfile.open(base_path, std::ios::app);
    }

void RollingFileSink::OpenNewFiles() 
    {
        if (outfile.is_open()) outfile.close();

        // 1. 決定這次備份的序號 (循環使用 1 ~ max_num)
        cur_num = (cur_num % max_num) + 1;
        std::string archive_name = base_path + "." + std::to_string(cur_num);
        std::string compressed_name = archive_name + ".gz";

        std::error_code ec;
        std::filesystem::remove(archive_name, ec);
        std::filesystem::remove(compressed_name, ec);

        std::filesystem::rename(base_path, archive_name, ec);
        
        openLogFile();
        if (!ec) 
        {
            std::thread([archive_name, compressed_name]() 
            {
                GzipUtil::compress(archive_name, compressed_name);
            }).detach();
        }
    }

