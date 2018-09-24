#include"server_handler.h"
#include "reliability_server.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

int client_length;
int bytes;
int eof = 0;
int j = 0;
int total = 0;
int totalbytes = 0;
#define MAXBUFSIZE 1024

FILE *fp;

const char const *commandlist[] = {"getfile","putfile","delfile","listfile","exitserver"};
char *del_mesg_success = "File existed and has been deleted" ;
char *del_mesg_fail = "File exits but could not be deleted";
char *file_not_exist = "File does not exist";
char *file_exist = "File exists on server ";
char *endoflist = "0";
char *endoffile = "0";
char md5sum[10] = "md5sum ";


int servercommandsearch(char *received_buffer)
{
    int i = 0;
    while(i < 5)
    {
        if (strcmp(commandlist[i], received_buffer) == 0)
        {
            return i;
        }
        i++;
    }
    return -1;
}

void getFileCommandHandler(int sockfd,struct sockaddr_in client_addr)
{
    bzero(filenamebuffer,sizeof(filenamebuffer));
    socklen_t client_addr_len = (socklen_t)sizeof(client_addr);
GETFILENAME:    if(bytes = recvfrom(sockfd, (char *)filenamebuffer, MAXBUFSIZE, MSG_WAITALL, ( struct sockaddr *) &client_addr,&client_addr_len)< 0)
    {
        if(errno == 11) 
        {
            goto GETFILENAME; //try receiving again
        }
       else
       {
            printf("receive from client unsuccessfull -- filename\n");
            exit(1);
       }
    }
    printf("received name of file to get file : %s\n",filenamebuffer);
    
    if (FileExists(filenamebuffer))
    	{
            strcat(md5sum,filenamebuffer);
            //sending status of file
            bytes = sendto(sockfd, (const char *)file_exist, strlen(file_exist), MSG_CONFIRM, (const struct sockaddr *) &client_addr,client_addr_len);
        	printf("File name '%s' exists\n", filenamebuffer);
			//transmit the requested file to the client
			fp = fopen(filenamebuffer, "rb");
			printf("file successfully opened\n");
            sendFile(fp, sockfd, client_addr);
            bzero(filenamebuffer,sizeof(filenamebuffer));
            printf("concatenated string %s\n",md5sum);
            system(md5sum);
            strcpy(md5sum,"md5sum ");
            
    	}
    	else
    	{
        	printf("File name %s does not exist\n",filenamebuffer);
             bzero(filenamebuffer,sizeof(filenamebuffer));
            //sending status of file
            bytes = sendto(sockfd, (const char *)file_not_exist, strlen(file_not_exist), MSG_CONFIRM, (const struct sockaddr *) &client_addr,client_addr_len);

    	}
}

void deleteFileCommandHandler(int sockfd,struct sockaddr_in client_addr)
{
    int file_stat;
    bzero(filenamebuffer,sizeof(filenamebuffer));
    socklen_t client_addr_len = (socklen_t)sizeof(client_addr);
    if(bytes = recvfrom(sockfd, (char *)filenamebuffer, MAXBUFSIZE, MSG_WAITALL, ( struct sockaddr *) &client_addr,&client_addr_len)< 0)
    {
        printf("receive from client unsuccessfull\n");
        exit(1);
    }
    if (FileExists(filenamebuffer))
    {
        printf("File name '%s' exists\n", filenamebuffer);
        //delete the requested file by the client
        fp = fopen(filenamebuffer, "rb");
        file_stat = remove(filenamebuffer);
        if (file_stat == 0)
        {
            printf("%s file deleted successfully.\n", filenamebuffer);
            bytes = sendto(sockfd,del_mesg_success, strlen(del_mesg_success), MSG_CONFIRM, (const struct sockaddr *) &client_addr,client_addr_len);
            fclose(fp);
        }
        else
        {
            printf("Unable to delete the file\n");
            perror("Error");
            bytes = sendto(sockfd, (const char *)del_mesg_fail, strlen(del_mesg_fail), MSG_CONFIRM, (const struct sockaddr *) &client_addr,client_addr_len);
            
        }
    }
    else
    {
        printf("File does not exists at path '%s'\n",filenamebuffer);
        bytes = sendto(sockfd, (const char *)file_not_exist, strlen(file_not_exist), MSG_CONFIRM, (const struct sockaddr *) &client_addr,client_addr_len);
    }
}
void putFileCommandHandler(int sockfd,struct sockaddr_in client_addr)
{
    int com_stat;
    size_t FileSize = 0;
    socklen_t client_addr_len = (socklen_t)sizeof(client_addr);
    bzero(filenamebuffer,sizeof(filenamebuffer));
    //receiving name of file
 PUTFILENAME:   if((bytes = recvfrom(sockfd, (char *)filenamebuffer, MAXBUFSIZE, MSG_WAITALL, ( struct sockaddr *) &client_addr,&client_addr_len))< 0)
    {
        
       
        if(errno == 11) 
        {
            goto PUTFILENAME; //try receiving again
        }
        else 
        {
            printf("receive from client unsuccessfull - filename\n");
            exit(1);
        }

    }
    printf("The name of the file from client : %s\n",filenamebuffer);
    strcat(md5sum,filenamebuffer);

    fp = fopen(filenamebuffer,"wb+");

    //receiving file size content
    int size = 0;
PUTFILESIZE:    if((bytes = recvfrom(sockfd, (char *)&size, sizeof(size) , MSG_WAITALL, ( struct sockaddr *) &client_addr,&client_addr_len))< 0)
    {
        if(errno == 11) goto PUTFILESIZE;
        else 
        {
            printf("receive from client unsuccessfull - filename\n");
            exit(1);
        }
    }
    printf("size of file %d\n",size);
    bzero(filecontentbuffer,sizeof(filecontentbuffer));
    size_t totalFileBytesRecvd = 0;
    int copyofsize = size;
    //receiving file content
    my_recvfrom(fp, sockfd,size,client_addr);
    fclose(fp);
    system(md5sum);
    strcpy(md5sum,"md5sum ");
}
void listFileCommandHandler(int sockfd, struct sockaddr_in client_addr)//https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/

  {
    int current_seq_num = 0;
    socklen_t client_addr_len = (socklen_t)sizeof(client_addr);
    int filecount = 0;
    
    struct dirent *de;  // Pointer for directory entry 
  
    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir("."); 
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
        exit(1); 
    } 
  
    struct udp_struct_t *send_buf = malloc(sizeof( struct udp_struct_t));
    //sending the list of files to client
    printf("List of file on the server side :\n");
    while ((de = readdir(dr)) != NULL) 
    { 
        if (de->d_type == DT_REG) 
        { 
            strcpy(send_buf-> payload ,de->d_name);
            send_buf-> payloadSize = strlen(de->d_name);
            send_buf-> sequence_num = current_seq_num;
            send_buf-> code = UDP_SEND;
            filecount++;
            printf("%s\n", de->d_name);
            //send the file names to the client 
            bytes = sendto(sockfd,send_buf, sizeof(*send_buf), MSG_CONFIRM, (const struct sockaddr *) &client_addr,client_addr_len);
        }
        
    }
    printf("file count is %d\n",filecount);
    send_buf-> code = UDP_END;
    bytes = sendto(sockfd,send_buf, sizeof(*send_buf), MSG_CONFIRM, (const struct sockaddr *) &client_addr,client_addr_len);

    closedir(dr); 
    free(send_buf);    
    
}  
int servercommandhandler(int received_index, int sockfd,struct sockaddr_in client_addr)
{
    int ret = 0;
    switch(received_index)
    {
        case 0 : 
            printf("Command index is 0\n");
            getFileCommandHandler(sockfd,client_addr);
            break;            
        case 1 :
            printf("Command index is 1\n");
            putFileCommandHandler(sockfd,client_addr);
            break;
        case 2 :
            printf("Command index is 2\n");
            deleteFileCommandHandler(sockfd,client_addr);
            break;
        case 3 :
            printf("Command index is 3\n");
            listFileCommandHandler(sockfd,client_addr);
            break;
        case 4 :
            printf("Gracefully exiting the server\n");
            ret = 1;
            break;
        default :
            printf("No index received \n");
            ret = -1;
            break;

    }
    return ret;
}

//function to check if a file exits on the server source-http://www.zentut.com/c-tutorial/c-file-exists/

int FileExists(const char *path)
{
    // Check for file existence
    if (access(path, F_OK) == -1)
        return 0;
    else
        return 1;
}

int sendFile(FILE *fp, int sockfd, struct sockaddr_in client_addr)
{
    char readbuffer[1024];
    int eof = 0;
    int rbytes = 0;
    size_t totalFileSize = 0, Filesize = 0;
    struct stat fstatbuff;
    if((fstat(fileno(fp), &fstatbuff))<0)
    {
        printf("error code :%d error mesg :%s\n",errno,strerror(errno));
    }
    printf("Filesize: %llu\n",fstatbuff.st_size);
    totalFileSize = fstatbuff.st_size;
    Filesize = fstatbuff.st_size;
    //sending total length of file
    if((bytes = sendto(sockfd, (char *)&totalFileSize, sizeof(size_t) , MSG_CONFIRM, (const struct sockaddr *) &client_addr,sizeof(client_addr))) < 0)
    {
        printf("sending file size to client unsuccessfull- sendto function fail\n");
        return -1;
    }
    printf("File size send from server to client\n");
    my_sendto(fp,sockfd,totalFileSize,client_addr);
    fclose(fp);
    return 0;

}

int readFile(FILE* fp, char* buf, int size) //reference- https://www.geeksforgeeks.org/c-program-for-file-transfer-using-udp/
{   
    size_t ret_stat;
    ret_stat = fread(buf, 1, size, fp);
    return ret_stat;

}

