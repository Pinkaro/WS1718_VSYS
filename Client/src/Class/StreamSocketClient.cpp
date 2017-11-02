#include "./src/Header/StreamSocketClient.h"

#define MAXDATASIZE 1024

streamSocketClient::streamSocketClient(char * _ip, char * _port) {
	ip = _ip;
	port = atoi(_port);
	
	//make sure that the struct is empty
	memset(&server_address, 0, sizeof(server_address));
	
	//setup the server_address struct
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	inet_aton(ip, &server_address.sin_addr);
	
	//setup the socket
	if( (sockfd = socket(server_address.sin_family, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		exit(1);
	}
}

streamSocketClient::~streamSocketClient() {
	
}

int streamSocketClient::handleSend(char * buffer) {
	string sender;
	string recipient;
	string topic;
	string messagePart;
	string messageWhole(buffer);
	
	cout << "\nSender: ";
	cin >> sender;
	sender.append("\n");
	
	cout << "\nRecipient: ";
	cin >> recipient;
	recipient.append("\n");
	
	cout << "\nTopic: ";
	cin >> topic;
	topic.append("\n");
	
	cout << "\nMessage:\n";
	while(messagePart != ".\n"){
		cin >> messagePart;
		messagePart.append("\n");
		messageWhole.append(messagePart);
	}
	
	//make sure buffer is empty
	memset(buffer, 0, strlen(buffer));
	strcpy(buffer, messageWhole.c_str());
	
	return send(sockfd, buffer, strlen(buffer),0);
}

void streamSocketClient::handleRecv(char * buffer) {
	cout << buffer;
	memset(buffer, 0, strlen(buffer));
	fgets(buffer, MAXDATASIZE, stdin);
	
	if(strcmp(buffer, "SEND\n") == 0){
		cout << "Bytes sent: " << handleSend(buffer);
		
	}else if(strcmp(buffer,"LIST\n") == 0){
		
	}else if(strcmp(buffer,"READ\n") == 0){
		
	}else if(strcmp(buffer,"DEL\n") == 0){
		
	}else if(strcmp(buffer,"QUIT\n") == 0){
		exit(1);
		
	}else{
		
	}
	memset(buffer, 0, strlen(buffer));
}

void streamSocketClient::startConnection() {
	if( (connect(sockfd, (struct sockaddr *) &server_address, sizeof(struct sockaddr))) == -1) {
		perror("Connection failed.");
		exit(1);
	}else {
		printf ("Connection with server (%s) established\n", inet_ntoa (server_address.sin_addr));
	}
	
	int bytesInBuffer;
	char buffer[MAXDATASIZE];
	while (1) {
		cout << "Listening..." << endl;
		if( (bytesInBuffer = recv(sockfd, buffer, MAXDATASIZE-1, 0)) > 0) {
			buffer[bytesInBuffer]='\0';
			handleRecv(buffer);
		}
		else {
			perror("Connect error - no server available");
			exit(1);
		}
	}
}