#ifndef COMMANDHANDLER_HPP_
#define COMMANDHANDLER_HPP_
#include <bits/stdc++.h> 
#include <vector>
#include <string>
#include <unistd.h>
#include <dirent.h>

using namespace std;
//parse string using delimiter
vector<string> splitStrings(string str, char dl);
// parse config file and check
int check_configfile(string username,string password);
int FileExists(const char *path);

#endif