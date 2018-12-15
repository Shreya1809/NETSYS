
#include "library.hpp"
config_t config;
get_packet_t getfile;
string liststring;
int part[4]={0,0,0,0};
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

int parse_config_file(string configfile)
{
    if ((configfile.compare("dfc1.conf") == 0) || (configfile.compare("dfc2.conf")) == 0) 
    {
        cout << "Valid config file" <<endl;
    }
    else
    {
        cout << "New config file" <<endl;
        //exit(1);
    }
    FILE *fp;
    int i = 0 ,j = 0;
    char buffer[1024] = {0};
    size_t rbytes = 0;
    fp = fopen( configfile.c_str(), "r");
    if (fp!=NULL)
    {
        if((rbytes = fread(buffer, 1, 1024, fp)) > 0)
		{
            vector<string> line = splitStrings(buffer, '\n'); 
            vector<string> line1 = splitStrings(line[0], ' ');
            vector<string> line2 = splitStrings(line[1], ' ');
            vector<string> line3 = splitStrings(line[2], ' ');
            vector<string> line4 = splitStrings(line[3], ' ');
            vector<string> line5 = splitStrings(line[4], ' ');
            vector<string> line6 = splitStrings(line[5], ' ');
            strcpy(config.server[0] ,line1[1].c_str());
            strcpy(config.server[1] ,line2[1].c_str());
            strcpy(config.server[2] ,line3[1].c_str());
            strcpy(config.server[3] ,line4[1].c_str());
            strcpy(config.port[0] ,line1[3].c_str());
            strcpy(config.port[1] ,line2[3].c_str());
            strcpy(config.port[2] ,line3[3].c_str());
            strcpy(config.port[3] ,line4[3].c_str()); 
            config.ip = line1[2];
            config.username = line5[1];
            config.password = line6[1];
            cout << "Username :"<<config.username <<endl;
            cout << "Password :"<< config.password << endl;
        }
    }
    fclose(fp);
    return 0;
}

int FileExists(const char *path)
{
    // Check for file existence
    if (access(path, F_OK) == -1)
        return 0;
    else
        return 1;
}

// Calculate the MD5sum of every file and calculate md5sum%4 and return the remainder
unsigned char md5hash(string filename,size_t filesize, FILE *f)
{

  char *buf = NULL;
  unsigned char md5s[MD5_DIGEST_LENGTH] ={0}; //(char*)malloc(MD5_DIGEST_LENGTH); 

 
  buf = (char*)malloc(filesize);

  fread(buf,1, filesize, f);
  fseek(f,0,SEEK_SET);

  MD5((const unsigned char*)buf, filesize, md5s);
  printf("MD5 (%s) = ", filename.c_str());
  for (int i=0; i < MD5_DIGEST_LENGTH; i++)
  {
    printf("%x",  md5s[i]);

  }  
  printf("\n");
  printf("\nRemainder - %d\n  ",md5s[15]%4);
  free(buf);
  if(md5s[15]%4 == 0)
  {
    return '0';
  }
  else if(md5s[15]%4 == 1) 
  {
    return '1';
  }
  else if(md5s[15]%4 == 2) 
  {
    return '2';   
  }
  else if(md5s[15]%4 == 3) 
  {
    return '3';  
  }
  else
  {
    return '4';
  }
 
}
void ClientPutCommandHandler(file_packet_t filepart,int sock,int server_no,int socketfailflag)
{
    if (socketfailflag ==1)
    {
        //cout << "Socket does not exists for server "<< server_no+1 <<endl;
        printf("--------#STATUS : Server %d Down\n",server_no+1);
        //cout << "No operation for this particular server" << endl;
        return;   
    }
    char status[20] = {0};
    //cout << "inside put command handler for client" << endl;
    //cout << "server_no : " << server_no << endl;
    //cout << filepart->md5val << endl;
    if(filepart.md5val == '0')
    {
        if(server_no == 0)
        {
            filepart.server_num = 1;
            filepart.file_part_1 = 0;
            filepart.file_part_2 = 1;
            filepart.file_part_data1 = filepart.file_part_data[0];
            filepart.file_part_data2 = filepart.file_part_data[1];
            
        }
        if(server_no == 1)
        {
            filepart.server_num = 2;
            filepart.file_part_1 = 1;
            filepart.file_part_2 = 2;
            filepart.file_part_data1 = filepart.file_part_data[1];
            filepart.file_part_data2 = filepart.file_part_data[2];
            
        }
        if(server_no == 2)
        {
            filepart.server_num = 3;
            filepart.file_part_1 = 2;
            filepart.file_part_2 = 3;
            filepart.file_part_data1 = filepart.file_part_data[2];
            filepart.file_part_data2 = filepart.file_part_data[3];
            
        }
        if(server_no == 3)
        {
            filepart.server_num = 4;
            filepart.file_part_1 = 3;
            filepart.file_part_2 = 0;
            filepart.file_part_data1 = filepart.file_part_data[3];
            filepart.file_part_data2 = filepart.file_part_data[0];
            
        }
        
    }
    if(filepart.md5val == '1')
    {
        if(server_no == 0)
        {
            filepart.server_num = 1;
            filepart.file_part_1 = 3;
            filepart.file_part_2 = 0;
            filepart.file_part_data1 = filepart.file_part_data[3];
            filepart.file_part_data2 = filepart.file_part_data[0];
            
        }
        if(server_no == 1)
        {
            filepart.server_num = 2;
            filepart.file_part_1 = 0;
            filepart.file_part_2 = 1;
            filepart.file_part_data1 = filepart.file_part_data[0];
            filepart.file_part_data2 = filepart.file_part_data[1];
            
        }
        if(server_no == 2)
        {
            filepart.server_num = 3;
            filepart.file_part_1 = 1;
            filepart.file_part_2 = 2;
            filepart.file_part_data1 = filepart.file_part_data[1];
            filepart.file_part_data2 = filepart.file_part_data[2];
            
        }
        if(server_no == 3)
        {
            filepart.server_num = 4;
            filepart.file_part_1 = 2;
            filepart.file_part_2 = 3;
            filepart.file_part_data1 = filepart.file_part_data[2];
            filepart.file_part_data2 = filepart.file_part_data[3];
            
        }
        
    }
    if(filepart.md5val == '2')
    {
        if(server_no == 0)
        {
            filepart.server_num = 1;
            filepart.file_part_1 = 2;
            filepart.file_part_2 = 3;
            filepart.file_part_data1 = filepart.file_part_data[2];
            filepart.file_part_data2 = filepart.file_part_data[3];
            
        }
        if(server_no == 1)
        {
            filepart.server_num = 2;
            filepart.file_part_1 = 3;
            filepart.file_part_2 = 0;
            filepart.file_part_data1 = filepart.file_part_data[3];
            filepart.file_part_data2 = filepart.file_part_data[0];
            
        }
        if(server_no == 2)
        {
            filepart.server_num = 3;
            filepart.file_part_1 = 0;
            filepart.file_part_2 = 1;
            filepart.file_part_data1 = filepart.file_part_data[0];
            filepart.file_part_data2 = filepart.file_part_data[1];
            
        }
        if(server_no == 3)
        {
            filepart.server_num = 4;
            filepart.file_part_1 = 1;
            filepart.file_part_2 = 2;
            filepart.file_part_data1 = filepart.file_part_data[1];
            filepart.file_part_data2 = filepart.file_part_data[2];
            
        }
        
    }
    if(filepart.md5val == '3')
    {
        if(server_no == 0)
        {
            filepart.server_num = 1;
            filepart.file_part_1 = 1;
            filepart.file_part_2 = 2;
            filepart.file_part_data1 = filepart.file_part_data[1];
            filepart.file_part_data2 = filepart.file_part_data[2];
            
        }
        if(server_no == 1)
        {
            filepart.server_num = 2;
            filepart.file_part_1 = 2;
            filepart.file_part_2 = 3;
            filepart.file_part_data1 = filepart.file_part_data[2];
            filepart.file_part_data2 = filepart.file_part_data[3];
            
        }
        if(server_no == 2)
        {
            filepart.server_num = 3;
            filepart.file_part_1 = 3;
            filepart.file_part_2 = 0;
            filepart.file_part_data1 = filepart.file_part_data[3];
            filepart.file_part_data2 = filepart.file_part_data[0];
            
        }
        if(server_no == 3)
        {
            filepart.server_num = 4;
            filepart.file_part_1 = 0;
            filepart.file_part_2 = 1;
            filepart.file_part_data1 = filepart.file_part_data[0];
            filepart.file_part_data2 = filepart.file_part_data[1];
            
        }
        
    }
    
    int sendbytes = 0;
    string infostring = filepart.command+","+filepart.username + "," +filepart.password+","+std::to_string(filepart.server_num)+","+filepart.nameoffile+","
    +filepart.md5val+","+std::to_string(filepart.file_part_1)+","+std::to_string(filepart.file_part_size[filepart.file_part_1])+","+std::to_string(filepart.file_part_2)+","+
    std::to_string(filepart.file_part_size[filepart.file_part_2]);
    cout << "#FROM CLIENT TO SERVER ---> "<< infostring << endl;
    //send information string to designated server
    
    if((sendbytes = send(sock,infostring.c_str(), 1024, 0)) < 0)
    {
        //printf("Error: %s and code %d\n", strerror( errno ), errno);
        printf("--------#STATUS : Server %d Down\n",filepart.server_num);
    }
    //printf("bytes sent = %d\n", sendbytes);

    read(sock,status ,20);
    if(strlen(status)<3)
    {
        printf("Status : Server Number %d is broken\n",filepart.server_num);
    }
    else
    {
        printf("--------#STATUS : %s\n",status);
        sendbytes = 0;
        string encrypted_data1 = encryptdecrypt(filepart.file_part_data1,"5");
        if ((sendbytes = send(sock,encrypted_data1.c_str(),filepart.file_part_size[filepart.file_part_1],0)) < 0)
        {
            printf("Error: %s and code %d\n", strerror( errno ), errno);    
        }
        string encrypted_data2 = encryptdecrypt(filepart.file_part_data2,"5");
        if ((sendbytes = send(sock,encrypted_data2.c_str(),filepart.file_part_size[filepart.file_part_2],0)) < 0)
        {
            printf("Error: %s and code %d\n", strerror( errno ), errno);    
        }
    }
    
}

void ClientGetCommandHandler(string filename,file_packet_t filepart,int sock,int serverno,int  socketfailflag)
{

    if (socketfailflag ==1)
    {
        //cout << "Socket does not exists for server "<< serverno+1 <<endl;
        printf("Server %d Down\n",serverno+1);
        //cout << "No operation for this particular server" << endl;
        return;   
    }
    
    //int part1 = 0, part2 = 1,part3 = 2,part4 = 3;
    string part_req;
    char status[20] = {0};
    char getinfo[1024] = {0};
    char *p_getinfo = &getinfo[0];
    char clientmesg[15] = "Client says OK";
    int datasize1 = 0;
    int datasize2 = 0;
    filepart.server_num = serverno;
    filepart.command = "GET";
    filepart.nameoffile = filename;
    int serverdownflag = 0;
    for(int a = 0;a < 4;a++)
    {
        if(part[a]== 0)
        {
            part_req += std::to_string(a)+",";
        }
        else
        {
            cout << "Part Checking for Traffic Optimisation : " << a << " available in string" <<endl;
        }

    }
    string infostring = filepart.command+","+ filepart.nameoffile+"," + std::to_string(serverno) +"," + filepart.username + "- " + part_req;
    cout << infostring << endl;
    int sendbytes = 0;
    if ((sendbytes = send(sock,infostring.c_str(),infostring.length(),0)) < 0)
    {
        //printf("Error: %s and code %d\n", strerror( errno ), errno); 
        printf("--------#STATUS: Server %d Down\n",filepart.server_num);
        serverdownflag = filepart.server_num;
    }
    read(sock,status ,11);
    if(strlen(status)<3)
    {
        printf("Status : Server Number %d is broken\n",filepart.server_num);
    }
    else
    {
        printf("--------#STATUS : %s\n",status);
    }
    memset(p_getinfo,0,1024);
    size_t infoRead  = read(sock,p_getinfo,1024);
    if(infoRead == 0){
        cout<<"info read Error"<<endl;
    }
    cout<<"#FROM SERVER TO CLIENT ----> "<< getinfo << endl;;
    getinfo[infoRead+1] = 0;
    //cout << getinfo << endl;
    string getinfostring = string(getinfo,infoRead+1);
    vector<string> getelement = splitStrings(getinfostring, ',');
    getfile.username = getelement[1];
    getfile.server_num = stoi(getelement[2]);
    getfile.nameoffile = getelement[3];
    getfile.file_part_1 = stoi(getelement[4]);
    datasize1 = stoi(getelement[5]);
    datasize2 = stoi(getelement[7]);
    getfile.file_part_2 = stoi(getelement[6]);
    //char datapart1[datasize1] = {0};
    //char datapart2[datasize2] = {0};
    char servermesg[50]={0};
    char partsize[50]={0};
    int receiveflagmesg =0;
    int receiveflagsize =0;
    //cout 
    cout << "size of 1st file part : " << datasize1 << endl;
    cout << "size of 2nd file part : " << datasize2 << endl;
    if((datasize1 == 0) && (datasize2 == 0))
    {
        return;
    }


    sendbytes = 0;
    if ((sendbytes = send(sock,clientmesg,strlen(clientmesg)+1,0)) < 0)
    {
        //printf("Error: %s and code %d\n", strerror( errno ), errno); 
        printf("--------#STATUS : Server %d Down\n",getfile.server_num);
    }
    // get the no of parts from server 
    read(sock,servermesg,sizeof(servermesg));
    string server_mesg = string(servermesg);
    cout <<"Part available in this server for receiving : "<<servermesg <<endl;
    if(server_mesg.length() != 1)
    {
        vector<string> splitserver = splitStrings(server_mesg, '\n'); 
        if(splitserver[0].length() == 0)
        {
            receiveflagmesg == -1;
        }
        if((splitserver[0]).length()>2)
        {
            vector<string> splitservermesg = splitStrings(splitserver[0], ','); 
            getfile.file_part_1 = stoi(splitservermesg[0]);
            getfile.file_part_2 = stoi(splitservermesg[1]);
            part[getfile.file_part_1]++;
            part[getfile.file_part_2]++;
            cout << "part["<< getfile.file_part_1 << "] is "<< part[getfile.file_part_1] << endl;
            cout << "part["<< getfile.file_part_2 << "] is "<< part[getfile.file_part_2] << endl;
            receiveflagmesg = 1;
            


        }
        else
        {
            
            getfile.file_part_1 = stoi(splitserver[0]);
            part[getfile.file_part_1]++;
            cout << "part["<< getfile.file_part_1 << "] is "<< part[getfile.file_part_1] << endl;
            receiveflagmesg = 0;
        }
    }
    //get the size of parts
    read(sock,partsize,sizeof(partsize));
    string part_size = string(partsize);
    cout << "Part Sizes : " << part_size <<endl;
    //cout << "size of part sizes : " << part_size.length() << endl;
    if(part_size.length() > 1)
    {
        vector<string> splitsize = splitStrings(part_size, '\n'); 
        if(splitsize[0].length() == 0)
        {
            receiveflagsize == -1;
            cout << " receive flag is -1" <<endl;
        }
        //if((splitsize[0]).length()>2)
        //{
        size_t found = (splitsize[0]).find(",");
        if (found==std::string::npos)//single item
        {
            getfile.file_part_1_size = stoi(splitsize[0]);
            receiveflagsize = 0;
            
        }
        else
        {
            vector<string> splitsizestring = splitStrings(splitsize[0], ','); 
            getfile.file_part_1_size = stoi(splitsizestring[0]);
            getfile.file_part_2_size = stoi(splitsizestring[1]);
            receiveflagsize = 1;
            
        }
    }
    else return;
    //sending ok to server to go ahead with the transmission
    int sbytes = 0;
    if ((sbytes = send(sock,"OK",3,0)) < 0)
        {
            printf("Error: %s and code %d\n", strerror( errno ), errno);    
        }
    cout << "Receiving data from server ....." << endl;
    if((receiveflagmesg == 1) &&(receiveflagsize == 1))
    {
        int rbytes = 0;
        char datapart1[getfile.file_part_1_size] = {0};
        char datapart2[getfile.file_part_2_size] = {0};
        if ((rbytes =read(sock,datapart1,sizeof(datapart1))) < 0 )
        {
            printf("rbytes : %d\n",rbytes);

        }
        getfile.file_part_data[getfile.file_part_1] = string(datapart1,sizeof(datapart1));
        //else  cout << " data part : " << getfile.file_part_1<<  "\n data: " << datapart1 <<endl;
        if ((rbytes =read(sock,datapart2,sizeof(datapart2))) < 0 )
        {
            printf("rbytes : %d\n",rbytes);
            
        }
        getfile.file_part_data[getfile.file_part_2] = string(datapart2,sizeof(datapart2));
        //else cout << " data part : " << getfile.file_part_2<<  "\n data: " << datapart2 <<endl;
    }
    else if((receiveflagmesg == 0) &&(receiveflagsize == 0))
    {
        char datapart1[getfile.file_part_1_size] = {0};
        read(sock,datapart1,sizeof(datapart1));
        getfile.file_part_data[getfile.file_part_1] = string(datapart1,sizeof(datapart1));
        //cout << " data part : " << getfile.file_part_1<<  "\n data: " << datapart1 <<endl;
        //printf(" data part :%d \n data: %s\n",getfile.file_part_1,datapart1);
    }
    
}
vector<string> splitString(std::string s, const std::string &delimiter){

    // cout<<"To be split:"<<s<<endl;
    vector<string> splitStrings;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token  = s.substr(0, pos);
        // std::cout << "token:"<<token << std::endl;
        splitStrings.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    // std::cout << s << std::endl;

    return splitStrings;
}

string getFileStatusFromParts(string s){
    
    std::string delimiter = "\n";
    vector<string> splits = splitString(s,delimiter);
    unordered_map<string,vector<int>> filemap;
    for(auto str: splits){
        // cout<<"Split:"<<str<<endl;
        vector<string> fileParts;
        fileParts = splitString(str,",");
        // cout<<"I Split size:"<<fileParts.size()<<endl;
        // for(auto k: fileParts){
        //     cout<<k<<endl;
        // }
        if(fileParts.size() == 2 ){
            // cout<<"Filename:"<<fileParts[0]<<" Partno:"<<fileParts[1]<<endl;
            auto itr = filemap.find(fileParts[0]);
            if(itr == filemap.end()){
                //key does not exist
                //make a vector of size 4 for 4 parts and init with 0
                filemap.insert(std::pair<string,vector<int>>(fileParts[0],vector<int>(4,0)));
            }
            itr = filemap.find(fileParts[0]);
            int partno = stoi(fileParts[1]);
            // cout<<"Part no:"<<partno<<endl;
            if( partno < 4){
                itr->second[partno]++;
                // cout<<"Updated:";
                // for(auto x:itr->second){
                //     cout<<x;
                // }
                // cout<<endl;
            }
            
        }
    }

    string fileAvailability = "";
    for(auto x: filemap){
        // cout<<"Filename:"<<x.first<<" Parts Available:";
        fileAvailability += string(x.first);
        bool flag = false;
        for(auto i =0; i< x.second.size(); i++){
            
            if(x.second[i] == 0){
                flag = true;
                break;
            }
            // else{
            //     cout<<i;
            // }
        }
        if(flag){
            fileAvailability += " [incomplete]\n";
        }
        else{
            fileAvailability += " [complete]\n";
        }
        // cout<<endl;
    }

    // cout<<"FileStatus:\n"<<fileAvailability<<endl;
    return fileAvailability;
    
}


void ClientListCommandHandler(int sock,int serverno,int socketfailflag)
{
    if (socketfailflag ==1)
    {
        //cout << "Socket does not exists for server "<< serverno+1 <<endl;
        printf("--------#STATUS : Server %d Down\n",serverno+1);
        //cout << "No operation for this particular server" << endl;
        return;   
    }
    
    char status[15] = {0};
    char listitems[1024] = {0};
    int sendbytes = 0;
    int server = serverno +1;
    string listdata = "LIST," + config.username+ "," + config.password +","+ std::to_string(server);
    if ((sendbytes = send(sock,listdata.c_str(),listdata.length(),0)) < 0)
    {
        printf("--------#STATUS : Server %d Down\n",serverno);
    }

    read(sock,status,15);
    //cout << listitems << endl;
    if(strlen(status)<3)
    {
       printf("--------#STATUS : Server %d Down\n",serverno);
    }
    else printf("--------#STATUS : %s\n",status);
    //
    
    read(sock,listitems,sizeof(listitems));
    liststring +=listitems;
        
}
void ClientMkdirCommandHandler(string subfolder,int sock,int serverno,int  socketfailflag)
{
    if(socketfailflag == 1)
    {
        printf("--------#STATUS : Server %d Down\n",serverno+1);
        return;   
    }
    string mkbuf;
    char status[15]={0};
    mkbuf = "MKDIR," + subfolder + "," + config.username + "," + std::to_string(serverno);
    send(sock,mkbuf.c_str(), mkbuf.size(), 0);
    read(sock,status,15);
    if(strlen(status)<3)
    {
       printf("--------#STATUS : Server %d Down\n",serverno);
    }
    else printf("--------#STATUS : %s\n",status);
}

void encryptDecrypt(char inpString[]) 
{ 
    // Define XOR key 
    // Any character value will work 
    char xorKey = 'P'; 
  
    // calculate length of input string 
    int len = strlen(inpString); 
  
    // perform XOR operation of key 
    // with every caracter in string 
    for (int i = 0; i < len; i++) 
    { 
        inpString[i] = inpString[i] ^ xorKey; 
        printf("%c",inpString[i]); 
    } 
} 
string encryptdecrypt(string msg, string key)
{
    // Make sure the key is at least as long as the message
    std::string tmp(key);
    while (key.size() < msg.size())
        key += tmp;
    
    // And now for the encryption part
    for (string::size_type i = 0; i < msg.size(); ++i)
        msg[i] ^= key[i];
    return msg;
}


    
 
