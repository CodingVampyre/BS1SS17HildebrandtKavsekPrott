#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

int main() {

	printf("Hello World\n");

	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sock<0) {
		perror("creating stream socket");
		exit(2);
	}

	struct sockaddr_in	 	server;
	server.sin_family = 	 	AF_INET;
	server.sin_addr.s_addr = 	INADDR_ANY;
	server.sin_port =	 	htons(4711);	//htons: usigned short host byte order --> Internet network byte order

	bind (sock, (struct sockaddr *) &server, sizeof(server));
	
	exit(0);
}
