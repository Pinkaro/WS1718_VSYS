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

#ifndef STREAMSOCKETCLIENT_H_
#define STREAMSOCKETCLIENT_H_

using namespace std;

class streamSocketClient {

public:
	streamSocketClient(char * _ip, char * _port);
	virtual ~streamSocketClient();
	
	void startConnection();
	
private:
	int sockfd;
	struct sockaddr_in server_address;
	char * ip;
	int port;
	
	void handleRecv(char * buffer);
	int handleSend(char * buffer);
	int sendall(int sockfd, char* buffer, int length);
	int recvall(int sockfd, char* buffer);
	int handleList(char * buffer);
};

#endif /* STREAMSOCKETCLIENT_H_ */