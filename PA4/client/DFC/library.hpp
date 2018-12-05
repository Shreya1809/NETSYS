#ifndef LIBRARY_HPP_
#define LIBRARY_HPP_
#include <bits/stdc++.h> 
#include <vector>
#include <string>
#include <unistd.h>
#include <dirent.h>


using namespace std;

typedef struct
{
    string ip;
    char port[4][20];
    char server[4][20];
    string username;
    string password;
    
}config_t;

//to parse string 
vector<string> splitStrings(string str, char dl);
// tonparse config file
int parse_config_file(string configfile);
//to check if file exits
int FileExists(const char *path);

#endif