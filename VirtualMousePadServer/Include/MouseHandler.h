#ifndef MOUSEHANDLER_H_INCLUDED
#define MOUSEHANDLER_H_INCLUDED

#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <syslog.h>
#include <errno.h>
#include <sys/stat.h>

#include <iostream>
using namespace std;

#include "Logger.h"

#define EV_MOVE 0
#define MOUSE_USLEEP_TIME 5000
#define MAXBUF 1024

class MouseHandler {
	public:
		MouseHandler(const char* mouseFilePathArg, class Logger *loggerArg);
		~MouseHandler();
		bool openMouse();
		bool closeMouse();
		bool sendMouseEvent(const unsigned char *data);

	private:
		int mouseFd;
		char *mouseFilePath;
		class Logger *logger;
};

#endif

