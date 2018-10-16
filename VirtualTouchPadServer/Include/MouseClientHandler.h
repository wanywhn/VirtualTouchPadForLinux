#ifndef MOUSE_CLIENT_HANDLER_H_INCLUDED
#define MOUSE_CLIENT_HANDLER_H_INCLUDED

#include "ClientHandler.h"
#include <sys/time.h> //setpriority()
#include <sys/resource.h> //setpriority()

class MouseClientHandler : ClientHandler {
	public:
		MouseClientHandler(const int connectionSocketArg, class Logger *loggerArg, char* mouseSemName,
 char* mouseFilePath);
		~MouseClientHandler();
		bool handleClient();

	private:
		bool readyDeviceHandler();

		class MouseHandler *mouseHandler;
};

#endif

