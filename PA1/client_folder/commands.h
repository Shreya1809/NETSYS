/**
 * @file commands.h
 * @brief commands option for the client
 * @author Shreya Chakraborty
 * @version 1
 * @date 2018-09-10
 */
#ifndef __COMMANDS_H__
#define __COMMANDS_H__

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
#include <errno.h>


extern const char const *commandlist[];
char recvbuffer[1024];
char filecontentbuffer[1024];
char filestatus[100];
/** function void getfile
 * @brief The server transmits the requested file to the client (use files of
 * small size in order of 2 to 5 KB for transfer like any jpeg file).
 * @param char ptr filename.
 * @return nothing
 */
void getfile(int sockfd,char *filename,struct sockaddr_in server_addr);

/** function void putfile
 * @brief The server receives the transmitted file by the client and stores it
 * locally (use files of small size in order of 2 to 5 KB for transfer like any jpeg file)
 * @param char ptr filename.
 * @return nothing
 */
void putfile(int sockfd,char *filename,struct sockaddr_in server_addr);

/** function void deletefile
 * @brief The server delete file if it exists. Otherwise do nothing.
 * @param char ptr filename.
 * @return nothing
 */
void deletefile(int sockfd,char *filename,struct sockaddr_in server_addr);

/** function void listfiles
 * @brief The server should search all the files it has in its local directory and send
 *  a list of all these files to the client.
 * @param server address.
 * @return nothing
 */
void listfiles(int sockfd,struct sockaddr_in server_addr);
void listfileclient();

/** function void serverexit
 * @brief The server must exit gracefully
 * @param sock file descriptor and server address structure.
 * @return nothing
 */
void serverexit(int sockfd, struct sockaddr_in server_addr);

/** function void command handler
 * @brief handles all the other functions through index matching
 * @param sock file descriptor and server address structure, option.
 * @return nothing
 */
void commandhandler(char option, char *filename, int sockfd, struct sockaddr_in server_addr);

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
int sendFile(FILE *fp, int sockfd, struct sockaddr_in server_addr);


#endif