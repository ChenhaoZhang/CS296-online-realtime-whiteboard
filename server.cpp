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
    char msg[1025];

    fd_set readfds;
    int client_socket[30];
    int max_clients = 30;
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
    int serverSd = socket(AF_INET, SOCK_STREAM, 0){
        perror("socket failed");
        exit(EIXT_FAILURE);
    }
    if( setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }  

    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
 

    
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, 
        sizeof(servAddr));
    cout << "Waiting for clients to connect..." << endl;
    if(listen(serverSd, 5)<0){
        perror("listen");
        exit(EXIT_FAULURE);
    };

    int addrlen = sizeof(servAddr);
    int max_sd;
    int sd;
    int activity;
    int new_socket;
    while(true){
        FD_ZERO(&readfds); 
        FD_SET(serverSd, &readfds);   
        max_sd = master_socket;
        for(int i = 0; i < max_clients; i++){
            sd = client_socket[i];
            if(sd > 0){
                FD_SET(sd, &readfds);
            }
            if(sd > max_sd){
                max_sd = sd;
            }
        }
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        } 

        if (FD_ISSET(serverSd, &readfds))   
        {   new_socket = accept(serverSd,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen) 
            cout<<"New connection, socket fd is "<< new_socket<<endl;
            cout<<"ip is "<< inet_ntoa(address.sin_addr);
            cout<<"port is "<< ntohs(address.sin_port); 
            printf("New connection , socket fd is %d , ip is : %s , port : %d  
                  \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
                  (address.sin_port));   
            
            if(send(new_socket, message, strlen(message), 0) != strlen(message))   
            {   
                perror("send");   
            }                      
            //add new socket to array of sockets  
            for (i = 0; i < max_clients; i++)   
            {   
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                    cout<<"Adding to list of sockets as "<< i ;    
                    break;   
                }   
            }   
        } 
        int valread;
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];        
            if (FD_ISSET(sd , &readfds))   
            {
                if ((valread = read(sd , msg, 1024)) == 0)   
                { 
                    // getpeername(sd , (struct sockaddr*)&address , \ 
                    //     (socklen_t*)&addrlen);   
                    // printf("Host disconnected , ip %s , port %d \n" ,  
                    //       inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                         
                    close(sd);   
                    client_socket[i] = 0;   
                }    
                else 
                {   
                    msg[valread] = '\0';   
                    send(sd , msg , strlen(msg) , 0 );   
                }   
            }   
        }   
    }
    //sockaddr_in newSockAddr;
    //socklen_t newSockAddrSize = sizeof(newSockAddr);
    //int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);


    //cout << "Connected with client!" << endl;
    //struct timeval start1, end1;
    //gettimeofday(&start1, NULL);
    //int bytesRead, bytesWritten = 0;
    // while(1)
    // {
    //     cout << "Awaiting client response..." << endl;
    //     memset(&msg, 0, sizeof(msg));
    //     bytesRead += recv(newSd, (char*)&msg, sizeof(msg), 0);
    //     cout << "Client: " << msg << endl;
    //     cout << ">";
    //     string data;
    //     getline(cin, data);
    //     memset(&msg, 0, sizeof(msg));
    //     strcpy(msg, data.c_str());
    //     bytesWritten += send(newSd, (char*)&msg, strlen(msg), 0);
    // }
    // gettimeofday(&end1, NULL);
    close(new_socket);
    close(serverSd);
    return 0;   
}
