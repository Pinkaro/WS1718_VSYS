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
bool MessageHandlerServer::createFileAtPath(string filePath, string name, string message) {
	string fullPath = filePath + name;
	ofstream newFile (fullPath.c_str());
	
	cout << "Entered createFileAtPath() function\nFull path: " << fullPath << "\nMessage: " << message << endl;
	
	if(newFile.is_open()){
		newFile << message;
		newFile.close();
		messageResult = true;
		return true;
	}
	cerr << "Error: " << strerror(errno);
	return false;
}

string MessageHandlerServer::getTopicOfMail(string filePath) {
	ifstream file(filePath);
	string topic;
	
	if(file.is_open()) {
		for(int i = 1; i <= 3; i++) { //change the parameter to receive different information from mail (e.g.: 3 = topic)
			getline(file, topic);
		}
		return topic;
		
	}else{
		cerr << "Error: " << strerror(errno);
		exit(1);
	}
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
	
	if(ending.compare(".\n") != 0) {
		return false;
	}
	
	return true;
}

// to handle  LIST command
// first checks how many files are in the given user's directory and sets this number in the first line
// the following lines are followed by each respective topic
// this information is returned as a pointer to a character array
char* MessageHandlerServer::handleList(string username) {
	string userPath = path + username + "/";
	int filesOfUser = countFilesInDirectory(userPath);
	int counter = 1;
	string replyMessage = to_string(filesOfUser) + "\n";
	string filePath;
	
	cout << "Determined path: " << userPath << endl;
	cout << "I've found this many files: " << replyMessage << endl;
	
	if(filesOfUser == 0) {
		messageResult = false;
		strcpy(buffer, replyMessage.c_str());
		return buffer;
	}
	
	while(counter <= filesOfUser) {
		filePath = userPath + to_string(counter) + ".txt";
		replyMessage.append(getTopicOfMail(filePath));
		replyMessage.append("\n");
		counter++;
	}
	
	cout << "And the following topics: " << replyMessage << endl;
	
	messageResult = true;
	strcpy(buffer, replyMessage.c_str());
	
	cout << "Vomiting buffer: \n" << buffer << endl;
	
	return buffer;
	
}

// to handle a SEND command
// first splits the message into an array in 5 parts then lets checkSendPartsLength() check the validity of
// the message, if everything goes fine it saves it in a file on the server
char* MessageHandlerServer::handleSend(string messageWhole) {
	
	string msgCopy (messageWhole);
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
	
	// validate for correct input
	if(!checkSendPartsLength(messageSplitted)) {
		messageResult = false;
		strcpy(buffer, "SEND: PARTS-ERR\n");
		cout << buffer << endl;
		return buffer;
	}

	
	const string sender = messageSplitted[0]; // if message has passed validation, sender will always be at index 0
	string filePath = path+sender+"/";
	
	// check if there are no files yet at the path, if there are 0, create path
	int fileNumber;
	if( (fileNumber = countFilesInDirectory(filePath) ) == 0) {
		cout << "Files found: " << fileNumber << "creating directory" << endl;
		string createDirectoryCommand = "mkdir -p "+filePath;
		system(createDirectoryCommand.c_str());
	}
	cout << "Files found: " << fileNumber << "not creating directory" << endl;
	fileNumber += 1;
	string fileName = to_string(fileNumber) + ".txt";
	
	cout << "The path I'm sending over: " << filePath << endl;
	// save the email at specified path
	if(createFileAtPath(filePath, fileName, msgCopy)){
		messageResult = true;
		strcpy(buffer, "OK\n");
		return buffer;
	}else{
		messageResult = false;
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
		char* test = handleList(message.substr(0, message.size() - 1));
		cout << "Buffer before sending over to StreamSocketServer: " << test << endl;
		return test;
		
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