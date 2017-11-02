#include "./src/Header/StreamSocketClient.h"

// to check if passed arguments are only two, if first argument is in fact an IP-Address and if port is within usable scope
void checkGivenArguments (int argCounter, char *argValues[]) {
	if(argCounter != 3)
	{
		perror("invalid number of arguments\nExpected: <file> <ip-address> <port>");
		exit(1);
	}

	string path(argValues[1]);
	//to check if input was in fact an ip address
	if(!regex_match(path, regex("(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])")))
	{
		perror("invalid path, has to start with \"/home/\"\nExpected: <file> <ip-address> <port>");
		exit(1);
	}

	int port = atoi(argValues[2]);
	//to check if port number is within reasonable scope
	if(port <= 1023 || port > 65535)
	{
		perror("port is out of range (must be greater than 1023 and lesser than 65535)\nExpected: <file> <ip-address> <port>");
		exit(1);
	}
}

int main (int argc, char **argv)
{
	checkGivenArguments(argc, argv);
	
	streamSocketClient client = streamSocketClient(argv[1], argv[2]);
	client.startConnection();

}