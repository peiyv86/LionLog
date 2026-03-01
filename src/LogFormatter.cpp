#include"LogFormatter.h"

LogFormatter::LogFormatter(const std::string& pattern_) : pattern(pattern_)
	{
		for (size_t i = 0; i < pattern.size(); ++i) {
			// 情況 A：處理普通字元 (如 '[' , ']' , ' ' 或 '-' )
			if (pattern[i] != '%' || i + 1 >= pattern.size()) {
				char c = pattern[i];
				items.emplace_back([c](const LogRecord&) {
					return std::string(1,c);
					});
				continue;
			}

			char formatChar = pattern[i + 1];
			switch (formatChar) {
			case 'l':
				items.emplace_back([](const LogRecord& r) 
					{ return r.getLevelStr(); }); 
				break;
			case 'm':
				items.emplace_back([](const LogRecord& r) 
					{ return r.message; }); 
				break;
			case 't':
				items.emplace_back([](const LogRecord& r) 
					{ return r.thread_id; }); 
				break;
			case 'd':
				items.emplace_back([](const LogRecord& r) 
					{ return r.time; }); 
				break;
			case 'f':
				items.emplace_back([](const LogRecord& r) 
					{ return r.file.size()==0 ? "unknown" : r.file; });
				break;
			case 'n':
				items.emplace_back([](const LogRecord& r) 
					{ return std::to_string(r.line); }); 
				break;
			case '%':
				items.emplace_back([](const LogRecord&) 
					{ return "%"; }); 
				break;
			default:
				// 未知佔位符，把 %z 原樣存下來
				items.emplace_back([formatChar](const LogRecord&) {
					return "%" + formatChar;
					});
				break;
			}
			i++; // 跳過已處理的格式字元
		}
	};
std::string LogFormatter::format(const LogRecord& record,bool color) 
	{
		std::string out="";
		if (color)
		{
			switch (record.level)
			{
			case LogLevel::DEBUG:
				out+= "\033[32m";
				break;
			case LogLevel::INFO:
				out+= "\033[34m";
				break;
			case LogLevel::WARN:
				out+= "\033[33m";
				break;
			case LogLevel::ERROR:
				out+= "\033[31m";
				break;
			}
		}
		for (auto& item : items) 
		{
			out+= item(record);
		}
		if(color)out+= "\033[0m";
		return out;
	}