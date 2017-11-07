#include <stdio.h>
#include "./src/Header/StreamSocketServer.h"

// to check if arguments handed over to main are only two and if they match the required criteria (usable port, pattern of path)
void checkGivenArguments (int argCounter, char *argValues[]) {
	if(argCounter!= 3)     {
		perror("invalid number of arguments\nExpected: <file> <path> <port>");
		exit(1);
	}

	string path(argValues[1]);
	//to check if path is absolute and goes into /home/, and end with /
	if(!regex_match(path, regex("(/home)/([^,:])*/"))) {
		perror("invalid path, has to start with \"/home/\" and end with \"/\"\nExpected: <path> <port>");
		exit(1);
	}

	int port = atoi(argValues[2]);
	//to check if port number is within reasonable scope
	if(port < 1023 || port > 65535) {
		perror("port is out of range (must be greater than 1023 and lesser than 65535)\nExpected: <path> <port>");
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	printf("Starting Server ... \n");
	checkGivenArguments(argc, argv);
	streamSocketServer server = streamSocketServer(argv[2], argv[1]);
	server.startConnection();
}
