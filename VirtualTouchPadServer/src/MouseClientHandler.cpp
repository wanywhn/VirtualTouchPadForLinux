#include "MouseClientHandler.h"
#include <unistd.h>


MouseClientHandler::MouseClientHandler(const int connectionSocketArg, class Logger *loggerArg, char* mouseSemName,  
		char* mouseFilePathArg)
	: ClientHandler( connectionSocketArg, loggerArg, mouseSemName, mouseFilePathArg ) {
		mouseHandler = nullptr;
	}

MouseClientHandler::~MouseClientHandler() {
	if (mouseHandler) {
		mouseHandler->closeMouse();
		close(connectionSocket);
		delete mouseHandler;
	}
}

bool MouseClientHandler::handleClient() {

	int *optval = new int(1);
	if (setsockopt(connectionSocket,SOL_SOCKET,SO_KEEPALIVE,optval, sizeof(int)) == -1) {
		logger->error("setsockopt() error",errno);
		delete optval;
		return false;
	}

	if (setsockopt(connectionSocket,SOL_TCP,TCP_KEEPCNT,optval, sizeof(int)) == -1) {
		logger->error("setsockopt() error",errno);
		delete optval;
		return false;
	}

	*optval = 60;
	if (setsockopt(connectionSocket,SOL_TCP,TCP_KEEPIDLE,optval, sizeof(int)) == -1) {
		logger->error("setsockopt() error",errno);
		delete optval;
		return false;
	}
	delete optval;

	if ( !readyDeviceHandler() ) {
		logger->error("Could not ready MouseHandler");
		return false;
	}

	//TODO 6 bytes and delete
	unsigned char *buffer=(unsigned char *)malloc(6*sizeof(char));
	const int msgLength=6;
	struct pollfd pollSock;


	auto ret=read(connectionSocket,buffer,6);
	if(ret==6){
		sem_wait(semaphore);
		mouseHandler->sendMouseEvent(buffer);
		auto ret=mouseHandler->getMouseStatu();
		if(ret!=1){
			return false;
		}
		sem_post(semaphore);
	}else{
		return false;
	}

	pollSock.fd = connectionSocket;
	pollSock.events = (0 | POLLIN | POLLRDHUP); // there is input data or connection closed
	pollSock.revents = 0;

	if (!sendReady()) {
		logger->error("sendReady() error");
		return true;
	}

//	errno = 0;
//	if ( (setpriority(PRIO_PROCESS,PRIO_PROCESS,-19) < -1) || ( errno != 0 ) ) {
//		logger->error("setpriority() errror",errno);
//	}
//
//	struct sched_param sp;
//	sp.sched_priority = 50;
//
//	errno = 0;
//	if ( (sched_setscheduler(0,SCHED_RR,&sp) < -1) || ( errno != 0 ) ) {
//		logger->error("sched_setscheduler() errror",errno);
//	}

	while (!receivedEndSignal) {

		int ret = poll(&pollSock,1,900000);
		if (ret == 0) {
			logger->error("Client timeout");
			break;
		} else if (ret == -1) {
			if (errno != EINTR) {
				logger->error("poll() error",errno);
			}
			break;
		}
		if ( (pollSock.revents & POLLRDHUP) == POLLRDHUP ) {
			logger->printMessage("Mouse disconnected");
			break;
		}

		int readBytes = 0;
		int retVal = 0;


		while (readBytes != msgLength) {
			if (receivedEndSignal) {
				return true;
			}
			if ( (retVal =read(pollSock.fd,buffer+readBytes, msgLength-readBytes )) == -1 ) {
				logger->error("read() error",errno);
				return true;
			}
			readBytes += retVal;
		}
		if (readBytes != msgLength) {
			char tmp[128];
			sprintf(tmp,"20.12.2011 23:47:10 could not read message (read %d bytes of expected %d). Client timeout.",readBytes,msgLength);
			logger->error(tmp);
			break;
		}

		sem_wait(semaphore);
		mouseHandler->sendMouseEvent(buffer);
		sem_post(semaphore);
	}
	delete buffer;
	return true;
}


bool MouseClientHandler::readyDeviceHandler(){

	if (!(mouseHandler = new MouseHandler(deviceFilePath,logger)) ) {
		logger->error("error creating mouseHandler");
		logger->error("readyHandlers() failed");
		return false;
	}

	if (!mouseHandler->openMouse()) {
		logger->error("error opening mouse");
		logger->error("readyHandlers() failed");
		return false;
	}

	return true;
}

