// Server side C/C++ program to demonstrate Socket programming 
#include "includes.h"
#include<pthread.h>
#include "library.hpp"
/*#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> */

#define BACKLOG     10  /* Passed to listen() */
int cachetimeout = 0;
 pthread_mutex_t mutexlock;


//char c = 'g'; 
//char c = 'g'; 
void *handle(void *pnewsock)
{
    pthread_mutex_lock(&mutexlock); 
    if(pnewsock == NULL){
        perror("Invalid socket thread param"); 
        exit(EXIT_FAILURE); 
    }
    int clientsock = *(int*)pnewsock;
    char buffer[1024] = {0};    
    http_request_t Request;
    http_response_t Response;
    url_parse_t parse;
    http_server_request_t ServerRequest;
    int retvalue;
    /* send(), recv(), close() */
    //cout<< "entered thread function" << endl;
    printf("clientsock: %d\n pthread id: %lu\n", clientsock, pthread_self());

    memset(buffer, 0, sizeof(buffer));
    int valread = read(clientsock , buffer, 1024); 
    
    printf("%s\n",buffer);
    sscanf(buffer, "%s %s %s", Request.method, Request.URL, Request.version); 
    //send(clientsock , hello , strlen(hello) , 0 ); 
    printf("Method is %s , URL is %s and version is %s\n",Request.method, Request.URL, Request.version);
    retvalue = error_handler(clientsock,Request,Response);
    if (retvalue == 0)
    {
        parse_request(&Request,&parse);
        reconstruct_request(Request, ServerRequest, parse, Response, clientsock,cachetimeout);
    }
    close(clientsock);
    pthread_mutex_unlock(&mutexlock);
}

int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket;
    struct sockaddr_in server_addr; 
    int opt = 1; 
    int addrlen = sizeof(server_addr);  
    //char dl = ' '; //delimiter for parsing string
    char http_req[1024] = {0};  
    pthread_t thread;
    int port;
    if(argc < 3)
    {
        printf("USAGE: ./proxy [PORT NUMBER] [CACHE TIMEOUT]\n");
        return 1;
    } 
    port = atoi(argv[1]);
    cachetimeout = atoi(argv[2]); 
    printf("------------------Blacklist----------------------\n");
    populateForbiddenMap("blacklist.txt"); //add blacklist to the list
    printf("---------------End of Blacklist-------------------\n");
    // Creating socket file descriptor 
    if (pthread_mutex_init(&mutexlock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    printf("socket file descriptor created\n") ;
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons( port ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&server_addr,sizeof(server_addr))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    printf("Binding...\n") ;
    if (listen(server_fd, BACKLOG) == -1) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    printf("Listening...\n") ;
    while(1)
    {
        //printf("In the while 1\n") ;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }
        printf("Accepting...\n") ;
   
        printf("Got a connection from %s on port %d\n", inet_ntoa(server_addr.sin_addr), htons(server_addr.sin_port));
        /* Make a safe copy of newsock */
        int *safesock = (int*)malloc(sizeof(int));
        if (safesock) {
            *safesock = new_socket;
            if (pthread_create(&thread, NULL, handle, safesock) != 0) {
                fprintf(stderr, "Failed to create thread\n");
            }
        }
        else {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        
    }

    cout<<"PROXY EXITING"<<endl;
 
    close(server_fd);
   
    return 0; 
} 
