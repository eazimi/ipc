#include "server.h"
#include <memory>

int main()
{
  unique_ptr<Server> server = make_unique<Server>();
  server->run();
  return 0;
}