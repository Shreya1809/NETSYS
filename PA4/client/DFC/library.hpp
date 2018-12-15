#ifndef LIBRARY_HPP_
#define LIBRARY_HPP_
#include <bits/stdc++.h> 
#include <vector>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <openssl/md5.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <unordered_map>


using namespace std;
//extern file_packet_t filepart;
typedef struct
{
    string ip;
    char port[4][20];
    char server[4][20];
    string username;
    string password;
    
}config_t;

typedef struct
{
    int file_part_serial_no;
    int file_part_1;
    int file_part_2;
    int file_part_size[4];
    string file_part_data[4];
    //int file_part_size2;
    string file_part_data1;
    string file_part_data2;
    string command;
    string username;
    string password;
    char md5val;
    int server_num;
    string nameoffile;
}file_packet_t;

typedef struct
{
    int file_part_serial_no;
    int file_part_1;
    int file_part_2;
    int file_part_1_size;
    int file_part_2_size;
    int file_part_size[4];
    string file_part_data[4];
    string command;
    string username;
    string password;
    char md5val;
    int server_num;
    string nameoffile;
}get_packet_t;


//to parse string 
vector<string> splitStrings(string str, char dl);
// tonparse config file
int parse_config_file(string configfile);
//to check if file exits
int FileExists(const char *path);
//finds the md5hash value and which pair to upload to which server
unsigned char md5hash(string filename,size_t filesize, FILE *f);
//put command handler
void ClientPutCommandHandler(file_packet_t filepart,int sock,int server_no,int socketfailflag);
//get command handler
void ClientGetCommandHandler(string filename,file_packet_t filepart,int sock,int serverno,int socketfailflag);
//list command handler
void ClientListCommandHandler(int sock,int serverno,int socketfailflag);
//mkdir command handler
void ClientMkdirCommandHandler(string subfolder,int sock,int serverno,int  socketfailflag);
vector<string> splitString(std::string s, const std::string &delimiter);
string getFileStatusFromParts(string s);
string encryptdecrypt(string msg, string key);
#endif