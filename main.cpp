#include <memory>
#include <iostream>
#include "server.h"
#include "client.h"

using namespace std;

int main(int argc, char** argv)
{
  if (argc != 2) {
    cout << "wrong format, try this: ./ipc [server or client]" << endl;
    return 0;
  }
  
  unique_ptr<base_socket> ipc;
  if ((string)argv[1] == "client")
    ipc = make_unique<Client>();
  else if ((string)argv[1] == "server") {
    ipc = make_unique<Server>();
  } else {
    cout << "wrong format, try this: ./ipc [server or client]" << endl;
    return 0;
  }

  ipc->run();
  return 0;
}