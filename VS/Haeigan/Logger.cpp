#include "Logger.h"


Logger::Logger(void)
{
	out_log.open(OUT_LOG_NAME, std::ios::trunc);
	err_log.open(ERR_LOG_NAME, std::ios::trunc);
	
}


Logger::~Logger(void)
{
	out_log.close();
	err_log.close();
}

Logger* Logger::Get_instance()
{
	static Logger* instance = new Logger();
	return instance;
}

std::string Logger::timetag()
{
	std::time_t time_now;
	std::time(&time_now);
	std::string time_tag = std::ctime(&time_now);
	time_tag.resize(time_tag.length() - 6);
	return time_tag;
}

void Logger::log(int level, std::string msg)
{
	std::string tag = "[DEBUG] ";
	std::string time_tag = timetag();

	if(level >= 1)
	{
		tag = "[ERROR] ";
		err_log << tag.c_str() << "<" << time_tag.c_str() << "> " << msg.c_str() << std::endl;
	}
	out_log << tag.c_str() << "<" << time_tag.c_str() << "> " << msg.c_str() << std::endl;
}

// public api:

void Logger::Debug(std::string msg)
{
	log(0, msg);
	return;
}

void Logger::Error(std::string msg)
{
	log(1, msg);
	return;
}