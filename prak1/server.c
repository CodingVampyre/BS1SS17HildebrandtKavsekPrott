
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

int doprocessing (int sock);

getwords(char *line, char *words[], int maxwords);
int put(char* key, char* value, char* res);
int get(char* key, char* res);
int del(char* key, char* res);

struct KeyPair {
  char key[33];
  char value[1024];
};

int id;
const int NUM_KEY_PAIRS = 1024;
int mem_id;
struct KeyPair* keys;

int main(int argc, char *argv[] ) {

  int sockfd, newsockfd, portno, clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n, pid, ptr;

  mem_id = shmget(IPC_PRIVATE, sizeof(struct KeyPair) * NUM_KEY_PAIRS, IPC_CREAT|0777);
  keys = (struct KeyPair*)shmat(mem_id, 0, 0);

  printf("Keys: %i\n", keys);
  memset(keys, 0, sizeof(struct KeyPair) * NUM_KEY_PAIRS); // TODO Solve Segmentation fault 11 occuring here!

  /* socket funktion aufrufen */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(1);
  }

  /* initialisiere socket */
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = 6669;

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

      //result = shmctl(id, cmd, buffer);

    if (pid < 0) {
      perror("ERROR on fork");
      exit(1);
    }

    if (pid == 0) {

      while (doprocessing(newsockfd) == 0) {}
      close(sockfd);
      shmdt(keys);
      shmctl(id, IPC_RMID, 0);
      exit(0);
    } else {
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
  sprintf(szOutput, "Output: %s\n Id: %i\n", res, id);
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
  for(int i = 0; i < NUM_KEY_PAIRS; i++) {
    if (strlen(keys[i].key) == 0) {
      strncpy(keys[i].key, key, sizeof(keys[i].key));
      strncpy(keys[i].value, value, sizeof(keys[i].value));
      return 0;
    }
  }
  strcpy(res, "NIL");
  return 1;
}

int get(char* key, char* res) {
  for(int i = 0; i < NUM_KEY_PAIRS; i++) {
    if (strncmp(keys[i].key, key, sizeof(keys[i].key)) == 0) {
      strcpy(res, keys[i].value);
      return 0;
    }
  }
  strcpy(res, "NIL");
  return 1;
}

int del(char* key, char* res) {

  for(int i = 0; i < NUM_KEY_PAIRS; i++) {
    if (strncmp(keys[i].key, key, sizeof(keys[i].key)) == 0) {
      strncpy(res, keys[i].value, sizeof(keys[i].value));
      memset(&keys[i], 0, sizeof(keys[i]));
      return 0;
    }
  }
  strcpy(res, "NIL");
  return 1;
}
