#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>
#include <regex>
#include <iostream>
#include <string.h>
#include <ctype.h>

using namespace std;

int main(int argc, char **argv)
{
//	string messageWhole = "if17b127\nMax Glaida\nNicest of topics\nDear Max\nI've been captured by cannibals\nPlease help!\nYours,\nAlex\n.\n";
//	char seperator = '\n';
//	int counter = 0;
//	int currentPos = 0;
//	string messageSplitted [5];
//	int temp = 0;
//	cout << messageWhole << "\n" << endl;
//	cout << "String size: " << messageWhole.size() << "\nSplit up to:" <<endl;
//	while( counter != 3 ) {
//		messageSplitted[counter] = messageWhole.substr(0, messageWhole.find(seperator));
//		
//		counter++;
//		temp = currentPos;
//		currentPos = messageWhole.find(seperator, temp) + 1;
//		messageWhole.erase(0, messageWhole.find(seperator) + 1);
//	}
//	
//	messageSplitted[3] = messageWhole.substr(0, messageWhole.find(".\n"));
//	messageWhole.erase(0, messageWhole.find(".\n"));
//	messageSplitted[4] = messageWhole;
	
	string sender;
	string messageWhole;
	string recipient;
	
	cout << "\nSender: ";
	getline(cin, sender);
	cout << "Sender before appending: " << sender << endl;
	sender.append("\n");
	cout << "Sender after appending: " << sender << endl;
	messageWhole.append(sender);
	cout << "Appending sender to message: " << messageWhole << endl;
	
	cout << "Recipient: ";
	getline(cin, recipient);
	cout << "Recipient before appending: " << recipient << endl;
	recipient.append("\n");
	cout << "Recipient after appending: " << recipient << endl;
	messageWhole.append(recipient);
	cout << "Appending recipient to message: " << messageWhole << endl;
}
