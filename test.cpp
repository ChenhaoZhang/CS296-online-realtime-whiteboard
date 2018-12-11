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
#include <vector>
#include <thread>
#include <queue>
#include <deque>
#include <list>
#include <SDL2/SDL.h>
#include <sstream>
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

static deque<string> inmsgqueue;
static deque<string> outmsgqueue;
static int client_socket[30];
static int max_clients = 30;
static int outqueueindex = 0;
static int inqueueindex = 0;
//static int nodeFd = 0;
// front end part global
static SDL_Window * window;
static SDL_Renderer * renderer;
static SDL_Texture * texture;
static Uint32 * pixels;
static bool leftMouseButtonDown = false;
static bool quit = false;
static SDL_Event event;
static vector<tuple<int, int>> data_container;




void external(int argc, char *argv[]){
// for(auto& thread : threads){
//         thread.join();
//     }



	int opt = 1;
    int port = atoi(argv[2]);
    char msg[1025];
    int serverSd;
//cout<<"in here"<<endl;

    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    fd_set readfds;

    for (int i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }
    if((serverSd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(0);
    }
//cout<<"serversd"<<serverSd<<endl;
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
    int max_sd = 0;
    int sd;
    int activity;
    int new_socket;

    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);

//cout<<"in there"<<endl;
//connect to exist socket
    char *serverIp = argv[1];
    //int port = atoi(argv[2]);
    int ex_port = 0;
    //char msg[1500];
    struct hostent* host = gethostbyname(serverIp);
    sockaddr_in sendSockAddr;
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    //sendSockAddr.sin_port = htons(port);
    //int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    int clientSd = 0;
    //int status = connect(clientSd,(sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    int connectstatus = 0;
    // if(connectstatus < 0)
    // {
    //     cout<<"Error connecting to socket!"<<endl;
    //     exit(1);
    // }
    // cout << "Connected to the server!" << endl;

    for(int i = 3; i < argc; i++){
    	ex_port = atoi(argv[i]);
    	sendSockAddr.sin_port = htons(ex_port);
    	clientSd = socket(AF_INET, SOCK_STREAM, 0);
    	connectstatus = connect(clientSd, (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    	if(connectstatus < 0)
	    {
	        cout<<"Error connecting to socket!"<<endl;
	        exit(1);
	    }
	    for (int j = 0; j < max_clients; j++)
            {
                if( client_socket[j] == 0 )
                {
                    client_socket[j] = clientSd;//???????
//cout<<"clientsd: "<<clientSd<<endl;
                    cout<<"Client Adding to list of sockets as "<< j <<endl;
                    break;
                }
            }
    	}

  //   std::vector<std::thread> networkingthreads;
 	// networkingthreads.push_back(std::thread(networking, argc, argv));
 	// networkingthreads.push_back(std::thread(frontend));

 	// for(auto& thread : networkingthreads){
  //       thread.join();
  //   }




    while(true){
//cout<<"in the loop"<<endl;
        FD_ZERO(&readfds);
        FD_SET(serverSd, &readfds);
        //max_sd = serverSd;//revised here

		//FD_SET(nodeFd, &readfds);
		// cout<<"nodeFD: "<<nodeFd<<endl;
		// cout<<"setsuccess"<<endsl;
		//max_sd = nodeFd;

        if(serverSd > max_sd){
            max_sd = serverSd;
        }
//cout<<"here"<<endl;
        for(int i = 0; i < max_clients; i++){
            sd = client_socket[i];
            if(sd > 0){
                FD_SET(sd, &readfds);
//cout<<"setset"<<endl;
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
            printf("select error\n");
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

            // if(send(new_socket, msg, strlen(msg), 0) != strlen(msg))
            // {
            //     perror("send");
            // }

            //add new socket to array of sockets
            int newclientindex = 0;
            for (int i = 0; i < max_clients; i++)
            {
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
 //cout<<"newsocketsd"<<new_socket<<endl;
                    cout<<"Server Adding to list of sockets as "<< i <<endl;
                    newclientindex = i;
                    break;
                }
            }

            //snapshot
            int sendnode = 0;
            for(int j = 0; j < max_clients; j++){
            	if(client_socket[j] != 0){
            		sendnode = j;
  //cout<<"find sendnode"<<sendnode<<endl;
            		break;
            	}
            }
   //cout<<"serversd"<<serverSd<<endl;
   //cout<<"clientsocket"<<client_socket[sendnode]<<endl;
            if(serverSd == client_socket[sendnode]-1){
 //cout<<"in here"<<endl;
 				if(!outmsgqueue.empty()){
 //cout<<"in send"<<endl;
	            	 char msgtemp[1025];
	            	memset(&msgtemp, 0, sizeof(msgtemp));
	            	// strcpy(msgtemp, outmsgqueue.at(0).c_str());
	            	// send(client_socket[newclientindex], (char*)&msgtemp, strlen(msgtemp), 0);

//cout<<"inqueuesize"<<outmsgqueue.size()<<endl;
	        for(int i = 0; i < outmsgqueue.size(); i++){
	//cout<<"in loop"<<endl;
		    	memset(&msgtemp, 0, sizeof(msgtemp));
	        	strcpy(msgtemp, outmsgqueue.at(i).c_str());
	//cout<<"inmsgqueuesize: "<<inmsgqueue.size()<<endl;
	        	//cout<<"client: "<<msg<<endl;
	        	//inqueueindex++;
	//cout<<"int here"<<endl;
	        	send(client_socket[newclientindex], (char*)&msgtemp, strlen(msgtemp), 0);
		    }
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
                    // cout << "Client: " << msg << endl;
                    // cout << ">";
                    string data = msg;
                    // strcpy(data, msg);
                    inmsgqueue.push_back(data);
                }
            }
        }

        //check new msg
// cout<<outmsgqueue.size()<<endl;
//         for(int i = outqueueindex; i < outmsgqueue.size(); i++){
// cout<<"get data"<<endl;
//         	//may optimize here
//         	memset(&msg, 0, sizeof(msg));
//         	strcpy(msg, outmsgqueue.at(i).c_str());
// cout<<outmsgqueue.at(i)<<endl;
// cout<<msg<<endl;
//         	for(int j = 0; j < max_clients; j++){
// cout<<"in"<<endl;
//         		sd = client_socket[j];
//         		if (FD_ISSET(sd , &readfds)){
// cout<<"inin"<<endl;
// 	        		send(sd, (char*)&msg, strlen(msg), 0);
// cout<<"notstuck"<<endl;
// 	        	}
//         	}
//         	outqueueindex++;
//         }

    }
    close(new_socket);
    close(serverSd);
    //return 0;

}


// void internal(){
//         //input from itself
// cout<<outmsgqueue.size()<<endl;
// 		//close(nodeFd);
// 		//nodeFd = 0;
// char msg[1025];
// int sd;
// 	while(1){
// 		for(int i = outqueueindex; i < outmsgqueue.size(); i++){
// 	cout<<"get data"<<endl;
// 	        	//may optimize here
// 	        	memset(&msg, 0, sizeof(msg));
// 	        	strcpy(msg, outmsgqueue.at(i).c_str());
// 	cout<<outmsgqueue.at(i)<<endl;
// 	cout<<msg<<endl;
// 	        	for(int j = 0; j < max_clients; j++){
// 	cout<<"in"<<endl;
// 	        		sd = client_socket[j];
// 	        		if (sd > 0){
// 	cout<<"inin"<<endl;
// 		        		send(sd, (char*)&msg, strlen(msg), 0);
// 	cout<<"notstuck"<<endl;
// 		        	}
// 	        	}
// 	        	outqueueindex++;
// 	        }
// 	    }

// }



void networking(int argc, char *argv[]){
//cout<<"in here"<<endl;
// cout<<argc<<endl;
// cout<<argv[0]<<endl;
// cout<<argv[1]<<endl;
// cout<<argv[2]<<endl;


	// std::vector<std::thread> networkingthreads;
 // 	threads.push_back(std::thread(internal));
 // 	threads.push_back(std::thread(external, argc, argv));
 // 	for(auto& thread : networkingthreads){
 //        thread.join();
 //    }

    //std::thread t7 (internal);
    //std::thread t5 (internal);
    std::thread t6 (external, argc, argv);
    //t5.join();




char msg[1025];
int sd;
	while(1){
		for(int i = outqueueindex; i < outmsgqueue.size(); i++){
	//cout<<"get data"<<endl;
	        	//may optimize here
	        	memset(&msg, 0, sizeof(msg));
	        	strcpy(msg, outmsgqueue.at(i).c_str());
	//cout<<outmsgqueue.at(i)<<endl;
	//cout<<msg<<endl;
	        	for(int j = 0; j < max_clients; j++){
	//cout<<"in"<<endl;
	        		sd = client_socket[j];
	        		if (sd > 0){
	//cout<<"inin"<<endl;
		        		send(sd, (char*)&msg, strlen(msg), 0);
	//cout<<"notstuck"<<endl;
		        	}
	        	}
	        	outqueueindex++;
	        }
	    }



    t6.join();

}

void getdata() {

	while(true) {
    for (int i = inqueueindex; i < inmsgqueue.size(); i++) {
      string temp = inmsgqueue.at(i);
      string delimiter = ",";
      size_t pos = temp.find(delimiter);
      string temp_x = temp.substr(0, pos);
      string temp_y = temp.substr(pos + 1);
      stringstream x(temp_x);
      stringstream y(temp_y);
      int x_cor;
      int y_cor;
      x >> x_cor;
      y >> y_cor;
      pixels[y_cor * WINDOW_WIDTH + x_cor] = 0;
    }
	}
}

// void displaydata(){
//
// 	while (!quit)
//         {
//             SDL_UpdateTexture(texture, NULL, pixels, WINDOW_WIDTH * sizeof(Uint32));
//             SDL_Delay(1);
//             SDL_WaitEvent(&event);
//
//             switch (event.type)
//             {
//                 case SDL_QUIT:
//                     quit = true;
//                     break;
//                 case SDL_MOUSEBUTTONUP:
//                     if (event.button.button == SDL_BUTTON_LEFT)
//                         leftMouseButtonDown = false;
//                     break;
//                 case SDL_MOUSEBUTTONDOWN:
//                     if (event.button.button == SDL_BUTTON_LEFT)
//                         leftMouseButtonDown = true;
//                 case SDL_MOUSEMOTION:
//                     if (leftMouseButtonDown)
//                     {
//                         int mouseX = event.motion.x;
//                         int mouseY = event.motion.y;
//                         pixels[mouseY * WINDOW_WIDTH + mouseX] = 0;
//                         tuple<int, int> temp (mouseX, mouseY);
//                         data_container.push_back(temp);
//                         string x_cor = to_string(get<0>(data_container.back()));
//                         string y_cor = to_string(get<1>(data_container.back()));
//                         cout << x_cor + y_cor + "\n";
//                     }
//                     break;
//             }
//             //outfile.close();
//
//
//             SDL_RenderClear(renderer);
//             SDL_RenderCopy(renderer, texture, NULL, NULL);
//             SDL_RenderPresent(renderer);
//         }
//
// }



int main(int argc, char *argv[]){
  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("Board",
                                         SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  renderer = SDL_CreateRenderer(window, -1, 0);
  texture = SDL_CreateTexture(renderer,
                                            SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WINDOW_WIDTH, WINDOW_HEIGHT);
  pixels = new Uint32[WINDOW_WIDTH * WINDOW_HEIGHT];

  memset(pixels, 255, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(Uint32));
    std::thread t1 (networking, argc, argv);
    std::thread t2 (getdata);


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
                          string x_cor = to_string(mouseX);
                          string y_cor = to_string(mouseY);
                          string temp = x_cor + "," + y_cor;
                          outmsgqueue.push_back(temp);
                          cout << temp + "\n";
                      }
                      break;
              }
              // for (int i = inqueueindex; i < inmsgqueue.size(); i++) {
              //   string temp = inmsgqueue.at(i);
              //   string delimiter = ",";
              //   size_t pos = temp.find(delimiter);
              //   string temp_x = temp.substr(0, pos);
              //   string temp_y = temp.substr(pos + 1);
              //   stringstream x(temp_x);
              //   stringstream y(temp_y);
              //   int x_cor;
              //   int y_cor;
              //   x >> x_cor;
              //   y >> y_cor;
              //   pixels[y_cor * WINDOW_WIDTH + x_cor] = 0;
              // }


              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, texture, NULL, NULL);
              SDL_RenderPresent(renderer);
          }

    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);
    SDL_Quit();
    t2.join();
    t1.join();

    return 0;



}
