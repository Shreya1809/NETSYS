
#include "commandhandler.hpp" 
received_packet_t filepart;
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

int check_configfile(string username,string password)
{
char buffer[100] = {0};
size_t rbytes = 0;
FILE *fp;
fp = fopen( "dfs.conf", "r"); 
    if (fp!=NULL)
    {
        if((rbytes = fread(buffer, 1, 1024, fp)) > 0)
        {
            //printf("%s\n",buffer);
            vector<string> line = splitStrings(buffer, '\n'); 
            vector<string> line1 = splitStrings(line[0], ' ');
            vector<string> line2 = splitStrings(line[1], ' ');
            if(username.compare(line1[0])==0) //||(username.compare(line2[0])==0))
            {
                //cout << "Username Authenticated - " << line1[0] << endl;
                if(password.compare(line1[1])==0)
                {
                    //cout << "Password Authenticated - " << line1[1] << endl;
                    return 0;
                }
                else return 1;
            }
            if(username.compare(line2[0])==0)
            {
                //cout << "Username Authenticated - " << line2[0] << endl;
                if(password.compare(line2[1])==0)
                {
                    //cout << "Password Authenticated - "<< line2[1] << endl;
                    return 0;
                }
                else return 1;
            }
            else return 1;

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
    fclose(fp);
}

int FileExists(const char *path)
{
    // Check for file existence
    if (access(path, F_OK) == -1)
        return 0;
    else
        return 1;
}
/*typedef struct
{
    int file_part_serial_no;
    int file_part_1;
    int file_part_2;
    int file_part_size[4];
    string file_part_data[4];
    //int file_part_size2;
    string file_part_data1;
    string file_part_data2;
    string command;
    string username;
    string password;
    string md5val;
    int server_num;
    string nameoffile;
}received_packet_t;*/
void parse_infostring(string infostring, int commandflag)
{
    if(commandflag == 0) //GET
    {
        vector<string> getcommand = splitStrings(infostring, ','); 
        filepart.command = getcommand[0];
        filepart.nameoffile = getcommand[1];
        filepart.server_num = stoi(getcommand[2]) + 1; 
        cout << "command : " << filepart.command << endl;
        cout << "name of file : " << filepart.nameoffile << endl;
        cout << "Server No : " << filepart.server_num << endl; 
    }
    if(commandflag == 1) //PUT
    {
        vector<string> element = splitStrings(infostring, ',');  
        filepart.server_num = stoi(element[3]);
        filepart.username = element[1];
        filepart.password = element[2];
        filepart.command = element[1];
        filepart.nameoffile = element[4];
        filepart.md5val = element[5];
        filepart.file_part_1 = stoi(element[6]);
        filepart.file_part_1_size = stoi(element[7]);
        filepart.file_part_2 = stoi(element[8]);
        filepart.file_part_2_size = stoi(element[9]);

        cout << "value of structure  packet elements received " << endl;
        cout << "username : " << filepart.username << endl;
        cout << "password : " << filepart.password << endl;
        cout << "command : " << filepart.command << endl;
        cout << "name of file : " << filepart.nameoffile << endl;
        cout << "Server No : " << filepart.server_num << endl;
        cout << "md5val : " << filepart.md5val << endl;
        cout << "1st file part no from client : " << filepart.file_part_1 << endl;
        cout << "2nd file part no from client : " << filepart.file_part_2 << endl;
        cout << "data in 1st file part no from client : " << filepart.file_part_data1 << endl;
        cout << "data in 2nd file part no from client : " << filepart.file_part_data2 << endl;
        cout << "size of 1st file part : " << filepart.file_part_1_size << endl;
        cout << "size of 2nd file part : " << filepart.file_part_2_size << endl;
    }
    
}