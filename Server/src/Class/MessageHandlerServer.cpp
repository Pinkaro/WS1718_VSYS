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
bool MessageHandlerServer::doesFileExist (string filepath) {
	ifstream checkFile (filepath);
	return checkFile.good();
}

// count *.txt files in a directory with the nameconvention of 1.txt, 2.txt, 3.txt
int MessageHandlerServer::countFilesInDirectory (string directorypath) {
	int counter = 1;
	string filepath = directorypath + to_string(counter) + ".txt";
	
	while(doesFileExist(filepath.c_str())) {
		counter++;
		filepath.clear();
		filepath = directorypath + to_string(counter) + ".txt";
		
	}
	return counter-1;
}

// creates a new file at the passed path and insert message
bool MessageHandlerServer::createFileAtPath(string path, string name, string message) {
	string fullPath = path + name;
	ofstream newFile (fullPath.c_str());
	
	if(newFile.is_open()){
		newFile << message;
		newFile.close();
		messageResult = true;
		return true;
	}
	return false;
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
	
	char seperator = '\n';
	int counter = 0;
	int currentPos = 0;
	string messageSplitted [5];
	int temp = 0;
	cout << messageWhole << "\n" << endl;
	cout << "String size: " << messageWhole.size() << "\nSplit up to:" <<endl;
	
	while( counter != 3 ) {
		messageSplitted[counter] = messageWhole.substr(0, messageWhole.find(seperator));
		counter++;
		temp = currentPos;
		currentPos = messageWhole.find(seperator, temp) + 1;
		messageWhole.erase(0, messageWhole.find(seperator) + 1);
	}
	
	messageSplitted[3] = messageWhole.substr(0, messageWhole.find(".\n"));
	messageWhole.erase(0, messageWhole.find(".\n"));
	messageSplitted[4] = messageWhole;
	
	cout << "I split the message up and initiating control of parts!" << endl;
	// validate for correct input
	if(!checkSendPartsLength(messageSplitted)) {
		messageResult = false;
		strcpy(buffer, "SEND: PARTS-ERR\n");
		return buffer;
	}
	cout << "Parts passed validation, preparing to count files" << endl;
	
	const string sender = messageSplitted[0]; // if message has passed validation, sender will always be at index 0
	string path = path+sender+"/";
	
	// check if there are no files yet at the path, if there are 0, create path
	int fileNumber;
	if( (fileNumber = countFilesInDirectory(path) ) == 0) {
		string createDirectoryCommand = "mkdir -p "+path;
		system(createDirectoryCommand.c_str());
	}
	fileNumber += 1;
	string fileName = to_string(fileNumber) + ".txt";
	
	// save the email at specified path
	if(createFileAtPath(path, fileName, messageWhole)){
		strcpy(buffer, "OK\n");
		return buffer;
	}else{
		strcpy(buffer, "ERR\n");
		return buffer;
	}
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
	strcpy(buffer, "ERR\n");
	return buffer;
}