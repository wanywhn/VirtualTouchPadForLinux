#ifndef _ANDROID_INPUT_SERVER_H_INCLUDED
#define _ANDROID_INPUT_SERVER_H_INCLUDED

#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <libgen.h>
#include <fcntl.h>

#include "Logger.h"
#include "MouseHandler.h"
#include "MouseClientHandler.h"

#define MOUSE_DEFAULT_PORT 6781

#define MOUSE_DEFAULT_FILE_PATH "/dev/virtual_touchpad"
#define OPTIONS_DEFAULT_FILE_PATH "/etc/virtual_touchpad_server.conf"

class InputServer {
	public:
		InputServer();
		~InputServer();
		bool initialize(int argc, char* argv[]);
		bool andListen();
		int receiveMessage();
		int sendMessage();
		static void handleEndSignal(int s);
		static void handleSigChild(int s);

	private:
		bool isNumber(const char *string);
		void usage(const char *programName);
		bool parseOptions(int argc, char* argv[]);
		bool parseOptionsFile();
		bool getDefaultPaths();
		bool daemonize();
		bool readySocket(int *listeningSocket, struct sockaddr_in *serverAddress, int listeningPort);
		bool semaphoresInit();
		void handleClient(const int acceptedFromListeningSocket);
		int handleConnectionRequest(const int acceptedFromListeningSocket);

	void parseOptionsFilePortOpt(const char *optName, int *optVariable, char *optArg, int lineNo);
		void parseOptionsFileDevOpt(const char *optName, char **optVariable, char *optArg, int lineNo);

		static bool receivedEndSignal;
		static bool receivedSigChild;
		static int activeConnections;
		int mouseListeningPort;
		int mouseListeningSocket;
		int maxConnections;
		int clientSocket;
		bool isDaemon;
		bool debug;
		char *programName;
		bool child;

		sem_t *mouseSem;
		char *mouseSemName;



		struct sockaddr_in mouseServerAddress;
		struct sockaddr_in clientAddress;

		char *optionsFilePath;
		char *mouseFilePath;

		class MouseClientHandler *mouseClientHandler;

		class Logger *logger;
};

#endif

