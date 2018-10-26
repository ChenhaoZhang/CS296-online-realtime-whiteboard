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
#include <errno.h>
using namespace std;

int main(int argc, char *argv[])
{
    pid_t pid = fork();

//.  ip serverport clientconnect port
    if(argc != 4)
    {
        cerr << "Usage: port#" << endl;
        exit(0);
    }

    if (pid == 0)
    {
        // child process
        // server

    // if(argc != 2)
    // {
    //     cerr << "Usage: port#" << endl;
    //     exit(0);
    // }
    int opt = 1;
    int port = atoi(argv[3]);
    char msg[1025];
    int serverSd;


    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    fd_set readfds;
    int client_socket[30];
    int max_clients = 30;
    for (int i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
    if((serverSd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(0);
    }
    if( setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(0);   
    }  
    
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, sizeof(servAddr));
    if(bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }
    cout << "Waiting for clients to connect..." << endl;
    if(listen(serverSd, 3)<0){
        perror("listen");
        exit(0);
    };

    int addrlen = sizeof(servAddr);
    int max_sd;
    int sd;
    int activity;
    int new_socket;

    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    while(true){
//cout<<"in the loop"<<endl;
        FD_ZERO(&readfds); 
        FD_SET(serverSd, &readfds);   
        max_sd = serverSd;
//cout<<"here"<<endl;
        for(int i = 0; i < max_clients; i++){
            sd = client_socket[i];
            if(sd > 0){
                FD_SET(sd, &readfds);
            }
            if(sd > max_sd){
                max_sd = sd;
            }
        }
//cout<<"there"<<endl;
//cout<<"port#: "<<port<<endl;
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
//cout<<"stuck here"<<endl;
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        } 
//cout<<"what"<<endl;
        if (FD_ISSET(serverSd, &readfds))   
        {   
//cout<<"in here"<<endl;
            new_socket = accept(serverSd,  
                    (struct sockaddr *)&servAddr, (socklen_t*)&addrlen); 
            cout<<"New connection, socket fd is "<< new_socket<<endl;
            cout<<"ip is "<< inet_ntoa(servAddr.sin_addr)<<endl;
            cout<<"port is "<< ntohs(servAddr.sin_port)<<endl; 
            // printf("New connection , socket fd is %d , ip is : %s , port : %d  
            //       \n" , new_socket , inet_ntoa(servAddr.sin_addr) , ntohs 
            //       (servAddr.sin_port));   
            
            if(send(new_socket, msg, strlen(msg), 0) != strlen(msg))   
            {   
                perror("send");   
            }                      
            //add new socket to array of sockets  
            for (int i = 0; i < max_clients; i++)   
            {   
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                    cout<<"Adding to list of sockets as "<< i <<endl;    
                    break;   
                }   
            }   
        } 
//cout<<"wtf"<<endl;
        int valread;
        for (int i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];        
            if (FD_ISSET(sd , &readfds))   
            {
                if ((valread = read(sd , msg, 1024)) == 0)   
                { 
                    getpeername(sd , (struct sockaddr*)&servAddr , 
                        (socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" ,  
                          inet_ntoa(servAddr.sin_addr) , ntohs(servAddr.sin_port));   
                         
                    close(sd);   
                    client_socket[i] = 0;   
                }    
                else 
                {   
                    msg[valread] = '\0';   
                    // send(sd , msg , strlen(msg) , 0 );   
                    cout << "Client: " << msg << endl;
                    cout << ">";
                }   
            }   
        }   
    }
    close(new_socket);
    close(serverSd);
    return 0;



    }
    else if (pid > 0)
    {
        // parent process
        // client
sleep(10);

    //     if(argc != 3)
    // {
    //     cerr << "Usage: ip_address port" << endl; exit(0);
    // }
    char *serverIp = argv[1]; int port = atoi(argv[2]); 
    char msg[1500]; 
    struct hostent* host = gethostbyname(serverIp); 
    sockaddr_in sendSockAddr;   
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr)); 
    sendSockAddr.sin_family = AF_INET; 
    sendSockAddr.sin_addr.s_addr = 
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    int status = connect(clientSd,
                         (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl;
        exit(1);
    }
    cout << "Connected to the server!" << endl;
    int bytesRead, bytesWritten = 0;
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    while(1)
    {
        cout << ">";
        string data;
        getline(cin, data);
        memset(&msg, 0, sizeof(msg));
        strcpy(msg, data.c_str());
        if(data == "exit")
        {
            send(clientSd, (char*)&msg, strlen(msg), 0);
            break;
        }
        bytesWritten += send(clientSd, (char*)&msg, strlen(msg), 0);
        //cout << "Awaiting server response..." << endl;
        //memset(&msg, 0, sizeof(msg));
        //bytesRead += recv(clientSd, (char*)&msg, sizeof(msg), 0);
        // if(!strcmp(msg, "exit"))
        // {
        //     cout << "Server has quit the session" << endl;
        //     break;
        // }
        // cout << "Server: " << msg << endl;
    }
    gettimeofday(&end1, NULL);
    close(clientSd);
    cout << "********Session********" << endl;
    cout << "Bytes written: " << bytesWritten << 
    " Bytes read: " << bytesRead << endl;
    cout << "Elapsed time: " << (end1.tv_sec- start1.tv_sec) 
      << " secs" << endl;
    cout << "Connection closed" << endl;
    return 0;

    }
    else
    {
        // fork failed
        cout<<"fork() failed!"<<endl;
        return 1;
    }

    return 0;
}


