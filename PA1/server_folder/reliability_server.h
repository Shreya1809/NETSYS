#ifndef __RELIABILITY_SERVER_H__
#define __RELIABILITY_SERVER_H__

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


/**
 * @brief enums for structure code to send
 * 
 */
typedef enum
{ 
    UDP_SEND,
    UDP_ACK, 
    UDP_END
}udp_enum_t;

const char const *UDP_ENUM_STR[UDP_END] = {

    "UDP_SEND",
    "UDP_ACK"
};



struct __attribute__((__packed__)) udp_struct_t {
    uint32_t sequence_num;
    udp_enum_t code;
    uint32_t payloadSize;
    uint8_t payload[1024];
    uint32_t CRC32;
};


struct udp_struct_t udp_packet;
/**
 * @brief timelimit for 2sec
 * 
 * @param sockfd sock file descriptor
 * @param io 
 */
void timelimit(int sockfd, int io);
/**
 * @brief toreceive file reliably
 * 
 * @param fp file pointer
 * @param sockfd socket file des
 * @param size size of each packet
 * @param client_addr address of the server
 */
void my_recvfrom(FILE *fp,int sockfd,int size,struct sockaddr_in client_addr);

/**
 * @brief to send files reliably
 * 
 * @param fp file pointer
 * @param sockfd socket file descriptor
 * @param size size of each packet
 * @param server_addr address of server
 */
void my_sendto(FILE *fp,int sockfd,int size,struct sockaddr_in client_addr);

#endif 