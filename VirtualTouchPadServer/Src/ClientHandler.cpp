#include "ClientHandler.h"
#include <unistd.h>

ClientHandler::ClientHandler(const int connectionSocketArg, class Logger *loggerArg, char* semName, 
char* filePathArg){
	deviceFilePath = strcpy(new char[strlen(filePathArg) + 1],filePathArg);
	connectionSocket = connectionSocketArg;
	receivedEndSignal = false;
	receivedEndMessage = false;
	logger = loggerArg;
	if ((semaphore = sem_open(semName,0,O_RDWR,0)) == SEM_FAILED) {
		logger->error("sem_open failed",errno);
	}
}

ClientHandler::~ClientHandler() {
	sem_close(semaphore);
	if (deviceFilePath) delete[] deviceFilePath;
	close(connectionSocket);
}


bool ClientHandler::sendReady() {
//	unsigned char *buffer = new unsigned char[1];
//	unsigned char a = MSG_READY;
//	memcpy(buffer,&a,sizeof(char));
//	if ( SSL_write(ssl, buffer, sizeof(char)) != sizeof(char) ) {
//		errError("16.12.2011 22:16:40 write() error");
//		delete[] buffer;
//		return false;
//	}
//	delete[] buffer;
	return true;
}

void ClientHandler::errError(const char* msg) {
	logger->error(msg);
}

