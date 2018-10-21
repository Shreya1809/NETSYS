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
int pipelineflag = 0;

struct timeval timeout;
void timelimit(int sockfd)
{
timeout.tv_sec = 10.0;
timeout.tv_usec = 0.0;
    
	if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
	{
		printf("setsockopt for timeout-output failed");
	}
   
}

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

void send_response(http_response_t Response,int new_socket)
{
	//for GET and POST
	Sendbuf = Response.ver + " " + Response.status_code + " " + Response.status_mesg + "\r\nContent-Type: " + Response.content_type 
			+ "\r\nContent-Length: "+ Response.content_length + "\r\n\r\n" + Response.file_content;
	send(new_socket , Sendbuf.c_str() , Sendbuf.size(), 0 );
	
}
void logVec(vector<string> vec)
{
	for(int i = 0; i < vec.size(); i++)
		cout<<vec[i]<<endl;
}

void  RequestServiceHandler(int new_socket,string HTTP_req)
{
	//parsing the string to get components -method,url and version
	vector<string> res = splitStrings(HTTP_req, dl); 
	//cout << "vector size: "<<HTTP_req<<endl;
	logVec(res);
	cout << "Method :"<< res[0] << endl; 
	if(res[0]!= "GET" && res[0]!= "POST") //error handling for method
	{
		cout << "Invalid method" << endl;
		exit(1);

	}
	cout << "URI :" << res[1] << endl; 
	Request.method = res[0];
	Request.URI = res[1];
	cout<<"size of res :"<<res.size()<<endl;
	//cout<<"res[0] : "<<res[0];
	//cout<<"  res[1] :"<<res[1];
	//cout<<"  res[2] :"<<res[2];
	
	
	if(res[0]== "POST") //handling for POST -extra components in the request
	{
		vector<string> res_1 = splitStrings(res[2], '\\'); //parsing substring to get version for POST
		Request.version = res_1[0];
		//vector<string> res_2 = splitStrings(res[4], '\\');//parsing substring to get connection and post data
		//Request.connection = res_2[0];
		//Request.postdata = res_2[4].erase(0,1);
		cout<<"V:"<<Request.version<<endl;
		cout<<"size of res :"<<res.size()<<endl;
		cout<<"res[4] : "<<res[4];
		cout<<"  res[5] :"<<res[5];
		cout<<"  res[6] :"<<res[6];
		cout<<"  res[7] :"<<res[7];
		cout<<"  res[8] :"<<res[8];
		
		
		Request.postdata = res[8].erase(0,1);
		
	}
	else{
		 Request.version = res[2]; //version for GET method
		 cout<< "Version :"<<res[2]<<endl;
	}

	if(Request.version != "HTTP/1.1" && Request.version != "HTTP/1.0" )
	{
		cout<<"invalid version"<< endl;
			exit(1);
	}
	/*if(Request.version != "HTTP/1.1\n" && Request.version != "HTTP/1.0" ) //error handling for version
	{
		vector<string> res_3 = splitStrings(Request.version, '\\');
		if(res_3[0]!="HTTP/1.1" && res_3[0]!="HTTP/1.0")
		{
			cout<< "version :" << Request.version << endl;
			cout<<"invalid version"<< endl;
			exit(1);
		}
		Request.version = res_3[0];
		
	}*/
	if (Request.URI == "/") //default index page if no URL
	{
		Request.URI = "/index.html";
	}
	if((Request.method.compare("GET")) ==0 ||(Request.method.compare("POST"))== 0)// common steps for both methods
	{	
		
		path.clear();
		std::streampos begin = 0, end = 0, diff = 0;
		path = "root" + Request.URI; //appending root to the path as all files are in root folder
		//cout << "path: " << path <<endl;
		//to check if the given file exits
		if (FileExists(path.c_str())== 1)
		{
			Sendbuf.clear();
			//cout << "File exists"<< endl;
			Response.status_code = "200"; //filling structure
			Response.ver = Request.version;
			Response.status_mesg = "OK";
			if (myFile.is_open()) 
				myFile.close();
			myFile.open(path.c_str(),ios::out | ios::binary); //open file
			//cout << "path.c_str(): " << path.c_str() <<endl;
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
			myFile.close();						
			//cout << "File content :" << buffer.str() << endl;
			
			 //fill structure
			Response.content_type = extension;
			if (Request.method == "GET") //file content for GET
			{
				Response.file_content = buffer.str();
				Response.content_length =to_string(diff);
				
				//vector<string> res_4 = splitStrings(res[4], '\\');//parsing substring to get connection and post data
				//Request.connection = res_4[2];
				//cout<<"Request Connection type :"<<Request.connection<<endl;
				/*if(res.size()>3)
				{
					if((Request.connection.compare("Keepalive") == 0) || (Request.connection.compare("Keep-alive") ==0) )
					{	
						
						int i = 0;
						while(res[i].size() > 1)
						{
							//cout << res[i+4]<< " index is " <<i <<endl;
							//i=i+4;
							i++;
						}
						cout << "no of substrings " << i <<endl;
						//printf("inside keepalive\n");
						Response.content_length = to_string(diff) + "\r\nConnection: Keep-alive";
						//pipelineflag = 1;
					}
					else 
					{
						cout<<"connection is close"<<endl;
						Response.content_length = to_string(diff) + "\r\nConnection: Close";
					}
				}
				else
				{
					Response.file_content = buffer.str();
					Response.content_length =to_string(diff);
				}*/
			}
			else if(Request.method == "POST") //POST
			{
				cout<<"Request Postdata:" <<Request.postdata;
				cout<<"After request postdata"<<endl;
				Response.file_content = "<html><body><pre><h1>" + Request.postdata + "</h1></pre>" + buffer.str(); //file content fot POST along eith POST header 
				if((Request.connection.compare("Keepalive") == 0) || (Request.connection.compare("Keep-alive") ==0) )
				{
					cout<<"Server NOT closing connection"<<endl;
					Response.content_length = to_string(Response.file_content.size()) + "\r\nConnection: Keep-alive";
				}
				else
				{
					//TO DO: we need to adjust size for POST header?
					cout<<"Server closing connection"<<endl;
					Response.content_length =to_string(Response.file_content.size()) + "\r\nConnection: Close";
				}
			}
			else{
				cout<<"Invalid Method"<<endl;
				exit(1);
			}

			send_response(Response,new_socket); //function to send data over socket
			
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
			send_response(Response,new_socket);
		}
	}	

}

