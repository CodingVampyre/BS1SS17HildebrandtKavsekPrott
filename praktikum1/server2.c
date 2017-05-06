#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

int main() {

	struct sockaddr_in client;
	in fd;
	int client_len;
	char in[2000];
	char out[2000];
	client_len = sizeof(client);

	while(TRUE) {
		fd = accept(sock, &client, &client_len);
		while(read(fd, in, 2000) > 0) {
			write(fd, out, 2000);
		}
		close(fd);
	}

}
