#include "./src/Header/MessageHandlerServer.h"

MessageHandlerServer::MessageHandlerServer(char* _buffer, char* _path) {
	this->buffer = string(_buffer);
	this->path = string (_path);
	this->messageResult = false;
	this->commands = "\nSEND, LIST, READ, DEL, QUIT\nEnter command: ";
}

MessageHandlerServer::~MessageHandlerServer() {
	
}

bool MessageHandlerServer::getResult() {
	return this->messageResult;
}

// checks if a file exists in a specific path
bool MessageHandlerServer::doesFileExist (string filepath) {
	if(ifstream(filepath)){
		return true;
	}else{
		return false;
	}
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
	
	if(newFile.is_open()){
		newFile << message;
		newFile.clear();
		messageResult = true;
		return true;
	}
	cerr << "Error: " << strerror(errno);
	return false;
}

// as files are saved as follow: 1.txt, 2.txt, 3.txt, ...
// we have to rename all following files if one gets deleted in between
void MessageHandlerServer::renameFilesAfterFilename(int fileNumber, string path) {
	string fileNameOld;
	string fileNameNew;
	
	while(doesFileExist(path + to_string(fileNumber+1) + ".txt")){ // if next file exists
		fileNameOld = path + to_string(fileNumber+1) + ".txt";
		fileNameNew = path + to_string(fileNumber) + ".txt";
		
		if(rename(fileNameOld.c_str(), fileNameNew.c_str()) == 0) {
			fileNumber++;
		}else {
			perror("FILE RENAME ERROR\n");
			break;
		}
	}
}

bool MessageHandlerServer::deleteFileAtPath(string filepath) {
	if( (remove(filepath.c_str())) != 0 ) {
		perror("DEL ERR\n");
		return false;
	}else {
		return true;
	}
}

string MessageHandlerServer::getFullMail(string filePath) {
	ifstream file(filePath);
	string line;
	string fullMail;
	
	if(file.is_open()){
		while( (getline(file, line)) ) {
			fullMail += line + "\n";
		}
	}
	return fullMail;
}

string MessageHandlerServer::getTopicOfMail(string filePath) {
	ifstream file(filePath);
	string topic;
	
	if(file.is_open()) {
		for(int i = 1; i <= 3; i++) { //change the parameter to receive different information from mail (e.g.: 3 = topic)
			getline(file, topic);
		}
		file.clear();
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

string MessageHandlerServer::handleDel(string wholeMessage) {
	string username = wholeMessage.substr(0, wholeMessage.find("\n"));
	wholeMessage.erase(0, wholeMessage.find("\n") + 1);
	string fileNumber = wholeMessage.substr(0, wholeMessage.find("\n"));
	string directoryPath = path + username + "/";
	string filePath = path + username + "/" + fileNumber + ".txt";
	string message;
	
	if( !(doesFileExist(filePath)) ) {
		messageResult = false;
		message = "DEL-ERR: File does not exist";
		message.append(commands);
		return message;
	}
	
	if( !(deleteFileAtPath(filePath)) ) {
		messageResult = false;
		message = "DEL-ERR: Could not delete file\n";
		message.append(commands);
		return message;
	}
	
	renameFilesAfterFilename(atoi(fileNumber.c_str()), directoryPath);
	
	message = "OK\n";
	return message;
}

string MessageHandlerServer::handleRead(string wholeMessage) {
	string username = wholeMessage.substr(0, wholeMessage.find("\n"));
	wholeMessage.erase(0, wholeMessage.find("\n") + 1);
	string fileNumber = wholeMessage.substr(0, wholeMessage.find("\n"));
	string filePath = path + username + "/" + fileNumber + ".txt";
	string message;
	
	if( !(doesFileExist(filePath)) ) {
		messageResult = false;
		message = "READ-ERR\n";
		message.append(commands);
		return message;
	}
	
	message = "OK\n";
	message += getFullMail(filePath);
	message.append(commands);
	messageResult = true;
	return message;
}

// to handle  LIST command
// first checks how many files are in the given user's directory and sets this number in the first line
// the following lines are followed by each respective topic
// this information is returned as a pointer to a character array
string MessageHandlerServer::handleList(string username) {
	string userPath = path + username + "/";
	int filesOfUser = countFilesInDirectory(userPath);
	int counter = 1;
	string replyMessage = to_string(filesOfUser) + "\n";
	string filePath;
	
	if(filesOfUser == 0) {
		messageResult = false;
		buffer = replyMessage;
		buffer.append(commands);
		return buffer;
	}
	
	while(counter <= filesOfUser) {
		filePath = userPath + to_string(counter) + ".txt";
		replyMessage.append(getTopicOfMail(filePath));
		replyMessage.append("\n");
		counter++;
	}
	
	
	messageResult = true;
	buffer = replyMessage;
	buffer.append(commands);
	
	return buffer;
}

// to handle a SEND command
// first splits the message into an array in 5 parts then lets checkSendPartsLength() check the validity of
// the message, if everything goes fine it saves it in a file on the server
string MessageHandlerServer::handleSend(string messageWhole) {
	
	string msgCopy (messageWhole);
	char seperator = '\n';
	int counter = 0;
	int currentPos = 0;
	string messageSplitted [5];
	int temp = 0;
	
	// fill the first three indexes of the string array messageSplitted
	while( counter != 3 ) {
		messageSplitted[counter] = messageWhole.substr(0, messageWhole.find(seperator));
		counter++;
		temp = currentPos;
		currentPos = messageWhole.find(seperator, temp) + 1;
		messageWhole.erase(0, messageWhole.find(seperator) + 1);
	}
	
	// fill the rest. I did it this way as I don't know how many newlines are in a message
	// there's a better solution but I'm too tired to implement it, so this will have to do.
	messageSplitted[3] = messageWhole.substr(0, messageWhole.find("\n.\n"));
	messageWhole.erase(0, messageWhole.find("\n.\n") + 1);
	messageSplitted[4] = messageWhole;
	// validate for correct input
	if(!checkSendPartsLength(messageSplitted)) {
		messageResult = false;
		buffer = "SEND-Err Parts";
		buffer.append(commands);
		return buffer;
	}
	const string recipient = messageSplitted[1]; // if message has passed validation, recipient will always be at index 1
	string filePath = path+recipient+"/";
	
	// check if there are no files yet at the path, if there are 0, create path
	//ḾUTEX.lock
	int fileNumber;
	if( (fileNumber = countFilesInDirectory(filePath) ) == 0) {
		string createDirectoryCommand = "mkdir -p "+filePath;
		system(createDirectoryCommand.c_str());
	}
	fileNumber += 1;
	string fileName = to_string(fileNumber) + ".txt";
	// save the email at specified path
	if(createFileAtPath(filePath, fileName, msgCopy)){
		messageResult = true;
		buffer = "OK";
		buffer.append(commands);
		//MUTEX.unlock
		return buffer;
	}else{
		messageResult = false;
		buffer = "ERR";
		buffer.append(commands);
		//MUTEX.unlock
		return buffer;
	}
}

// sends the received message the right way according to the command that has been sent with it
// returns the message to be sent to the client
char* MessageHandlerServer::HandleMessage() {
	string message(buffer);
	
	string command = message.substr(0, message.find("\n"));
	message.erase(0, message.find("\n") + 1); //delete command line, don't need it anymore
	char* backSender = new char[1024];
	
	if(strcmp(command.c_str(),"SEND") == 0){
		buffer = handleSend(message);
		strcpy(backSender, buffer.c_str());
		return backSender;
		
	}else if(strcmp(command.c_str(),"LIST") == 0){
		buffer = handleList(message.substr(0, message.find("\n")));
		strcpy(backSender, buffer.c_str());
		return backSender;
		
	}else if(strcmp(command.c_str(),"READ") == 0){
		buffer = handleRead(message);
		strcpy(backSender, buffer.c_str());
		return backSender;
		
	}else if(strcmp(command.c_str(),"DEL") == 0){
		buffer = handleDel(message);
		strcpy(backSender, buffer.c_str());
		return backSender;
		
	}else if(strcmp(command.c_str(),"QUIT") == 0){
		exit(1);
		
	}
	
	buffer = "NO COMMAND ERR\n";
	strcpy(backSender, buffer.c_str());
	return backSender;
}