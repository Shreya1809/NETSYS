#ifndef LIBRARY_HPP_
#define LIBRARY_HPP_
#include <bits/stdc++.h> 
#include <vector>
#include <string>
using namespace std;
//to parse string 
vector<string> splitStrings(string str, char dl);
//to handle requests
void  RequestServiceHandler(int new_socket, string HTTP_req, int nfds);
//to send the http response
void send_response(http_response_t Response,int new_socket,int nfds);
//to check if the file exists
int FileExists(const char* path);
#endif