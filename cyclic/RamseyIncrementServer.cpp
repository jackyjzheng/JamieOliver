#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstdlib>
#include <unistd.h>
#include "SearchManager.cpp"

#define MY_PORT		2049
#define MAXBUF		1024

void IncrementThread(IncrementManager *increment) {
	Increment->init_increment();
	return;
}

int start_server() {
	int sockfd;

	/*---Create streaming socket---*/
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Socket");
		exit(errno);
	}

	/*---Initialize address/port structure---*/
	struct sockaddr_in self = { 0 };
	self.sin_family = AF_INET;
	self.sin_port = htons(MY_PORT);
	self.sin_addr.s_addr = INADDR_ANY;

	/*---Assign a port number to the socket---*/
    if ( bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0 )
	{
		perror("socket--bind");
		exit(errno);
	}

	/*---Make it a "listening socket"---*/
	if ( listen(sockfd, 20) != 0 )
	{
		perror("socket--listen");
		exit(errno);
	}
	return sockfd;
}


int main(int argc, char **argv) {
	if (argc != 3) {
		printf("./listener ramseyNumber, tabuSize\n");
		return -1;
	}

	// Bind server to port
	int sockfd = start_server();

	// Initialize search in another thread
	int rNum = atoi(argv[1]);
	int tabuSize = atoi(argv[2]);

	// Char buffer to hold new input
	char buffer[MAXBUF];
		
	while (1) {	
		int clientfd;
		struct sockaddr_in client_addr;
		int addrlen=sizeof(client_addr);

		/*---accept a connection (creating a data pipe)---*/
		clientfd = accept(sockfd, (struct sockaddr*) &client_addr, (socklen_t*) &addrlen);
		printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		/*---Echo back anything sent---*/
		ssize_t data_sent = recv(clientfd, buffer, MAXBUF, 0);
		int *new_colors = new int[data_sent-9];
		for (int i = 5; i < data_sent-4; ++i) {
			new_colors[i-5] = (int) buffer[i];
		}
		
		printf("Amount of data sent: %zu.\n", data_sent);
		search->reinitialize(rNum, tabuSize, 4);	
		search->end_search();

		//send(clientfd, buffer, recv(clientfd, buffer, MAXBUF, 0), 0);

		/*---Close data connection---*/
		close(clientfd);
	}

	/*---Clean up (should never get here!)---*/
	searchThread.join();
	close(sockfd);
	return 0;
}