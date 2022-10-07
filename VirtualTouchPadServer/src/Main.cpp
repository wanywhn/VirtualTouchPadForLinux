#include "Main.h"

bool InputServer::receivedEndSignal = false;
bool InputServer::receivedSigChild = false;
int InputServer::activeConnections = 0;

struct sigaction sigactionEnd;
struct sigaction sigactionChild;

int main(int argc, char *argv[]) {

	auto *server = new InputServer();

	system("modprobe virtual_touchpad");

	if ( !(server->initialize(argc, argv)) ) {
		delete server;
		return 1;
	}

	sigactionEnd.sa_handler = InputServer::handleEndSignal;
	sigactionChild.sa_handler = InputServer::handleSigChild;

	sigaction(SIGINT, &sigactionEnd, nullptr);
	sigaction(SIGTERM, &sigactionEnd, nullptr);
	sigaction(SIGCHLD, &sigactionChild, nullptr);

	server->andListen();

	delete server;
	return 0;
}

