#include "includes.h"
#include "library.hpp"
#include<fstream>
#include <stdbool.h> 
#include <unordered_map>
#include <list>
#include <algorithm>

using namespace std;

const char dl = ' ';

unordered_map<string,string> URLCache;
unordered_map<long,PAGE_CACHE_T> PageCache;
list<string> ForbiddenSiteList;

void populateForbiddenMap(string filename)
{
	//file
	//facebook.com 10.10.10.10
	//myname.com 12.23.2.3
	//hostname ip
	
	ifstream fs;
	fs.open(filename);
	if(fs.is_open() == false){
		printf("Blacklist file error\n");
		exit(0);
	}
	string hostname;

	while(true)
	{
		fs>>hostname;
		if(fs.eof()) break;
		ForbiddenSiteList.push_back(hostname);
	}

	ForbiddenSiteList.sort();

	cout<<"Forbidden Site list:"<<endl;
	for (auto x: ForbiddenSiteList)
	{
		cout<<x<<endl;
	}
}

bool checkIfSiteForbidden(string hostname)
{
	return binary_search(ForbiddenSiteList.begin(), ForbiddenSiteList.end(), hostname);
}


vector<string> splitStrings(string str, char dl) //REFERENCE - https://www.geeksforgeeks.org/split-string-substrings-using-delimiter/
{ 
	string word = ""; 

	// to count the number of split strings 
	int num = 0; 

	// adding delimiter character at the end 
	// of 'str' 
	str = str + dl; 

	// length of 'str' 
	int l = str.size();
	//cout << "length of entered string including spaces :" << l << endl; 
	//cout<< "string :" << str << endl;

	// traversing 'str' from left to right 
	vector<string> substr_list; 
	for (int i = 0; i < l; i++) { 

		// if str[i] is not equal to the delimiter 
		// character then accumulate it to 'word' 
		if (str[i] != dl && str[i] != '\r') 
			word = word + str[i]; 

		else { 

			// if 'word' is not an empty string, 
			// then add this 'word' to the array 
			// 'substr_list[]' 
			if ((int)word.size() != 0) 
				substr_list.push_back(word); 

			// reset 'word' 
			word = ""; 
		} 
	} 

	// return the splitted strings 
	return substr_list; 
} 
void send_response(http_response_t Response,int new_socket)
{
	string Sendbuf;
	//for GET and POST
	Sendbuf = Response.ver + " " + Response.status_code + " " + Response.status_mesg + "\r\nContent-Type: " + Response.content_type 
			+ "\r\nContent-Length: "+ Response.content_length + "\r\n\r\n" + Response.file_content;
	send(new_socket , Sendbuf.c_str() , Sendbuf.size(), 0 );
}
int error_handler(int socket_client, http_request_t Request, http_response_t Response)
{
	//cout << "in error handler , method : " << Request.method << endl; 
	//int i = 5;
	//return i;
	//int errflag = 0;
	ifstream myFile;
	int methoderror = 0, versionerror = 0, urlerror = 0;
	char host_name[1024];
	sscanf(Request.URL, "%*[^/]%*[/]%[^/]",host_name);
	printf("URL is %s\n", host_name);
	
	
	
	bool b = checkIfSiteForbidden(host_name); //check if in blacklist
	if (b)
	{
		ifstream myFile("blocked.html"); //send error.html file in response
		std::stringstream buffer;
		buffer << myFile.rdbuf();
		Response.ver = Request.version;
		Response.status_code = "403";
		Response.status_mesg = "FORBIDDEN ERROR";
		Response.content_type = "html";
		Response.file_content = buffer.str();
		Response.content_length = to_string(Response.file_content.size());
		cout << "FORBIDDEN ERROR "<< endl;
		send_response(Response,socket_client);
		return 1;
	}

	struct hostent* host_valid;
	host_valid = gethostbyname(host_name);
	if((strcmp(Request.method, "GET") != 0))
	{
		methoderror = 1;
	}
	if ((strcmp(Request.version, "HTTP/1.1" ) != 0) && (strcmp(Request.version, "HTTP/1.0" ) != 0))
	{
		versionerror = 1;
	}
	if (host_valid == NULL)
	{
		urlerror = 1;
	}
	
	if ((methoderror == 1) && (versionerror == 0)&& (urlerror == 0))
	{

		ifstream myFile("methoderror.html"); //send error.html file in response
		std::stringstream buffer;
		buffer << myFile.rdbuf();
		Response.ver = Request.version;
		Response.status_code = "501";
		Response.status_mesg = "NOT IMPLEMENTED";
		Response.content_type = "html";
		Response.file_content = buffer.str();
		Response.content_length = to_string(Response.file_content.size());
		cout << "METHOD ERROR "<< endl;
		send_response(Response,socket_client);
	    return 1;
	}
	
	if ((methoderror == 0) && (versionerror == 0)&& (urlerror == 1))
	{
		printf("gethostbyname error: %s\n", strerror(h_errno));
		ifstream myFile("badrequest.html"); //send error.html file in response
		std::stringstream buffer;
		buffer << myFile.rdbuf();
		Response.ver = Request.version;
		Response.status_code = "404";
		Response.status_mesg = "NOT FOUND";
		Response.content_type = "html";
		Response.file_content = buffer.str();
		Response.content_length = to_string(Response.file_content.size());
		cout << "URL ERROR "<< endl;
		send_response(Response,socket_client);
		return 1;

	}
	
	if ((methoderror == 0) && (versionerror == 1)&& (urlerror == 0))
	{
		ifstream myFile("versionerror.html"); //send error.html file in response
		std::stringstream buffer;
		buffer << myFile.rdbuf();
		Response.ver = Request.version;
		Response.status_code = "505";
		Response.status_mesg = "HTTP VERSION NOT SUPPORTED";
		Response.content_type = "html";
		Response.file_content = buffer.str();
		Response.content_length = to_string(Response.file_content.size());
		cout << "VERSION ERROR "<< endl;
		send_response(Response,socket_client);
		return 1;
	}
	
	if ((methoderror == 0) && (versionerror == 0)&& (urlerror == 0))
	{
		cout<< "No Error. Proceeding to parsing..." << endl;
		return 0;
	}
	else 
	{
		ifstream myFile("error.html"); //send error.html file in response
		std::stringstream buffer;
		buffer << myFile.rdbuf();
		Response.ver = Request.version;
		Response.status_code = "500";
		Response.status_mesg = "INTERNAL SERVER ERROR";
		Response.content_type = "html";
		Response.file_content = buffer.str();
		Response.content_length = to_string(Response.file_content.size());
		cout << "MULTIPLE ERROR "<< endl;
		printf("Method error %d , version error %d , url error %d\n", methoderror,versionerror,urlerror);
		send_response(Response,socket_client);
		if((strcmp(Request.method, "") == 0))
		{
			printf("blank method\n");
			return 1;
		}
		if((strcmp(Request.method, "CONNECT") == 0))
		{
			printf("Invalid method CONNECT\n");
			return 1;
		}
		 
		
		
		
	}
	
}

int parse_request(http_request_t *Request, url_parse_t *parse)
{
	string path;
    string url;
    string host;
    int index0 = 0,index1 = 0, index2 = 0,index3 = 0;
	url = Request->URL;
	//url = url.erase(0,1);
	cout<< "modified url:" << url << endl;
    size_t found = url.find('?'); //if the url contains a query 
    if (found != string::npos) 
    {    
        index0 = found;
        vector<string> res = splitStrings(url, '?');
        parse->query = res[1];
        cout <<"url : "<< res[0] << endl;
        cout <<"query : "<< res[1] << endl;
    }
    
    found = url.find('/'); 
    found = url.find('/', found+1); 
	if (found != string::npos) index1 = found; 
    found = url.find('/', found+2); 
	if (found != string::npos) index2 = found; 
	else 
	{
		parse->pathname = "";
	}
    host = url.substr(index1+1,index2-index1-1);
    if (index2 != 0)
	{
		parse->pathname = url.substr(index2+1,index0-index2-1);
	}
    cout << "protocol : http " << endl;
    parse->scheme = "http";
    cout << "path :" << parse->pathname << endl;
    size_t colon = host.find(':');
    if (colon != string::npos) 
    {   
        index3 = colon;
        cout<< "port :" << host.substr(index3+1,host.size()-1) << endl;
        parse->portnum = host.substr(index3+1,host.size()-1);
        parse->hostname = url.substr(index1+1,index3);
        cout << "hostname :" << parse->hostname <<endl;
    }
    else
    {
        parse->hostname = host;
        cout<< "port: 80" <<endl;
        parse->portnum = "80";
        cout << "hostname :" << parse->hostname <<endl;
    }
	cout << "Parsing Complete. Proceeding to reconstructing HTTP request..." << endl;
	//cout << "parse pathname: "<<parse.pathname << endl;
	return 0;
}


int reconstruct_request(http_request_t Request, http_server_request_t ServerRequest, url_parse_t parse, http_response_t Response, int client_socket)
{
	struct hostent *host_entry; 
	char hostbuffer[256];
	const char *IPbuffer; 
	string Send_reqbuff;
	sscanf(Request.URL, "%*[^/]%*[/]%[^/]",hostbuffer);

	
	auto itr = URLCache.find(string(hostbuffer)); //check hostname-ip cache
	if(itr != URLCache.end()){
		printf("-------IP fetched from cache\n");
		IPbuffer = itr->second.c_str();
	}
	else{
		host_entry = gethostbyname(hostbuffer);
		IPbuffer = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
		URLCache[string(hostbuffer)] = string(IPbuffer); 
		printf("-------IP not found. IP added to cache\n");
	}
	ServerRequest.server_method = Request.method;
	ServerRequest.server_uri = parse.pathname;
	
	ServerRequest.server_version = Request.version;
	
	
	printf("Host IP: %s\n", IPbuffer); 
	//cout << "port : " << parse.portnum << endl;
	Send_reqbuff = ServerRequest.server_method + " /" + ServerRequest.server_uri + " " + ServerRequest.server_version + "\r\nHost: " + parse.hostname + "\r\nConnection: close\r\n\r\n";
	//Send_reqbuff ="GET / HTTP/1.1\r\nHost: neverssl.com\r\nConnection: close\r\n\r\n";
	//printf("Server Req: %s\n",Send_reqbuff);
	cout<< "Server Req constructed :" << Send_reqbuff << endl;
	server_side_handler(client_socket,Send_reqbuff,IPbuffer,parse);
}

int server_side_handler(int client_socket,string Send_reqbuff,const char *IPbuffer,url_parse_t parse)
{
	struct sockaddr_in address; 
    int sock = 0, valread, valsend; 
    struct sockaddr_in serv_addr;  
    char buffer[1024] = {0}; 
	char copybuffer[1024] = {0}; 
	char c;
	FILE *fp,*fp1;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
  //cout << "port : " << parse.portnum << endl;
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(stoi(parse.portnum)); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, IPbuffer , &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
	// std::string str("GET http://"+ string(IPbuffer)+"/ HTTP/1.0 \r\n\r\n");
	//std::string str("GET / HTTP/1.0 \r\n\r\n");
	
    valsend = send(sock , Send_reqbuff.c_str() , Send_reqbuff.size() , 0 );
	int headersize = 0 , flag = 0;
	size_t bytesread = 0; 
	string filename = "pages/page";
	string ext = ".html";
	/*fp = fopen( string(filename+to_string(client_socket)+ext).c_str() , "w" );
	if(fp == NULL)
	{
		printf("ERROR open FILE\n");
		exit(0);
	}*/
	char *ret = NULL;
	while(1)
	{
		bzero(buffer,1024);
    	valread = read(sock , buffer, 1024); 
		//else flag = 0;
		//printf("--valread: %d\n", valread);
		//if(valread == 0) break;
		//if(valread > 0 ){
		if (valread > 0) {
			bytesread = bytesread + valread;
			//printf("%s\n",buffer );
			/*if((ret = strcasestr(buffer,"<!DOCTYPE html>"))!= NULL)
			{
				headersize = ret - buffer;
				//memcpy(copybuffer,buffer,headersize);
				cout<<"the size of the header : " << ret - buffer << endl;
				flag = 1;
			}
			bytesread = bytesread + valread;
			if (1 ||flag == 1)
			{
				//printf("Writing to file/n");
				headersize = 0;
				fwrite(buffer+headersize , 1 , valread-headersize, fp );
				headersize = 0;
			}*/
			//send(client_socket,buffer,valread-1, 0);
			send(client_socket,buffer,valread, 0);
			//cout<<"bytesread: " << bytesread <<endl;
			//if(/*buffer[valread-2] == '\r' && */buffer[valread-1] == '\n')
			//	break;
		}
		else
			break;

	}
	close(sock);
	//fclose(fp);  
	cout<<"Total bytesread: " << bytesread << endl;
	//cout<<"The size of the header : " << headersize << endl;
	//printf("%s\n", copybuffer);
	//printf("bytes read = %d\n",valread);
  	//printf("%s\n",buffer ); 
	//send(client_socket,buffer,strlen(buffer), 0);
    return 0; 
}

ostream& operator<<(ostream &os, const PAGE_CACHE_T &obj){
		os<<"Filepath:"<<obj.filepath<<" Time:"<<obj.time;
		return os;
	}

template<class X, class Y>
void printCache(unordered_map<X,Y> &map)
{
	for(auto x: map)
	{
		cout<<"Key:"<<x.first<<" Val:"<<x.second<<endl;
	}

}