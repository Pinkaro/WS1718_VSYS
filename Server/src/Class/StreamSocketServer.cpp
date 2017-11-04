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

// shamelessly copied from "Beej's Guide to network programming"
int streamSocketServer::sendall (int sockfd, char* buffer, int length) {
	int total = 0;        // how many bytes we’ve sent
	int bytesleft = length; // how many we have left to send
	int n;
	
	while(total < length) {
		n = send(sockfd, buffer+total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}
	return total; // return -1 on failure, 0 on success
}

// make sure to receive everything sent, returns the total number of received bytes
int streamSocketServer::recvall (int sockfd, char* buffer) {
	int total = 0; // how many bytes we've received in total
	int bytesReceived = 0; // how many bytes we've received per call
	string temp; // to save messages received and copy it to buffer at the end of the recv loop
	
	if( (bytesReceived = recv(sockfd, buffer, MAXDATASIZE-1, 0) ) == 0) {
		return bytesReceived;
	}
	temp = string(buffer);
	total =+ bytesReceived;
	
	sleep(1); // make sure the client has enough time to send all his data
	
	while( !(bytesReceived <= 0) ) {
		bytesReceived = 0;
		bytesReceived = recv(sockfd, buffer, MAXDATASIZE, MSG_DONTWAIT); // this time do not wait for send call from client
		
		
		if( !(bytesReceived < 0) ){
			total =+ bytesReceived;
			temp.append(buffer);
		}
	}
	strcpy(buffer, temp.c_str()); // get the whole message into buffer
	return total;
}

// to handle all incoming messages from client
void streamSocketServer::handleRecv (char * buffer) {
	string message(buffer);
	
	// make a copy of buffer to pass on to messagehandler as we do not want to
	// fiddle with original buffer
	char* buffercopy = new char [message.size() + 1];
	copy(message.begin(), message.end(), buffercopy);
	buffercopy[message.size()] = '\0';
	
	string command = message.substr(0, message.find("\n"));
	//delete command line, don't need it anymore
	message.erase(0, message.find("\n") + 1);
	
	MessageHandlerServer messageHandler = MessageHandlerServer(buffercopy, path);
	memset(buffer, 0, strlen(buffer));
	buffer = messageHandler.HandleMessage();
	
	if(strcmp(command.c_str(),"SEND") == 0){
		if(messageHandler.getResult()){
			sendall(clientfd, buffer, 3);
		}else{
			sendall(clientfd, buffer, 4);
		}
		
	}else if(strcmp(command.c_str(),"LIST") == 0){
		cout << "Sending LIST result... " << endl;
		cout << buffer << endl;
		if(messageHandler.getResult()){
			sendall(clientfd, buffer, MAXDATASIZE-1);
		}else{
			sendall(clientfd, buffer, 2);
		}
		
	}else if(strcmp(command.c_str(),"READ") == 0){

		
	}else if(strcmp(command.c_str(),"DEL") == 0){

		
	}else if(strcmp(command.c_str(),"QUIT") == 0){
		exit(1);
		
	}else{
		strcpy(buffer, "Send me a real command, dude. (STREAMSOCKET)");
		cout << "dunno it fam (buffer size: " << strlen(buffer) << "), bytes send: " << sendall(clientfd, buffer, strlen(buffer)) << endl;
	}
	
	//delete[] buffercopy; // free buffercopy, dont need it anymore
	memset(buffer, 0, strlen(buffer));	
}

//start the recv/send loop
void streamSocketServer::initCommunicationWithClient () {
	int bytesInBuffer;
	char buffer[MAXDATASIZE];
	
	//send welcome message and list of available commands
	strcpy(buffer, "Welcome to my server!\nSEND, LISŢ, READ, DEL, QUIT\nEnter command: ");
	sendall(clientfd, buffer, strlen(buffer));
	memset(buffer, 0, sizeof buffer);
	
	while(1) {
		cout << "Listening ..." << endl;
		if( (bytesInBuffer = recvall (clientfd, buffer)) > 0 ) {
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