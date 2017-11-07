#include <string>
#include <iostream>
#include <string.h>
#include <ctype.h>
#include <fstream>
#include <regex>

#ifndef MESSAGEHANDLERSERVER_H_
#define MESSAGEHANDLERSERVER_H_

using namespace std;

class MessageHandlerServer {

public:
	MessageHandlerServer(char* _buffer, char* _path);
	virtual ~MessageHandlerServer();
	
	char* HandleMessage();
	bool getResult();
	
private:
	string buffer;
	bool messageResult;
	string path;
	
	string handleSend(string wholeMessage);
	string handleList(string username);
	string handleRead(string wholeMessage);
	string handleDel(string wholeMessage);
	bool checkSendPartsLength (string * messageSplitted);
	bool doesFileExist (string filepath);
	bool createFileAtPath(string path, string name, string message);
	bool deleteFileAtPath(string filepath);
	int countFilesInDirectory (string directorypath);
	string getTopicOfMail(string filePath);
	string getFullMail(string filePath);
	
};

#endif /* MESSAGEHANDLERSERVER_H_ */