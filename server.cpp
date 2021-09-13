#include "server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <thread>
#include <list>
#include <string>

#define PORT 7000
#define IP "127.0.0.1"
#define BUFFER_SIZE 1024

using namespace std;

static unsigned int last_fibo = 0;
static list<int> li;
static struct sockaddr_in servaddr;
static socklen_t len;

static constexpr unsigned long int fibo(long int n)
{
  if (n == 0 || n == 1)
    return n;
  return fibo(n - 1) + fibo(n - 2);
}

static void getConn(int s)
{
  while (1) {
    int conn = accept(s, (struct sockaddr*)&servaddr, &len);
    li.push_back(conn);
    printf("%d\n", conn);
  }
}

static void getData()
{
  struct timeval tv;
  tv.tv_sec  = 2;
  tv.tv_usec = 0;
  while (1) {
    std::list<int>::iterator it;
    for (it = li.begin(); it != li.end(); ++it) {
      fd_set rfds;
      FD_ZERO(&rfds);
      int maxfd  = 0;
      int retval = 0;
      FD_SET(*it, &rfds);
      if (maxfd < *it) {
        maxfd = *it;
      }
      retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
      if (retval == -1) {
        printf("select error\n");
      } else if (retval == 0) {
        // printf("not message\n");
      } else {
        char buf[BUFFER_SIZE];
        memset(buf, 0, sizeof(buf));
        int len = recv(*it, buf, sizeof(buf), 0);
        std::cout << buf << std::endl;
        auto num      = strtol(buf, nullptr, 10);
        auto fibo_val = 0;
        if (num < 0)
          fibo_val = last_fibo;
        else {
          fibo_val  = fibo(num);
          last_fibo = fibo_val;
        }

        char sendbuf[BUFFER_SIZE];
        // fgets(sendbuf, sizeof(sendbuf), stdin);
        // stringstream ss;
        // ss << fibo_val;
        strcpy(sendbuf, to_string(fibo_val).c_str());
        send(*it, sendbuf, strlen(sendbuf), 0); // Send out
        memset(sendbuf, 0, sizeof(sendbuf));
      }
    }
    sleep(1);
  }
}

static void sendMess()
{
  while (1) {
    char buf[1024];
    fgets(buf, sizeof(buf), stdin);
    // printf("you are send %s", buf);
    std::list<int>::iterator it;
    for (it = li.begin(); it != li.end(); ++it) {
      send(*it, buf, sizeof(buf), 0);
    }
  }
}

void Server::run()
{
  // new socket
  s = socket(AF_INET, SOCK_STREAM, 0);

  // int flags = fcntl(s, F_GETFL);
  // fcntl(s, F_SETFL, flags | O_NONBLOCK);

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_port        = htons(PORT);
  servaddr.sin_addr.s_addr = inet_addr(IP);
  if (bind(s, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
    perror("bind");
    exit(1);
  }
  if (listen(s, 20) == -1) {
    perror("listen");
    exit(1);
  }
  len = sizeof(servaddr);

  // thread : while ==>> accpet
  std::thread t(getConn, s);
  t.detach();
  // printf("done\n");
  // thread : input ==>> send
  std::thread t1(sendMess);
  t1.detach();
  // thread : recv ==>> show
  std::thread t2(getData);
  t2.detach();
  while (1) {
    sleep(1);
  }
}
