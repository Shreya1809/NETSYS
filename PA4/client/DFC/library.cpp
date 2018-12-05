
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