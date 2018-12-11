
#include "library.hpp"
config_t config;
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
        cout << "Invalid config file" <<endl;
        exit(1);
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
void ClientPutCommandHandler(file_packet_t filepart,int sock,int server_no)
{
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
    /*cout << "value of structure  packet elements to send to server " << endl;
    cout << "username : " << filepart.username << endl;
    cout << "password : " << filepart.password << endl;
    cout << "command : " << filepart.command << endl;
    cout << "name of file : " << filepart.nameoffile << endl;
    cout << "Server No : " << filepart.server_num << endl;
    cout << "md5val : " << filepart.md5val << endl;
    cout << "1st file part no to server : " << filepart.file_part_1 << endl;
    cout << "2nd file part no to server : " << filepart.file_part_2 << endl;
    
    cout << "size of 1st file part : " << filepart.file_part_size[filepart.file_part_1] << endl;
    cout << "size of 2nd file part : " << filepart.file_part_size[filepart.file_part_2] << endl;

    cout << "size of the structure to be sent " << sizeof(filepart) << endl;*/
    //cout << "data in 1st file part no to server : " << filepart.file_part_data1 << endl;
    //cout << "data in 2nd file part no to server : " << filepart.file_part_data2 << endl;
    //printf("data with c_str() : %s\n",filepart.file_part_data1.c_str());
    int sendbytes = 0;
    string infostring = filepart.command+","+filepart.username + "," +filepart.password+","+std::to_string(filepart.server_num)+","+filepart.nameoffile+","
    +filepart.md5val+","+std::to_string(filepart.file_part_1)+","+std::to_string(filepart.file_part_size[filepart.file_part_1])+","+std::to_string(filepart.file_part_2)+","+
    std::to_string(filepart.file_part_size[filepart.file_part_2]);
    cout << infostring << endl;
    //send information string to designated server
    
    if((sendbytes = send(sock,infostring.c_str(), 1024, 0)) < 0)
    {
        //printf("Error: %s and code %d\n", strerror( errno ), errno);
        printf("Server %d Down\n",filepart.server_num);
    }
    //printf("bytes sent = %d\n", sendbytes);

    read(sock,status ,20);
    if(strlen(status)<3)
    {
        printf("Status : Server Number %d is broken\n",filepart.server_num);
    }
    else
    {
        printf("Status : %s\n",status);
        sendbytes = 0;
        if ((sendbytes = send(sock,filepart.file_part_data1.c_str(),filepart.file_part_size[filepart.file_part_1],0)) < 0)
        {
            printf("Error: %s and code %d\n", strerror( errno ), errno);    
        }
        if ((sendbytes = send(sock,filepart.file_part_data2.c_str(),filepart.file_part_size[filepart.file_part_2],0)) < 0)
        {
            printf("Error: %s and code %d\n", strerror( errno ), errno);    
        }
    }
    
}

void ClientGetCommandHandler(string filename,file_packet_t filepart,int sock,int serverno)
{
    char status[20] = {0};
    filepart.server_num = serverno;
    filepart.command = "GET";
    filepart.nameoffile = filename;
    string infostring = filepart.command+","+ filepart.nameoffile+"," + std::to_string(serverno);
    int sendbytes = 0;
    if ((sendbytes = send(sock,infostring.c_str(),infostring.length(),0)) < 0)
    {
        //printf("Error: %s and code %d\n", strerror( errno ), errno); 
        printf("Server %d Down\n",filepart.server_num);
    }
    read(sock,status ,20);
    if(strlen(status)<3)
    {
        printf("Status : Server Number %d is broken\n",filepart.server_num);
    }
    else
    {
        printf("Status : %s\n",status);
    }

    
}