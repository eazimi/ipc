#ifndef SERVER_H
#define SERVER_H

#include "base_socket.hpp"
class Server : public base_socket {
private:
  int s;

public:
  explicit Server() = default;
  void run();
};

#endif