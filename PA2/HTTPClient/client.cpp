#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#define PORT 8888
   
int main(int argc, char const *argv[]) 
{ 
    struct sockaddr_in address; 
    int sock = 0, valread; 
    char httprequest[1000];
    char httpmethod[10];
    char httpuri[100];
    char httpver[100];
    struct sockaddr_in serv_addr; 
    char *hello = "Hello from client"; 
    char buffer[1024] = {0}; 

    if (argc < 4)
	{
		printf("USAGE:  <METHOD> </URI/>  <VERSION>\n");
		exit(1);
	}
    sscanf(argv[1], " %s", httpmethod); 
    sscanf(argv[2], " %s", httpuri);
    sscanf(argv[3], " %s", httpver);
    strcat(httprequest,httpmethod);
    strcat(httprequest," ");
    strcat(httprequest,httpuri);
    strcat(httprequest," ");
    strcat(httprequest,httpver);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    send(sock , httprequest , strlen(httprequest) , 0 ); 
    printf("Http request message sent\n"); 
    printf("entered : %s\n",httprequest);
    valread = read( sock , buffer, 1024); 
    printf("%s\n",buffer ); 
    return 0; 
} 

