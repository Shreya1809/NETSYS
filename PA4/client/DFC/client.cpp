
#include "includes.h"
#include "library.hpp"
#include <errno.h>

#define SERVERNUM 4
extern config_t config;
file_packet_t filepart;
int main(int argc, char const *argv[]) 
{ 
    string enteredstring, configmesg;
    char buff[1024] = {0};
    int server_fd, new_socket, valread;
    struct sockaddr_in server_addr[4]; 
    int addrlen = sizeof(server_addr);  
    int port,opt = 1;
    int sock[4] = {0};
    unsigned char md5 = 3;
    FILE *fp,*fp2;
    size_t filesize = 0;
    if(argc < 2)
    {
        printf("USAGE: ./client [dfc config file]\n");
        return 1;
    } 
    parse_config_file(argv[1]);
    filepart.username = config.username;
    filepart.password = config.password;
    configmesg = config.username + "," + config.password;
    cout << configmesg << endl;
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
        //cout << "command entered : " << enteredstring << endl;
        //exit(0);
        vector<string> entered_option = splitStrings(enteredstring, ' ');
        for (int j = 0;j<4;j++)
        {
            //cout << "j is " << j << endl;
            if ((sock[j] = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
            { 
                printf("\n Socket creation error \n"); 
                return -1; 
            }
           // cout << "Created socket " << sock[j]<< endl;
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
                //printf("Server addr: %u\n",server_addr[j]);
                //return -1; 
            } 
            cout << "Connected socket " << sock[j] << endl;
            //printf("Server addr: %u\n",server_addr[j]);
            send(sock[j] , configmesg.c_str() , configmesg.length() , 0 ); 
            cout<<"Configuration Information Sent to server " << j << endl; 
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
            for(int a = 0; a <4; a++)
            {
                ClientGetCommandHandler(entered_option[1],filepart,sock[a],a);
            }
            
        }
        else if((entered_option[0].compare("PUT")==0) || (entered_option[0].compare("put")==0))
        {
            printf("Command Entered is PUT and filename is %s\n",entered_option[1].c_str());
            filepart.command = "PUT";
            filepart.nameoffile = entered_option[1];
            string filename = "root/" + entered_option[1];
            printf("filename %s\n",filename.c_str());
            // check if file exits
            if((FileExists(filename.c_str())) == 1)
            {
                cout << "file exists" << endl;
                fp =fopen(filename.c_str(),"rb");
                //fine file size
                struct stat st;
                stat(filename.c_str(), &st);
                filesize = st.st_size;
                printf("filesize %d\n",filesize);
                //char * filepartbuf = NULL; // variable to store parts of file
                
                //fine md5 to decide which parts to send to which server
                md5 = md5hash(filename,filesize,fp);
                filepart.md5val = md5;
                cout << "md5val : "<< filepart.md5val << endl; 
                float f = (filesize);
                //devide filesize into 4 parts
                f = f/SERVERNUM;
                int file_Size_Server = ceil(f);
                char filepartbuf[file_Size_Server] = {0};
                //set size for each part in case filesize is not divisible by 4
                for(filepart.file_part_serial_no = 0;filepart.file_part_serial_no < SERVERNUM;filepart.file_part_serial_no++)
                {
                    if(filepart.file_part_serial_no == 3)
                    {
                        filepart.file_part_size[filepart.file_part_serial_no] = filesize - 3*file_Size_Server;
                        
                    }
                    else
                    {
                        filepart.file_part_size[filepart.file_part_serial_no] = file_Size_Server;
                    } 
                    //filepartbuf = (char*)malloc(filepart.file_part_size);
                    //cout <<"size of part " << filepart.file_part_serial_no << " is "<<filepart.file_part_size << endl;
                    memset(filepartbuf, 0, sizeof(filepartbuf)); 
                    fread(filepartbuf,1,filepart.file_part_size[filepart.file_part_serial_no],fp);
                    filepart.file_part_data[filepart.file_part_serial_no] = std::string(filepartbuf,filepart.file_part_size[filepart.file_part_serial_no]);
                    //cout << "Size:"<<filepart.file_part_data[filepart.file_part_serial_no].size()<<" data is :";
                    // for(auto x : filepart.file_part_data[filepart.file_part_serial_no]){
                    //     cout<<x;
                    // }
                    //free(filepartbuf);

                }
                
                string final_mesg = filepart.file_part_data[0]+filepart.file_part_data[1]+filepart.file_part_data[2]+filepart.file_part_data[3];
                //cout << final_mesg <<endl;
                //cout << "Final Msg Size:"<<final_mesg.size()<<endl;
                char filewrite[filesize] = {0};
                memcpy(filewrite,final_mesg.c_str(),filesize);
                fp2 = fopen("file","wb");
                fwrite(filewrite,1,sizeof(filewrite),fp2);
                fclose(fp2);
                fclose(fp);
                for(int k = 0;k<4;k++)
                {
                    ClientPutCommandHandler(filepart,sock[k],k);
                }
                
            }
            else
            {
                cout <<"file does not exist" << endl;
                return -1;
            }
             
        }
        else if((entered_option[0].compare("LIST")==0) || (entered_option[0].compare("list")==0))
        {
            printf("Command Entered is LIST and the list is as below:\n");
        }
        else if((entered_option[0].compare("MKDIR")==0) || (entered_option[0].compare("mkdir")==0))
        {
            printf("Command Entered is MKDIR and the subfolder is %s\n",entered_option[1].c_str());
        }
        else
        {
            cout << "INVALID COMMAND" << endl;
            exit(0);
        }
        for (int i = 0;i < 4;i++)
        {
            close(sock[i]);
        }
    }
    return 0;
}
    