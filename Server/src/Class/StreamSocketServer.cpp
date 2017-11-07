#include "./src/Header/StreamSocketServer.h"
#include <thread>

#define MAXDATASIZE 1024
#define BACKLOG 5



streamSocketServer::streamSocketServer(char * _port, char * _path){
	port = atoi(_port);
	path = _path;
	commands = "\nLOGIN, SEND, LIST, READ, DEL, QUIT\nEnter command: ";
	
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
	//delete bannedIPs;
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
	return total; // return total number of bytes sent
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
void streamSocketServer::handleRecv (char * buffer, int clientfd) {
	string message(buffer);
	
	// make a copy of buffer to pass on to messagehandler as we do not want to
	// fiddle with original buffer
	char* buffercopy = new char[message.size() + 1];
	strcpy(buffercopy, message.c_str());
	buffercopy[message.size()] = '\0';
	
	string command = message.substr(0, message.find("\n"));
	//delete command line, don't need it anymore
	message.erase(0, message.find("\n") + 1);
	
	MessageHandlerServer messageHandler = MessageHandlerServer(buffercopy, path);
	memset(buffer, 0, strlen(buffer));
	buffer = messageHandler.HandleMessage();
	
	delete[] buffercopy;
	
	if(strcmp(command.c_str(),"SEND") == 0){
		if(messageHandler.getResult()){
			sendall(clientfd, buffer, 3);
		}else{
			sendall(clientfd, buffer, 4);
		}
		
	}else if(strcmp(command.c_str(),"LIST") == 0){
		if(messageHandler.getResult()){
			sendall(clientfd, buffer, MAXDATASIZE-1);
		}else{
			sendall(clientfd, buffer, 4);
		}
		
	}else if(strcmp(command.c_str(),"READ") == 0){
		if(messageHandler.getResult()){
			sendall(clientfd, buffer, MAXDATASIZE-1);
		}else{
			sendall(clientfd, buffer, 4);
		}
		
	}else if(strcmp(command.c_str(),"DEL") == 0){
		if(messageHandler.getResult()){
			sendall(clientfd, buffer, MAXDATASIZE-1);
		}else{
			sendall(clientfd, buffer, 4);
		}
		
	}else if(strcmp(command.c_str(),"QUIT") == 0){
		exit(1);
		
	}else{
		strcpy(buffer, "Send me a real command, dude. (STREAMSOCKET)");
		cout << "dunno it fam (buffer size: " << strlen(buffer) << "), bytes send: " << sendall(clientfd, buffer, strlen(buffer)) << endl;
	}
	
	delete[] buffer;	
}

// to check if an IP is already in our map where we store blocked IP addresses
bool streamSocketServer::isIpAlreadyBlocked (in_addr address) {
	if( !(bannedIPs.find(inet_ntoa(address)) == bannedIPs.end()) ) { // if IP is blocked, check time
		chrono::milliseconds now = chrono::duration_cast< chrono::milliseconds >(chrono::system_clock::now().time_since_epoch());
		chrono::milliseconds then = bannedIPs[inet_ntoa(address)];
		
		auto difference = chrono::duration_cast<chrono::milliseconds> (then - now).count();
		
		if(difference >= BLOCKTIME) { // if time difference has surpassed BLOCKTIME, unblock IP address
			bannedIPs.erase(inet_ntoa(address));
			return false;
		}else{ 
			return true;
		}
	}
	return false;
}

// adds a new ipaddress to the map of banned, only adds addresses that do not yet exist in our map
// chrono function shamelessly copied from stackoverflow
void streamSocketServer::blockIpAddress (in_addr address) {
	if(!isIpAlreadyBlocked(address)) {
		bannedIPs[inet_ntoa(address)] = chrono::duration_cast< chrono::milliseconds >(chrono::system_clock::now().time_since_epoch());
	}
}

// checks if given username and password are correct, if wrong three times the method calls another function to block 
// the ip address for a certain amount of time
bool streamSocketServer::checkLogin(char* buffer, int tryNumber, in_addr address) {
	if(tryNumber >= 3) {
		blockIpAddress(address);
		return false;
	}
	
	string wholeMessage(buffer);
	string tmp = wholeMessage.substr(0, wholeMessage.find("\n"));
	wholeMessage.erase(0, wholeMessage.find("\n") + 1);
	string password = wholeMessage.substr(0, wholeMessage.find("\n"));
	string username = "(uid=" + tmp + ")";
	
	// here check for LDAP connection, return true if user + password has been found, false otherwise
	LDAP *ld;			/* LDAP resource handle */
	LDAPMessage *result, *e;	/* LDAP result handle */


   int i,rc=0;

   char *attribs[3];		/* attribute array for search */

   attribs[0]=strdup("uid");		/* return uid and cn of entries */
   attribs[1]=strdup("cn");
   attribs[2]=NULL;		/* array must be NULL terminated */


   /* setup LDAP connection */
   if ((ld=ldap_init(LDAP_HOST, LDAP_PORT)) == NULL)
   {
      perror("ldap_init failed");
      return false;
   }

   printf("connected to LDAP server %s on port %d\n",LDAP_HOST,LDAP_PORT);

   /* anonymous bind */
   rc = ldap_simple_bind_s(ld,BIND_USER,BIND_PW);

   if (rc != LDAP_SUCCESS)
   {
      fprintf(stderr,"LDAP error: %s\n",ldap_err2string(rc));
      return false;
   }
   else
   {
      printf("bind successful\n");
   }

   /* perform ldap search */
   rc = ldap_search_s(ld, SEARCHBASE, SCOPE, username.c_str(), attribs, 0, &result);

   if (rc != LDAP_SUCCESS)
   {
      fprintf(stderr,"LDAP search error: %s\n",ldap_err2string(rc));
      return false;
   }
	//after getting an approval that the user exists we make another bind with the password this time.
	 rc = ldap_simple_bind_s(ld,username.c_str(),password.c_str());

   if (rc != LDAP_SUCCESS)
   {
      fprintf(stderr,"LDAP error: %s\n",ldap_err2string(rc));
      return false;
   }
   else
   {
      printf("bind successful\n");
   }
   
   // just some printing of data on the server side.
	printf("Total results: %d\n", ldap_count_entries(ld, result));

      printf("\n");
   
  
   /* free memory used for result */
   ldap_msgfree(result);
   free(attribs[0]);
   free(attribs[1]);
   printf("LDAP search suceeded\n");
   
   ldap_unbind(ld);
   return true;
	
}

//start the recv/send loop
void streamSocketServer::initCommunicationWithClient (struct clientinfo ci) {
	int bytesInBuffer;
	char buffer[MAXDATASIZE];
	
	//send welcome message and list of available commands
	string welcomeMsg = "Welcome to my mail server!\nPlease send me your username and password.";
	welcomeMsg.append(commands);
	
	strcpy(buffer, welcomeMsg.c_str());
	sendall(ci.clientfd, buffer, strlen(buffer));
	memset(buffer, 0, sizeof buffer);
	bool loginSuccess = false;
	
	while(1) {
		cout << "Listening ..." << endl;
		if( (bytesInBuffer = recvall (ci.clientfd, buffer)) > 0 ) {
			buffer[bytesInBuffer] = '\0';
		
		}else if (bytesInBuffer == 0) {
			printf("Client closed remote socket\n");
			break;
		}else {
			perror("recv() error");
			exit(1);
		}
		
		if(!isIpAlreadyBlocked(ci.clientaddress) && loginSuccess) { // if IP is not blocked and login succeeded, we will handle the message
			handleRecv(buffer, ci.clientfd);
		}else if(isIpAlreadyBlocked(ci.clientaddress)){
			message = "IP-Address (" + string(inet_ntoa(ci.clientaddress)) + ") is currently blocked.";
			message.append(commands);
			memset(buffer, 0, bytesInBuffer); // zero out the buffer
			strcpy(buffer, message.c_str());
			sendall(ci.clientfd, buffer, strlen(buffer));
		}
		
		int tryNumber; // number of tries user tried to log in
		//check if we already succeeded the login
		while(!loginSuccess && !isIpAlreadyBlocked(ci.clientaddress)) {
			if(!checkLogin(buffer, tryNumber, ci.clientaddress)) {
				perror("Login rejected.");
				
				message = "LOGIN ERR\n";
				message.append(commands);
				
				memset(buffer, 0, bytesInBuffer); // zero out the buffer
				strcpy(buffer, message.c_str());
				sendall(ci.clientfd, buffer, strlen(buffer));
				tryNumber++; //increment if login was rejected
			}else{
				message = "LOGIN OK\n";
				message.append(commands);
				
				memset(buffer, 0, bytesInBuffer); // zero out the buffer
				strcpy(buffer, message.c_str());
				sendall(ci.clientfd, buffer, strlen(buffer));
				loginSuccess = true;
				break;
			}
			
			if(tryNumber >= 3) {
				break;
			}
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
	
	while(true) {
		struct sockaddr_in client_address;
		int clientfd;
		
		if( (clientfd = accept(sockfd, (struct sockaddr *)&client_address, &clientLength))  == -1) {
			perror("accept error");
			exit(1);
		}
		
		printf("Client connected from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
		
		struct clientinfo ci;
		ci.clientfd = clientfd;
		ci.clientaddress = client_address.sin_addr;
		
		// thread it hard
		thread clientThread (&streamSocketServer::initCommunicationWithClient, this, ci);
		clientThread.detach();
	}
}