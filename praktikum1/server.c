#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

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

	while (1) {
		newsockfd = accept (sockfd, (struct sockaddr *) &cli_addr, &client_length);
		if (newsockfd < 0) {
			perror("ERROR while accepting");
			exit(3);
		}

		// NEW PROCESS
		pid = fork();
		if (pid < 0) {
			perror("ERROR while forking");
			exit(4);
		}

		if (pid == 0) {
			int running = 1;
			while (running != 0) {
				if (handle_content(newsockfd) == 0) {
					close (sockfd);
					exit(0);
				}
			}
		} else {
			close(newsockfd);
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
