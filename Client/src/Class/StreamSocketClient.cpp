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

// shamelessly copied from "Beej's Guide to network programming"
int streamSocketClient::sendall (int sockfd, char* buffer, int length) {
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
int streamSocketClient::recvall (int sockfd, char* buffer) {
	int total = 0; // how many bytes we've received in total
	int bytesReceived = 0; // how many bytes we've received per call
	string temp; // to save messages received and copy it to buffer at the end of the recv loop
	
	if( (bytesReceived = recv(sockfd, buffer, MAXDATASIZE-1, 0) ) == 0) {
		return bytesReceived;
	}
	temp = string(buffer);
	total =+ bytesReceived;
	
	sleep(1); // make sure the server has enough time to send all his data
	
	while( !(bytesReceived <= 0) ) {
		bytesReceived = 0;
		bytesReceived = recv(sockfd, buffer, MAXDATASIZE, MSG_DONTWAIT); // this time do not wait for send call from server
		
		
		if( !(bytesReceived < 0) ){
			total =+ bytesReceived;
			temp.append(buffer);
		}
	}
	strcpy(buffer, temp.c_str()); // get the whole message into buffer
	return total;
}

int streamSocketClient::handleList(char * buffer) {
	string userName;
	string messageWhole(buffer);
	
	cout << "\nUsername: ";
	getline(cin, userName);
	userName.append("\n");
	messageWhole.append(userName);
	
	// make sure buffer is empty
	memset(buffer, 0, strlen(buffer));
	strcpy(buffer, messageWhole.c_str());
	
	string buffer_str(buffer);
	return sendall(sockfd, buffer, buffer_str.size());
}

int streamSocketClient::handleSend(char * buffer) {
	string sender;
	string recipient;
	string topic;
	string messagePart;
	string messageWhole(buffer);
	
	cout << "\nSender: ";
	getline(cin, sender);
	sender.append("\n");
	messageWhole.append(sender);
	
	cout << "Recipient: ";
	getline(cin, recipient);
	recipient.append("\n");
	messageWhole.append(recipient);
	
	cout << "Topic: ";
	getline(cin, topic);
	topic.append("\n");
	messageWhole.append(topic);
	
	cout << "Message:\n";
	while(messagePart != ".\n"){
		getline(cin, messagePart);
		messagePart.append("\n");
		messageWhole.append(messagePart);
	}
	
	//make sure buffer is empty
	memset(buffer, 0, strlen(buffer));
	strcpy(buffer, messageWhole.c_str());
	
	string buffer_str (buffer);
	return sendall(sockfd, buffer, buffer_str.size());
}

void streamSocketClient::handleRecv(char * buffer) {
	// print whatever just came in
	cout << buffer;
	memset(buffer, 0, strlen(buffer));
	// get user input
	fgets(buffer, MAXDATASIZE, stdin);
	
	if(strcmp(buffer, "SEND\n") == 0){
		cout << "Bytes sent: " << handleSend(buffer) << endl;
		
	}else if(strcmp(buffer,"LIST\n") == 0){
		cout << "Bytes sent: " << handleList(buffer) << endl;
		
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
		if( (bytesInBuffer = recvall(sockfd, buffer)) > 0) {
			buffer[bytesInBuffer]='\0';
			handleRecv(buffer);
		}
		else {
			perror("Connect error - no server available");
			exit(1);
		}
	}
}