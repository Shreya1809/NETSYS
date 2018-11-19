#ifndef LIBRARY_HPP_
#define LIBRARY_HPP_
#include <bits/stdc++.h> 
#include <vector>
#include <string>

using namespace std;

typedef struct pagecache{
	string filepath;
	long int time;

	friend ostream& operator<<(ostream &os, const struct pagecache &obj);

}PAGE_CACHE_T;

ostream& operator<<(ostream &os, const struct pagecache &obj);

template<class X, class Y>
void printCache(unordered_map<X,Y> &map);

void populateForbiddenMap(string filename);

//to parse string 
vector<string> splitStrings(string str, char dl);
//to handle requests
//void  RequestServiceHandler(int new_socket, string HTTP_req, int nfds);
//to send the http response
void send_response(http_response_t Response,int new_socket);
//to check if the file exists
//int FileExists(const char* path);
int error_handler(int socket_client, http_request_t Request, http_response_t Response);
int parse_request(http_request_t *Request, url_parse_t *parse);
int reconstruct_request(http_request_t Request, http_server_request_t ServerRequest, url_parse_t parse, http_response_t Response, int client_socket,int timeout);
int server_side_handler(int client_socket,string Send_rebuff,const char *IPbuffer,url_parse_t parse, int timeout);
#endif