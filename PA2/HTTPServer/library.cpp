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
//string postdata;
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
		//for GET and POST
		Sendbuf = Response.ver + " " + Response.status_code + " " + Response.status_mesg + "\r\nContent-Type: " + Response.content_type 
				+ "\r\nContent-Length: "+ Response.content_length + "\r\n\r\n" + Response.file_content;
		send(new_socket , Sendbuf.c_str() , Sendbuf.size(), 0 );
	}
	
}
void  RequestServiceHandler(int new_socket,string HTTP_req)
{
	//parsing the string to get components -method,url and version
	vector<string> res = splitStrings(HTTP_req, dl); 
	cout << "Method :"<< res[0] << endl; 
	if(res[0]!= "GET" && res[0]!= "POST") //error handling for method
	{
		cout << "Invalid method" << endl;
		exit(1);

	}
	cout << "URI :" << res[1] << endl; 
	Request.method = res[0];
	Request.URI = res[1];
	if(res[0]== "POST") //handling for POST -extra components in the request
	{
		vector<string> res_1 = splitStrings(res[2], '\\'); //parsing substring to get version for POST
		Request.version = res_1[0];
		vector<string> res_2 = splitStrings(res[4], '\\');//parsing substring to get connection and post data
		Request.connection = res_2[0];
		Request.postdata = res_2[4].erase(0,1);
		
		
	}
	else Request.version = res[2]; //version for GET method

	if(Request.version != "HTTP/1.1" && Request.version != "HTTP/1.0" ) //error handling for version
	{
		cout<<"invalid version"<< endl;
		exit(1);
	}
	if (Request.URI == "/") //default index page if no URL
	{
		Request.URI = "/index.html";
	}
	if((Request.method.compare("GET")) ==0 ||(Request.method.compare("POST"))== 0)// common steps for both methods
	{	
		
		path.clear();
		std::streampos begin = 0, end = 0, diff = 0;
		path = "root" + Request.URI; //appending root to the path as all files are in root folder
		cout << "path: " << path <<endl;
		//to check if the given file exits
		if (FileExists(path.c_str())== 1)
		{
			Sendbuf.clear();
			cout << "File exists"<< endl;
			Response.status_code = "200"; //filling structure
			Response.ver = Request.version;
			Response.status_mesg = "OK";
			if (myFile.is_open()) 
				myFile.close();
			myFile.open(path.c_str(),ios::out | ios::binary); //open file
			cout << "path.c_str(): " << path.c_str() <<endl;
			if (!myFile.is_open()) 
			{
        		cout << "Unable to open file"<< endl;
        		//exit(1); // terminate with error
    		}
			//cout << "File opened"<< endl;
			extension = path.substr(path.find_last_of(".") + 1);//find file extension for content type
    		//cout << "File type :" << extension <<endl;
			myFile.seekg (0, ios::beg);
			begin = myFile.tellg();
			myFile.seekg (0, ios::end);
			end = myFile.tellg();
			myFile.seekg (0, ios::beg);
			diff = (end - begin); //find file size for content length
			//cout << "Filesize: " << to_string(diff) << " bytes." << endl;
			ifstream myFile(path.c_str());
			std::stringstream buffer;
			buffer << myFile.rdbuf();//read file
			//cout << "File content :" << buffer.str() << endl;
			
			 //fill structure
			Response.content_type = extension;
			if (Request.method == "GET") //file content for GET
			{
				Response.file_content = buffer.str();
				Response.content_length =to_string(diff);
			}
			else  //POST
			{
			Response.file_content = "<html><body><pre><h1>" + Request.postdata + "</h1></pre>" + buffer.str(); //file content fot POST along eith POST header 
			//send(new_socket,buffer.str().c_str(),(end-begin) ,0);
			//TO DO: we need to adjust size for POST header?
			Response.content_length =to_string(Response.file_content.size()); 
			}
			send_response(2,Response,new_socket); //function to send data over socket
			myFile.close();
		}
		else 
		{
			//if file does not exit
			cout << "file does not exist at path " << Request.URI.c_str() << endl;
			ifstream myFile("root/error.html"); //send error.html file in response
			std::stringstream buffer;
			buffer << myFile.rdbuf();
			//filing structure for error condition according to the header style
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

}

