
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>

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



using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// Both threads needs to agree on port number, make it global
int g_portNum = 51717;

void* threadRoutine(void* threadId)
{
   cout << "CLIENT: Waiting before connecting." << endl;
   sleep(3);
   
   int masterSockfd, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;

	//set of socket descriptors 
	fd_set readfds; 

   char buffer[256];
   
   masterSockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (masterSockfd < 0) 
   {
      error("ERROR opening socket");
   }

   cout << "CLIENT: Socket opened." << endl;

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
   cout << "CLIENT: Socket connected." << endl;
   
   strcpy(buffer, "Greetings from the client thread");
   n = write(masterSockfd,buffer,strlen(buffer));
   
   if (n < 0) 
   {
      error("ERROR writing to socket");
   }

   cout << "CLIENT: Message sent." << endl;

   bzero(buffer,256);
   n = read(masterSockfd,buffer,255);
   
   if (n < 0) 
   {
      error("ERROR reading from socket");
   }

   cout << "CLIENT: Answer received: " << buffer << endl;
   
   
   long tid;
   tid = (long)threadId;
   
   for (int i = 0; i < 5; i++)
   {
      cout << "CLIENT: Tock!" << endl;
      
      strcpy(buffer, "Tock");

      cout << "CLIENT: Will send: '" << buffer << "', of " << strlen(buffer) << " lenght." << endl;

      n = write(masterSockfd,buffer,strlen(buffer));
      
      if (n < 0) 
      {
         error("ERROR writing to socket");
      }

      cout << "CLIENT: Message sent." << endl;

      bzero(buffer,256);
      n = read(masterSockfd,buffer,255);
      
      if (n < 0) 
      {
         error("ERROR reading from socket");
      }

      cout << "CLIENT: Answer received: " << buffer << endl;
      
      
      sleep(3);
   }

   close(masterSockfd);

   cout << "CLIENT: Socket closed." << endl;

}

int main(int argc, char *argv[])
{
   cout << "Welcome to socket server with multi threaded clients!" << endl;

   cout << "SERVER: Now trying to create sockets..." << endl;
   
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

   //a message 
	char message[40];

   strcpy(message, "ECHO Daemon v1.0 \r\n");

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
   cout << "SERVER: Socket openend." << endl;
   
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
      
   cout << "SERVER: Socket bound." << endl;

   
   cout << "SERVER: Creating thread..." << endl;
   
   pthread_t threadId;
   int result = pthread_create(&threadId, NULL, threadRoutine, (void*)4711);
   if (result)
   {
      cout << "Thread could not be created, " << result << endl;
      exit(1);
   }
      
   cout << "SERVER: Thread created." << endl;

   
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
         cout << "SERVER: Socket accepted." << endl;
   
			//inform user of socket number - used in send and receive commands 
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , newSockfd , inet_ntoa(cli_addr.sin_addr) , ntohs(cli_addr.sin_port)); 
		
         bzero(buffer,256);
         n = read(newSockfd,buffer,255);
         if (n < 0) 
         {
            error("ERROR reading from socket");
         }
         cout << "SERVER: Message received from new client: " << buffer << endl;

         n = write(newSockfd,"I got your message",18);
         if (n < 0) 
         {
            error("ERROR writing to socket");
         }
         cout << "SERVER: Answer sent." << endl;
				
			//add new socket to array of sockets 
			for (i = 0; i < max_clients; i++) 
			{ 
				//if position is empty 
				if( clientSocket[i] == 0 ) 
				{ 
					clientSocket[i] = newSockfd; 
					cout << "Adding to list of sockets as " << i << endl;
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
					
				//Echo back the message that came in 
				else
				{ 
               // Terminate receiver string after the number of bytes received.
               buffer[valread] = 0;
               
               cout << "SERVER: Message received from new client: " << buffer << endl;

               n = write(sd,"I got your message",18);
               if (n < 0) 
               {
                  error("ERROR writing to socket");
               }
               cout << "SERVER: Answer sent." << endl;
				} 
			} 
		} 
   }
   
   
   
   
   
/*   
   
   
   
   newSockfd = accept(masterSockfd, 
   (struct sockaddr *) &cli_addr, 
   &clilen);
   if (newSockfd < 0) 
   {
      error("ERROR on accept");
   }
   cout << "SERVER: Socket accepted." << endl;
   
   bzero(buffer,256);
   n = read(newSockfd,buffer,255);
   if (n < 0) 
   {
      error("ERROR reading from socket");
   }
   cout << "SERVER: Message received: " << buffer << endl;

   n = write(newSockfd,"I got your message",18);
   if (n < 0) 
   {
      error("ERROR writing to socket");
   }
   cout << "SERVER: Answer sent." << endl;

   close(newSockfd);
   close(masterSockfd);

   cout << "SERVER: Socket closed." << endl;
   
   while(true)
   {

      cout << "Tick!" << endl;

      sleep(1);
   }


*/
   
   
   /*
   
   	//accept the incoming connection 
	puts("Waiting for connections ..."); 
		
	while(true) 
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
			printf("select error"); 
		} 
			
		//If something happened on the master socket , 
		//then its an incoming connection 
		if (FD_ISSET(masterSockfd, &readfds)) 
		{ 
			if ((newSockfd = accept(masterSockfd, 
					(struct sockaddr *)&cli_addr, &clilen))<0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			} 
			
			//inform user of socket number - used in send and receive commands 
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , newSockfd , inet_ntoa(cli_addr.sin_addr) , ntohs(cli_addr.sin_port)); 
		
			//send new connection greeting message 
			if( send(newSockfd, message, strlen(message), 0) != strlen(message) ) 
			{ 
				perror("send"); 
			} 
				
			puts("Welcome message sent successfully"); 
				
			//add new socket to array of sockets 
			for (i = 0; i < max_clients; i++) 
			{ 
				//if position is empty 
				if( clientSocket[i] == 0 ) 
				{ 
					clientSocket[i] = newSockfd; 
					printf("Adding to list of sockets as %d\n" , i); 
						
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
					
				//Echo back the message that came in 
				else
				{ 
					//set the string terminating NULL byte on the end 
					//of the data read 
					buffer[valread] = '\0'; 
					send(sd , buffer , strlen(buffer) , 0 ); 
				} 
			} 
		} 

   }
   
   
   
   
   
   
   
   
   
   /*
   

   */
   return 0;
}









/*


//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
	
#define TRUE 1 
#define FALSE 0 
#define PORT 8888 
	
int main(int argc , char *argv[]) 
{ 
	int opt = TRUE; 
	int master_socket , addrlen , new_socket , clientSocket[30] , 
		max_clients = 30 , activity, i , valread , sd; 
	int max_sd; 
	struct sockaddr_in address; 
		
	char buffer[1025]; //data buffer of 1K 
		
	//set of socket descriptors 
	fd_set readfds; 
		
	//a message 
	char *message = "ECHO Daemon v1.0 \r\n"; 
	
	//initialise all clientSocket[] to 0 so not checked 
	for (i = 0; i < max_clients; i++) 
	{ 
		clientSocket[i] = 0; 
	} 
		
	//create a master socket 
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this 
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
		sizeof(opt)) < 0 ) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	
	//type of socket created 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
		
	//bind the socket to localhost port 8888 
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("Listener on port %d \n", PORT); 
		
	//try to specify maximum of 3 pending connections for the master socket 
	if (listen(master_socket, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
		
	//accept the incoming connection 
	addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 
		
	while(TRUE) 
	{ 
		//clear the socket set 
		FD_ZERO(&readfds); 
	
		//add master socket to set 
		FD_SET(master_socket, &readfds); 
		max_sd = master_socket; 
			
		//add child sockets to set 
		for ( i = 0 ; i < max_clients ; i++) 
		{ 
			//socket descriptor 
			sd = clientSocket[i]; 
				
			//if valid socket descriptor then add to read list 
			if(sd > 0) 
				FD_SET( sd , &readfds); 
				
			//highest file descriptor number, need it for the select function 
			if(sd > max_sd) 
				max_sd = sd; 
		} 
	
		//wait for an activity on one of the sockets , timeout is NULL , 
		//so wait indefinitely 
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} 
			
		//If something happened on the master socket , 
		//then its an incoming connection 
		if (FD_ISSET(master_socket, &readfds)) 
		{ 
			if ((new_socket = accept(master_socket, 
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			} 
			
			//inform user of socket number - used in send and receive commands 
			printf("New connection , socket fd is %d , ip is : %s , port : %d 
				\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
				(address.sin_port)); 
		
			//send new connection greeting message 
			if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
			{ 
				perror("send"); 
			} 
				
			puts("Welcome message sent successfully"); 
				
			//add new socket to array of sockets 
			for (i = 0; i < max_clients; i++) 
			{ 
				//if position is empty 
				if( clientSocket[i] == 0 ) 
				{ 
					clientSocket[i] = new_socket; 
					printf("Adding to list of sockets as %d\n" , i); 
						
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
					getpeername(sd , (struct sockaddr*)&address , \ 
						(socklen_t*)&addrlen); 
					printf("Host disconnected , ip %s , port %d \n" , 
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
						
					//Close the socket and mark as 0 in list for reuse 
					close( sd ); 
					clientSocket[i] = 0; 
				} 
					
				//Echo back the message that came in 
				else
				{ 
					//set the string terminating NULL byte on the end 
					//of the data read 
					buffer[valread] = '\0'; 
					send(sd , buffer , strlen(buffer) , 0 ); 
				} 
			} 
		} 
	} 
		
	return 0; 
} 

*/