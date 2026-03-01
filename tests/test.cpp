#include<gtest/gtest.h>

#include"ThreadPool.h"
#include"LogLevel.h"
#include"LogRecord.h"
#include"LogFormatter.h"
#include"Logger.h"

#include"LogSink.h"
#include"ConsoleSink.h"
#include"FileSink.h"
#include"RollingFileSink.h"

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

TEST(LoggerTest, LogLevelLogic) {
    EXPECT_LT(LogLevel::DEBUG, LogLevel::INFO);
    EXPECT_LT(LogLevel::INFO, LogLevel::WARN);
    EXPECT_LT(LogLevel::WARN, LogLevel::ERROR);
}

TEST(FormatterTest, CustomPatternParsing) {
    LogRecord record;
    record.level = LogLevel::ERROR;
    record.message = "TestMsg";
    record.time = "2026-01-01 12:00:00";
    record.thread_id = "1234";
    record.file = "test.cpp";
    record.line = 42;

    LogFormatter formatter("[%l] %m (%f:%n)");
    std::string result = formatter.format(record, false); // 關閉顏色以便比對字串

    EXPECT_EQ(result, "[ERROR] TestMsg (test.cpp:42)");
}

TEST(LoggerTest, LevelFiltering) {
    Logger& logger = Logger::getInstance(2);
    logger.addConsoleSink();
    
    logger.setLevel(LogLevel::ERROR);
    
    EXPECT_NO_THROW({
        logger.debug("Should be filtered");
        logger.info("Should be filtered");
        logger.error("Should be processed");
    });
}

TEST(SinkTest, FileSinkWrite) {
    std::string testFile = "test_write.log";
    std::filesystem::remove(testFile); // 先清理

    {
        Logger& logger = Logger::getInstance(2);
        logger.addFileSink(testFile);
        logger.setLevel(LogLevel::DEBUG);
        
        logger.info("Hello File");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    } 

    std::ifstream ifile(testFile);
    std::string line;
    std::getline(ifile, line);
    
    EXPECT_NE(line.find("Hello File"), std::string::npos);
    std::filesystem::remove(testFile);
}

// 5. 測試 RollingFileSink 的輪轉與壓縮 (核心功能測試)
TEST(SinkTest, RollingAndCompression) {
    std::string baseFile = "rolling.log";
    std::filesystem::remove(baseFile);
    
    std::filesystem::remove(baseFile + ".1.gz");
    std::filesystem::remove(baseFile + ".2.gz");

    Logger& logger = Logger::getInstance(1); // 保持單一工作線程方便測試
    logger.addRollingFileSink(baseFile, 1024, 2);
    
    logger.setLevel(LogLevel::DEBUG);

    std::string longMsg(100, 'A'); // 100 bytes
    for(int i = 0; i < 15; ++i) {
        logger.info(longMsg); 
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    EXPECT_TRUE(std::filesystem::exists(baseFile));
    EXPECT_LT(std::filesystem::file_size(baseFile), 1024);

    bool compressedExists = std::filesystem::exists(baseFile + ".1.gz") || 
                            std::filesystem::exists(baseFile + ".2.gz");
    
    std::filesystem::remove(baseFile);
    std::filesystem::remove(baseFile + ".1.gz");
    std::filesystem::remove(baseFile + ".2.gz");
}

// 6. 測試多線程安全性 (壓力測試)
TEST(LoggerTest, MultiThreadStress) {
    Logger& logger = Logger::getInstance(4);
    logger.addConsoleSink();

    auto worker = [&logger](int id) {
        for(int i = 0; i < 100; ++i) {
            logger.info("Thread " + std::to_string(id) + " msg " + std::to_string(i));
        }
    };

    std::vector<std::thread> threads;
    for(int i = 0; i < 10; ++i) {
        threads.emplace_back(worker, i);
    }

    for(auto& t : threads) {
        t.join();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
