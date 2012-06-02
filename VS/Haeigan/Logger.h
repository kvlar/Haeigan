#pragma once
#include <iostream>
#include <fstream>
#include <ctime>

const std::string OUT_LOG_NAME = "stdout.txt";
const std::string ERR_LOG_NAME = "stderr.txt";

class Logger
{
private:
	Logger(void);
public:
	static Logger* Get_instance();
	void Debug(std::string);
	void Error(std::string);
	~Logger(void);

private:
	void log(int, std::string);
	std::string timetag();

private:
	std::ofstream out_log;
	std::ofstream err_log;

};
