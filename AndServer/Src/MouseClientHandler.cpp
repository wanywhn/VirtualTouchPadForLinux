#include "MouseClientHandler.h"
#include <unistd.h>


MouseClientHandler::MouseClientHandler(const int connectionSocketArg, class Logger *loggerArg, char* mouseSemName,  
		char* mouseFilePathArg)
	: ClientHandler( connectionSocketArg, loggerArg, mouseSemName, mouseFilePathArg ) {
		mouseHandler = NULL;
	}

MouseClientHandler::~MouseClientHandler() {
	if (mouseHandler) {
		mouseHandler->closeMouse();
		delete mouseHandler;
	}
}

bool MouseClientHandler::handleClient() {

	int *optval = new int(1);
	if (setsockopt(connectionSocket,SOL_SOCKET,SO_KEEPALIVE,optval, sizeof(int)) == -1) {
		logger->error("02.03.2012 17:14:53 setsockopt() error",errno);
		delete optval;
		return false;
	}

	if (setsockopt(connectionSocket,SOL_TCP,TCP_KEEPCNT,optval, sizeof(int)) == -1) {
		logger->error("02.03.2012 17:24:57 setsockopt() error",errno);
		delete optval;
		return false;
	}

	*optval = 60;
	if (setsockopt(connectionSocket,SOL_TCP,TCP_KEEPIDLE,optval, sizeof(int)) == -1) {
		logger->error("02.03.2012 17:24:57 setsockopt() error",errno);
		delete optval;
		return false;
	}
	delete optval;

	if ( !readyDeviceHandler() ) {
		logger->error("11.12.2011 22:49:53 Could not ready MouseHandler");
		return false;
	}

	//TODO 6 bytes and delete
	unsigned char *buffer=(unsigned char *)malloc(6*sizeof(char));
	const int msgLength=6;
	struct pollfd pollSock;

	pollSock.fd = connectionSocket;
	pollSock.events = (0 | POLLIN | POLLRDHUP); // there is input data or connection closed
	pollSock.revents = 0;

	if (!sendReady()) {
		logger->error("11.12.2011 23:25:02 sendReady() error");
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
			logger->error("21.12.2011 14:12:10 Client timeout");
			break;
		} else if (ret == -1) {
			if (errno != EINTR) {
				logger->error("21.12.2011 14:12:05 poll() error",errno);
			}
			break;
		}
		if ( (pollSock.revents & POLLRDHUP) == POLLRDHUP ) {
			logger->printMessage("10.12.2011 23:59:58 Mouse disconnected");
			break;
		}

		int readBytes = 0;
		int retVal = 0;


		while (readBytes != msgLength) {
			if (receivedEndSignal) {
				return true;
			}
			if ( (retVal =read(pollSock.fd,buffer+readBytes, msgLength-readBytes )) == -1 ) {
				logger->error("21.12.2011 02:25:03 read() error",errno);
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
		logger->error("06.12.2011 01:34:22 error creating mouseHandler");
		logger->error("readyHandlers() failed");
		return false;
	}

	if (!mouseHandler->openMouse()) {
		logger->error("07.12.2011 15:27:33 error opening mouse");
		logger->error("readyHandlers() failed");
		return false;
	}

	return true;
}

