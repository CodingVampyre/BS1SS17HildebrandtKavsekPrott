#include <netinet/in.h>

int main() {

	printf("Hello World\n");

	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sock<0) {
		perror("creating stream socket");
		exit(2);
	}
}
