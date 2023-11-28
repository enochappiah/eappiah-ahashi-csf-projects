#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  Connection conn;

  // TODO: connect to server
  conn.connect(server_hostname, server_port);
  Message rlogin_message(TAG_RLOGIN, username);

  conn.send(rlogin_message);

  Message join_message(TAG_JOIN, room_name);

  conn.send(join_message);

  while(true) {
    Message received_message;
    conn.receive(received_message);
    if (received_message.tag == TAG_DELIVERY) {
      std::cout << received_message.data << std::endl;
    }
  }

  conn.close();

  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)

  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)


  return 0;
}
