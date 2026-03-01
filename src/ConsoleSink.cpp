#include"ConsoleSink.h"
#include"LogLevel.h"

void ConsoleSink::log(const LogRecord& record)
	{
		std::string str = format->format(record,true);
		std::lock_guard<std::mutex> lk(mtx_out);
		std::cout << str << '\n';
	}
