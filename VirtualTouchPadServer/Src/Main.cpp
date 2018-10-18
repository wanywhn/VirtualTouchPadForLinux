#include "Main.h"

bool AndroidInputServer::receivedEndSignal = false;
bool AndroidInputServer::receivedSigChild = false;
int AndroidInputServer::activeConnections = 0;

struct sigaction sigactionEnd;
struct sigaction sigactionChild;

int main(int argc, char *argv[]) {

	auto *server = new AndroidInputServer();

	if ( !(server->initialize(argc, argv)) ) {
		delete server;
		return 1;
	}

	sigactionEnd.sa_handler = AndroidInputServer::handleEndSignal;
	sigactionChild.sa_handler = AndroidInputServer::handleSigChild;

	sigaction(SIGINT, &sigactionEnd, nullptr);
	sigaction(SIGTERM, &sigactionEnd, nullptr);
	sigaction(SIGCHLD, &sigactionChild, nullptr);

	server->andListen();

	delete server;
	return 0;
}

