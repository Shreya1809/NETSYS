#include "reliability_server.h"


struct timeval timeout;
void timelimit(int sockfd, int io)
{
timeout.tv_sec = 0.0;
timeout.tv_usec = 500000.0;
    if (io == 1)
    {
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&timeout,sizeof(struct timeval)) < 0) 
        {
            //printf("setsockopt for timeout-input failed\n");
            //printf("error code :%s\n",strerror(errno));
        }
    }
    if(io == 2)
    {
        if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
        {
            printf("setsockopt for timeout-output failed");
        }
    }
}

//function to receive file from the client
void my_recvfrom(FILE *fp,int sockfd,int size,struct sockaddr_in client_addr)
{
   int rebytes = 0,sebytes = 0,BytesRecvd = 0;
    socklen_t client_addr_len = (socklen_t)sizeof(client_addr);
    int current_seq_count = 0;
    //structure for packet storage
    struct udp_struct_t *recv_buf = malloc(sizeof( struct udp_struct_t));
    struct udp_struct_t *send_buf = malloc(sizeof( struct udp_struct_t));
    if(recv_buf != NULL && send_buf != NULL)
	{
        
        while(size)
        {
        RECV:    
            if((rebytes = recvfrom(sockfd, recv_buf, sizeof(*recv_buf), MSG_WAITALL, ( struct sockaddr *) &client_addr,&client_addr_len))< 0)
            {
                if(errno == EAGAIN) //timeout occurs then errno is 11
                {
                    printf("Timeout\n");
                    goto RECV;
                }
                else
                {
                    printf("receive from client unsuccessfull-myrecvfrom\n");
                    exit(1);
                }
            }
            if(recv_buf->code == UDP_SEND)
            {
                BytesRecvd += rebytes;
                printf("Data received from client seq no - %d\n",recv_buf->sequence_num);
                printf("received code : %s\n",UDP_ENUM_STR[recv_buf->code]);
                printf("received payload size = %d\n", recv_buf->payloadSize);
                printf("current_seq_count = %d\n", current_seq_count);
                printf("received seq number = %d\n",recv_buf->sequence_num);
                if(recv_buf->sequence_num == current_seq_count-1){
                    
                    //getting old data again whose sent ack was lost
                    printf("old packet recvd again. Sending just the ack num %d\n",recv_buf->sequence_num);
                    send_buf-> code = UDP_ACK;
                    send_buf-> sequence_num = recv_buf->sequence_num;
                    send_buf-> CRC32 = 0;   

                    if((sebytes = sendto(sockfd, send_buf, sizeof(*send_buf), MSG_CONFIRM, (const struct sockaddr *) &client_addr,sizeof(client_addr))) < 0)
                    {
                        printf("sendto unsuccessful \n");
                        exit(1);
                    }
                    printf("ACK resent for old packet\n");

                }
                else if (current_seq_count == recv_buf->sequence_num) //check if sequence matches
                {
                    
                    printf("sequence match\n");
                    //send ack to server
                    send_buf-> code = UDP_ACK;
                    send_buf-> sequence_num = current_seq_count;
                    send_buf-> CRC32 = 0;   

                    if(sebytes = sendto(sockfd, send_buf, sizeof(*send_buf), MSG_CONFIRM, (const struct sockaddr *) &client_addr,sizeof(client_addr)) < 0)
                    {
                        printf("sendto unsuccessful \n");
                        exit(1);
                    }
                    printf("ACK sent to client from server - %d\n",send_buf-> sequence_num);

                    //write to the file
                    fwrite(recv_buf->payload,1,recv_buf->payloadSize,fp);
                    size -= recv_buf->payloadSize;
                    current_seq_count++;
                }
                else
                {
                    printf("Received sequence num does not match with current sequence number. Drop packet\n");
                    BytesRecvd -= rebytes;
                    goto RECV;
                }
            }
            //else not UDP_SEND
        }
    }
    else 
    { 
        printf("recv_buff or send buff Null\n");
        return;
    }

    free(recv_buf);
    free(send_buf);

}

//function to send file to the client
void my_sendto(FILE *fp,int sockfd, int size,struct sockaddr_in client_addr)
{
    //int a = 0;
    int rebytes = 0,sebytes = 0,filesize = 0, ack = 0;
    size_t bytesread = 0;
    socklen_t client_addr_len = (socklen_t)sizeof(client_addr);
    int current_seq_count = 0;
    //structure for packet storage
    struct udp_struct_t *recv_buf = malloc(sizeof( struct udp_struct_t));
    struct udp_struct_t *send_buf = malloc(sizeof( struct udp_struct_t));
    if(send_buf != NULL && recv_buf != NULL)
	{
        timelimit(sockfd,1);
        while(filesize < size)
        { 
            
            bytesread = fread(send_buf-> payload, 1, 1024, fp);
            filesize += bytesread;
            send_buf->payloadSize = bytesread;
            send_buf-> sequence_num = current_seq_count;
            send_buf-> code = UDP_SEND;
 SEND:    if(sebytes = sendto(sockfd, send_buf, sizeof(*send_buf), MSG_CONFIRM, (const struct sockaddr *) &client_addr,sizeof(client_addr)) < 0)
            {
               printf("sendto unsuccessful-error code %d: %s\n",errno,strerror(errno));
               
                goto SEND;
            }
            printf("Code send from server to client : %d\n",send_buf->code);
            printf("Data sent to client from server - seg num - %d\n",send_buf->sequence_num);

            //check for ack from client
    TRYACKAGAIN:     if((rebytes = recvfrom(sockfd, recv_buf, sizeof(*recv_buf), MSG_WAITALL, ( struct sockaddr *) &client_addr,&client_addr_len))< 0)
            {
                
                if(errno == 11) //if resource is unavailable
                {
                     printf("timeout occured ..resend data to server\n");
                    goto SEND;
                }
                else
                {
                    printf("receive from client unsuccessfull -mysendto\n");
                    exit(1);
                }
            }
           
            printf("ACK received\n");
            printf("Received ACK sequence num : %d\n",recv_buf->sequence_num);
            printf("Received ACK code : %s\n",UDP_ENUM_STR[recv_buf->code]);
            printf("current sequence count = %d\n",current_seq_count);
            if(current_seq_count == recv_buf->sequence_num)
            {
                current_seq_count++;
                printf("sequence count match\n");
            }
            else
            {
                printf("ACK sequence num does not match\n");
                printf("resending the same sequence number %d\n",current_seq_count);
                fseek(fp, bytesread, SEEK_SET);
                filesize -= bytesread;
            }
            
        }
    }
    else {
        printf("send_buf Null or recv_buf NUll\n");
        return;
    }

    free(recv_buf);
    free(send_buf);
}

