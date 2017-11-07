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
	
	// unneeded if/else query, is already being handled in MessageHandlerServerl.HandleMessage()
	// update if there's still time
	if(strcmp(command.c_str(),"SEND") == 0){
		if(messageHandler.getResult()){
			sendall(clientfd, buffer, MAXDATASIZE-1);
		}else{
			sendall(clientfd, buffer, MAXDATASIZE-1);
		}
		
	}else if(strcmp(command.c_str(),"LIST") == 0){
		if(messageHandler.getResult()){
			sendall(clientfd, buffer, MAXDATASIZE-1);
		}else{
			sendall(clientfd, buffer, MAXDATASIZE-1);
		}
		
	}else if(strcmp(command.c_str(),"READ") == 0){
		if(messageHandler.getResult()){
			sendall(clientfd, buffer, MAXDATASIZE-1);
		}else{
			sendall(clientfd, buffer, MAXDATASIZE-1);
		}
		
	}else if(strcmp(command.c_str(),"DEL") == 0){
		if(messageHandler.getResult()){
			sendall(clientfd, buffer, MAXDATASIZE-1);
		}else{
			sendall(clientfd, buffer, MAXDATASIZE-1);
		}
		
	}else if(strcmp(command.c_str(),"QUIT") == 0){
		exit(1);
		
	}else{
		string confusedMessage = "Send me a real command, dude.";
		confusedMessage.append(commands);
		strcpy(buffer, confusedMessage.c_str());
		sendall(clientfd, buffer, strlen(buffer));
	}
	
	delete[] buffer;
}

// to check if an IP is already in our map where we store blocked IP addresses
bool streamSocketServer::isIpAlreadyBlocked (clientinfo& ci) {
	cout << "CHECKING IF CLIENT IS STILL BLOCKED, CURRENT TRIES: " << ci.loginTries << endl;
	if( !(bannedIPs.find(inet_ntoa(ci.clientaddress)) == bannedIPs.end()) ) { // if IP is blocked, check time
		cout << "CLIENT IS BLOCKED, CHECK IF ENOUGH TIME HAS PASSED" << endl;
		chrono::milliseconds now = chrono::duration_cast< chrono::milliseconds >(chrono::system_clock::now().time_since_epoch());
		chrono::milliseconds then = bannedIPs[inet_ntoa(ci.clientaddress)];
		
		auto now2 = now.count();
		auto then2 = then.count();
		
		auto difference = chrono::duration_cast<chrono::milliseconds> (now - then).count();
		
		cout << "NOW: " << now2 << endl;
		cout << "THEN: " << then2 << endl;
		cout << "DIFFERENCE: " << difference << endl;
		cout << "BLOCKTIME: " << BLOCKTIME << endl;
		
		if(difference >= BLOCKTIME) { // if time difference has surpassed BLOCKTIME, unblock IP address
			cout << "ENOUGH TIME HAS PASSED ... UNBANANING" << endl;
			bannedIPs.erase(inet_ntoa(ci.clientaddress));
			ci.loginTries = 0;
			return false;
		}else{ 
			return true;
		}
	}
	return false;
}

// adds a new ipaddress to the map of banned, only adds addresses that do not yet exist in our map
// chrono function shamelessly copied from stackoverflow
void streamSocketServer::blockIpAddress (clientinfo& ci) {
	if(!isIpAlreadyBlocked(ci)) {
		bannedIPs[inet_ntoa(ci.clientaddress)] = chrono::duration_cast< chrono::milliseconds >(chrono::system_clock::now().time_since_epoch());
	}
}

// checks if given username and password are correct, if wrong three times the method calls another function to block 
// the ip address for a certain amount of time
bool streamSocketServer::checkLogin(char* buffer, clientinfo& ci) {
	if(ci.loginTries >= 3) {
		cout << "CLIENT SURPASSED THREE TRIES, BLOCKING THAT FUCKER" << endl;
		blockIpAddress(ci);
		return false;
	}
	
	string wholeMessage(buffer);
	wholeMessage.erase(0, wholeMessage.find("\n") + 1); // erase first line as it is only the command
	string tmp = wholeMessage.substr(0, wholeMessage.find("\n"));
	wholeMessage.erase(0, wholeMessage.find("\n") + 1);
	string username = "uid=" + tmp;
	string passwordClean = wholeMessage.substr(0, wholeMessage.find("\n"));
	
	// here check for LDAP connection, return true if user + password has been found, false otherwise
	LDAP *ld;			/* LDAP resource handle */
	LDAPMessage *result, *e;	/* LDAP result handle */


   int rc=0;

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
   
   if( (e = ldap_first_entry(ld, result)) != NULL) {
	   string dn = ldap_get_dn(ld,e);
		cout << dn << endl;
   
  
		// just some printing of data on the server side.
		printf("Total results: %d\n", ldap_count_entries(ld, result));
		//after getting an approval that the user exists we make another bind with the password this time.
		cout << username << endl;
	 
		rc = ldap_simple_bind_s(ld,dn.c_str(),passwordClean.c_str());

		if (rc != LDAP_SUCCESS)
		{
			fprintf(stderr,"LDAP error: %s\n",ldap_err2string(rc));
			return false;
		}
		else
		{
			printf("bind successful\n");
		}
		printf("\n");
		
		/* free memory used for result */
		ldap_msgfree(result);
		free(attribs[0]);
		free(attribs[1]);
		printf("LDAP search succeeded\n");
		ldap_unbind(ld);
		
		return true;
	}else {
		/* free memory used for result */
		ldap_msgfree(result);
		free(attribs[0]);
		free(attribs[1]);
		printf("LDAP search succeeded\n");
		ldap_unbind(ld);
		
		return false;
	}
}

//start the recv/send loop
void streamSocketServer::initCommunicationWithClient (clientinfo ci) {
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
		
		if(isIpAlreadyBlocked(ci)){
			cout << "CLIENT IS CURRENTLY BLOCKED" << endl;
			message = "IP-Address (" + string(inet_ntoa(ci.clientaddress)) + ") is currently blocked.";
			message.append(commands);
			memset(buffer, 0, bytesInBuffer); // zero out the buffer
			strcpy(buffer, message.c_str());
			sendall(ci.clientfd, buffer, strlen(buffer));
			continue;
		}
		
		if(loginSuccess) { // login succeeded, we will handle the message
			cout << "CLIENT IS NOT BLOCKED AND LOGGED IN, HE CAN PROCEED" << endl;
			handleRecv(buffer, ci.clientfd);
		}else {
			// at this point we can only handle the Login command, send Error if not Login request
			string messageWhole(buffer);
			string command = messageWhole.substr(0, messageWhole.find("\n"));
			if(strcmp(command.c_str(), "LOGIN") != 0){
				string replyMessage = "Need successful Login request to process other commands. Please retry.";
				replyMessage.append(commands);
				strcpy(buffer, replyMessage.c_str());
				sendall(ci.clientfd, buffer, strlen(buffer));
				continue;
			}
			
			cout << "CLIENT IS NOT LOGGED IN AND NOT BLOCKED" << endl;
			if(!checkLogin(buffer, ci)) {
				cout << "CLIENT FAILED LOGIN, TRY: " << ci.loginTries << endl;
				
				perror("Login rejected.");
				
				message = "LOGIN ERR\n";
				message.append(commands);
				
				memset(buffer, 0, bytesInBuffer); // zero out the buffer
				strcpy(buffer, message.c_str());
				sendall(ci.clientfd, buffer, strlen(buffer));
				ci.loginTries++; //increment if login was rejected
			}else{
				cout << "CLIENT LOGGED IN SUCCESSFULLY" << endl;
				string temp_commands = "\nSEND, LIST, READ, DEL, QUIT\nEnter command: "; // temp because we only want to send the commands like this once
				message = "LOGIN OK\n";
				message.append(temp_commands);
				
				memset(buffer, 0, bytesInBuffer); // zero out the buffer
				strcpy(buffer, message.c_str());
				sendall(ci.clientfd, buffer, strlen(buffer));
				loginSuccess = true;
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
		ci.loginTries = 0;
		
		// thread it hard
		thread clientThread (&streamSocketServer::initCommunicationWithClient, this, ci);
		clientThread.detach();
	}
}
