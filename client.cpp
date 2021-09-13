#include "client.h"
#include <arpa/inet.h>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MYPORT 7000
#define BUFFER_SIZE 1024

using namespace std;

void Client::run()
{
  int sock_cli;
  fd_set rfds;
  struct timeval tv;
  int retval, maxfd;

  /// Define sockfd
  sock_cli = socket(AF_INET, SOCK_STREAM, 0);
  /// Define sockaddr_in
  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_port        = htons(MYPORT);          /// Server Port
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); /// server ip

  // Connect to the server, successfully return 0, error return - 1
  if (connect(sock_cli, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    perror("connect");
    exit(1);
  }

  while (1) {
    /*Clear the collection of readable file descriptors*/
    FD_ZERO(&rfds);
    /*Add standard input file descriptors to the collection*/
    FD_SET(0, &rfds);
    maxfd = 0;
    /*Add the currently connected file descriptor to the collection*/
    FD_SET(sock_cli, &rfds);
    /*Find the largest file descriptor in the file descriptor set*/
    if (maxfd < sock_cli)
      maxfd = sock_cli;
    /*Setting timeout time*/
    tv.tv_sec  = 5;
    tv.tv_usec = 0;
    /*Waiting for chat*/
    retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
    if (retval == -1) {
      printf("select Error, client program exit\n");
      break;
    } else if (retval == 0) {
      // printf("The client does not have any input information, and the server does not have any information coming. "
      //        "waiting...\n");
      continue;
    } else {
      /*The server sent a message.*/
      if (FD_ISSET(sock_cli, &rfds)) {
        char recvbuf[BUFFER_SIZE];
        int len;
        len = recv(sock_cli, recvbuf, sizeof(recvbuf), 0);
        std::cout << recvbuf << std::endl;
        memset(recvbuf, 0, sizeof(recvbuf));
      }

      /*When the user enters the information, he begins to process the information and send it.*/
      if (FD_ISSET(0, &rfds)) {
        char sendbuf[BUFFER_SIZE];
        fgets(sendbuf, sizeof(sendbuf), stdin);
        send(sock_cli, sendbuf, strlen(sendbuf), 0); // Send out
        memset(sendbuf, 0, sizeof(sendbuf));
      }
    }
  }

  close(sock_cli);
}