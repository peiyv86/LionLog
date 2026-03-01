#include"FileSink.h"
#include"LogLevel.h"

FileSink::FileSink(const std::string& s) :path(s) {
		outfile.open(path, std::ios::app);
	};
    
void FileSink::log(const LogRecord& record)
	{
		if (!outfile.is_open()) return;
			std::string str = format->format(record,false);
			{
				std::lock_guard<std::mutex> lk(mtx_file);
				outfile << str << '\n';
				outfile.flush();
			}
	};

FileSink::~FileSink()
	{
		if(outfile.is_open())outfile.close();
	};
