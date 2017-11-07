#include "./src/Header/StreamSocketClient.h"

#define MAXDATASIZE 1024

streamSocketClient::streamSocketClient(char * _ip, char * _port) {
	ip = _ip;
	port = atoi(_port);
	userNameGlobal = "NOT SET XXX"; // unset username, is too long by default
	loginSuccess = false;
	
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

int streamSocketClient::handleDel(char* buffer) {
	string messageNumber;
	string messageWhole(buffer);
	
	messageWhole.append(userNameGlobal);
	messageWhole.append("\n");
	
	cout << "\nMessage number: ";
	getline(cin, messageNumber);
	messageNumber.append("\n");
	messageWhole.append(messageNumber);
	
		// make sure buffer is empty
	memset(buffer, 0, strlen(buffer));
	strcpy(buffer, messageWhole.c_str());
	
	string buffer_str(buffer);
	return sendall(sockfd, buffer, buffer_str.size());
}

int streamSocketClient::handleRead(char* buffer) {
	string messageNumber;
	string messageWhole(buffer);
	
	messageWhole.append(userNameGlobal);
	messageWhole.append("\n");
	
	cout << "Message number: ";
	getline(cin, messageNumber);
	messageNumber.append("\n");
	messageWhole.append(messageNumber);
	
	// make sure buffer is empty
	memset(buffer, 0, strlen(buffer));
	strcpy(buffer, messageWhole.c_str());
	
	string buffer_str(buffer);
	return sendall(sockfd, buffer, buffer_str.size());
}

int streamSocketClient::handleList(char* buffer) {
	string messageWhole(buffer);
	
	messageWhole.append(userNameGlobal);
	messageWhole.append("\n");
	
	// make sure buffer is empty
	memset(buffer, 0, strlen(buffer));
	strcpy(buffer, messageWhole.c_str());
	
	string buffer_str(buffer);
	return sendall(sockfd, buffer, buffer_str.size());
}

int streamSocketClient::handleSend(char* buffer) {
	string recipient;
	string topic;
	string messagePart;
	string messageWhole(buffer);
		
	messageWhole.append(userNameGlobal);
	messageWhole.append("\n");
	
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

int streamSocketClient::handleLogin(char* buffer) {
	string password;
	string messageWhole;
	
	cout << "\nUsername: ";
	getline(cin, userNameTemp);
	userNameTemp.append("\n");
	messageWhole.append(userNameTemp);
	
	cout << "Password: ";
	getline(cin, password);
	password.append("\n");
	messageWhole.append(password);
	
	//make sure buffer is empty
	memset(buffer, 0, strlen(buffer));
	strcpy(buffer, messageWhole.c_str());
	
	string buffer_str (buffer);
	return sendall(sockfd, buffer, buffer_str.size());
}

void streamSocketClient::handleRecv(char* buffer) {
	// print whatever just came in
	cout << buffer;
	
	// if login has not succeeded yet, check for status sent by server
	if(!loginSuccess) {
		string messageWhole (buffer);
		string status = messageWhole.substr(0, messageWhole.find("\n")); // extract status of sent command
		
		if(strcmp(status.c_str(), "LOGIN OK\n")) { // if server tells us that we successfully logged in, we set the global username
			loginSuccess = true;
			userNameGlobal = userNameTemp;
		}
	}
	
	
	memset(buffer, 0, strlen(buffer));
	
	fgets(buffer, MAXDATASIZE, stdin);
	
	if( (strcmp(buffer, "LOGIN\n") == 0 ) && !loginSuccess){
		cout << "(LOGIN) Bytes sent: " << handleLogin(buffer) << endl;
		
	}else if(strcmp(buffer, "SEND\n")) {
		cout << "(SEND) Bytes sent: " << handleSend(buffer) << endl;
		
	}
	else if(strcmp(buffer,"LIST\n") == 0){
		cout << "(LIST) Bytes sent: " << handleList(buffer) << endl;
		
	}else if(strcmp(buffer,"READ\n") == 0){
		cout << "(READ) Bytes sent: " << handleRead(buffer) << endl;
		
	}else if(strcmp(buffer,"DEL\n") == 0){
		cout << "(DEL) Bytes sent: " << handleRead(buffer) << endl;
		
	}else if(strcmp(buffer,"QUIT\n") == 0){
		exit(1);
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