#include "includes.h"
#include "library.hpp"

//REFERENCE - https://www.geeksforgeeks.org/socket-programming-cc/
//https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/

int main(int argc, char const *argv[]) 
{ 
    int new_socket,master_socket, valread,max_clients = 50, activity,client_socket[50],i,sd; 
    struct sockaddr_in client_address; 
    int opt = 1; 
    int max_sd; 
    int addrlen = sizeof(client_address);
    //char *mesg = "HTTP Request received on server side"; 
    char dl = ' '; //delimiter for parsing string
    char http_req[1024] = {0};   

    //set of socket descriptors  
    fd_set readfds;   

    //initialise all client_socket[] to 0 so not checked  
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
    // Creating socket file descriptor 
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    client_address.sin_family = AF_INET; 
    client_address.sin_addr.s_addr = INADDR_ANY; 
    client_address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(master_socket, (struct sockaddr *)&client_address,  sizeof(client_address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(master_socket, 10) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    while ( 1 ) 
	{
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket; 

         //add child sockets to set  
        for ( i = 0 ; i < max_clients ; i++)   
        {   
            //socket descriptor  
            sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }   
        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }   

        if (FD_ISSET(master_socket, &readfds))   
        {   
           
            if ((new_socket = accept(master_socket, (struct sockaddr *)&client_address,  (socklen_t*)&addrlen))<0) 
            { 
                perror("accept"); 
                exit(EXIT_FAILURE);    
            } 
            //inform user of socket number - used in send and receive commands  
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(client_address.sin_addr) , ntohs (client_address.sin_port));   

            //add new socket to array of sockets  
            for (i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                    printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            } 
        }
        //else its some IO operation on some other socket 
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];   
                 
            if (FD_ISSET( sd , &readfds))   
            {    
                if((valread = read( new_socket ,http_req, sizeof(http_req))) == 0)
                {
                    //printf("retry\n");
                    //Somebody disconnected , get his details and print  
                    getpeername(sd , (struct sockaddr*)&client_address ,(socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" ,  inet_ntoa(client_address.sin_addr) , ntohs(client_address.sin_port));   
                        
                //Close the socket and mark as 0 in list for reuse  
                close( sd );   
                client_socket[i] = 0;
                }
            
                printf("%s\n",http_req );
                RequestServiceHandler(new_socket, string(http_req));
                //close(new_socket);
                
            }
        }
    }
    return 0; 
} 
