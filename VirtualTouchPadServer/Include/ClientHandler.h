
#ifndef CLIENT_HANDLER_H_INCLUDED
#define CLIENT_HANDLER_H_INCLUDED

#include <poll.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netinet/tcp.h>

#include "messageTypes.h"
#include "MouseHandler.h"

class ClientHandler {
	public:
		ClientHandler(const int connectionSocketArg, class Logger *loggerArg, char* semName ,
					 char* filePathArg);
		virtual ~ClientHandler();
		virtual bool handleClient() = 0;

	protected:
		virtual bool readyDeviceHandler() = 0;
		bool sendReady();
		void errError(const char* msg);

		bool receivedEndSignal;
		bool receivedEndMessage;
		int connectionSocket;

		sem_t *semaphore;

		class Logger *logger;

		char *deviceFilePath;
};

#endif

