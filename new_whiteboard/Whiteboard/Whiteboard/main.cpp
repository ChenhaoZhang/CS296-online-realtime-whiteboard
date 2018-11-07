//
//  main.cpp
//  Whiteboard
//
//  Created by Felix Yang on 10/12/18.
//  Copyright © 2018 Felix Yang. All rights reserved.
//

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: felixyang
 *
 * Created on October 12, 2018, 2:14 PM
 */

#include <list>
#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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
#include <errno.h>
#include <tuple>
using namespace std;

#define WINDOW_HEIGHT 400
#define WINDOW_WIDTH 500

struct Line
{
    int x1;
    int y1;
    int x2;
    int y2;
};

int main(int argc, char ** argv)
{
    // variables
    
//    bool quit = false;
//    SDL_Event event;
//    int x1 = 0;
//    int y1 = 0;
//    int x2 = 0;
//    int y2 = 0;
//    bool drawing = false;
    std::list<Line> lines;
    std::string line_data;
    vector<tuple<int, int>> data_container;
    
    bool leftMouseButtonDown = false;
    bool quit = false;
    SDL_Event event;
    
    pid_t overall_pid = fork();
    // back end code
    
    if (overall_pid == 0) {
        // child, back end
       
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
            
            int bindStatus = ::bind(serverSd, (struct sockaddr*) &servAddr, sizeof(servAddr));
            
            if( bindStatus < 0 )
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
                long valread;
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
    /* end of back end code
    
-------------------------------------------------------------------------------------------------------------------
    
    start of front end code */
    
    else if (overall_pid > 0) {
        
        // drawing pixels
        
        SDL_Init(SDL_INIT_VIDEO);
        
        SDL_Window * window = SDL_CreateWindow("Board",
                                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
        
        SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
        SDL_Texture * texture = SDL_CreateTexture(renderer,
                                                  SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WINDOW_WIDTH, WINDOW_HEIGHT);
        Uint32 * pixels = new Uint32[WINDOW_WIDTH * WINDOW_HEIGHT];
        
        memset(pixels, 255, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(Uint32));
        
        while (!quit)
        {
            SDL_UpdateTexture(texture, NULL, pixels, WINDOW_WIDTH * sizeof(Uint32));
            SDL_Delay(1);
            SDL_WaitEvent(&event);
            
            switch (event.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT)
                        leftMouseButtonDown = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                        leftMouseButtonDown = true;
                case SDL_MOUSEMOTION:
                    if (leftMouseButtonDown)
                    {
                        int mouseX = event.motion.x;
                        int mouseY = event.motion.y;
                        pixels[mouseY * WINDOW_WIDTH + mouseX] = 0;
                        tuple<int, int> temp (mouseX, mouseY);
                        data_container.push_back(temp);
                    }
                    break;
            }
            
            
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
        
        // testing for the data
        //cout << "why";
        for (int i = 0; i < data_container.size(); i++) {
            string x_cor = to_string(get<0>(data_container[i]));
            string y_cor = to_string(get<1>(data_container[i]));
            cout << "x_cor is " + x_cor + ", y_cor is " + y_cor + "\n";
        }
        
        
        delete[] pixels;
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        
        SDL_DestroyWindow(window);
        SDL_Quit();
        
        return 0;
        
        // end of drawing pixels and below is the code for drawing lines
        
        
        // init SDL
        
        // drawing lines
        /*
        SDL_Init(SDL_INIT_VIDEO);
        SDL_Window * window = SDL_CreateWindow("SDL2 line drawing",
                                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
        SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
        
        // handle events
        
        while (!quit)
        {
            SDL_Delay(1);
            SDL_PollEvent(&event);
            
            switch (event.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    switch (event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        x1 = event.motion.x;
                        y1 = event.motion.y;
                        x2 = event.motion.x;
                        y2 = event.motion.y;
                        drawing = true;
                        break;
                }
                    break;
                case SDL_MOUSEBUTTONUP:
                    switch (event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        drawing = false;
                        Line line = { x1, y1, x2, y2 };
                        lines.push_back(line);
                        break;
                }
                    break;
                case SDL_MOUSEMOTION:
                    if (drawing)
                    {
                        x2 = event.motion.x;
                        y2 = event.motion.y;
                    }
                    break;
//                case SDL_USEREVENT:
//                    cout << "user event created!!" + to_string(user_count);
//                    user_count++;
//                    break;
            }
            
            // clear window
            
            SDL_SetRenderDrawColor(renderer, 242, 242, 242, 255);
            SDL_RenderClear(renderer);
            
            // draw stored lines
            
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
            
            for (std::list<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
            {
                Line line = *i;
                SDL_RenderDrawLine(renderer, line.x1, line.y1, line.x2, line.y2);
            }
            
            
            
            // draw current line
            
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            if (drawing)
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            
            // render window
            
            SDL_RenderPresent(renderer);
        }
        
        // save the different line data to data container
        //int count = 1;
        for (std::list<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i) {
            Line line = *i;
            //auto index = std::to_string(count);
            auto x1_cor = to_string(line.x1) + " ";
            auto y1_cor = to_string(line.y1) + " ";
            auto x2_cor = to_string(line.x2) + " ";
            auto y2_cor = to_string(line.y2) + " \n";
            line_data = "line: " + x1_cor + y1_cor + x2_cor + y2_cor;
            if (data_container.empty()) {
                data_container.push_back(line_data);
            } else {
                if (line_data.compare(data_container.back()) != 0) {
                    data_container.push_back(line_data);
                }
            }
        }
        
        // adding line data to the file
        outfile.open("test.txt");
        for (int i = 0; i < data_container.size(); i++) {
            
            outfile << data_container[i];
        }
        outfile.close();
        
        // cleanup SDL
        
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        
        return 0;
        */
        // end of drawing lines
    }
    return 0;
}


