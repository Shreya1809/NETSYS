
#include "commandhandler.hpp" 
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
            }
            if(username.compare(line2[0])==0)
            {
                //cout << "Username Authenticated - " << line2[0] << endl;
                if(password.compare(line2[1])==0)
                {
                    //cout << "Password Authenticated - "<< line2[1] << endl;
                    return 0;
                }
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