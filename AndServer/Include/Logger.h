#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <iostream>
#include <syslog.h>
#include <string.h>
#include <stdio.h>

using namespace std;

class Logger {
	public:
		Logger();
		void printMessage(const char* message);
		void error(const char* message);
		void error(const char* message, const int localerrno);

		bool isDaemon;
};

#endif

