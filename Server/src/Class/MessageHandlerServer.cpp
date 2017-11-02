#include "./src/Header/MessageHandlerServer.h"

MessageHandlerServer::MessageHandlerServer(char* _buffer, char* _path) {
	this->buffer = _buffer;
	this->path = string (_path);
	this->messageResult = false;
}

MessageHandlerServer::~MessageHandlerServer() {
	
}

bool MessageHandlerServer::getResult() {
	return this->messageResult;
}

// checks if a file exists in a specific path
bool doesFileExist (string filepath) {
	ifstream checkFile (filepath);
	return checkFile.good();
}

// counts *.txt files in a directory with the nameconvention of 1.txt, 2.txt, 3.txt
int countFilesInDirectory (string directorypath) {
	int counter = 1;
	string directorypath (directorypath);
	string filepath = directorypath + to_string(counter) + ".txt";
	
	while(doesFileExist(filepath.c_str())) {
		counter++;
		filepath.clear();
		filepath = directorypath + to_string(counter) + ".txt";
		
	}
	return counter-1;
}

// checks every part of the whole message, returns true if every test has passed.
bool MessageHandlerServer::checkSendPartsLength (string * messageSplitted) {
	string sender = messageSplitted[0];
	string recipient = messageSplitted[1];
	string topic = messageSplitted[2];
	// we don't care about the message as there are no restrictions placed on it
	string ending = messageSplitted[4];
	
	if(sender.length() > 8) {
		return false;
	}
	
	if(recipient.length() > 8) {
		return false;
	}
	
	if(topic.length() > 80) {
		return false;
	}
	
	if(ending.compare(".") != 0) {
		return false;
	}
	
	return true;
}

// to handle a SEND command
// first splits the message into an array in 5 parts then lets checkSendPartsLength() check the validity of
// the message, if everything goes fine it saves it in a file on the server
char* MessageHandlerServer::handleSend(string messageWhole) {
	string messageSplitted [5];
	int counter = 0;
	regex seperator("(\n\n|\n)");
	std::regex_token_iterator<string::iterator> tokens(messageWhole.begin(), messageWhole.end(), seperator, -1);
	std::regex_token_iterator<string::iterator> rend;
	
	while(tokens != rend) {
		messageSplitted[counter] = *tokens++;
		counter++;
	}
	
	// validate for correct input
	if(!checkSendPartsLength(messageSplitted)) {
		messageResult = false;
		strcpy(buffer, "ERR\n");
		return buffer;
	}
	
	const string sender = messageSplitted[0]; // if message has passed validation, sender will always be at index 0
	int counter = 1;
	string path = path+sender+"/";
	string createDirectoryCommand = "mkdir -p "+path;
	system(createDirectoryCommand.c_str());
	
	while(counter != 5){
		
		string filePath (path + to_string(counter) + ".txt");
		ofstream myfile (filePath.c_str());
		counter++;
			myfile << "This is a line.\n";
			myfile << "This is another line.\n";
			myfile.close();
	}
	
	
	strcpy(buffer, "OK\n");
	return buffer;
}

// sends the received message the right way according to the command that has been sent with it
// returns the message to be sent to the client
char* MessageHandlerServer::HandleMessage() {
	string message(buffer);
	string command = message.substr(0, message.find("\n"));
	//delete command line, don't need it anymore
	message.erase(0, message.find("\n") + 1);
	
	if(strcmp(command.c_str(),"SEND") == 0){
		return handleSend(message);
		
	}else if(strcmp(command.c_str(),"LIST") == 0){
		
		
	}else if(strcmp(command.c_str(),"READ") == 0){
		
		
	}else if(strcmp(command.c_str(),"DEL") == 0){
		
		
	}else if(strcmp(command.c_str(),"QUIT") == 0){
		exit(1);
		
	}else{
		
	}
	memset(buffer, 0, strlen(buffer));
}