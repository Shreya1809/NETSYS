
#include "commandhandler.hpp" 
#include <sys/stat.h>
 #include <dirent.h>
received_packet_t filepart;
send_packet_t sendfile;
list_packet_t listfile;
mkdir_t mkdirfile;
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

int check_configfile(const char* buffer)
{
char readbuffer[100] = {0};
size_t rbytes = 0;
FILE *fp;
fp = fopen( "dfs.conf", "r"); 
    if (fp!=NULL)
    {
        
        if((rbytes = fread(readbuffer, 1, 1024, fp)) > 0)
        {
            fclose(fp);
            char *p = strstr(readbuffer,buffer);
            if(p)
            {
                return 0;
            }
            else 
            {
               cout << "invalid username/password" << endl;
               return 1;

            }
        }
        else
        {
            cout << "fread failed" << endl;
            return 1;
        }
    }
    else
    {
        cout << "fp is null" << endl;
        return 1;
    }
}

int FileExists(const char *path)
{
    // Check for file existence
    if (access(path, F_OK) == -1)
        return 0;//does not exist
    else
        return 1;//exist
}

void getfilehandler(string pathname,send_packet_t sendfile, int sock)
{
    FILE *fp3;
    int flag = 0;
    size_t partsize;
    char status[20];
    struct stat st;
    int sendflag[2] = {0,0};
    string filepartno;
    string servermesg;
    string partsizemesg;
    int caseflag = 0;
    for( int i = 0; i < 4; i++)
    {
        filepartno = pathname + std::to_string(i);
        cout <<"pathname for get :" << filepartno << endl;
        if((fp3 = fopen(filepartno.c_str(),"rb")) != NULL)// file exits
        {
            cout << "file exists" << endl; 
            stat(filepartno.c_str(), &st);
            partsize = st.st_size;
            char readbuffer[partsize];
            memset(readbuffer,0,partsize);
            if (flag == 0)
            {
                sendfile.file_part_1_size = partsize;
                sendfile.file_part_1 = i;
                fread(readbuffer,1,partsize,fp3);
                sendfile.file_part_data1 = string(readbuffer,partsize);
                sendfile.file_part_data[i]= string(readbuffer,partsize);
                flag = 1;
            }
            sendfile.file_part_2_size = partsize;
            sendfile.file_part_2 = i;
            fread(readbuffer,1,partsize,fp3);
            sendfile.file_part_data2 = string(readbuffer,partsize);
            sendfile.file_part_data[i]= string(readbuffer,partsize);
            fclose(fp3); 
        }
           
    }
    for(int c = 0;c<4;c++)
    {
        cout <<"send file data["<<c<<"] :" <<sendfile.file_part_data[c] << endl;
    }
    string getinfo  = sendfile.command+","+sendfile.username + "," +std::to_string(sendfile.server_num)+","+sendfile.nameoffile+","
    +std::to_string(sendfile.file_part_1)+","+std::to_string(sendfile.file_part_1_size)+","+std::to_string(sendfile.file_part_2)+","+
    std::to_string(sendfile.file_part_2_size);
    //cout << getinfo << endl;
    printf("Info:%s\n",getinfo.c_str());
    int sendbytes = 0;
    uint8_t retryCount = 2;
    while (retryCount &&  (sendbytes = send(sock, getinfo.c_str(), getinfo.size(), 0)) < 0)
    {
        if(sendbytes != getinfo.size()){
            printf("Could not send all bytes. Error: %s and code %d\n", strerror( errno ), errno); 
        }
        else{
            printf("Error: %s and code %d\n", strerror( errno ), errno); 
            retryCount--;   
        }
    }
    cout << "INFO STRING SIZE: "<<getinfo.size() << " Bytes sent: "<<sendbytes<<endl;
    size_t readStatus = read(sock, status ,20);
    if(strlen(status)<3)
    {
        printf("Status : Client is broken\n");
    }
    else
    {
        printf("Status : %s\n",status);
        memset(status,0,20);
       
        size_t found = (sendfile.partsrequested).find(std::to_string(sendfile.file_part_1));
        if (found==std::string::npos)//not on list
        {
            //sendfile.getlist +=std::to_string(sendfile.file_part_1);
            //cout << "list for get is--> " << sendfile.getlist <<endl;
            

        }
        else //on list
        {
            cout << "already in the list" << endl;
            cout << "data part " << sendfile.file_part_1<< " is in the required list from client " << sendfile.partsrequested <<endl;
            sendflag[0]++;
            cout << "sendflag[0] : " <<sendflag[0] << endl;
        }
        found = (sendfile.partsrequested).find(std::to_string(sendfile.file_part_2));
        if (found==std::string::npos)//not on list
        {
            //sendfile.getlist +=std::to_string(sendfile.file_part_2);
            
        }
        else //on list
        {
            //do nothing
            cout << "already in th list" << endl;
            cout << "data part " <<sendfile.file_part_2<< " is in the required list from client " << sendfile.partsrequested <<endl;
            sendflag[1]++;
            cout << "sendflag[1] : " <<sendflag[1] << endl;
        }
        sendbytes = 0;
        //cout << "before cases "<< endl;
        if((sendflag[0] == 1) && (sendflag[1] == 1))
        {
            cout <<"case 1 " <<endl;
            cout << "sendflag[0] : " <<sendflag[0] << endl;
            cout << "sendflag[1] : " <<sendflag[1] << endl;
            partsizemesg = std::to_string(sendfile.file_part_1_size) + "," + std::to_string(sendfile.file_part_2_size)+"\n";
            servermesg = std::to_string(sendfile.file_part_1) + "," + std::to_string(sendfile.file_part_2) +"\n";
            caseflag = 1;
        }
        else if((sendflag[0]==1) && (sendflag[1]==0))
        {
            cout <<"case 2 " <<endl;
            cout << "sendflag[0] : " <<sendflag[0] << endl;
            cout << "sendflag[1] : " <<sendflag[1] << endl;
            partsizemesg = std::to_string(sendfile.file_part_1_size) + "\n";
            servermesg = std::to_string(sendfile.file_part_1) + "\n" ;
            caseflag = 2;   
        }
        else if((sendflag[0]!=1) && (sendflag[1]==1))
        {
            cout <<"case 3 " <<endl;
            cout << "sendflag[0] : " <<sendflag[0] << endl;
            cout << "sendflag[1] : " <<sendflag[1] << endl;
            partsizemesg = std::to_string(sendfile.file_part_2_size) + "\n";
            servermesg = std::to_string(sendfile.file_part_2) + "\n" ; 
            caseflag = 3; 
        }
        else if((sendflag[0]!=1) && (sendflag[1]!=1))
        {
            cout <<"case 4 " <<endl;
            cout << "sendflag[0] : " <<sendflag[0] << endl;
            cout << "sendflag[1] : " <<sendflag[1] << endl;
            servermesg = "\n" ;
            partsizemesg = "\n";
            caseflag = -1;   
        }
        else 
        {
            cout <<"case weird" <<endl;
            cout << "sendflag[0] : " <<sendflag[0] << endl;
            cout << "sendflag[1] : " <<sendflag[1] << endl;
            
        }
        //sending part number vis server mesg
        cout << "servermesg for parts : "<< servermesg<< endl;
        if ((sendbytes = send(sock,servermesg.c_str(),servermesg.size(),0)) < 0)
        {
            printf("Error: %s and code %d\n", strerror( errno ), errno);    
        }
        //sending the part sizes in order
        cout << "partsizemesg for parts : "<< partsizemesg << endl;
        if ((sendbytes = send(sock,partsizemesg.c_str(),partsizemesg.size(),0)) < 0)
        {
            printf("Error: %s and code %d\n", strerror( errno ), errno);    
        }
        sendbytes = 0;
        // client says go ahead
        char ok[3];
        int rbytes = 0;
        if ((rbytes =read(sock,ok,sizeof(ok))) < 0 )
        {
            printf("rbytes : %d\n",rbytes);

        }
        cout << "sending data to client .... " << endl;
        struct timeval timeout = {0,5000};
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout,sizeof(struct timeval));
        int sendpartno = 0;
        if(caseflag == 1)
        {

            //sending part 1
            size_t filesize = sendfile.file_part_1_size;
            const char *fileData = sendfile.file_part_data[sendfile.file_part_1].c_str();
            uint32_t retry = 0;
            while(filesize){
                size_t n = filesize > 1024? 1024 : filesize;
                sendbytes = send(sock,fileData,n,0);
                if(sendbytes <0){
                    printf("Error: %s and code %d\n", strerror( errno ), errno); 
                    if(retry < 10){
                        retry++; 
                    }
                    else{
                        printf("Retry overflow. Returninng.\n");
                        return; 
                    }  
                }
                else{
                    filesize = filesize - sendbytes;
                    fileData += sendbytes;
                }
            }
            printf("File bytes remainig:%d/%d\n",filesize,sendfile.file_part_1_size);
            //sending part 2
            sendbytes = 0;
            filesize = sendfile.file_part_2_size;
            fileData = sendfile.file_part_data[sendfile.file_part_2].c_str();
            retry = 0;
            while(filesize){
                size_t n = filesize > 1024? 1024 : filesize;
                sendbytes = send(sock,fileData,n,0);
                if(sendbytes <0){
                    printf("Error: %s and code %d\n", strerror( errno ), errno); 
                    if(retry < 10){
                        retry++; 
                    }
                    else{
                        printf("Retry overflow. Returninng.\n");
                        return; 
                    }  
                }
                else{
                    filesize = filesize - sendbytes;
                    fileData += sendbytes;
                }
            }
            printf("File bytes remainig:%d/%d\n",filesize,sendfile.file_part_2_size);
            /*if ((sendbytes = send(sock,sendfile.file_part_data[sendfile.file_part_1].c_str(),sendfile.file_part_1_size,0)) < 0)
            {
                printf("Error: %s and code %d\n", strerror( errno ), errno);    
            }
            sendbytes = 0;
            //cout << "Sending to client part :" << sendfile.file_part_2 << "\n data :" << sendfile.file_part_data[sendfile.file_part_2] << endl;
            if ((sendbytes = send(sock,sendfile.file_part_data[sendfile.file_part_2].c_str(),sendfile.file_part_2_size,0)) < 0)
            {
                printf("Error: %s and code %d\n", strerror( errno ), errno);    
            }*/
            
        }
        sendbytes = 0;
        if(caseflag == 2)
        {
            /*if ((sendbytes = send(sock,sendfile.file_part_data[sendfile.file_part_1].c_str(),sendfile.file_part_1_size,0)) < 0)
            {
                printf("Error: %s and code %d\n", strerror( errno ), errno);    
            }*/  
            int sendbytes = 0;
            size_t filesize = sendfile.file_part_1_size;
            const char *fileData = sendfile.file_part_data[sendfile.file_part_1].c_str();
            uint32_t retry = 0;
            while(filesize){
                size_t n = filesize > 1024? 1024 : filesize;
                sendbytes = send(sock,fileData,n,0);
                if(sendbytes <0){
                    printf("Error: %s and code %d\n", strerror( errno ), errno); 
                    if(retry < 10){
                        retry++; 
                    }
                    else{
                        printf("Retry overflow. Returninng.\n");
                        return; 
                    }  
                }
                else{
                    filesize = filesize - sendbytes;
                    fileData += sendbytes;
                }
            } 
            printf("File bytes remainig:%d/%d\n",filesize,sendfile.file_part_1_size);
        }
        sendbytes = 0;
        if(caseflag == 3)
        {
            /*if ((sendbytes = send(sock,sendfile.file_part_data[sendfile.file_part_2].c_str(),sendfile.file_part_2_size,0)) < 0)
            {
                printf("Error: %s and code %d\n", strerror( errno ), errno);    
            }*/ 
            int sendbytes = 0;
            size_t filesize = sendfile.file_part_2_size;
            const char *fileData = sendfile.file_part_data[sendfile.file_part_2].c_str();
            uint32_t retry = 0;
            while(filesize){
                size_t n = filesize > 1024? 1024 : filesize;
                sendbytes = send(sock,fileData,n,0);
                if(sendbytes <0){
                    printf("Error: %s and code %d\n", strerror( errno ), errno); 
                    if(retry < 10){
                        retry++; 
                    }
                    else{
                        printf("Retry overflow. Returninng.\n");
                        return; 
                    }  
                }
                else{
                    filesize = filesize - sendbytes;
                    fileData += sendbytes;
                }
            } 
            printf("File bytes remainig:%d/%d\n",filesize,sendfile.file_part_2_size);  
        }
       
    }

    
}

void parse_infostring(string infostring, int commandflag)
{
    if(commandflag == 0) //GET
    {
        vector<string> splitgetcomm = splitStrings(infostring, '-');
        vector<string> getcommand = splitStrings(splitgetcomm[0], ',');
        sendfile.command = getcommand[0];
        sendfile.nameoffile = getcommand[1];
        sendfile.subfolder = getcommand[2];
        sendfile.server_num = stoi(getcommand[3]) + 1; 
        sendfile.username = getcommand[4];
        cout << "command : " << sendfile.command << endl;
        cout << "name of file : " << sendfile.nameoffile << endl;
        cout << "Server No : " << sendfile.server_num << endl;
        cout << "Username : " << sendfile.username << endl; 
        //for(int i = 0;getparts[i].length > 0;i++)
        //{
        sendfile.partsrequested = splitgetcomm[1]; 
        //}
        cout << "Parts Requested : "<< sendfile.partsrequested << endl;

    }
    if(commandflag == 1) //PUT
    {
        vector<string> element = splitStrings(infostring, ',');  
        filepart.server_num = stoi(element[3]);
        filepart.username = element[1];
        filepart.password = element[2];
        filepart.command = element[0];
        filepart.nameoffile = element[4];
        filepart.subfolder = element[5];
        filepart.md5val = element[6];
        filepart.file_part_1 = stoi(element[7]);
        filepart.file_part_1_size = stoi(element[8]);
        filepart.file_part_2 = stoi(element[9]);
        filepart.file_part_2_size = stoi(element[10]);

        cout << "value of structure  packet elements received " << endl;
        cout << "username : " << filepart.username << endl;
        cout << "password : " << filepart.password << endl;
        cout << "command : " << filepart.command << endl;
        cout << "name of file : " << filepart.nameoffile << endl;
        cout << "subfolder : " << filepart.subfolder << endl;
        cout << "Server No : " << filepart.server_num << endl;
        cout << "md5val : " << filepart.md5val << endl;
        cout << "1st file part no from client : " << filepart.file_part_1 << endl;
        cout << "2nd file part no from client : " << filepart.file_part_2 << endl;
        //cout << "data in 1st file part no from client : " << filepart.file_part_data1 << endl;
        //cout << "data in 2nd file part no from client : " << filepart.file_part_data2 << endl;
        cout << "size of 1st file part : " << filepart.file_part_1_size << endl;
        cout << "size of 2nd file part : " << filepart.file_part_2_size << endl;
    }
    if(commandflag == 2)//list
    {
        vector<string> listelement = splitStrings(infostring, ','); 
        listfile.command = listelement[0];
        listfile.username = listelement[1];
        listfile.subfolder = listelement[2];
        listfile.password = listelement[3];
        listfile.server_num = stoi(listelement[4]);

    }
    if(commandflag == 3)
    {
        vector<string> mkdircom = splitStrings(infostring, ',');
        mkdirfile.subfolder = mkdircom[1];
        mkdirfile.username = mkdircom[2];
        mkdirfile.server_num = stoi(mkdircom[3]) + 1;
        cout << "server no : " << mkdirfile.server_num << endl;
        cout << "username : " << mkdirfile.username << endl;
        cout << "subfolder : " << mkdirfile.subfolder<< endl;
    }
    
}

void listfilehandler(string pathname,list_packet_t listfile, int sock)
{
    string filename[100];
    FILE *fp;
    size_t filesize = 0;
    struct stat st;;
    size_t val = 0;
    string sendlist;
    DIR *dir;
    struct dirent *ent;
    cout << "pathname : " << pathname << endl;
    
    if ((dir = opendir (pathname.c_str())) != NULL) 
    {
        
        int i = 0;
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            filename[i] = string(ent->d_name);
            i++;
            
            
        }
        
        closedir (dir);
        cout <<"i is " << i << endl;
        for(int j = 0;j < i;j++)
        {
            cout << "filename[j]" << filename[j] << endl;
            //cout << "filename[j] length" << filename[j].() << endl;
            //cout << "list item  " << j << endl;
            if(filename[j].size() > 2)
            {
                size_t found = filename[j].find(".");
                if (found!=std::string::npos)//found
                {
                    vector<string> name = splitStrings(filename[j], '.');
                    cout << "name :" << name[0] << ", number :" << name[1]  << endl; 
                    listfile.nameoffile = name[0];
                    listfile.filepart = stoi(name[1]);
                    listfile.completefilename = filename[j];
                    sendlist += listfile.nameoffile +","+ std::to_string(listfile.filepart)+ ","+"\n";
                }
                else //its a subfolder 
                {
                    listfile.nameoffile = filename[j];
                    sendlist += listfile.nameoffile + ",9,\n";
                }   
            }
            
        }
        //cout << "IN HERE "<< endl;
        cout << "Sendlist: \n" << sendlist << endl;
        cout << "sendlist size : " <<sendlist.size() << endl;
        if(sendlist.size() == 0)
        {
            char dummy[2] = " ";
            cout <<"No files in the path " <<pathname << endl;
            if ((val = send(sock,dummy,sizeof(dummy),0)) < 0)
            {
                printf("Error: %s and code %d\n", strerror( errno ), errno); 
                printf("val ; %d\n",val);   
            }
            //return;
        }
        else
        {
            if ((val = send(sock,sendlist.c_str(),sendlist.size(),0)) < 0)
            {
                printf("Error: %s and code %d\n", strerror( errno ), errno); 
                printf("val ; %d\n",val);   
            }
        }
    } 
    else
    {
    /* could not open directory */
        cout<<"Cannot Open Dir:"<<pathname <<endl;
        perror ("");
    
    }           
}
int checksubfolder(int new_socket,received_packet_t filepart)
{
    string path1;
    string subfolderlist;
    DIR *dir;
    struct dirent *ent;
    path1 = "DFS" + std::to_string(filepart.server_num) +"/"+filepart.username;
    cout << "path1: " <<path1 << endl;
    if ((dir = opendir (path1.c_str())) != NULL) 
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            subfolderlist += string(ent->d_name) + ",";
            printf("in while of check\n");
            
        }
        closedir (dir);
    }
    cout << "subfolderlist  : " << subfolderlist << endl;
    size_t found = subfolderlist.find(filepart.subfolder);
    if (found!=std::string::npos)//found
    {
        send(new_socket,"FOUND",10,0);
        cout <<  "Subfolder " << filepart.subfolder << " exists on server side " <<endl;
        return 1;
    }
    else {
        send(new_socket,"NOT FOUND",10,0);
        cout <<  "Subfolder " << filepart.subfolder << " does not exists on server side " <<endl;
        return 0;
    }
}

int listsubfolder(int new_socket,list_packet_t listfile)
{
    string path1;
    string subfolderlist;
    DIR *dir;
    struct dirent *ent;
    path1 = "DFS" + std::to_string(listfile.server_num) +"/"+listfile.username;
    cout << "path1: " <<path1 << endl;
    if ((dir = opendir (path1.c_str())) != NULL) 
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            subfolderlist += string(ent->d_name) + ",";
            //printf("in while of check\n");
            
        }
        closedir (dir);
    }
    cout << "subfolderlist  : " << subfolderlist << endl;
    size_t found = subfolderlist.find(listfile.subfolder);
    if (found!=std::string::npos)//found
    {
        send(new_socket,"FOUND",10,0);
        cout <<  "Subfolder " << listfile.subfolder << " exists on server side " <<endl;
        return 1;
    }
    else {
        send(new_socket,"NOT FOUND",10,0);
        cout <<  "Subfolder " << listfile.subfolder << " does not exists on server side " <<endl;
        return 0;
    }
}

int getsubfolder(int new_socket,send_packet_t sendfile)
{
    string path1;
    string subfolderlist;
    DIR *dir;
    struct dirent *ent;
    path1 = "DFS" + std::to_string(sendfile.server_num) +"/"+sendfile.username;
    cout << "path1: " <<path1 << endl;
    if ((dir = opendir (path1.c_str())) != NULL) 
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            subfolderlist += string(ent->d_name) + ",";
            //printf("in while of check\n");
            
        }
        closedir (dir);
    }
    cout << "subfolderlist  : " << subfolderlist << endl;
    size_t found = subfolderlist.find(sendfile.subfolder);
    if (found!=std::string::npos)//found
    {
        send(new_socket,"FOUND",10,0);
        cout <<  "Subfolder " << sendfile.subfolder << " exists on server side " <<endl;
        return 1;
    }
    else {
        send(new_socket,"NOT FOUND",10,0);
        cout <<  "Subfolder " << sendfile.subfolder << " does not exists on server side " <<endl;
        return 0;
    }
}

