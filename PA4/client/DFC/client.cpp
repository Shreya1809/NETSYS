
#include "includes.h"
#include "library.hpp"
#include <errno.h>


extern config_t config;
int main(int argc, char const *argv[]) 
{ 
    string enteredstring, configmesg;
    char buff[1024] = {0};
    int server_fd, new_socket, valread;
    struct sockaddr_in server_addr[4]; 
    int addrlen = sizeof(server_addr);  
    int port,opt = 1;
    int sock[4] = {0};
    if(argc < 2)
    {
        printf("USAGE: ./client [dfc config file]\n");
        return 1;
    } 
    parse_config_file(argv[1]);
    configmesg = config.username + "," + config.password;
    for(int i = 0 ; i<4;i++)
    {
    bzero(&server_addr[i],sizeof(server_addr[i]));
    server_addr[i].sin_family = AF_INET;
    server_addr[i].sin_port = htons(atoi(config.port[i]));              
    server_addr[i].sin_addr.s_addr =  inet_addr(config.server[i]);
    }
    while(1)
    {
        printf("\nEnter any of the following commands: \n1. GET [filename] \n2. PUT [filename] \n3. LIST \n4. MKDIR [subfolder]\n");
        getline (cin, enteredstring);
        vector<string> entered_option = splitStrings(enteredstring, ' ');
        for (int j = 0;j<4;j++)
        {
            cout << "j is " << j << endl;
            if ((sock[j] = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
            { 
                printf("\n Socket creation error \n"); 
                return -1; 
            }
            cout << "Created socket " << sock[j]<< endl;
            // Convert IPv4 and IPv6 addresses from text to binary form 
            if((inet_pton(AF_INET, "127.0.0.1", &server_addr[j].sin_addr))<=0)  
            { 
                printf("\nInvalid address/ Address not supported \n"); 
                return -1; 
            }
            //cout << "converted" << endl;
            if ((connect(sock[j], (struct sockaddr *)&server_addr[j], sizeof(server_addr[j]))) < 0)
            { 
                printf("Error: %s and code %d\n", strerror( errno ), errno);
                printf("\nConnection Failed socket %d\n",sock[j]);
                printf("Server addr: %u\n",server_addr[j]);
                return -1; 
            } 
            cout << "Connected socket " << sock[j] << endl;
            //printf("Server addr: %u\n",server_addr[j]);
            send(sock[j] , configmesg.c_str() , configmesg.length() , 0 ); 
            cout<<"Configuration Information Sent to server" << endl; 
            valread = read( sock[j] , buff, 1024); 
            //printf("%s\n",buff); 
            string configbuff = string(buff);
            if(configbuff.compare("Authentication_Successful")== 0)
            {
                cout << "Authentication Successful" << endl;
            }
            else
            {
                cout << "Error: Authentication failed" << endl;
                exit(0);

            }
            //return 0; 
        }
        if((entered_option[0].compare("GET")==0) || (entered_option[0].compare("get")==0))
        {
            printf("Command Entered is GET and filename is %s\n",entered_option[1].c_str());
        }

    }
    return 0;
}
    