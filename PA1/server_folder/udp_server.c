#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "server_handler.h"
/* You will have to modify the program below */

#define MAXBUFSIZE 1024

int main (int argc, char * argv[] )
{

	int ret_mem;
	int sockfiledes;                           //This will be our socket
	struct sockaddr_in client_addr, server_addr;     //"Internet socket address structure"
	unsigned int client_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	int opt = 1; 
	int exitflag = 0;
	char buffer[MAXBUFSIZE],copybuffer[MAXBUFSIZE],sendbuf[MAXBUFSIZE];             //a buffer to store our received message
	char *hello = "Hello from server";
	char *clienthello = "Hello from client";
	FILE* fp;
	if (argc != 2)
	{
		printf ("USAGE:  <port>\n");
		exit(1);
	}

	//Causes the system to create a generic socket of type UDP (datagram)
	if ( (sockfiledes = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        printf("socket creation failed\n");
        exit(1);
    }
	printf("socket created on server\n");

	/******************
	  Once we've created a socket, we must bind that socket to the 
	  local address and port we've supplied in the sockaddr_in struct
	 ******************/

	// Forcefully attaching socket to the port 8080 
    if (setsockopt(sockfiledes, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        printf("setsockopt failure\n"); 
        exit(EXIT_FAILURE); 
    } 
	
	/******************
	  This code populates the sockaddr_in struct with
	  the information about our socket
	 ******************/
	bzero(&client_addr,sizeof(client_addr));                    //zero the struct
	bzero(&server_addr,sizeof(server_addr)); 
	server_addr.sin_family = AF_INET;                   //address family
	server_addr.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
	server_addr.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine


	if (bind(sockfiledes, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("bind failed\n");
        exit(0);
	}
	printf("server bind success\n");

	bzero(buffer,sizeof(buffer));
	printf("server waiting for mesg from client\n");

	client_length = sizeof(client_addr);
	if(nbytes = recvfrom(sockfiledes, (char *)buffer, MAXBUFSIZE, MSG_WAITALL, ( struct sockaddr *) &client_addr,&client_length)< 0)
		{
			printf("receive from client unsuccessfull\n");
			exit(1);
		}
RESTART:	printf("The client says : %s\n", buffer);			
	nbytes = sendto(sockfiledes, (const char *)hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *) &client_addr,client_length);
	printf("server waiting for mesg from client\n");
LABEL:	while(1)
	{
		
		//waits for an incoming message
		bzero(buffer,sizeof(buffer));
		
		
		if((nbytes = recvfrom(sockfiledes, (char *)buffer, MAXBUFSIZE, MSG_WAITALL, ( struct sockaddr *) &client_addr,&client_length))< 0)
		{
			if(errno == 11)
			{
				
				sleep(1);
				goto LABEL;
			}
			
			else printf("receive from client unsuccessfull - main\n");
			
		}
		if (ret_mem =memcmp(clienthello, buffer, sizeof(clienthello)) == 0)
		{
			printf("Client has restarted\n");
			goto RESTART;
		}
		int retStatus = servercommandhandler(servercommandsearch(buffer),sockfiledes,client_addr);
		if(retStatus == -1 && errno != 11)	printf("Invalid command %s\n",buffer);
		else printf("The client command executed : %s\n", buffer);
		sleep(1);
		if(retStatus == 1)
			break;
	}
	
	close(sockfiledes);
	return 0;
}

