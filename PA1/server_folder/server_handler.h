#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <dirent.h> 

char filenamebuffer[1024];
char filecontentbuffer[1024];
char filesizebuffer[20];
char copybuffer[1024];

extern const char const *commandlist[]; //comand index list
//searches for commands with the received index
int servercommandsearch(char *received_buffer);
//Handles all other command functions on the server, goes to the function pertaining the command index
int servercommandhandler(int received_index,int sockfd,struct sockaddr_in client_addr);
//Function to handle the getfile command from client
void getFileCommandHandler(int sockfd,struct sockaddr_in client_addr);
//Function to handle the deletefile command from client
void deleteFileCommandHandler(int sockfd,struct sockaddr_in client_addr);
//Function to handle the putfile command from client
void putFileCommandHandler(int sockfd,struct sockaddr_in client_addr);
//Function to handle the listfile command from client
void listFileCommandHandler(int sockfd, struct sockaddr_in client_addr);
//function to check if the file exists
/** function FileExits 
 * @brief checks if the given file exists
 * @param file name 
 * @return 1 if successful
 */
int FileExists(const char *path);
/** function readFile 
 * @brief reads file content into the buffer for the given size
 * @param file pointer, buffer to read into, size to read 
 * @return bytes read
 */
int readFile(FILE* fp, char* buf, int size);

/** function sendFile 
 * @brief checks if the given file exists
 * @param file pointer, sock filed descriptor, server address
 * @return 0 if suceessful
 */
int sendFile(FILE *fp, int sockfd, struct sockaddr_in client_addr);
