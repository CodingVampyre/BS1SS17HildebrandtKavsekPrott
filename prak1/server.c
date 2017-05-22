
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <ctype.h>

int doprocessing (int sock);

getwords(char *line, char *words[], int maxwords);
int put(char* key, char* value, char* res);
int get(char* key, char* res);
int del(char* key, char* res);

int main(int argc, char *argv[] ) {

  int sockfd, newsockfd, portno, clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n, pid;

  /* socket funktion aufrufen */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(1);
  }

  /* initialisiere socket */
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = 6969;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* die addresse an den Host binden */
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR on binding");
    exit(1);
  }

  /* start listening */
  listen(sockfd,5);
  clilen = sizeof(cli_addr);

  while (1) {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
      }

      /* child prozess erzeugen */
      pid = fork();

      if (pid < 0) {
        perror("ERROR on fork");
        exit(1);
      }

      if (pid == 0) {
        //close(sockfd);
        //doprocessing(newsockfd);
        //exit(0);

        while (doprocessing(newsockfd) == 0) {

        }
        close(sockfd);
        exit(0);
      }

      else {
        close(newsockfd);
      }
    }
  }

int doprocessing (int sock) {
  int n;
  char buffer[256];
  bzero(buffer, 256);
  n = read(sock,buffer,255);
  char *words[sizeof(buffer)/2];
  char res[256];

  int nwords = getwords(buffer, words, 10);



  if (n < 0) {
    perror("ERROR reading from socket");
    exit(1);
  }

  printf("Incomming: %s\n", buffer);

  if (n < 0) {
    perror("ERROR writing to socket");
    exit(1);
  }
  for (int h = 0; h < nwords; h++) {
    printf("%s\n", words[h]);
  }

  if(strncmp(buffer, "EXIT", 4) == 0) {
    return 1;
  }

  if (strncmp(words[0], "TST", 3) == 0){
    printf("ERROR SUCCESS \n");
  } else if(strncmp(words[0], "PUT", 3) == 0) {
    put(words[1], words[2], res);
  } else if(strncmp(words[0], "GET", 3) == 0) {
    get(words[1], res);
  } else if(strncmp(words[0], "DEL", 3) == 0) {
    del(words[1], res);
  }

  char szOutput[256];
  sprintf(szOutput, "Output: %s\n", res);
  n = write(sock, szOutput, strlen(szOutput));

  return 0;

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

}

int get(char* key, char* res) {

}

int del(char* key, char* res) {

}
