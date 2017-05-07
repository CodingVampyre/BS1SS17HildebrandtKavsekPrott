#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

int min(int value, int _min) { 
	return value < _min ? _min : value; 
}

int max(int value, int _max) { 
	return value > _max ? _max : value; 
}

int minmax(int value, int _min, int _max) { 
	return min(max(value, _max), _min); 
}

int handle_content(int sock);

int main(int argc, char *argv[]) {
	int sockfd, newsockfd, port_number, client_length;
	char buffer[512];
	struct sockaddr_in serv_addr, cli_addr;
	int n, pid;

	// SOCKET
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("ERROR while creating socket");
		exit (1);
	}

	// CREATE SOCKET
	bzero((char *) &serv_addr, sizeof(serv_addr));
	port_number  = 5001;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons (port_number);

	// BIND ADRESS
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror ("ERROR while bining");
		exit (2);
	}

	// LISTENING TO CLIENTS
	listen(sockfd, 5);
	client_length = sizeof(cli_addr);
	
	// USE SELECT
	fd_set fdRead;
	int socket_list[256] = {-1};

	while (1) {
		
		int max_fd = sockfd;
		
		FD_ZERO(&fdRead);
		FD_SET(sockfd, &fdRead);
		
		for (int i=0; i<sizeof(socket_list) / sizeof(socket_list[0]); ++i) {
			if (socket_list[i] > 0) {
				if (socket_list[i] > max_fd) {
					max_fd = socket_list[i];
				}
				FD_SET(socket_list[i], &fdRead);
			}
		}
		
		int res = select(maxfd+1, &fdread, 0, 0, 0); //TODO Test NULL instead of 0
		if (res < 0) {
			perror("ERROR while selecting");
			exit(7);
		}
		
		if (res == 0) {
			continue;
		}
		
		// CONTINUE HERE
		for (int j=0; j<sizeof(socket_list); ++j) {
			if (socket_list[i} > 0 && FD_ISSET(socket_list[i], fdRead) ) {
				if (handle_content(socket_list[i]) == 0) {
					close(socket_list[i]);
					socket_list = -1;
				}
			}
		}
					
		if (FD_ISSET(sockfd, fdRead)) {
			newsockfd = accept(sockfd, &client, &client_len);
			if (newsockfd < 0) {
				perror("ERROR while accepting");
				exit(3);
			}
			for (int y=0; y<sizeof(socket_list); ++y) {
				if (socket_list[y] == -1) {
					socket_list[y] = newsockfd;
					newsock = -1;
					break;
				}
			}
			if (newsockfd != -1) {
				close(newsockfd);
			}
		}
	}
}

int handle_content(int sock) {
	int n;
	char buffer[256];
	bzero(buffer, 256);
	n = read(sock, buffer, 255);

	if (n < 0) {
		perror("ERROR while reading from socket");
		exit(5);
	}

	printf("INFORMATION: %s\n", buffer);

	// DEFINE FUNCTIONS

	n = write(sock, "Got Information!\n", 18);

	if (n < 0) {
		perror("ERROR while writing to socket");
		exit(5);
	}

	if (strncmp(buffer, "EXIT", 4) == 0) {
		printf("EXIT WAS PROVIDED");
		return 0;
	}
}
