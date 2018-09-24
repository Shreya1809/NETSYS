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
#include <errno.h>
#include "commands.h"
#include "reliability_client.h"

#define MAXBUFSIZE 1024

/* You will have to modify the program below */
int main (int argc, char * argv[])
{
	int ret = 0, option = 0;
	int num_of_words = 0;
	char command[10][10];
	int nbytes;                             // number of bytes send by sendto()
	int sockfiledes;                               //this will be our socket
	char enteredoption[100];
	char dummyenter;
	char splitStrings[2][2];
	char previousoption;
	char buffer[MAXBUFSIZE];
    char *hello = "Hello from client";
	struct sockaddr_in server_addr;              //"Internet socket address structure"

	if (argc < 3)
	{
		printf("USAGE:  <server_ip> <server_port>\n");
		exit(1);
	}

	/******************
	  Here we populate a sockaddr_in struct with
	  information regarding where we'd like to send our packet 
	  i.e the Server.
	 ******************/
	bzero(&server_addr,sizeof(server_addr));               //zero the struct
	server_addr.sin_family = AF_INET;                 //address family
	server_addr.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
	server_addr.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address

	//Causes the system to create a generic socket of type UDP (datagram)
	// Creating socket file descriptor
    if ( (sockfiledes = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        printf("socket creation failed\n");
        exit(1);
    }
    printf("socket created on client \n");
	
	/******************
	  sendto() sends immediately.  
	  it will report an error if the message fails to leave the computer
	  however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
	 ******************/
	//char command[] = "apple";	
RECONNECT:
	if(nbytes = sendto(sockfiledes, (const char *)hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        printf("sendto unsuccessful \n");
        exit(1);
    }
    printf("Hello message sent from client.\n");

	// Blocks till bytes are received
	struct sockaddr_in from_addr;
	int addr_length = sizeof(struct sockaddr);
	bzero(buffer,sizeof(buffer));
	if(nbytes = recvfrom(sockfiledes, (char *)buffer, MAXBUFSIZE, MSG_WAITALL, (struct sockaddr *)&from_addr,&addr_length)< 0)
	{
		printf("receive from server unsuccessfull\n");
		exit(1);
	}

	printf("Server says %s\n", buffer);

	
    do
	{
		if(previousoption == '5')
		{
			printf("\nOnly available option now is 6 : Exit client\n");
		}
		printf("\n");
		printf("Choose any of the following operations to perfrom on the server\n");
		printf("1.Get file from server- Type - get filename \n");
		printf("2.Put or add file to the server - Type - put filename\n");
		printf("3.Delete a file from the server - Type - delete filename\n");
		printf("4.List all the files in the server  Type - ls server\n");
		printf("5.List all the files in the client  Type - ls client\n");
		printf("6.Exit the server gracefully - Type - exit server\n");
		printf("7.Exit the client gracefully - Type - exit client\n");
		printf("8.Reconnect to server - Type - reconnect server\n");
		
		printf("Enter your choice of option : \n");
		bzero(enteredoption,sizeof(enteredoption));
		bzero(command,sizeof(command));
		fgets(enteredoption, sizeof(enteredoption), stdin);
	
		// referenced from -https://www.w3resource.com/c-programming-exercises/string/c-string-exercise-31.php
		int  i= 0,j=0,ctr=0;
		for(i=0;i<=(strlen(enteredoption));i++)
		{
			
			if(enteredoption[i]==' '||enteredoption[i]=='\0'||enteredoption[i]== '\n')
			{
				command[ctr][j]='\0';
				ctr++;  
				j=0;    
			}
			else
			{
				command[ctr][j]=enteredoption[i];
				j++;
			}
		}
	
	if((strcmp("get",command[0])) == 0)
	{
		option = 0;
	}
	else if((strcmp("put",command[0])) == 0)
	{
		option = 1;
	}
	else if((strcmp("delete",command[0])) == 0)
	{
		option = 2;
	}
	
	else if((strcmp("ls",command[0])) == 0)
	{
		if((strcmp("server",command[1])) == 0)
			{
				option = 3;
			}
		if((strcmp("client",command[1])) == 0)
			{
				option = 5;
			}
	}
	else if((strcmp("exit",command[0])) == 0)
	{
		if((strcmp("server",command[1])) == 0)
			{
				option = 4;
			}
		if((strcmp("client",command[1])) == 0)
			{
				printf("signal received to exit client\n");
				printf("exiting ....\n.");
				exit(0);
			}

	}
	else if((strcmp("reconnect",command[0])) == 0)
	{
		if((strcmp("server",command[1])) == 0)
		{
			goto RECONNECT;
		}
		else
		{
			printf("invalid command\n");
			continue;
		}
	}
	else
	{
		printf("Invalid command\n");
		continue;
	}
	
	commandhandler(option, command[1],sockfiledes,server_addr);
		
	}while(1);
	
	close(sockfiledes);
    return 0;

}

