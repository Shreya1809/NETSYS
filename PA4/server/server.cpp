#include "includes.h"
#include "commandhandler.hpp"
// reference : https://www.geeksforgeeks.org/socket-programming-cc/

int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address,client_addr; 
    int opt = 1; 
    int addrlen = sizeof(address);
    int client_len = sizeof(client_addr); 
    char buffer[1024] = {0}; 
    char mesg[50]= "Authentication_Successful";
    char fail[10] = "fail";
    char path[50] = {0};
    //char *hello = "Hello from server"; 

    if(argc < 3)
    {
        printf("USAGE: ./server [DFS folder] [PORT NUM]\n");
        return 1;
    } 
    /*if((argv[1] != "DFS1") || (argv[1] != "DFS2") || (argv[1] != "DFS3") || (argv[1] != "DFS4"))
    {
        cout << "Invalid DFS" << endl;
        exit(0);
    }*/  
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    cout << "socket created" << endl;  
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = htonl(INADDR_ANY); 
    address.sin_port = htons( atoi(argv[2]) ); 
       
    // Forcefully attaching socket to the port 8080 
    if ((bind(server_fd, (struct sockaddr *)&address,  sizeof(address)))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    cout << "Binded" << endl;
    if ((listen(server_fd, 10)) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    cout << "Listening" << endl;
    while(1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr,  (socklen_t*)&client_len))<0) 
        { 
            perror("accept error"); 
            exit(EXIT_FAILURE); 
        } 
        cout << "connection accepted" << endl;

        if ( fork() == 0 )
        {
            close(server_fd);
            cout <<" child pid " << getpid() << " from  parent pid " << getppid() << endl;
            //for(int m = 0;m < 10,000; m++);
            valread = read( new_socket , buffer, 1024);
            //printf("%s\n",buffer ); 
            string configbuff = string(buffer); 
            vector<string> res = splitStrings(configbuff, ',');
            int ret = check_configfile(res[0],res[1]);
            if (ret == 0)
            {
                send(new_socket , mesg , strlen(mesg) , 0 );
                cout << "Authentication successful for pid " << getpid() <<   endl; 
            }
            else
            {
                send(new_socket , fail , strlen(fail) , 0 ); 
                cout << "Authentication failed for pid " << getpid() <<   endl;
            }
            sprintf(path,".%s/%s/",argv[1],res[0].c_str());
            if(FileExists(path))
            {
                cout << res[0] << " file exists in " << argv[1] << " folder"<< endl;
            }
            else
            {
                cout << res[0] << " file doesn't exists in " << argv[1] << " folder"<< endl;
                if((mkdir(path,0700))!=0)
                {
                    perror("mkdir error");
                    cout << "path:" << path <<endl;
                }
                cout << res[0] << " file created in " << argv[1] << " folder" <<endl;
            }
        //printf("%s\n",buffer ); 
        //send(new_socket , hello , strlen(hello) , 0 ); 
        //printf("Hello message sent\n"); 
        }
        //close(server_fd);
    }
    return 0; 
}
