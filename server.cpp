#include "server.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <list>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#define PORT 7000
#define IP "127.0.0.1"
#define BUFFER_SIZE 1024

using namespace std;

static unsigned int last_fibo = 0;
static list<int> li;

static constexpr unsigned long int fibo(long int n)
{
  if (n == 0 || n == 1)
    return n;
  return fibo(n - 1) + fibo(n - 2);
}

static void getConn(int s, sockaddr_in const& servaddr, socklen_t len)
{
  cout << "waiting ..." << endl;
  while (true) {
    int conn = accept(s, (struct sockaddr*)&servaddr, &len);
    li.push_back(conn);
    cout << "client connected, socket_id: " << conn << endl;
  }
}

static void send(int sock, int fibo_val)
{
  char sendbuf[BUFFER_SIZE];
  strcpy(sendbuf, to_string(fibo_val).c_str());
  ::send(sock, sendbuf, strlen(sendbuf), 0); // Send out
}

static unsigned long calculate_fibo(char* buf)
{
  auto num      = strtol(buf, nullptr, 10);
  auto fibo_val = 0;
  if (num < 0)
    fibo_val = last_fibo;
  else {
    fibo_val  = fibo(num);
    last_fibo = fibo_val;
  }
  return fibo_val;
}

static void getData()
{
  struct timeval tv;
  tv.tv_sec  = 2;
  tv.tv_usec = 0;
  while (true) {
    for (auto it:li) {
      fd_set rfds;
      FD_ZERO(&rfds);
      int maxfd  = 0;
      int retval = 0;
      FD_SET(it, &rfds);
      if (maxfd < it) {
        maxfd = it;
      }
      retval = select(maxfd + 1, &rfds, nullptr, nullptr, &tv);
      if (retval == -1) {
        cout << "select error" << endl;
      } else if (retval == 0) {
        continue; // no message
      } else {
        char buf[BUFFER_SIZE];
        memset(buf, 0, sizeof(buf));
        int len = recv(it, buf, sizeof(buf), 0);
        cout << "received from socket " << it << ", data: " << buf << endl;
        auto fibo_val = calculate_fibo(buf);
        send(it, fibo_val);
      }
    }
    sleep(1);
  }
}

void Server::run()
{
  // new socket
  s = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in servaddr;
  socklen_t len;

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_port        = htons(PORT);
  servaddr.sin_addr.s_addr = inet_addr(IP);
  if (bind(s, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
    cerr << "bind error" << endl;
    exit(1);
  }
  if (listen(s, 20) == -1) {
    cerr << "listen error" << endl;
    exit(1);
  }
  len = sizeof(servaddr);

  // thread : while ==>> accpet
  std::thread t(getConn, s, servaddr, len);
  t.detach();

  // thread : recv ==>> show
  std::thread t2(getData);
  t2.detach();

  while (true) {
    sleep(1);
  }
}
