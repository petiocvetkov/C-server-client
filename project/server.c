#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<pthread.h>

//internal headers
#include "constants/server/globals.h"
#include "models/events.h"
#include "models/models.h"
#include "methods/server/methods_declarations.h"
#include "methods/server/methods.h"

Node* mainList;

int runningThreads = 0;

int main(int argc, char const *argv[]) {

  mainList = loadDate();
  pthread_t tid[MAX_THREADS];
  int sockfd, connfd, len;

  struct sockaddr_in servaddr, cli;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  }
  else
    printf("Socket successfully created..\n");
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);

  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
    printf("socket bind failed...\n");
    exit(0);
  }
  else
    printf("Socket successfully binded..\n");

  while (1) {
    if ((listen(sockfd, 5)) != 0) {
      printf("Listen failed...\n");
      exit(0);
    }
    else
      printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
      printf("server acccept failed...\n");
      exit(0);
    }
    else
      printf("server acccept the client...\n");

  // create new thread to handle connected socket
    if(runningThreads < MAX_THREADS && pthread_create(&tid[runningThreads++], NULL, socketThread, &connfd) != 0 )
       printf("Failed to create thread\n");
  }
  int i;
  for (i = 0; i < runningThreads; i++) {
                pthread_join(tid[i],NULL);
  }

  return 0;
}


void * socketThread(void *arg){
  int newSocket = *((int *)arg);
  handleConnection(mainList, newSocket);
  close(newSocket);
  runningThreads--;
}
