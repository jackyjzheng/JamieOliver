#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstdlib>
#include <unistd.h>
#include "Manager.cpp"

#define MY_PORT		9091
#define MAXBUF		1024

void SearchThread(SearchManager *search) {
	search->init_search();
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
	SearchManager *search = new SearchManager(rNum, tabuSize, 4);
	std::thread searchThread(SearchThread, search);

	// Char buffer to hold new input
	char buffer[MAXBUF];
	char rNumString[100];
	int rBufferSize = 0;
	char tabuSizeString[100];
	int tabuBufferSize = 0;
	
	while (1) {	
		int clientfd;
		struct sockaddr_in client_addr;
		int addrlen=sizeof(client_addr);

		/*---accept a connection (creating a data pipe)---*/
		clientfd = accept(sockfd, (struct sockaddr*) &client_addr, (socklen_t*) &addrlen);
		printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		printf("==============RECEIVED INFORMATION FROM SOCKET================\n");

		// FORMAT: NEW325, 27.
		ssize_t data_sent = recv(clientfd, buffer, MAXBUF, 0);
		int counter = 0;
		while (buffer[counter] != 'N') {
			counter += 1;
			if (counter > MAXBUF)
				continue;
		}
		if (buffer[counter+1] == 'E' && buffer[counter+2] == 'W') {
			int i = counter+3;
			for (i; buffer[i] != ','; ++i) {
				rNumString[i-(counter+3)] = buffer[i];
				rBufferSize += 1;
				if (rBufferSize >= 100)
					break;
			}
			int next = i+2;
			for (int i = next; buffer[i] != '.'; ++i) {
				tabuSizeString[i-next] = buffer[i];
				tabuBufferSize += 1;
				if (tabuBufferSize >= 100)
					break;
			}	
		} else {
			continue;
		}
		
		if (rBufferSize >= 100 || tabuBufferSize >= 100)
			continue;
		else {
			rNumString[rBufferSize] = '\0';
			tabuSizeString[tabuBufferSize] = '\0';
		}

		sscanf(rNumString, "%d", &rNum);
		sscanf(tabuSizeString, "%d", &tabuSize);
		
		printf("==============INFORMATION FROM SOCKET================\n");
		printf("rNum is %i.\n", rNum);
		printf("tabuSize is %i.\n", tabuSize);
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