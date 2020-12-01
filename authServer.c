/*
Program demonstrating the server component of the communication system.
This server receives username and password from client application (authClient.c)
and checks the received byte-stream with the default username and password. 
The server returns one of the three messages below:
	-PROCEED if the user enters the right credentials
	-"You have x attempts left" informing the user of attempts left
	-DENIED if user enters invalid credentials 3 times in a row 

Written by Alexandru Bulgari
Student number: C18342126
29/11/2020
*/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"
#include <unistd.h>
#include <sys/stat.h>

#define BUFSIZE 200

int main(int argc, char* argv[]) 
{
	int numBytes  = 0;
	int MAXPENDING = 1;
	char recvbuffer[BUFSIZE], sendbuffer[BUFSIZE];
	char inp_user[BUFSIZE]; 		
	char inp_pass[BUFSIZE]; 	
	char pass[BUFSIZE] = "pass"; 	
	char user[BUFSIZE] = "admin"; 	
	int attemptsLeft = 3;	

	//check if the program has not been run with the correct number of parameters
	if (argc != 2) 
		DieWithUserMessage("Parameter(s)", "<Server Port>");	
	
	//set the server port to the second command line argument	
	in_port_t servPort = atoi(argv[1]);
	int servSock; 
	
	
	//create a socket,1st parameter specifying the protocol family AF_INET(IPV4)
	//2nd parameter is the type of socket being created (stream socket)
	//3rd parameter specifies TCP transport protocol will be used
	if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    		DieWithSystemMessage("socket() failed");
	 
	
	/*create a local server address structure
	This structure will hold information such as address
	and port
	*/
	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));       
  	servAddr.sin_family = AF_INET;                
  	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  	servAddr.sin_port = htons(servPort);
	

	//bind the socket created above to the local address created above
	if (bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
			DieWithSystemMessage("bind() failed");
	


	//accept incoming connection requests to the server socket
	//MAXPENDING is the maximum number of connections 
	//that should be queued for this socket
        if (listen(servSock, MAXPENDING) < 0)
                DieWithSystemMessage("listen() failed");


	for (;;)
	{ 

		//clear out the contents of the buffers	
		strcpy(sendbuffer, "");
		strcpy(recvbuffer, "");
		strcpy(inp_user, "");
		strcpy(inp_pass, "");
		

		//declare client address structure
		struct sockaddr_in clntAddr;
		socklen_t clntAddrLen = sizeof(clntAddr);
		
	
		//return new file descriptor referring
		//to the client socket , all read/write operations will be 
		//performed on this socket
		int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);

		//check if the client socket has not been created
		if (clntSock < 0)
			DieWithSystemMessage("accept() failed");
		
		//3 Attemts for the user to enter their credentials
		while(attemptsLeft != 0) 
		{
		
			//read in the data from the client and null terminate it
			while ((numBytes = recv(clntSock, recvbuffer, (BUFSIZE-1), 0)) > 0) {
				recvbuffer[numBytes] = '\0';    
				fputs(recvbuffer, stdout);      
				
				if(strstr(recvbuffer, "\r\n\r\n") > 0)
					break;
			}
			



			//check if data has not been received from the client
			if (numBytes < 0)
				DieWithSystemMessage("recv() failed");
			
			//read username and password into the server's buffers
			sscanf(recvbuffer, "%s %s", inp_user, inp_pass);

	//		printf("user input: %s\n", inp_user);
	//		printf("user_password: %s\n", inp_pass);

			//check if the entered username and pasword is correct
			if(	(strcmp(user, inp_user) == 0)
				&&
				(strcmp(pass, inp_pass) == 0)
			  )
			
			{
				//store the proceed message in the sendbuffer`
				snprintf(sendbuffer, sizeof(sendbuffer), "%s", "PROCEED\r\n");	
				
				//send the proceed message to the client, numbytes stores no. of bytes sent
				ssize_t numBytesSent = send(clntSock, sendbuffer, strlen(sendbuffer), 0);
				
				//check for errors while sending the message	
				if (numBytesSent < 0)
					DieWithSystemMessage("send() failed");
				
				//close the communication between the server and the client	
				close(clntSock);
				return 0;
			}
				
				
			else //the credentials entered were incorrect
			{
				attemptsLeft--;			
		
				//check if user has no attempts left
				if(attemptsLeft == 0) 
				{
					//store the denied message into the sendbuffer 
					snprintf(sendbuffer, sizeof(sendbuffer), "%s", "DENIED\r\n" );

					//send the denied message to the client, numbytes stores no. of bytes sent
					ssize_t numBytesSent = send(clntSock, sendbuffer, strlen(sendbuffer), 0);
				

					//check for errors while sending the message	
					if (numBytesSent < 0)
						DieWithSystemMessage("send() failed\r\n");
				
					//close the communication between the server and the client	
					close(clntSock);
					return 0;
				}
				 else
				 {
					//store message informing user of attempts left
					snprintf(sendbuffer, sizeof(sendbuffer)
						  , "You have %d attempt(s) left\r\n", attemptsLeft);
		

					

					//send the message to the client, numbytes stores no. of bytes sent
					ssize_t numBytesSent = send(clntSock, sendbuffer, strlen(sendbuffer), 0);
				

					//check for errors while sending the message	
					if (numBytesSent < 0)
						DieWithSystemMessage("send() failed\r\n");
				}
			}
		}	
	}
}


