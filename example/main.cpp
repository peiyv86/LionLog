#include"LogSink.h"
#include"ThreadPool.h"
#include"LogLevel.h"
#include"Logger.h"
#include"ConsoleSink.h"
#include"FileSink.h"

int main() {
	Logger& logger = Logger::getInstance(4);
	
	logger.addConsoleSink();
	logger.addFileSink("app_log");

	    // --- Test 1: Log Level Filtering (INFO) ---
    std::cout << "\n--- TEST 1: Level Filtering (INFO) ---" << std::endl;
    logger.setLevel(LogLevel::INFO);

    logger.debug("This is a DEBUG message (You should NOT see this in console/file)");
    logger.info("This is an INFO message, it should be displayed normally.");

    // --- Test 2: Mass Asynchronous Writing ---
    std::cout << "\n--- TEST 2: Mass Async Writing Test ---" << std::endl;
    for (int i = 1; i <= 5; ++i) {
        logger.warn("Warning message test ID: " + std::to_string(i));
        logger.error("Error message test ID: " + std::to_string(i));
    }

    // --- Test 3: Switching Level to ERROR ---
    std::cout << "\n--- TEST 3: Switch Level to ERROR ---" << std::endl;
    logger.setLevel(LogLevel::ERROR);
    logger.warn("This WARN message should NOT appear now.");
    logger.error("This is a critical ERROR message!");

    // Wait for the thread pool to finish tasks before exiting
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "\n--- TEST FINISHED ---" << std::endl;
    std::cout << "Please check the 'app_log.txt' file in the current directory." << std::endl;


	return 0;
}
