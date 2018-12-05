Objective:
To build a simple web proxy server that is capable of relaying HTTP requests from clients to the HTTP servers

Language:
The language used for the code is a mixture of c and c++. C++ string class has been used for parsing the string.

Execution instructions:
1. To check the main goal
a. Clean the server using the command "make clean"
b. Make the server using the command "make"
c. Then run the server uing the command with 2 arguments - port number and cache timeout "./proxy 8000 20
d. to check with browser,open in private mode make changes to the preference in network settings, add manual proxy with host ip 127.0.0.1 and port as 8000
e. the following websites can be used to test :
	1. netstech.org
	2. morse.colorado.edu
f.This proxy does not support https. It only supports http. 
g. To check in telnet type: telnet localhost 8000 , enter -"GET https://www.google.com HTTP/1.1" ,press double enter.

Error Handling
In case the method or version or url doesn't exist error pages are displayed. There are 4 error pages.
methoderror.html - only GET is supported
versionerror.html - only HTTP/1.1 and HTTP/1.0 supported
badrequest.html - for invalid urls
blocked.html - for blocked websites gievn in the blacklist.txt

Contents of folder
The PA3 folder contains  subfolder proxy which contains
1. library.cpp: It contains all the string parsing functions and string rearragement to send over to the client/browser/nc/telnet.
2. proxy.cpp: It contains all the basic web socket connection APIs to establish a connection and sends incoming command into a function
called errorhandler() followed by parse request and reconstruct request which is available in library.cpp
3. page_cache: contains all the  html cached files to send.
4. Makefile:To make the proxy
5. includes:All the #includes needed for socket programming which is common across different files

References:
1. https://www.geeksforgeeks.org/socket-programming-cc/
2. https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
3. www.stackoverflow.com
4. http://www.cplusplus.com/reference/unordered_map/unordered_map/find/
5. https://stackoverflow.com/questions/7627723/how-to-create-a-md5-hash-of-a-string-in-c
6. PA2 from my earlier submission.

This is my own work. Any resemblance to anything other than the refernces cites above is purely coincidental.
