#ifndef CLIENT_H
#define CLIENT_H

#include "base_socket.hpp"
class Client : public base_socket {
public:
  explicit Client() = default;
  void run();
};

#endif