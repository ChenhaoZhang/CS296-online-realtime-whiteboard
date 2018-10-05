#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
using namespace std;
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cerr << "Usage: port#" << endl;
        exit(0);
    }
    int port = atoi(argv[1]);
    char msg[1500];
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
 

    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, 
        sizeof(servAddr));
    cout << "Waiting for a client to connect..." << endl;
    listen(serverSd, 5);
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
    cout << "Connected with client!" << endl;
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    int bytesRead, bytesWritten = 0;
    while(1)
    {
        cout << "Awaiting client response..." << endl;
        memset(&msg, 0, sizeof(msg));
        bytesRead += recv(newSd, (char*)&msg, sizeof(msg), 0);
        cout << "Client: " << msg << endl;
        cout << ">";
        string data;
        getline(cin, data);
        memset(&msg, 0, sizeof(msg));
        strcpy(msg, data.c_str());
        bytesWritten += send(newSd, (char*)&msg, strlen(msg), 0);
    }
    gettimeofday(&end1, NULL);
    close(newSd);
    close(serverSd);
    return 0;   
}
