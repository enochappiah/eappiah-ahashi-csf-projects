#include "client_util.h"
#include "connection.h"
#include "csapp.h"
#include "message.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr
        << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  Connection conn;
  conn.connect(server_hostname, server_port);

  Message rlogin_message(TAG_RLOGIN, username);
  if (!conn.send(rlogin_message)) {
    std::cerr << "rlogin message failure" << std::endl;
    exit(1);
  }

  Message server_response;
  if (!conn.receive(server_response) || server_response.tag == TAG_ERR) {
    std::cerr << server_response.data;
    exit(1);
  }

  Message join_message(TAG_JOIN, room_name);
  if (!conn.send(join_message)) {
    std::cerr << "join message failure" << std::endl;
    exit(1);
  }

  Message join_response;
  if (!conn.receive(join_response) || join_response.tag == TAG_ERR) {
    exit(1);
  }

  while (true) {

    Message received_message;

    if (!conn.receive(received_message)) {
      std::cerr << "failed to receive message" << std::endl;
      exit(1);
    }

    if (received_message.tag == TAG_DELIVERY) {

      size_t room_colon_index = received_message.data.find(":");
      size_t sender_colon_index =
          received_message.data.find(":", room_colon_index + 1);
      
      std::string senderUsername = received_message.data.substr(
          room_colon_index + 1, sender_colon_index - room_colon_index - 1);
      
      std::string messageToPrint =
          received_message.data.substr(sender_colon_index + 1);

      std::cout << senderUsername << ": " << messageToPrint;

    } else if (received_message.tag == TAG_ERR) {
      break;
    }
  }

  conn.close();
  return 0;
}
