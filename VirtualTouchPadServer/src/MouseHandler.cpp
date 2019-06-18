#include "MouseHandler.h"
#include <unistd.h>
#include <MouseHandler.h>

MouseHandler::MouseHandler(const char* mouseFilePathArg, class Logger *loggerArg) {
	mouseFd = 0;
	mouseFilePath = new char[strlen(mouseFilePathArg) + 1];;
	strcpy(mouseFilePath,mouseFilePathArg);
	logger = loggerArg;
}

MouseHandler::~MouseHandler(){
	delete[] mouseFilePath;
}

bool MouseHandler::openMouse(){
	struct stat st;
	if ( stat(mouseFilePath, &st) != 0 ) {
		dev_t dev = makedev(60, 0);
		if ( mknod(mouseFilePath, S_IFCHR | S_IWUSR, dev) == -1 ) {
			logger->error("03.03.2012 14:13:16 openMouse: Could not create a mouse device file",errno);
			return false;
		}
	}

	if ( (mouseFd = open(mouseFilePath, O_WRONLY)) == -1 ) {
		logger->error("11.12.2011 21:53:45 openMouse() error: could not open mouse file",errno);
		return false;
	}
	return true;
}

bool MouseHandler::closeMouse() {
	if (mouseFd != 0 && mouseFd != -1) {
		if ( close(mouseFd) != 0 ) {
			char tmp[128];
			sprintf(tmp,"11.12.2011 22:16:23 close() mouse error: (%d) %s",errno, strerror(errno));
			logger->error(tmp);
			return false;
		}
	}
	return true;
}

bool MouseHandler::sendMouseEvent(const unsigned char *data){
	const int data_size=6;
	int writed=write(mouseFd,data,data_size);
	if(writed==-1){
		logger->error("11.12.2011 21:53:54 sendMouseEvent write() error");
		return false;
	}else if(writed!=data_size){
		char tmp[128];
		sprintf(tmp,"sendMouseEvent write bytes only %d,wanted %d",writed,data_size);
		logger->error(tmp);
	}
	return true;
}

int  MouseHandler::getMouseStatu() {
    int  ret;
    int readed=read(mouseFd,&ret,sizeof(int));
    if(readed==sizeof(int)){
        return ret;
    }else{
        return DEVICE_NOT_CONFIGURED;
    }

}




