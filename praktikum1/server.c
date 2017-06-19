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

struct keypair keys[1024];

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
		perror ("ERROR while binding");
		exit (2);
	}

	// LISTENING TO CLIENTS
	listen(sockfd, 5);
	client_length = sizeof(cli_addr);

	/*
	* Wir schreiben ein Array von Sockets, über das die eingehenden Anfragen abgearbeitet werden.
	* fdRead ist dabei später unser pool von zu bearbeitenden Sockets.
	*/
	fd_set fdRead;
	int socket_list[256] = {-1};
	memset(socket_list, -1, sizeof(socket_list));

	// WHILE SERVER IS RUNNING
	while (1) {

		// ♬♪♫
		// Nobody knows why it needs max_fd (nfds is the highest-numbered file descriptor in any of the three sets, plus 1.)
		int max_fd = sockfd;

		/*
		* Wir reinigen unser fd_readm fangen quasi bei 0 an.
		* Wir schieben sockfd in unseren Pool. Dieser ist später für eingehende Verbindungsanfragen wichtig.
		*/
		FD_ZERO(&fdRead); // CLEARS A SET
		FD_SET(sockfd, &fdRead); // ADD GIVEN FILE DESCRIPTOR FROM A SET

		/*
		* Wir iterieren durch unser array von Sockets und dort, wo etwas drin ist (also nicht -1 oder 0),
		* dort schieben wir den entsprechenden slot ebenfalls mit FD_SET in unseren pool (fdread) und erhöhen max_fd um diesen Wert.
		*/
		for (int i=0; i<sizeof(socket_list) / sizeof(socket_list[0]); ++i) {
			if (socket_list[i] > 0) {
				if (socket_list[i] > max_fd) {
					max_fd = socket_list[i];
				}
				FD_SET(socket_list[i], &fdRead);
			}
		}

		/*
		* Der eigentliche Aufruf von select(). DHier übergeben wir nfds als max_fd, unseren pool fdRead und ein paar
		* unwichtige Flags. Nun weiss unser Server, an welchen Slots er etwas bearbeiten muss. Yay.
		*/
		int res = select(max_fd+1, &fdRead, 0, 0, 0); //TODO Test NULL instead of 0
		if (res < 0) {
			perror("ERROR while selecting");
			exit(7);
		}

		// IF NO FREE SLOT WAS FOUND, ABORT CURRENT REQUEST
		if (res == 0) {
			continue;
		}

		/*
		* Wir gehen abermals durch unser Array an Sockets und schauen, ob sich an der Stelle sowohl Daten 
		* als auch eine gesetzte Anfrage befinden (122). Wenn dem so ist, können wir die Daten bearbeiten (handle_content)
		* Das passiert für ALLE Anfragen, die in dieser Runde gestellt wurden.
		*/
		for (int j=0; j<sizeof(socket_list) / sizeof(socket_list[0]); ++j) {
			if (socket_list[j] > 0 && FD_ISSET(socket_list[j], &fdRead) ) {
				if (handle_content(socket_list[j]) == 0) {
					close(socket_list[j]);
					socket_list[j] = -1;
				}
			}
		}

		/*
		* Wenn wir unsere Anfragen bearbeitet haben, dann testen wir noch, ob sich Jemand Neues verbinden will.
		* Mit accept() akzeptieren wir diese Verbindung dann. Dafür muss sockfd eine Anfrage beinhalten.
		* Die Verbindung wird dann auf einen der freien Slots geschrieben und kann in der nächsten Runde Daten abarbeiten.
		*/
		if (FD_ISSET(sockfd, &fdRead)) {
			newsockfd = accept(sockfd, &cli_addr, &client_length);
			if (newsockfd < 0) {
				perror("ERROR while accepting");
				exit(3);
			}#include <stdio.h>
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

struct keypair keys[1024];

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
		perror ("ERROR while binding");
		exit (2);
	}

	// LISTENING TO CLIENTS
	listen(sockfd, 5);
	client_length = sizeof(cli_addr);

	/*
	* Wir schreiben ein Array von Sockets, über das die eingehenden Anfragen abgearbeitet werden.
	* fdRead ist dabei später unser pool von zu bearbeitenden Sockets.
	*/
	fd_set fdRead;
	int socket_list[256] = {-1};
	memset(socket_list, -1, sizeof(socket_list));

	// WHILE SERVER IS RUNNING
	while (1) {

		// ♬♪♫
		// Nobody knows why it needs max_fd (nfds is the highest-numbered file descriptor in any of the three sets, plus 1.)
		int max_fd = sockfd;

		/*
		* Wir reinigen unser fd_readm fangen quasi bei 0 an.
		* Wir schieben sockfd in unseren Pool. Dieser ist später für eingehende Verbindungsanfragen wichtig.
		*/
		FD_ZERO(&fdRead); // CLEARS A SET
		FD_SET(sockfd, &fdRead); // ADD GIVEN FILE DESCRIPTOR FROM A SET

		/*
		* Wir iterieren durch unser array von Sockets und dort, wo "Etwas" drin ist (also nicht -1 oder 0),
		* dort schieben wir den entsprechenden Slot ebenfalls mit FD_SET in unseren pool (fdread)
		* und erhöhen max_fd auf diesen Wert.
		*/
		for (int i=0; i<sizeof(socket_list) / sizeof(socket_list[0]); ++i) {
			if (socket_list[i] > 0) {
				if (socket_list[i] > max_fd) {
					max_fd = socket_list[i];
				}
				FD_SET(socket_list[i], &fdRead);
			}
		}

		/*
		* Der eigentliche Aufruf von select(). DHier übergeben wir nfds als max_fd, unseren pool fdRead und ein paar
		* unwichtige Flags. Nun weiss unser Server, an welchen Slots er etwas bearbeiten muss. Yay.
		*/
		int res = select(max_fd+1, &fdRead, 0, 0, 0); //TODO Test NULL instead of 0
		if (res < 0) {
			perror("ERROR while selecting");
			exit(7);
		}

		// IF NO FREE SLOT WAS FOUND, ABORT CURRENT REQUEST
		if (res == 0) {
			continue;
		}

		/*
		* Wir gehen abermals durch unser Array an Sockets und schauen, ob sich an der Stelle sowohl Daten 
		* als auch eine gesetzte Anfrage befinden (122). Wenn dem so ist, können wir die Daten bearbeiten (handle_content)
		* Das passiert für ALLE Anfragen, die in dieser Runde gestellt wurden.
		*/
		for (int j=0; j<sizeof(socket_list) / sizeof(socket_list[0]); ++j) {
			if (socket_list[j] > 0 && FD_ISSET(socket_list[j], &fdRead) ) {
				if (handle_content(socket_list[j]) == 0) {
					close(socket_list[j]);
					socket_list[j] = -1;
				}
			}
		}

		/*
		* Wenn wir unsere Anfragen bearbeitet haben, dann testen wir noch, ob sich Jemand Neues verbinden will.
		* Mit accept() akzeptieren wir diese Verbindung dann. Dafür muss sockfd eine Anfrage beinhalten.
		* Die Verbindung wird dann auf einen der freien Slots geschrieben und kann in der nächsten Runde Daten abarbeiten.
		*/
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

	if (strcmp(words[0], "TST") == 0) {
		printf("TEST SUCCESS\n");
	} else if (strcmp(words[0], "PUT") == 0) {
		if(put(words[1], words[2], res) != 0) {
			strcpy(res, "NIL");
		}
	} else if (strcmp(words[0], "GET") == 0) {
		if(get(words[1], res) != 0) {
			strcpy(res, "NIL");
		}
	} else  if(strcmp(words[0], "DEL") == 0) {
		if (del(words[1], res) != 0) {
			strcpy(res, "NIL");
		}
	}

	char szOutput[256];
	sprintf(szOutput, "Output: %s\n", res);
	n = write(sock, szOutput, strlen(szOutput));

	memset(res, 0, sizeof(res));
	memset(szOutput, 0, sizeof(szOutput));

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
		if (strlen(keys[i].p_key) == 0) {
			strncpy(keys[i].p_key, key, sizeof(keys[i].p_key));
			strncpy(keys[i].p_value, value, sizeof(keys[i].p_value));
			strncpy(res, value, sizeof(res));
			return 0;
		}
	}
	return 1;
}

int get(char* key, char* res) {
	for (int i=0; i<sizeof(keys) / sizeof(keys[0]); i++) {
		if (strncmp(keys[i].p_key, key, sizeof(keys[i].p_key)) == 0) {
			strcpy(res, keys[i].p_value);
			return 0;
		}
	}
	return 1;
}

int del(char* key, char* res) {
	for (int i=0; i<sizeof(keys) / sizeof(keys[0]); i++) {
		if (strncmp(keys[i].p_key, key, sizeof(keys[i].p_key)) == 0) {
			strncpy(res, keys[i].p_value, sizeof(keys[i].p_value));
			memset(&keys[i], 0, sizeof(keys[i]));
			return 0;
		}
	}
	return 1;
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

	if (strcmp(words[0], "TST") == 0) {
		printf("TEST SUCCESS\n");
	} else if (strcmp(words[0], "PUT") == 0) {
		if(put(words[1], words[2], res) != 0) {
			strcpy(res, "NIL");
		}
	} else if (strcmp(words[0], "GET") == 0) {
		if(get(words[1], res) != 0) {
			strcpy(res, "NIL");
		}
	} else  if(strcmp(words[0], "DEL") == 0) {
		if (del(words[1], res) != 0) {
			strcpy(res, "NIL");
		}
	}

	char szOutput[256];
	sprintf(szOutput, "Output: %s\n", res);
	n = write(sock, szOutput, strlen(szOutput));

	memset(res, 0, sizeof(res));
	memset(szOutput, 0, sizeof(szOutput));

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
		if (strlen(keys[i].p_key) == 0) {
			strncpy(keys[i].p_key, key, sizeof(keys[i].p_key));
			strncpy(keys[i].p_value, value, sizeof(keys[i].p_value));
			strncpy(res, value, sizeof(res));
			return 0;
		}
	}
	return 1;
}

int get(char* key, char* res) {
	for (int i=0; i<sizeof(keys) / sizeof(keys[0]); i++) {
		if (strncmp(keys[i].p_key, key, sizeof(keys[i].p_key)) == 0) {
			strcpy(res, keys[i].p_value);
			return 0;
		}
	}
	return 1;
}

int del(char* key, char* res) {
	for (int i=0; i<sizeof(keys) / sizeof(keys[0]); i++) {
		if (strncmp(keys[i].p_key, key, sizeof(keys[i].p_key)) == 0) {
			strncpy(res, keys[i].p_value, sizeof(keys[i].p_value));
			memset(&keys[i], 0, sizeof(keys[i]));
			return 0;
		}
	}
	return 1;
}
