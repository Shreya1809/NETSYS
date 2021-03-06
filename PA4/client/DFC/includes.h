#ifndef INCLUDES_H_
#define INCLUDES_H_

#include <bits/stdc++.h> 
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <sys/select.h> 
#include <stdlib.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h> 
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <memory.h>
#include <errno.h>
#include <signal.h>
#include <openssl/md5.h>

using namespace std;

//#define PORT 8888

typedef enum
{
    OK = 200,  //successful
    BAD_REQUEST = 400, //server did not understand the request, client error
    NOT_FOUND = 404, //requested page not found, client error
    REQUEST_TIMEOUT = 408, //took too long to respond, client error
    REQUEST_NOT_IMPLEMENTED = 501// server does not support the functionality, server error
}
Error_code;




typedef struct
{
    string scheme;
    string hostname;
    string portnum;
    string pathname;
    string query;
}url_parse_t;

#endif