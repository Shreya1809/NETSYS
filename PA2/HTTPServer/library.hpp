#ifndef LIBRARY_HPP_
#define LIBRARY_HPP_
#include <bits/stdc++.h> 
#include <vector>
#include <string>
using namespace std;
 
vector<string> splitStrings(string str, char dl);
void  RequestServiceHandler(int new_socket, string HTTP_req);
void send_response(http_response_t Response,int new_socket);
int FileExists(const char* path);
void timelimit(int sockfd);
#endif