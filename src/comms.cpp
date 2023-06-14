#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <vector>

// For threads
#include <pthread.h>

// For sockets
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

// For socket client
#include <netdb.h>

#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
 
#include <errno.h> 
#include <arpa/inet.h> //close 

#include "comms.h"

using namespace std;

// Global variables:

// The message queue
extern pthread_mutex_t msgQueuMutex;
extern vector<string> g_messageQueue;

// Conditional variable to indicate message in queue
extern pthread_cond_t g_cv;
extern pthread_mutex_t g_cvLock;

// Status as exposed globally from Controller class
extern string g_latestStatus;


// Both threads needs to agree on port number, 
// and both threads are static, i.e. make it global
int g_portNum = 51717;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

Comms::Comms()
{
}

void Comms::initializeComms(void)
{
   pthread_t threadId;

   int result = pthread_create(&threadId, NULL, serverThread, NULL);
   if (result)
   {
      cout << "Server Thread could not be created, " << result << endl;
      exit(1);
   }
   
   result = pthread_create(&threadId, NULL, stepThread, NULL);
   if (result)
   {
      cout << "Step Thread could not be created, " << result << endl;
      exit(1);
   }
  
}

void* Comms::stepThread(void* threadId)
{
   cout << "CLIENT: Waiting before connecting." << endl;
   sleep(3);
   
   int masterSockfd, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;

   char buffer[256];
   
   masterSockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (masterSockfd < 0) 
   {
      error("ERROR opening socket");
   }

   server = gethostbyname("localhost");
   
   if (server == NULL) 
   {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, 
   (char *)&serv_addr.sin_addr.s_addr,
   server->h_length);
   serv_addr.sin_port = htons(g_portNum);

   if (connect(masterSockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
   {
      error("ERROR connecting");
   }

   while(true)
   {
      strcpy(buffer, "STEP");

      n = write(masterSockfd,buffer,strlen(buffer));
      
      if (n < 0) 
      {
         error("ERROR writing to socket");
      }

      sleep(12);
   }

   close(masterSockfd);

}

void Comms::handleMessage(int socketFd, char* buffer, int length)
{
   int n;
   
   // Check if the buffer contains broken messages
   // it is assumed that all messages are four characters.
   if ((length % 4) != 0)
   {
      // At least one broken message, discard the whole buffer
      return;
   }
   
   int numOfMessages = length / 4;
   
   string strBuffer = buffer;
   
   for (int i = 0; i < numOfMessages; i++)
   {
      string message = strBuffer.substr(i*4, 4);

      // We do a check for SREQ here and build the response immediatly
      if (message == "SREQ")
      {
         cout << "Poor design, sending status response in comms (and not controller)." << endl;

         n = write(socketFd, g_latestStatus.c_str(), g_latestStatus.length());
         if (n < 0) 
         {
            error("ERROR writing to socket");
         }
      }
      else
      {
         pthread_mutex_lock( &msgQueuMutex );
         g_messageQueue.push_back(message);
         pthread_mutex_unlock( &msgQueuMutex );
         pthread_cond_signal(&g_cv);
      }            
   }
}

void* Comms::serverThread(void* threadId)
{
   int masterSockfd;
   int newSockfd;
   socklen_t clilen;
   char buffer[1024];
   struct sockaddr_in serv_addr;
   struct sockaddr_in cli_addr;
   int n;
   
   int clientSocket[30]; 
   int max_clients = 30 ;
   int activity;
   int i;
   int valread;
   int sd; 
	int max_sd; 

   //initialise all clientSocket[] to 0 so not checked 
	for (i = 0; i < max_clients; i++) 
	{ 
		clientSocket[i] = 0; 
	} 
   
   //set of socket descriptors 
	fd_set readfds; 

   masterSockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (masterSockfd < 0) 
   {
      error("ERROR opening socket");
   }  
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   g_portNum = 51717;
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(g_portNum);
   
   while (bind(masterSockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
   {
      if (errno == EADDRINUSE)
      {
         cout << "Port " << g_portNum << " is already in use. ";
         g_portNum++;
         serv_addr.sin_port = htons(g_portNum);
         cout << "Trying " << g_portNum << " instead." << endl;
      }
      else
      {
         error("ERROR on binding");
      }
   }
      
   ofstream sockectConfigFile("socket_config.txt");
   cout << "Writing socket port to socket_config.txt" << endl;

   if (sockectConfigFile.is_open())
   {
      sockectConfigFile << g_portNum;
      sockectConfigFile.close();
   }
     
   listen(masterSockfd,5);
   clilen = sizeof(cli_addr);

   while (true)
   {
		//clear the socket set 
		FD_ZERO(&readfds); 
	
		//add master socket to set 
		FD_SET(masterSockfd, &readfds); 
		max_sd = masterSockfd; 
			
		//add child sockets to set 
		for ( i = 0 ; i < max_clients ; i++) 
		{ 
			//socket descriptor 
			sd = clientSocket[i]; 
				
			//if valid socket descriptor then add to read list 
			if(sd > 0) 
         {
            FD_SET( sd , &readfds); 
         }
				
			//highest file descriptor number, need it for the select function 
			if(sd > max_sd) 
         {
            max_sd = sd; 
         }
		} 
      
      //wait for an activity on one of the sockets , timeout is NULL , 
		//so wait indefinitely 
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
         cout << "SERVER: Error in select!" << endl;
         exit(1);
		} 

		//If something happened on the master socket , 
		//then its an incoming connection 
		if (FD_ISSET(masterSockfd, &readfds)) 
		{ 
   
         newSockfd = accept(masterSockfd, (struct sockaddr *) &cli_addr, &clilen);
         if (newSockfd < 0) 
         {
            error("ERROR on accept");
         }
   
			//inform user of socket number - used in send and receive commands 
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , newSockfd , inet_ntoa(cli_addr.sin_addr) , ntohs(cli_addr.sin_port)); 
		
         bzero(buffer,256);
         n = read(newSockfd,buffer,255);
         if (n < 0) 
         {
            error("ERROR reading from socket");
         }

         handleMessage(newSockfd, buffer, n);
         
			//add new socket to array of sockets 
			for (i = 0; i < max_clients; i++) 
			{ 
				//if position is empty 
				if( clientSocket[i] == 0 ) 
				{ 
					clientSocket[i] = newSockfd; 
					break; 
				} 
			} 
		} 
			
		//else its some IO operation on some other socket 
		for (i = 0; i < max_clients; i++) 
		{ 
			sd = clientSocket[i]; 
				
			if (FD_ISSET( sd , &readfds)) 
			{ 
				//Check if it was for closing , and also read the 
				//incoming message 
				if ((valread = read( sd , buffer, 1024)) == 0) 
				{ 
					//Somebody disconnected , get his details and print 
					getpeername(sd , (struct sockaddr*)&cli_addr , &clilen); 
					printf("Host disconnected , ip %s , port %d \n" , 
						inet_ntoa(cli_addr.sin_addr) , ntohs(cli_addr.sin_port)); 
						
					//Close the socket and mark as 0 in list for reuse 
					close( sd ); 
					clientSocket[i] = 0; 
				} 
					
				// Assess the message that came in 
				else
				{ 
               // Terminate receiver string after the number of bytes received.
               buffer[valread] = 0;

               handleMessage(sd, buffer, valread);
                             
				} 
			} 
		} 
   }
}

