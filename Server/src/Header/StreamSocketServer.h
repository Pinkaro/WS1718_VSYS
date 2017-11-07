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
#include <ldap.h>
#include <chrono>

#define LDAP_HOST "ldap.technikum-wien.at"
#define LDAP_PORT 389
#define SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE
#define FILTER "(uid=if17b127)"
#define BIND_USER NULL		/* anonymous bind with user and pw NULL */
#define BIND_PW NULL
#define BLOCKTIME 5000 //blocktime in milliseconds

#ifndef STREAMSOCKETSERVER_H_
#define STREAMSOCKETSERVER_H_

using namespace std;

struct clientinfo {
	int clientfd;
	struct in_addr clientaddress;
};

class streamSocketServer {

public:
	streamSocketServer(char * _port, char * _path);
	virtual ~streamSocketServer();
	
	void startConnection();
	
private:
	struct sockaddr_in server_address;
	int sockfd, port;
	char* path;
	map<char*, chrono::milliseconds> bannedIPs; // map to store currently banned IP addresses, key is the IP address itself
												// value is the timestamp in milliseconds
	void handleRecv (char* buffer, int clientfd);
	void setSocketOptions ();
	void bindSocketToPort ();
	void initCommunicationWithClient (struct clientinfo ci);
	bool isIpAlreadyBlocked (in_addr address);
	bool checkLogin (char* buffer, int tryNumber, in_addr address);
	void blockIpAddress (in_addr address);
	bool checkSendPartsLength(string * messageSplitted);
	int sendall (int sockfd, char* buffer, int length);
	int recvall (int sockfd, char* buffer); 
};

#endif /* STREAMSOCKETSERVER_H_ */