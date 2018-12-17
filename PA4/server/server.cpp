#include "includes.h"
#include "commandhandler.hpp"
// reference : https://www.geeksforgeeks.org/socket-programming-cc/
extern received_packet_t filepart;
extern send_packet_t sendfile;
extern list_packet_t listfile;
extern mkdir_t mkdirfile;
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
    char info[1024] = {0};
    FILE *fp1, *fp2 , *fp3;
    string pathname;
    //string getlist = " ";
    
    int commandflag = 0;
    

    //char *hello = "Hello from server"; 

    if(argc < 3)
    {
        printf("USAGE: ./server [DFS folder] [PORT NUM]\n");
        return 1;
    }   
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
    TRY:    valread = read( new_socket , buffer, 1024);
            //printf("%s\n",buffer ); 
            string configbuff = string(buffer); 
            vector<string> res = splitStrings(configbuff, ',');
            int ret = check_configfile(buffer);
            if (ret == 0)
            {
                send(new_socket , mesg , strlen(mesg) , 0 );
                cout << "Authentication successful for pid " << getpid() <<   endl; 
            }
            else
            {
                printf("return val is %d\n",ret);
                send(new_socket , fail , strlen(fail) , 0 ); 
                cout << "Authentication failed for pid " << getpid() <<   endl;
                goto TRY;
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
            int receivebytes = 0;
            //receive command from client
            if ((receivebytes =read(new_socket,info ,sizeof(info))) < 0)
            {
                printf("Error: %s and code %d\n", strerror( errno ), errno);
            }
            else
            {
                printf("Infostring : %s\n",info);
                string infostring = string(info);
                vector<string> clientinput = splitStrings(infostring, ','); 
                if(clientinput[0]== "GET")
                {
                    commandflag = 0;    
                }
                else if(clientinput[0]== "PUT")
                {
                    commandflag = 1;
                }
                else if(clientinput[0]== "LIST")
                {
                    commandflag = 2;
                }
                else if(clientinput[0]== "MKDIR")
                {
                    commandflag = 3;
                }

                else commandflag = 4;
                parse_infostring(infostring,commandflag);
                // handling commands on server side
                if (commandflag == 1)
                {
                    struct timeval timeout = {1,0};
			        setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout,sizeof(struct timeval));
                    char file_part_data1[filepart.file_part_1_size];
                    char file_part_data2[filepart.file_part_2_size];
                    memset(file_part_data1,0,filepart.file_part_1_size);
                    memset(file_part_data2,0,filepart.file_part_2_size);
                    string subfoldername;
                    //string filestat;
                    if(filepart.subfolder== ".")
                    {
                        subfoldername = "/";
                        send(new_socket,"DEFAULT",10,0);
                        //cout << "IN HERE "<<  endl;
                        
                    }
                    else
                    {
                        //cout << "IN HERE TOO" << endl;
                        subfoldername = "/" + filepart.subfolder + "/";
                        if(!checksubfolder(new_socket,filepart))
                        {
                            return 0 ;
                        }
                    } 
                    if (filepart.server_num == 1)
                    {
                        send(new_socket,"Server 1 OK",15,0);
                        pathname = "DFS1/"+filepart.username + subfoldername;
                        
                    }
                    if (filepart.server_num == 2)
                    {
                        send(new_socket,"Server 2 OK",15,0);
                        pathname = "DFS2/"+filepart.username + subfoldername;
                    }
                    if (filepart.server_num == 3)
                    {
                        send(new_socket,"Server 3 OK",15,0);
                        pathname = "DFS3/"+filepart.username + subfoldername;
                    }
                    if (filepart.server_num == 4)
                    {
                        send(new_socket,"Server 4 OK",15,0);
                        pathname = "DFS4/"+filepart.username + subfoldername;
                    }
                    // timeout = {2,0};
				    // setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout,sizeof(struct timeval));
                    // read(new_socket,file_part_data1 ,sizeof(file_part_data1));
                    size_t arbytes = 0;
                    int rbytes = 0;
                    while(arbytes < sizeof(file_part_data1)){
                        if ((rbytes =read(new_socket,&file_part_data1[arbytes],sizeof(file_part_data1)-arbytes)) < 0 )
                        {
                            printf("ERR: rbytes : %d\n",rbytes);
                        }
                        else{
                            printf("Recv:rbytes : %d\n",rbytes);
                            arbytes += rbytes;
                        }
                    }
                    printf("Exp: %d, Arbytes : %d\n",sizeof(file_part_data1), arbytes);


                    // timeout = {2,0};
				    // setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout,sizeof(struct timeval));
                    // read(new_socket,file_part_data2 ,sizeof(file_part_data2));
                    arbytes = 0;
                    rbytes = 0;
                    while(arbytes < sizeof(file_part_data2)){
                        if ((rbytes =read(new_socket,&file_part_data2[arbytes],sizeof(file_part_data2)-arbytes)) < 0 )
                        {
                            printf("ERR: rbytes : %d\n",rbytes);
                        }
                        else{
                            printf("Recv:rbytes : %d\n",rbytes);
                            arbytes += rbytes;
                        }
                    }
                    printf("Exp: %d, Arbytes : %d\n",sizeof(file_part_data2), arbytes);
                    // printf("data file part 1: %s\n",file_part_data1);
                    // printf("data file part 2: %s\n",file_part_data2);
                    // printf("username : %s\n",filepart.username.c_str());
                    string filepartname1 = pathname + filepart.nameoffile + "."+ std::to_string(filepart.file_part_1);
                    string filepartname2 = pathname + filepart.nameoffile + "."+ std::to_string(filepart.file_part_2);
                    //cout << filepartname1 << endl;
                    fp1 = fopen(filepartname1.c_str(),"wb");
                    fp2 = fopen(filepartname2.c_str(),"wb");
                    fwrite(file_part_data1 , 1 , sizeof(file_part_data1) , fp1 );
                    fclose(fp1);
                    fwrite(file_part_data2 , 1 , sizeof(file_part_data2) , fp2 );
                    fclose(fp2);
                    memset(file_part_data1,0,sizeof(file_part_data1));
                    memset(file_part_data2,0,sizeof(file_part_data2));
                }
                if (commandflag == 0) //get
                {
                    string subfoldername;
                    //string filestat;
                    if(sendfile.subfolder== ".")
                    {
                        subfoldername = sendfile.username + "/" + sendfile.nameoffile + "." ;
                        send(new_socket,"DEFAULT",10,0);
                       // cout << "IN HERE "<<  endl;
                        
                    }
                    else
                    {
                        cout << "IN HERE NOW" << endl;
                        subfoldername = sendfile.username+ "/" + sendfile.subfolder +"/"+ sendfile.nameoffile + "." ;;
                        if(!getsubfolder(new_socket,sendfile))
                        {
                            return 0 ;
                        }
                    } 
                    
                    if (sendfile.server_num == 1)
                    {
                        send(new_socket,"Server 1 OK",11,0);
                        pathname = "DFS1/"+ subfoldername;
                        getfilehandler(pathname,sendfile, new_socket);
                        //filepartfinder(pathname);   
                    }
                    if (sendfile.server_num == 2)
                    {
                        send(new_socket,"Server 2 OK",11,0);
                        pathname = "DFS2/"+ subfoldername;
                        getfilehandler(pathname,sendfile, new_socket);
                    }
                    if (sendfile.server_num == 3)
                    {
                        send(new_socket,"Server 3 OK",11,0);
                        pathname = "DFS3/"+ subfoldername;
                        getfilehandler(pathname,sendfile, new_socket);
                    }
                    if (sendfile.server_num == 4)
                    {
                        send(new_socket,"Server 4 OK",11,0);
                        pathname = "DFS4/"+ subfoldername;
                        getfilehandler(pathname,sendfile, new_socket);
                    }  
                    
                }
                if (commandflag == 2)
                {
                    int partno;
                    string subfoldername;
                    //string filestat;
                    if(listfile.subfolder== ".")
                    {
                        subfoldername = listfile.username;
                        send(new_socket,"DEFAULT",10,0);
                       // cout << "IN HERE "<<  endl;
                        
                    }
                    else
                    {
                        cout << "IN HERE NOW" << endl;
                        subfoldername = listfile.username+ "/" + listfile.subfolder;
                        if(!listsubfolder(new_socket,listfile))
                        {
                            return 0 ;
                        }
                    } 
                    if (listfile.server_num == 1)
                    {
                        send(new_socket,"Server 1 OK",15,0);
                        pathname = "./DFS1/"+subfoldername;
                        listfilehandler(pathname,listfile, new_socket);   
                    }
                    if (listfile.server_num == 2)
                    {
                        send(new_socket,"Server 2 OK",15,0);
                        pathname = "./DFS2/"+subfoldername;
                        listfilehandler(pathname,listfile, new_socket);
                    }
                    if (listfile.server_num == 3)
                    {
                        send(new_socket,"Server 3 OK",15,0);
                        pathname = "./DFS3/"+subfoldername;
                        listfilehandler(pathname,listfile, new_socket);
                    }
                    if (listfile.server_num == 4)
                    {
                        send(new_socket,"Server 4 OK",15,0);
                        pathname = "./DFS4/"+subfoldername;
                        listfilehandler(pathname,listfile, new_socket);
                        
                    } 
                
                }
                if(commandflag == 3)
                {
                    char mkdirbuf[100] = {0};
                    string pathname;
                    //read(new_socket,mkdirbuf,sizeof(mkdirbuf));
                    //cout << mkdirbuf << endl;
                    cout << "command flag " << commandflag << endl;
                    if(mkdirfile.server_num == 1)
                    {
                        send(new_socket,"Server 1 OK",15,0);
                        pathname = "DFS1/"+mkdirfile.username + "/" + mkdirfile.subfolder;
                        if((mkdirfile.subfolder).length() > 1)
                        {
                            mkdir(pathname.c_str(),0700);
                            cout << "Subfolder - "<< mkdirfile.subfolder << " created in folder "<< mkdirfile.username << " of DFS1" <<endl;
                        }
                        else cout << "Subfolder could not be created" << endl;
                        
                    }
                    if(mkdirfile.server_num == 2)
                    {
                        send(new_socket,"Server 2 OK",15,0);
                        pathname = "DFS2/"+mkdirfile.username + "/" + mkdirfile.subfolder;
                        if((mkdirfile.subfolder).length() > 1)
                        {
                            mkdir(pathname.c_str(),0700);
                            cout << "Subfolder - "<< mkdirfile.subfolder << " created in folder "<< mkdirfile.username << " of DFS2" <<endl;
                        }
                        else cout << "Subfolder could not be created" << endl;
                        
                    }
                    if(mkdirfile.server_num == 3)
                    {
                        send(new_socket,"Server 3 OK",15,0);
                        pathname = "DFS3/"+mkdirfile.username + "/" + mkdirfile.subfolder;
                        if((mkdirfile.subfolder).length() > 1)
                        {
                            mkdir(pathname.c_str(),0700);
                            cout << "Subfolder - "<< mkdirfile.subfolder << " created in folder "<< mkdirfile.username << " of DFS3" <<endl;
                        }
                        else cout << "Subfolder could not be created" << endl;
                        
                    }
                    if(mkdirfile.server_num == 4)
                    {
                        send(new_socket,"Server 4 OK",15,0);
                        pathname = "DFS4/"+mkdirfile.username + "/" + mkdirfile.subfolder;
                        if((mkdirfile.subfolder).length() > 1)
                        {
                            mkdir(pathname.c_str(),0700);
                            cout << "Subfolder - "<< mkdirfile.subfolder << " created in folder "<< mkdirfile.username << " of DFS4" <<endl;
                        }
                        else cout << "Subfolder could not be created" << endl;
                    }

                }
            }
        close(new_socket);
        exit(0); 
        }
               
    }
    return 0; 
}
