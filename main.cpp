#include "server.h"
#include <memory>

using namespace std;

int main(int argc, char** argv)
{
  if (argc != 2) {
    cout << "wrong format, try this: ./ipc [server or client]" << endl;
    return 0;
  }
  
  unique_ptr<Server> server;
  if ((string)argv[1] == "client")
    ;
  else if ((string)argv[1] == "server") {
    server = make_unique<Server>();
  } else {
    cout << "wrong format, try this: ./ipc [server or client]";
    return 0;
  }

  server->run();
  return 0;
}