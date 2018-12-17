
#include "includes.h"
#include "library.hpp"
#include <errno.h>

#define SERVERNUM 4
extern config_t config;
file_packet_t filepart;
extern string liststring;
extern get_packet_t getfile;
extern int part[4];
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
    int socketfailflag[4] = {0};
    FILE *fp,*fp2;
    size_t filesize = 0;
    struct timeval timeout = {1,0};
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
        printf("\nEnter any of the following commands: \n1. GET [filename] [subfolder] \n2. PUT [filename] [subfolder] \n3. LIST [subfolder] \n4. MKDIR [subfolder]\n");
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
                socketfailflag[j] = 1;
            
            } 
            else socketfailflag[j] = 0;
            if(socketfailflag[j] != 1)
            {
                //cout << "Connected socket " << sock[j] << endl;
                //printf("Server addr: %u\n",server_addr[j]);
                send(sock[j] , configmesg.c_str() , configmesg.length() , 0 ); 
                cout<<"Configuration Information Sent to server " << j+1 << endl; 
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
                    cout << "invalid username/password" << endl;
                    exit(0);
                }
                //return 0;
            } 
        }
        if((entered_option[0].compare("GET")==0) || (entered_option[0].compare("get")==0))
        {
            FILE *fp;
            //part[4]={0,0,0,0};
            int completeflag = 1;
            part[0]=0;
            part[1]=0;
            part[2]=0;
            part[3]=0;
            printf("Command Entered is GET and filename is %s\n",entered_option[1].c_str());
            // getfile = {0};
            for(int a = 0; a <4; a++)
            {
                ClientGetCommandHandler(entered_option[1], entered_option[2],filepart,sock[a],a,socketfailflag[a]);
            }
            if(part[0]== 1 && part[1] == 1 && part[2] == 1 && part[3] == 1)
            {
                string subfolderstring;
                if(filepart.subfolder == ".")
                {
                    subfolderstring = getfile.nameoffile;
                    
                }
                else 
                {
                    mkdir((getfile.subfolder).c_str(),0700);
                    subfolderstring =  getfile.subfolder +"/"+ getfile.nameoffile;
                    //cout << "subfolderstring " << subfolderstring << endl;
                }
                fp = fopen(subfolderstring.c_str(), "wb");
                cout << "#FILE COMPLETE" << endl;
                printf("%d,%d,%d,%d\n",part[0],part[1],part[2],part[3]);
                if(fp!=NULL)
                {
                    // string decryptedmesg = getfile.file_part_data[0] +
                    //                         getfile.file_part_data[1] +
                    //                         getfile.file_part_data[2] +
                    //                         getfile.file_part_data[3];
                    string decryptedmesg;
                    for(int j = 0;j < 4;j++)
                    {
                        //cout << "Data [" << j << "] : " << getfile.file_part_data[j] << endl ;
                        decryptedmesg += encryptdecrypt((getfile.file_part_data[j]),"5");
                        // decryptedmesg += getfile.file_part_data[j];
                    }
                    fwrite(decryptedmesg.c_str(),decryptedmesg.size(),1,fp);
                    fclose(fp);
                }
                else
                {
                    cout << "fp is null" << endl;
                }
               
            }
            else if((part[0]== 0 && part[1] == 0 && part[2] == 0 && part[3] == 0))
            {
                printf("%d,%d,%d,%d\n",part[0],part[1],part[2],part[3]);
                cout << "#FILE DOES NOT EXIST AT ALL" << endl;    
            }
            else
            {
                printf("%d,%d,%d,%d\n",part[0],part[1],part[2],part[3]);
                cout << "#CANNOT CREATE COMPLETE FILE" << endl;
                
            }
            
        }
        else if((entered_option[0].compare("PUT")==0) || (entered_option[0].compare("put")==0))
        {
            printf("Command Entered is PUT and filename is %s\n",entered_option[1].c_str());
            filepart.command = "PUT";
            filepart.nameoffile = entered_option[1];
            filepart.subfolder = entered_option[2];
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
                char filepartbuf[file_Size_Server];
                memset(filepartbuf,0,file_Size_Server);
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
                    
                    memset(filepartbuf, 0, sizeof(filepartbuf)); 
                    fread(filepartbuf,1,filepart.file_part_size[filepart.file_part_serial_no],fp);
                    filepart.file_part_data[filepart.file_part_serial_no] = std::string(filepartbuf,filepart.file_part_size[filepart.file_part_serial_no]);
                    
                }
                
                string final_mesg = filepart.file_part_data[0]+filepart.file_part_data[1]+filepart.file_part_data[2]+filepart.file_part_data[3];
                
                char filewrite[filesize];
                memset(filewrite,0,filesize);
                memcpy(filewrite,final_mesg.c_str(),filesize);
                fp2 = fopen("file","wb");
                fwrite(filewrite,1,sizeof(filewrite),fp2);
                fclose(fp2);
                fclose(fp);
                for(int k = 0;k<4;k++)
                {
                    ClientPutCommandHandler(filepart,sock[k],k,socketfailflag[k]);
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
            string subfolder = entered_option[1];
            printf("\nServer Status:\n");
            for(int b = 0; b <4; b++)
            {
                ClientListCommandHandler(subfolder,sock[b],b, socketfailflag[b]);
            }
            
            printf("\nThe list is as below:\n");
            cout <<getFileStatusFromParts(liststring) << endl;
            liststring = "";
            

        }
        else if((entered_option[0].compare("MKDIR")==0) || (entered_option[0].compare("mkdir")==0))
        {
            printf("Command Entered is MKDIR and the subfolder is %s\n",entered_option[1].c_str());
            string subfolder = entered_option[1];
            for(int i = 0;i < 4;i ++)
            {
                ClientMkdirCommandHandler(subfolder,sock[i],i,socketfailflag[i]);
            }
            
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
    