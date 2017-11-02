#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>
#include <regex>
#include <iostream>
#include <string.h>
#include <ctype.h>
#include "MessageHandlerServer.h"

#ifndef STREAMSOCKETSERVER_H_
#define STREAMSOCKETSERVER_H_

using namespace std;

class streamSocketServer {

public:
	streamSocketServer(char * _port, char * _path);
	virtual ~streamSocketServer();
	
	void startConnection();
	
private:
	struct sockaddr_in server_address, client_address;
	int sockfd, clientfd, port;
	char *path;
	
	void handleRecv (char * buffer);
	void setSocketOptions ();
	void bindSocketToPort ();
	void initCommunicationWithClient ();
	bool handleSend(string message);
	bool checkSendPartsLength(string * messageSplitted);
};

#endif /* STREAMSOCKETSERVER_H_ */