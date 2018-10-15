#include "Logger.h"

Logger::Logger() {
	isDaemon = true;
}

void Logger::printMessage(const char *message) {
	if ( isDaemon )
		syslog(LOG_INFO, "%s", message);
	else
		cout << message << endl;
}

void Logger::error(const char *message) {
	if ( isDaemon )
		syslog(LOG_ERR, "%s", message);
	else
		cerr << message << endl;
}

void Logger::error(const char *message, const int localerrno) {
	char buffer[256];
	sprintf(buffer, "%s: (%d) %s",message,localerrno,strerror(localerrno));
	error(buffer);
}
