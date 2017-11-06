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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ldap.h>
#include <chrono>
#include <ctime>
#include <ratio>

#define LDAP_HOST "ldap.technikum-wien.at"
#define LDAP_PORT 389
#define SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE
#define FILTER "(uid=if17b*)"
#define BIND_USER NULL		/* anonymous bind with user and pw NULL */
#define BIND_PW NULL

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
	
//	string sender;
//	string messageWhole;
//	string recipient;
//	
//	cout << "\nSender: ";
//	getline(cin, sender);
//	cout << "Sender before appending: " << sender << endl;
//	sender.append("\n");
//	cout << "Sender after appending: " << sender << endl;
//	messageWhole.append(sender);
//	cout << "Appending sender to message: " << messageWhole << endl;
//	
//	cout << "Recipient: ";
//	getline(cin, recipient);
//	cout << "Recipient before appending: " << recipient << endl;
//	recipient.append("\n");
//	cout << "Recipient after appending: " << recipient << endl;
//	messageWhole.append(recipient);
//	cout << "Appending recipient to message: " << messageWhole << endl;

//   LDAP *ld;			/* LDAP resource handle */
//   LDAPMessage *result, *e;	/* LDAP result handle */
//   BerElement *ber;		/* array of attributes */
//   char *attribute;
//   char **vals;
//
//   int i,rc=0;
//
//   char *attribs[3];		/* attribute array for search */
//
//   attribs[0]=strdup("uid");		/* return uid and cn of entries */
//   attribs[1]=strdup("cn");
//   attribs[2]=NULL;		/* array must be NULL terminated */
//
//
//   /* setup LDAP connection */
//   if ((ld=ldap_init(LDAP_HOST, LDAP_PORT)) == NULL)
//   {
//      perror("ldap_init failed");
//      return EXIT_FAILURE;
//   }
//
//   printf("connected to LDAP server %s on port %d\n",LDAP_HOST,LDAP_PORT);
//
//   /* anonymous bind */
//   rc = ldap_simple_bind_s(ld,BIND_USER,BIND_PW);
//
//   if (rc != LDAP_SUCCESS)
//   {
//      fprintf(stderr,"LDAP error: %s\n",ldap_err2string(rc));
//      return EXIT_FAILURE;
//   }
//   else
//   {
//      printf("bind successful\n");
//   }
//
//   /* perform ldap search */
//   rc = ldap_search_s(ld, SEARCHBASE, SCOPE, FILTER, attribs, 0, &result);
//
//   if (rc != LDAP_SUCCESS)
//   {
//      fprintf(stderr,"LDAP search error: %s\n",ldap_err2string(rc));
//      return EXIT_FAILURE;
//   }
//
//   printf("Total results: %d\n", ldap_count_entries(ld, result));
//
//   for (e = ldap_first_entry(ld, result); e != NULL; e = ldap_next_entry(ld,e))
//   {
//      printf("DN: %s\n", ldap_get_dn(ld,e));
//      
//      /* Now print the attributes and values of each found entry */
//
//      for (attribute = ldap_first_attribute(ld,e,&ber); attribute!=NULL; attribute = ldap_next_attribute(ld,e,ber))
//      {
//         if ((vals=ldap_get_values(ld,e,attribute)) != NULL)
//         {
//            for (i=0;vals[i]!=NULL;i++)
//            {
//               printf("\t%s: %s\n",attribute,vals[i]);
//            }
//            /* free memory used to store the values of the attribute */
//            ldap_value_free(vals);
//         }
//         /* free memory used to store the attribute */
//         ldap_memfree(attribute);
//      }
//      /* free memory used to store the value structure */
//      if (ber != NULL) ber_free(ber,0);
//
//      printf("\n");
//   }
//  
//   /* free memory used for result */
//   ldap_msgfree(result);
//   free(attribs[0]);
//   free(attribs[1]);
//   printf("LDAP search suceeded\n");
//   
//   ldap_unbind(ld);
//   return EXIT_SUCCESS;

	chrono::milliseconds now = chrono::duration_cast< chrono::milliseconds >(chrono::system_clock::now().time_since_epoch());
	sleep(5);
	chrono::milliseconds then = chrono::duration_cast< chrono::milliseconds >(chrono::system_clock::now().time_since_epoch());
	
	auto ms = chrono::duration_cast<chrono::milliseconds> (then - now).count();
	
	double ms2 = ms/1000;
	
	cout << ms2 << " Seconds" << endl;
   
}
