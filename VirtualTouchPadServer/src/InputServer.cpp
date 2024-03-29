#include "InputServer.h"

InputServer::InputServer() {
    mouseListeningPort = 0;
    maxConnections = 0;
    activeConnections = 0;
    mouseListeningSocket = 0;
    clientSocket = 0;
    isDaemon = true;
    debug = false;
    child = false;
    logger = new Logger();
    optionsFilePath = strcpy(new char[strlen(OPTIONS_DEFAULT_FILE_PATH) + 1], OPTIONS_DEFAULT_FILE_PATH);
    mouseFilePath = nullptr;
    mouseSem = nullptr;
    mouseSemName = nullptr;
    mouseClientHandler = nullptr;
    programName = nullptr;
}


InputServer::~InputServer() {
    if (mouseListeningSocket) close(mouseListeningSocket);
    if ((mouseSem) && (mouseSemName)) {
        sem_close(mouseSem);
        if (!child) {
            sem_unlink(mouseSemName);
        }
        delete[] mouseSemName;
    }
    if ((logger)) delete logger;
    delete mouseClientHandler;
    delete[] programName;
    delete[] mouseFilePath;
    delete[] optionsFilePath;
}

bool InputServer::initialize(int argc, char *argv[]) {
    // parse arguments, daemonize, create device handlers, ready sockets

    if (!parseOptions(argc, argv)) {
        logger->error("Error parsing parametres");
        return false;
    }

    if (!parseOptionsFile()) {
        logger->error("Error in config file");
        return false;
    }

    if (!getDefaultPaths()) {
        logger->error("Error setting options");
        return false;
    }

    if (debug) {
        logger->printMessage("Options parsed");
    }

    if (!semaphoresInit()) {
        logger->error("Semaphore initialization failed");
        return false;
    }

    if (isDaemon) {
        if (!daemonize()) {
            logger->error("daemonize() error");
            return false;
        }
    }

    if (isDaemon && debug) {
        logger->printMessage("Daemonized");
    }

    if (!readySocket(&mouseListeningSocket, &mouseServerAddress, mouseListeningPort)) {
        logger->error("readySocket() error");
        return false;
    }

    if (debug) {
        logger->printMessage("listeningSocket bound");
    }

    return true;
}

bool InputServer::andListen() {
    logger->printMessage("Virtual Touch Pad/Screen input server running");
    int listeningSocket;
    int listeningPort;
    char tmp[128];
//	int ret, ndfs;
//	fd_set rfds;
    sigset_t emptymask;
    sigemptyset(&emptymask);

//	ndfs = mouseListeningSocket  + 1 ;

    sprintf(tmp, "waiting for a mouse connection on port: %d", mouseListeningPort);
    logger->printMessage(tmp);

    for (;;) {
        if (receivedEndSignal) {
            if (debug) {
                logger->printMessage("12.12.2011 00:10:45 Received end signal. Will quit");
            }
            if (!child) {
                logger->printMessage("Received end signal. Server will exit after closing existing connections.");
            }
            kill(0, SIGTERM);
            return true;
        }

        // wait for end of at least one connection (or program termination)
        if (activeConnections >= maxConnections) {
            sigset_t mask, oldmask;
            sigemptyset(&mask);
            sigaddset(&mask, SIGTERM | SIGINT | SIGCHLD);
            sigsuspend(&oldmask);
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            continue;
        }

        int addrlen = sizeof(clientAddress);
        memset(&clientAddress, 0, sizeof(clientAddress));

//		FD_ZERO(&rfds);
//		FD_SET(mouseListeningSocket, &rfds);
//
//		if ( (ret = pselect(ndfs,&rfds,NULL,NULL,NULL,&emptymask)) < 0 ) {
//			if (errno != EINTR) logger->error("27.03.2012 18:27:18 select() error",errno);
//		} else {
//				 if ( FD_ISSET(mouseListeningSocket, &rfds)) {
        listeningSocket = mouseListeningSocket;
        listeningPort = mouseListeningPort;
//				}

        clientSocket = accept(listeningSocket, (struct sockaddr *) &clientAddress, (socklen_t *) &addrlen);

        if (receivedSigChild) {
            receivedSigChild = false;
            continue;
        }

        if ((clientSocket == -1) && !receivedEndSignal) {
            if (errno != EAGAIN) logger->error("Virtual MousePad Server:accept() error", errno);
            continue;
        } else if (receivedEndSignal) {
            if (debug) {
                logger->printMessage("Virtual MousePad: Received end signal. Will quit");
            }
            if (!child) {
                logger->printMessage("Received end signal. Server will exit after closing existing connections.");
            }
            return true;
        }

        int pid = handleConnectionRequest(listeningSocket);

        if (pid == 0) {
            return true;
        }

        if (pid == 1) {
            char tmp[128];
            sprintf(tmp, "Connection accepted on port %d", listeningPort);
            logger->printMessage(tmp);
            activeConnections++;
            usleep(500);
            continue;
        } else {
            logger->error("handleConnectionRequest() error");
            continue;
        }
    }
}

int InputServer::handleConnectionRequest(const int acceptedFromListeningSocket) {
    pid_t pid = fork();
    if (pid == -1) {
        logger->error("fork() error", errno);
        return -1;
    }
    if (pid == 0) { // child
        child = true;
        close(mouseListeningSocket);
        handleClient(acceptedFromListeningSocket);
        return 0;
    }
    // parent
    close(clientSocket);
    return 1;
}

void InputServer::handleClient(const int acceptedFromListeningSocket) {
//    if (acceptedFromListeningSocket == keyboardListeningSocket) {
        //keyboardClientHandler = new KeyboardClientHandler(clientSocket, logger, keyboardSemName, keyboardFilePath);
        //keyboardClientHandler->handleClient();
        //delete keyboardClientHandler;
//        keyboardClientHandler = NULL;
//    } else {
        mouseClientHandler = new MouseClientHandler(clientSocket, logger, mouseSemName, mouseFilePath);
        mouseClientHandler->handleClient();
        delete mouseClientHandler;
        mouseClientHandler = nullptr;
//    }
}

void InputServer::handleEndSignal(int s) {
    receivedEndSignal = true;
}

void InputServer::handleSigChild(int s) {
    while (waitpid(-1, 0, WNOHANG) > 0)
        activeConnections--;
    receivedSigChild = true;
}

void InputServer::usage(const char *programName) {
    cerr << "\nUsage: " << programName
         << " [-s] [-d] [-h] [-o CONFIGFILE]  [-m MPORT] [-l MAXCONNECTIONS]  [-M MSFILE] \n\n";
    cerr << "-s\tdo not run as daemon (all messages will be printed to stderr instead of syslog)\n";
    cerr << "-d\tturns debug output on\n";
    cerr << "-h\tdisplay this text\n";
    cerr << "-o\tuse [CONFIGFILE] to read configuration from\n";
    cerr << "-m\tchange mouse listening port to MPORT";
    cerr << " (default port is " << MOUSE_DEFAULT_PORT << ")\n";
    cerr << "-l\tset max connection";
    cerr << " (default value is 2)\n";
    cerr << "-M\tset mouse driver dev node path (default is \"/dev/virtual_touchpad\")\n";
    cerr << "\nYou can close the program by sending SIGINT or SIGKILL (kill PID).\n";
    exit(1);
}

bool InputServer::daemonize() {
    int i;
    pid_t pid;
    openlog(programName, LOG_PID, LOG_DAEMON);

    pid = fork();
    if (pid == -1) {
        logger->error("fork() error");
        return false;
    }

    if (pid != 0) {
        exit(EXIT_SUCCESS);
    }

    if (debug) {
        logger->printMessage("child: parent closed");
    }

    if (setsid() == -1) {
        logger->error("setsid() error");
        return false;
    }

    signal(SIGHUP, SIG_IGN);
    if ((pid = fork()) == -1) {
        logger->error("fork() error");
        return false;
    }
    if (pid != 0) {
        exit(EXIT_SUCCESS);
    }

    if (debug) {
        logger->printMessage("child: parent closed");
    }

    if (setsid() == -1) {
        logger->error("setsid() error");
        return false;
    }

    if ((chdir("/")) == -1) {
        logger->error("chdir() error");
        return false;
    }

    umask(0);

    for (i = getdtablesize() - 1; i >= 0; --i)
        close(i);

    return true;
}

bool InputServer::readySocket(int *listeningSocket, struct sockaddr_in *serverAddress, int listeningPort) {
    char tmp[128];
    int *optval = new int(1);

    *listeningSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*listeningSocket == -1) {
        sprintf(tmp, "socket() error: (%d) %s", errno, strerror(errno));
        logger->error(tmp);
        delete optval;
        return false;
    }
    memset(serverAddress, 0, sizeof(*serverAddress));
    serverAddress->sin_family = AF_INET;
    serverAddress->sin_addr.s_addr = INADDR_ANY;
    serverAddress->sin_port = htons(listeningPort);

    if (bind(*listeningSocket, (struct sockaddr *) serverAddress, sizeof(*serverAddress)) == -1) {
        logger->error(
                "bind() error. Perhaps the port numbers are too low? Use >=1024 if you are not root",
                errno);
        delete optval;
        return false;
    }

    if (listen(*listeningSocket, maxConnections) == -1) {
        logger->error("Listen() error", errno);
        delete optval;
        return false;
    }

    if (setsockopt(*listeningSocket, SOL_SOCKET, SO_REUSEADDR, optval, sizeof(int)) == -1) {
        logger->error("setsockopt() error", errno);
        delete optval;
        return false;
    }

   // if ((flags = fcntl(*listeningSocket, F_GETFL, 0)) < 0) {
   //     logger->error("27.03.2012 20:58:26 fcntl()", errno);
   //     delete optval;
   //     return false;
   // }

   // if (fcntl(*listeningSocket, F_SETFL, flags | O_NONBLOCK) < 0) {
   //     logger->error("27.03.2012 20:58:12 fcntl() ", errno);
   //     delete optval;
   //     return false;
   // }
    delete optval;
    return true;
}

bool InputServer::semaphoresInit() {
    mouseSemName = new char[16];
    memcpy(mouseSemName, "vtpSem", strlen("vtpSem") + 1);

    mouseSem = sem_open(mouseSemName, O_CREAT, S_IWUSR | S_IRUSR, 1);
    if (mouseSem == SEM_FAILED) {
        logger->error("sem_open() failed", errno);
        return false;
    }

    return true;
}

bool InputServer::parseOptions(int argc, char *argv[]) {
    int c;
    struct stat st;
    int ret;

    programName = new char[strlen(basename(argv[0])) + 1];
    programName = strncpy(programName, basename(argv[0]), strlen(basename(argv[0])));

    while ((c = getopt(argc, argv, "sdho:m:l:M:")) != -1) {
        switch (c) {
            case 's':
                isDaemon = false;
                logger->isDaemon = false;
                break;

            case 'd':
                debug = true;
                break;

            case 'h':
                usage(programName);

                break;

            case 'o':
                ret = stat(optarg, &st);
                if (ret != -0) {
                    cerr << "\n-o: \"" << optarg << "\" incorrect argument, file not found\n";
                    usage(programName);
                } else if (!S_ISREG(st.st_mode)) {
                    cerr << "\n-o: \"" << optarg << "\" incorrect argument, not a regular file\n";
                    usage(programName);
                } else {
                    delete[] optionsFilePath;
                    optionsFilePath = strcpy(new char[strlen(optarg) + 1], optarg);
                }
                break;

            case 'm':
                if (isNumber(optarg)) {
                        mouseListeningPort = atoi(optarg);
                    break;
                } else {
                    cerr << "\n-m: " << optarg << " is not a correct port number\n";
                    usage(programName);
                }
            case 'l':
                if (isNumber(optarg)) {
                    maxConnections = atoi(optarg);
                    break;
                } else {
                    cerr << "\n-l: " << optarg << " is not a correct number\n";
                    usage(programName);
                }


            case 'M':
                ret = stat(optarg, &st);
                if (ret != -0) {
                    cerr << "\n-M: \"" << optarg << "\" incorrect argument, file not found\n";
                    usage(programName);
                } else if ((ret == 0) && (!S_ISCHR(st.st_mode))) {
                    cerr << "\n-M: \"" << optarg << "\" incorrect argument, not a character special file\n";
                    usage(programName);
                } else {
                    if (mouseFilePath) delete[] mouseFilePath;
                    mouseFilePath = strcpy(new char[strlen(optarg) + 1], optarg);
                }
                break;

            default:
                usage(programName);
        }
    }
    return true;
}

bool InputServer::getDefaultPaths() {
    if (!mouseFilePath) {
        mouseFilePath = strcpy(new char[strlen(MOUSE_DEFAULT_FILE_PATH) + 1], MOUSE_DEFAULT_FILE_PATH);
        char tmp[128];
        sprintf(tmp, "Mouse file path not specified, using default : %s", mouseFilePath);
        logger->printMessage(tmp);
    }

    if (maxConnections == 0) {
        maxConnections = 2;
        logger->printMessage("Maximum number of connections not specified, using default: 2");
    }
    if (mouseListeningPort == 0) {
        mouseListeningPort = MOUSE_DEFAULT_PORT;
        char tmp[128];
        sprintf(tmp, "Mouse listening port not specified, using default : %d", MOUSE_DEFAULT_PORT);
        logger->printMessage(tmp);
    }
    return true;
}

bool InputServer::parseOptionsFile() {
    int bufferLength = 512;
    FILE *optsFile = NULL;
    char *optsLineBuffer = new char[bufferLength];
    char *optName = new char[bufferLength];
    char *optArg = new char[bufferLength];
    int lineNo = 0;
    bool error = false;
    char *fret;

    if ((optsFile = fopen(optionsFilePath, "r")) == NULL) {
        char tmp[128];
        sprintf(tmp, "could not open options file %s", optionsFilePath);
        logger->error(tmp, errno);
        goto finalize;
    }

    while (!feof(optsFile)) {
        memset(optsLineBuffer, 0, bufferLength);
        memset(optName, 0, bufferLength);
        memset(optArg, 0, bufferLength);

        fret = fgets(optsLineBuffer, bufferLength, optsFile);
        ++lineNo;
        if ((fret != NULL)) {
            int ret = sscanf(optsLineBuffer, "%s%s", optName, optArg);
            if ((ret) == EOF) {
                if (ferror(optsFile)) {
                    logger->error("03.03.2012 20:23:13; sscanf() error", errno);
                    error = true;
                    goto finalize;
                } else {
                    continue;
                }
            } else if (optsLineBuffer[0] == '#') {
                continue;
            } else if (ret < 2) {
                char tmp[128];
                sprintf(tmp, "Error in config file, line %d: \"%s %s\"", lineNo, optName, optArg);
                logger->error(tmp);
            } else {
                if (strcmp(optName, "mouse-port") == 0) {
                    parseOptionsFilePortOpt("mouse-port", &mouseListeningPort, optArg, lineNo);

                } else if (strcmp(optName, "max-connections") == 0) {
                    int arg = 0;
                    if (!isNumber(optArg) || (sscanf(optArg, "%d", &arg) != 1)) {
                        char tmp[128];
                        sprintf(tmp, "Error in config file, line %d: max-connections: incorrect option argument",
                                lineNo);
                        logger->error(tmp);
                    } else {
                        if (maxConnections == 0) maxConnections = arg;
                    }

                } else if (strcmp(optName, "mouse-device-file") == 0) {
                    parseOptionsFileDevOpt("mouse-device-file", &mouseFilePath, optArg, lineNo);

                } else {
                    char tmp[128];
                    sprintf(tmp, "Error in config file, line %d (unknown option): \"%s\"", lineNo, optName);
                    logger->error(tmp);
                }
            }
        } else if (ferror(optsFile)) {
            logger->error("03.03.2012 20:20:34 error parsing config file", errno);
            error = true;
            goto finalize;
        }
    }

    finalize:
    if (optsFile) {
        fclose(optsFile);
    }
    delete[] optArg;
    delete[] optName;
    delete[] optsLineBuffer;

    return !error;
}

void InputServer::parseOptionsFilePortOpt(const char *optName, int *optVariable, char *optArg, int lineNo) {
    int arg = 0;
    if (!isNumber(optArg) || (sscanf(optArg, "%d", &arg) != 1)) {
        char tmp[128];
        sprintf(tmp, "Error in config file, line %d: %s: incorrect option argument", lineNo, optName);
        logger->error(tmp);
    } else {
        if (*optVariable == 0) {
            if (arg == *optVariable) {
                char tmp[128];
                sprintf(tmp, "Error in config file, line %d: mouse-port and keyboard port cannot be the same", lineNo);
                logger->error(tmp);
            } else {
                *optVariable = arg;
            }
        }
    }
}

void InputServer::parseOptionsFileDevOpt(const char *optName, char **optVariable, char *optArg, int lineNo) {
    char *arg = new char[strlen(optArg) + 1];
    if (sscanf(optArg, "%s", arg) != 1) {
        char tmp[128];
        sprintf(tmp, "%s: incorrect option argument in config file, line %d", optName, lineNo);
        logger->error(tmp);
    } else {
        if (!*optVariable) {
            struct stat st;
            int r = stat(optArg, &st);
            if (r != 0) {
                char tmp[128];
                sprintf(tmp,
                        "Warning (config file, line %d): \"%s\" - file not found, will be created upon a successful connection",
                        lineNo, optArg);
                logger->error(tmp);
                *optVariable = strcpy(new char[strlen(arg) + 1], arg);
            } else if (!S_ISCHR(st.st_mode)) {
                char tmp[128];
                sprintf(tmp, "Error in config file, line %d: \"%s\" is not a character special file", lineNo, optArg);
                logger->error(tmp);
            } else {
                *optVariable = strcpy(new char[strlen(arg) + 1], arg);
            }
        }
    }
    delete[] arg;
}

bool InputServer::isNumber(const char *string) {
    int i = 0;
    for (i = 0; string[i] != 0; ++i) {
        if (string[i] < '0' || string[i] > '9') {
            return false;
        }
    }
    return true;
}	

