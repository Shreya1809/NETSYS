
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "commands.h"
#include "reliability_client.h"

char md5sum[10] = "md5sum ";
int numofbytes, bytes,byteswrite;
char  nameoffile[20];
char listend[] = {"0"};
char endoffile[] = {"0"};
char filenotexistmesg[] = "File does not exist ";
FILE *fp;

const char const *commandlist[] = {"getfile","putfile","delfile","listfile","exitserver"};
 
void getfile(int sockfd,char *filename,struct sockaddr_in server_addr)
{
    int mem_ret = 0;
    //sending command index
    printf("Selected Opton: get file from server\n");
    bzero(filestatus,sizeof(filestatus));
    socklen_t server_addr_len = sizeof(server_addr);
    if(numofbytes = sendto(sockfd, (const char *)(commandlist[0]), strlen(commandlist[0]), MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        printf("sendto unsuccessful \n");
        exit(1);
    }
    printf("Command %s sent by the client to the server for file %s\n", commandlist[0],filename);
    numofbytes = 0;
    sleep(2);
    //sending filename
    if(numofbytes = sendto(sockfd, (filename), strlen(filename), MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        printf("sendto unsuccessful \n");
        exit(1);
    }
    printf("name of the file sent by the client to the server for get file %s\n", filename);
    
    //receive status of file
    if(bytes = recvfrom(sockfd, (char *)filestatus, 100 , MSG_WAITALL, ( struct sockaddr *) &server_addr,&server_addr_len)< 0)
    {
        printf("receive from client unsuccessfull\n");
        exit(1);
    }
    printf("File Status:  %s\n",filestatus);
    mem_ret = strcmp(filestatus,"File does not exist");
    if(mem_ret == 0)
    {
        return;
    }
    strcat(md5sum,filename);
    fp = fopen(filename,"wb+");
    //receive file size from server
    int size = 0;
    if(bytes = recvfrom(sockfd, (char *)&size, sizeof(size) , MSG_WAITALL, ( struct sockaddr *) &server_addr,&server_addr_len)< 0)
    {
        printf("receive from client unsuccessfull\n");
        exit(1);
    }
    printf("size of file %d\n",size);
    bzero(filecontentbuffer,sizeof(filecontentbuffer));
    size_t totalFileBytesRecvd = 0;
    int copyofsize = size;
    bzero(recvbuffer,sizeof(recvbuffer));
    //receiving file content
    my_recvfrom(fp, sockfd,size,server_addr);
    fclose(fp);
    printf("concatenated string %s\n",md5sum);
    system(md5sum);
    strcpy(md5sum,"md5sum ");
    
}

void putfile(int sockfd,char *filename,struct sockaddr_in server_addr)
{
    char ch;
    printf("Selected Opton: put file on server\n");
    if (FileExists(filename))
    {
        printf("file exists on the client side\n");
        //sending command index
        if(numofbytes = sendto(sockfd, (const char *)(commandlist[1]), strlen(commandlist[1]), MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        {
            printf("sendto unsuccessful \n");
            exit(1);
        }
        sleep(2);
        printf("signal sent by the client to the server to put file\n");
        numofbytes = 0;
        //sending filename
        if(numofbytes = sendto(sockfd, (const char *)(filename), strlen(filename), MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        {
            printf("sendto unsuccessful \n");
            exit(1);
        }
        sleep(2);
        printf("name of the file sent by the client to the server to put file\n");
    
        //sending file content
        fp = fopen(filename, "rb");
        sendFile(fp,sockfd, server_addr);
        strcat(md5sum,filename);
        system(md5sum);
        strcpy(md5sum,"md5sum ");
        
    }
    else 
    {
        printf("file does not exist on client\n");

    }
} 

void deletefile(int sockfd,char *filename,struct sockaddr_in server_addr)
{
    printf("Selected Opton: delete file on server\n");
    //sending command index
    if(numofbytes = sendto(sockfd, (const char *)(commandlist[2]), strlen(commandlist[2]), MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        printf("sendto unsuccessful \n");
        exit(1);
    }
    printf("signal sent by the client to the server to delete file\n");
    numofbytes = 0;
    //sending filename
    if(numofbytes = sendto(sockfd, (const char *)(filename), strlen(filename), MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        printf("sendto unsuccessful \n");
        exit(1);
    }
    printf("name of the file sent by the client to the server to delete file\n");
    //receving the status of delete file from server
     bzero(recvbuffer,sizeof(recvbuffer));
    socklen_t server_addr_len = sizeof(server_addr);
	if((bytes = recvfrom(sockfd, (char *)recvbuffer, 1024, MSG_WAITALL, (struct sockaddr *)&server_addr, &server_addr_len))< 0)
	{
		printf("receive del file status from server unsuccessfull\n");
		exit(1);
	}

    printf("Delete file status : %s\n", recvbuffer);


}
void listfiles(int sockfd,struct sockaddr_in server_addr)
{
    int filelist = 0;
    socklen_t server_addr_len = sizeof(server_addr);
    printf("Selected Opton: list files on server\n");
    //sending command index
    if(numofbytes = sendto(sockfd, (const char *)(commandlist[3]), strlen(commandlist[3]), MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        printf("sendto unsuccessful \n");
        exit(1);
    }
    printf("signal sent by the client to the server to list file\n");
    numofbytes = 0;
    struct udp_struct_t *recv_buf = malloc(sizeof( struct udp_struct_t));
    printf("list of files\n");
    while(1)
    {
        if((bytes = recvfrom(sockfd, recv_buf, 1024, MSG_WAITALL, (struct sockaddr *)&server_addr, &server_addr_len))< 0)
        {
            printf("receive list file status from server unsuccessfull\n");
            exit(1);
        }
       
        if(recv_buf->code == UDP_END)
        {
            printf("Number of files listed : %d\n",filelist);
            break;
        }
        filelist++;
        printf("%s\n", recv_buf->payload);
        

    }
    free(recv_buf);
}
void serverexit(int sockfd, struct sockaddr_in server_addr)
{
    
   
    if(numofbytes = sendto(sockfd, (const char *)(commandlist[4]), strlen(commandlist[4]), MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        printf("sendto unsuccessful \n");
        exit(1);
    }
    
    printf("signal sent by the client to exit the server\n");
}

void listfileclient()
{
    int filecount = 0;
    
    struct dirent *de;  // Pointer for directory entry 
  
    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir("."); 
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
        exit(1); 
    } 
    printf("List of file on the client side :\n");
    while ((de = readdir(dr)) != NULL) 
    { 
        if (de->d_type == DT_REG) 
        { 
            filecount++;
            printf("%s\n", de->d_name);
            
        }
        
    }
    printf("file count is %d\n",filecount);

}


void commandhandler(char option, char *filename, int sockfd, struct sockaddr_in server_addr)
{

    switch(option)
    {
        case 0 : 
            printf("The name of the file to get from server : %s\n",filename);
            getfile(sockfd,filename,server_addr);
            break;
        case 1 :
            printf("The name of the file to add to server : %s\n",filename);
            putfile(sockfd,filename,server_addr);
            break;
        case 2 :
            printf("The name of the file to be deleted from server : %s\n",filename);
		    deletefile(sockfd,filename,server_addr);
            break;
        case 3 :
            printf("To list the number of files in the server\n");
	        listfiles(sockfd,server_addr);
            break;
        case 4 :
            printf("To exit the server gracefully\n");
	        serverexit(sockfd,server_addr);
            break;
        case 5 :
            printf("To list files on the client\n");
	        listfileclient();
            break;
        default :
            printf("Enter a valid option\n");
            printf("what you entered : %c\n",option);
            break;

    }
}

int FileExists(const char *path)
{
    // Check for file existence
    if (access(path, F_OK) == -1)
        return 0;
    else
        return 1;
}

int sendFile(FILE *fp, int sockfd, struct sockaddr_in server_addr)
{
    char readbuffer[1024];
    int eof = 0;
    int rbytes = 0;
    size_t totalFileSize = 0;

    struct stat fstatbuff;
    fstat(fileno(fp), &fstatbuff);
    printf("Filesize: %ld\n",fstatbuff.st_size);
    totalFileSize = fstatbuff.st_size;
    //sending total length of file
    if((bytes = sendto(sockfd, (char *)&totalFileSize, sizeof(size_t) , MSG_CONFIRM, (const struct sockaddr *) &server_addr,sizeof(server_addr))) < 0)
    {
        printf("sending file to server unsuccessfull- sendto function fail\n");
        return -1;
    }
    
     my_sendto(fp,sockfd,totalFileSize,server_addr);
     fclose(fp);
    
    return 0;

}

int readFile(FILE* fp, char* buf, int size) 
{
    size_t ret_stat;
    ret_stat = fread(buf, 1, size, fp);
    return ret_stat;
}
