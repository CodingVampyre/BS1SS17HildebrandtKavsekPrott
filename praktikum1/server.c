#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

struct keypair {
	char p_key[33];
	char p_value[1025];
};

int min(int value, int _min) {
	return value < _min ? _min : value;
}

int max(int value, int _max) {
	return value > _max ? _max : value;
}

int minmax(int value, int _min, int _max) {
	return min(max(value, _max), _min);
}

getwords (char *line, char *words[], int maxwords);
int handle_content(int sock);
int put(char* key, char* value, char* res);
int get(char* key, char* res);
int del(char* key, char* res);

int main(int argc, char *argv[]) {
	int sockfd, newsockfd, port_number, client_length;
	char buffer[512];
	struct sockaddr_in serv_addr, cli_addr;
	int n, pid;
	struct keypair keys[1024];

	memset(keys, 0, sizeof(keys));

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
	memset(socket_list, -1, sizeof(socket_list));

	// WHILE SERVER IS RUNNING
	while (1) {

		int max_fd = sockfd;

		FD_ZERO(&fdRead);
		FD_SET(sockfd, &fdRead);

		// SEARCH FOR FREE SLOTS AND PUT FD TO FREE SOCKET
		for (int i=0; i<sizeof(socket_list) / sizeof(socket_list[0]); ++i) {
			if (socket_list[i] > 0) {
				if (socket_list[i] > max_fd) {
					max_fd = socket_list[i];
				}
				FD_SET(socket_list[i], &fdRead);
			}
		}

		// EXECUTE SELECTION
		int res = select(max_fd+1, &fdRead, 0, 0, 0); //TODO Test NULL instead of 0
		if (res < 0) {
			perror("ERROR while selecting");
			exit(7);
		}

		// IF NO FREE SLOW WAS FOUND, ABORT CURRENT REQUEST
		if (res == 0) {
			continue;
		}

		// ITERATE THROUGH socket-list and handle it's content
		for (int j=0; j<sizeof(socket_list) / sizeof(socket_list[0]); ++j) {
			if (socket_list[j] > 0 && FD_ISSET(socket_list[j], &fdRead) ) {
				if (handle_content(socket_list[j]) == 0) {
					close(socket_list[j]);
					socket_list[j] = -1;
				}
			}
		}

		// ACCEPT REQUESTS
		if (FD_ISSET(sockfd, &fdRead)) {
			newsockfd = accept(sockfd, &cli_addr, &client_length);
			if (newsockfd < 0) {
				perror("ERROR while accepting");
				exit(3);
			}
			for (int y=0; y<sizeof(socket_list) / sizeof(socket_list[0]); ++y) {
				if (socket_list[y] == -1) {
					socket_list[y] = newsockfd;
					newsockfd = -1;
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

	// USE FUNCTIONS PUT, DEL, GET HERE
	char *words[sizeof(buffer)/2];
	char res[256];
  int nwords = getwords(buffer, words, 10);

  for (int h = 0; h < nwords; h++) {
    printf("%s\n", words[h]);
  }

	if (strcmp(words[0], "TST") == 0) {
		printf("TEST SUCCESS\n");
	} else if (strcmp(words[0], "PUT") == 0) {
		put(words[1], words[2], res);
	} else if (strcmp(words[0], "GET") == 0) {
		get(words[1], res);
	} else  if(strcmp(words[0], "DEL") == 0) {
		del(words[1], res);
	}

	n = write(sock, "Got Information!\n", 18);

	if (n < 0) {
		perror("ERROR while writing to socket");
		exit(5);
	}

	if (strncmp(buffer, "EXIT", 4) == 0) {
		printf("EXIT WAS PROVIDED\n");
		return 0;
	}

	return 1;
}

getwords (char *line, char *words[], int maxwords) {
  char *p = line;
  int nwords = 0;

  while (1) {
    while (isspace(*p)) {
      p++;
    }

    if (*p == "\0") {
      return nwords;
    }

    words[nwords++] = p;

    while (!isspace (*p) && *p != '\0') {
      p++;
    }
    if (*p == '\0') {
      return nwords;
    }

    *p++ = '\0';

    if (nwords >= maxwords) {
      return nwords;
    }
  }
}

int put(char* key, char* value, char* res) {
	for(int i=0; i<sizeof(keys) / sizeof(keys[0]); i++) {
		if (strlen(keys[i].p_name) == 0) {
			strncpy(keys[i].p_key, key, sizeof(keys[i].p_key));
			strncpy(keys[i].p_value, value, sizeof(keys[i].p_value));
			return 0;
		}
	}
	return 1;
}

int get(char* key, char* res) {
	for (int i=0; i<sizeof(keys); i++) {
		if (strncmp(keys[i].p_name, key, sizeof(keys[i].p_name)) == 0) {
			strcpy(res, keys[i].p_value);
			return 0;
		}
	}
	return 1;
}

int del(char* key, char* res) {
	for (int i=0; i<sizeof(keys); i++) {
		if (strcmp(keys[i].p_name, key) == 0) {
			memset(&keys[i], 0, sizeof(keys[i])
			return 0;
		}
	}
	return 1;
}
