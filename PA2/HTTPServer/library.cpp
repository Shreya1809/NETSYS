#include "includes.h"
#include "library.hpp"
#include<fstream>

using namespace std; 

char dl = ' ';
int flag = 0;
http_request_t Request;
http_response_t Response;
string Sendbuf;
string path;
string extension;
ifstream myFile;


//struct http_response_t *Response_buf = malloc(sizeof( struct http_response_t));
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
	cout << "length of entered string including spaces :" << l << endl; 

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
int FileExists(const char* path)
{
    // Check for file existence
    if (access(path, F_OK) == -1)
        return 0;
    else
        return 1;
}
void send_response(int flag,http_response_t Response,int new_socket)
{

	
	 if(flag == 2)
	{
		//file exists
		Sendbuf = Response.ver + " " + Response.status_code + " " + Response.status_mesg + "\r\nContent-Type: " + Response.content_type 
				+ "\r\nContent-Length: "+ Response.content_length + "\r\n\r\n" + Response.file_content;
		//cout <<"Content length: "<< Response.content_length << endl;
		send(new_socket , Sendbuf.c_str() , Sendbuf.size(), 0 );
	}
	
}
void  RequestServiceHandler(int new_socket,string HTTP_req)
{
	vector<string> res = splitStrings(HTTP_req, dl); 
	//printf("In request service handler.................\n");
	cout << "Method :"<< res[0] << endl; 
	cout << "URI :" << res[1] << endl; 
	cout << "Version :" << res[2] << endl;  
	Request.method = res[0];
	Request.URI = res[1];
	Request.version = res[2];
	//printf("Before method.............\n");
	if (Request.URI == "/")
	{
		Request.URI = "/index.html";
	}
	if((Request.method.compare("GET")) == 0)
	{	
		
		path.clear();
		cout << "GET method" << endl;
		std::streampos begin = 0, end = 0, diff = 0;
		//cout << Request.URI.c_str()<< endl;
		path = "root" + Request.URI;
		cout << "path: " << path <<endl;
		if (FileExists(path.c_str())== 1)
		{
			Sendbuf.clear();
			cout << "File exists"<< endl;
			Response.status_code = "200";
			Response.ver = Request.version;
			Response.status_mesg = "OK";
			if (myFile.is_open()) 
				myFile.close();
			myFile.open(path.c_str(),ios::out | ios::binary);
			cout << "path.c_str(): " << path.c_str() <<endl;
			if (!myFile.is_open()) 
			{
        		cout << "Unable to open file"<< endl;
        		//exit(1); // terminate with error
    		}
			cout << "File opened"<< endl;
			extension = path.substr(path.find_last_of(".") + 1);
    		cout << "File type :" << extension <<endl;
			myFile.seekg (0, ios::beg);
			begin = myFile.tellg();
			myFile.seekg (0, ios::end);
			end = myFile.tellg();
			myFile.seekg (0, ios::beg);
			diff = (end - begin);
			cout << "Filesize: " << to_string(diff) << " bytes." << endl;
			ifstream myFile(path.c_str());
			std::stringstream buffer;
			buffer << myFile.rdbuf();
			//cout << "File content :" << buffer.str() << endl;
			
			Response.content_length =to_string(diff);
			Response.content_type = extension;
			Response.file_content = buffer.str();
			
			//send(new_socket,buffer.str().c_str(),(end-begin) ,0);
			
			send_response(2,Response,new_socket);
			myFile.close();
		}
		else 
		{
			cout << "file does not exist at path " << Request.URI.c_str() << endl;
			ifstream myFile("root/error.html");
			std::stringstream buffer;
			buffer << myFile.rdbuf();
			Response.ver = Request.version;
			Response.status_code = "500";
			Response.status_mesg = "Internal Server Error";
			Response.content_length = "93";
			Response.content_type = "html";
			Response.file_content = buffer.str();
			//cout << "error.html" << buffer.str() << endl;
			send_response(2,Response,new_socket);
		}
	}	
    else if((Request.method.compare("POST")) == 0)
	{	cout << "POST method" << endl;
		Response.status_code = "501";
		Response.ver = Request.version;
		Response.status_mesg = "INTERNAL SERVER ERROR";
		send_response(3,Response,new_socket);
	}
	
	else
	{	flag = 1;
		cout << "method not supported by this server" << endl;
		Response.status_code = "501";
		Response.ver = Request.version;
		Response.status_mesg = "INTERNAL SERVER ERROR";
		send_response(1,Response,new_socket);
	}
}

