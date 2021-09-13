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

#define PORT 7000
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
  servaddr.sin_port        = htons(PORT);            /// Server Port
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); /// server ip

  // Connect to the server, successfully return 0, error return - 1
  if (connect(sock_cli, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    cerr << "connect error" << endl;
    exit(1);
  }

  cout << "connected to server" << endl;

  while (true) {
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
    retval = select(maxfd + 1, &rfds, nullptr, nullptr, &tv);
    if (retval == -1) {
      cout << "select Error, client program exit ..." << endl;
      break;
    } else if (retval == 0) {
      continue; // no input information, keep waiting
    } else {
      /*The server sent a message.*/
      if (FD_ISSET(sock_cli, &rfds)) {
        char recvbuf[BUFFER_SIZE];
        int len;
        len = recv(sock_cli, recvbuf, sizeof(recvbuf), 0);
        if (len)
          std::cout << "received from server: " << recvbuf << endl;
      }

      /*When the user enters the information, he begins to process the information and send it.*/
      if (FD_ISSET(0, &rfds)) {
        char sendbuf[BUFFER_SIZE];
        memset(sendbuf, 0, sizeof(sendbuf));
        fgets(sendbuf, sizeof(sendbuf), stdin);
        std::cout << "sent to server: " << sendbuf << endl;
        send(sock_cli, sendbuf, strlen(sendbuf), 0); // Send out        
      }
    }
  }

  close(sock_cli);
}