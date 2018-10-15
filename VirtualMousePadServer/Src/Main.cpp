#include "Main.h"

bool AndroidInputServer::receivedEndSignal = false;
bool AndroidInputServer::receivedSigChild = false;
int AndroidInputServer::activeConnections = 0;

struct sigaction sigactionEnd;
struct sigaction sigactionChild;

int main(int argc, char *argv[]) {

	AndroidInputServer *server = new AndroidInputServer();
	if(server==NULL){
		return 1;
	}

	if ( !(server->initialize(argc, argv)) ) {
		delete server;
		return 1;
	}

	sigactionEnd.sa_handler = server->handleEndSignal;
	sigactionChild.sa_handler = server->handleSigChild;

	sigaction(SIGINT, &sigactionEnd, NULL);
	sigaction(SIGTERM, &sigactionEnd, NULL);
	sigaction(SIGCHLD, &sigactionChild, NULL);

	server->andListen();

	delete server;
	return 0;
}

