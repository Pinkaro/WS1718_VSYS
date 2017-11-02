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
	char*  buffer;
	bool messageResult;
	string path;
	
	char* handleSend(string wholeMessage);
	char* handleList();
	char* handleRead();
	char* handleDel();
	bool checkSendPartsLength (string * messageSplitted);
	bool doesFileExist (string filepath);
	bool createFileAtPath(string path, string name, string message);
	int countFilesInDirectory (string directorypath);
	
};

#endif /* MESSAGEHANDLERSERVER_H_ */