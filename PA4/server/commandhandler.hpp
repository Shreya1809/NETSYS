#ifndef COMMANDHANDLER_HPP_
#define COMMANDHANDLER_HPP_
#include <bits/stdc++.h> 
#include <vector>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <sys/socket.h>

using namespace std;
typedef struct
{
    int file_part_serial_no;
    int file_part_1;
    int file_part_2_size;
    int file_part_2;
    int file_part_1_size;
    string file_part_data[4];
    //int file_part_size2;
    string file_part_data1;
    string file_part_data2;
    string command;
    string username;
    string password;
    string md5val;
    int server_num;
    string nameoffile;
}received_packet_t;

typedef struct
{
    
    int file_part_1;
    int file_part_2_size;
    int file_part_2;
    int file_part_1_size;
    string file_part_data[4];
    //int file_part_size2;
    string file_part_data1;
    string file_part_data2;
    string command;
    string username;
    string password;
    int server_num;
    string nameoffile;
    string partsrequested;
}send_packet_t;

typedef struct
{
    int filepart;
    string nameoffile;
    string filedata[4];
    string username;
    string password;
    int server_num;
    string command;
    string completefilename;
}list_packet_t;

typedef struct
{
    string username;
    string subfolder;
    int server_num;
}
mkdir_t;
//parse string using delimiter
vector<string> splitStrings(string str, char dl);
// parse config file and check
int check_configfile(const char* buffer);
int FileExists(const char *path);
void getfilehandler(string pathname,send_packet_t sendfile, int sock);
void parse_infostring(string infostring, int commandflag);
void listfilehandler(string pathname,list_packet_t listfile, int sock);
int CheckFileCompleteness(int new_socket,list_packet_t listfile);
#endif