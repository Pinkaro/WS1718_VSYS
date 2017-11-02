#include "./src/Header/StreamSocketServer.h"

#define MAXDATASIZE 1024
#define BACKLOG 5

streamSocketServer::streamSocketServer(char * _port, char * _path){
	port = atoi(_port);
	path = _path;
	
	//make sure that the struct is empty
	memset(&server_address, 0, sizeof(server_address));
	
	//setup the server_address struct
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = INADDR_ANY;
	
	//setup the socket
	if( (sockfd = socket(server_address.sin_family, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		exit(1);
	}
	
	//setSocketOptions();
	bindSocketToPort();
}

streamSocketServer::~streamSocketServer() {
	
}

//to set the socket options, specifically to be able to reuse a port immidiately
void streamSocketServer::setSocketOptions () {
    printf("Set socket options ... \n");
    int yes = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("set socket options error");
        exit(1);
    }
}

//to bind a socket to a port, duh
void streamSocketServer::bindSocketToPort () {
    printf("Binding socket to port ... \n");
    if(bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("bind error");
        exit(1);
    }
}

// to handle all incoming messages from client
void streamSocketServer::handleRecv (char * buffer) {
	
	MessageHandlerServer messageHandler = MessageHandlerServer(buffer, path);
	memset(buffer, 0, strlen(buffer));
	buffer = messageHandler.HandleMessage();
	
	string message(buffer);
	string command = message.substr(0, message.find("\n"));
	//delete command line, don't need it anymore
	message.erase(0, message.find("\n") + 1);
	
	if(strcmp(command.c_str(),"SEND") == 0){
		if(messageHandler.getResult()){
			send(clientfd, buffer, 3, 0);
		}else{
			send(clientfd, buffer, 4, 0);
		}
		
	}else if(strcmp(command.c_str(),"LIST") == 0){
		cout << "got into LIST if" << endl;
		//strcat(buffer, " command received");
		cout << "LIST Bytes sent: " << send(clientfd, buffer, sizeof buffer, 0) << endl;;
		
	}else if(strcmp(command.c_str(),"READ") == 0){
		cout << "got into READ if" << endl;
		//strcat(buffer, " command reveiced.");
		send(clientfd, buffer, sizeof buffer, 0);
		
	}else if(strcmp(command.c_str(),"DEL") == 0){
		cout << "got into DEL if" << endl;
		//strcat(buffer, " command received.");
		send(clientfd, buffer, strlen(buffer), 0);
		
	}else if(strcmp(command.c_str(),"QUIT") == 0){
		exit(1);
		
	}else{
		cout << "got into ELSE if n e w" << endl;
		strcpy(buffer, "Send me a real command, dude.");
		cout << "dunno it fam (buffer size: " << strlen(buffer) << "), bytes send: " << send(clientfd, buffer, strlen(buffer), 0) << endl;
	}
	memset(buffer, 0, strlen(buffer));
	cout << "cycled through all ifs in handleRecv()" << endl;
}

//start the recv/send loop
void streamSocketServer::initCommunicationWithClient () {
	int bytesInBuffer;
	char buffer[MAXDATASIZE];
	
	//send welcome message and list of available commands
	strcpy(buffer, "Welcome to my server!\nSEND, LISŢ, READ, DEL, QUIT\nEnter command: ");
	send(clientfd, buffer, strlen(buffer), 0);
	memset(buffer, 0, sizeof buffer);
	
	while(1) {
		cout << "Listening ..." << endl;
		if( (bytesInBuffer = recv (clientfd, buffer, MAXDATASIZE - 1, 0)) > 0 ) {
			buffer[bytesInBuffer] = '\0';
			
			handleRecv(buffer);
			
		}else if (bytesInBuffer == 0) {
			printf("Client closed remote socket\n");
			break;
		}else {
			perror("recv() error");
			exit(1);
		}
	}
}

//blocks the program until a client has successfully connected
void streamSocketServer::startConnection() {
	//define backlog, more specifically how many clients can queue up for connection
	if(listen(sockfd, BACKLOG) == -1) {
        perror("listen error");
        exit(1);
    }
	
	printf("Starting searching for clients ... \n");

	socklen_t clientLength = sizeof(struct sockaddr_in);
	if( (clientfd = accept(sockfd, (struct sockaddr *)&client_address, &clientLength))  == -1) {
		perror("accept error");
		exit(1);
	}
    printf("Client connected from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    close(sockfd); // only one connection allowed
	
	initCommunicationWithClient();
}