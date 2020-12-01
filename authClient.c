#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"

int main(int argc, char *argv[]) {
	char username[20], password[20], recvbuffer[BUFSIZE], sendbuffer[BUFSIZE];
	int numBytes = 0;

	if (argc < 3)
    		DieWithUserMessage("Parameter(s)", "<Server Address> <Server Port>");

	char *servIP = argv[1];
  
	in_port_t servPort = atoi(argv[2]);

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0)
		DieWithSystemMessage("socket() failed");

	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;

	int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
	if (rtnVal == 0)
		DieWithUserMessage("inet_pton() failed", "invalid address string");

	else if (rtnVal < 0)
		DieWithSystemMessage("inet_pton() failed");

	servAddr.sin_port = htons(servPort);

	if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
		DieWithSystemMessage("connect() failed");

	do
	{

		memset(recvbuffer, 0, BUFSIZE);
		memset(sendbuffer, 0, BUFSIZE);
		printf("\nEnter username (10 characters max):\n");
		fgets(username, sizeof(username), stdin);
		strcat(sendbuffer, username);
		strcat(sendbuffer, " ");

		printf("\nEnter password (10 characters max):\n");
		fgets(password, sizeof(password), stdin);
		strcat(sendbuffer, password);
		strcat(sendbuffer, "\r\n\r\n");

		ssize_t numBytesSent = send(sock, sendbuffer, strlen(sendbuffer), 0);

		if (numBytesSent < 0)
			DieWithSystemMessage("send() failed");

		while ((numBytes = recv(sock, recvbuffer, BUFSIZE - 1, 0)) > 0) {
			if(strstr(recvbuffer, "\r\n") > 0)
				break;
		}
		if (numBytes < 0)
			DieWithSystemMessage("recv() failed");

		if (strstr(recvbuffer, "PROCEED") > 0)
		{
			printf("\nCongratulations you can Proceed.\n\n");
			break;

		}
		if (strstr(recvbuffer, "DENIED") > 0)
		{
			printf("\nYou have been Denied access.\n\n");
			break;
		}
		else
			printf("\n%s",recvbuffer);

	} while(1);

	fputc('\n', stdout);

	close(sock);
	exit(0);
}
