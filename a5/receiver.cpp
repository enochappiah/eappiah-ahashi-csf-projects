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

  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)

  Message rlogin_message(TAG_RLOGIN, username);
  if(!conn.send(rlogin_message)) {
    std::cerr << "rlogin message failure" << std::endl;
    exit(1);
  }

  Message server_response;
  if (!conn.receive(server_response) || server_response.tag == TAG_ERR) {
    std::cerr << server_response.data << std::endl;
    exit(1);
  }

  Message join_message(TAG_JOIN, room_name);
  if (!conn.send(join_message)) {
    std::cerr << "join message failure" << std::endl;
    exit(1);
  }

  Message join_response;
  if (!conn.receive(join_response) || join_response.tag == TAG_ERR) {
    std::cerr << join_response.data << std::endl;
    exit(1);
  }

  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)

  while(true) {
    std::cout << "New loop" << std::endl;

    Message received_message;

    if (!conn.receive(received_message)) {
      std::cout << "Is this where you are?" << std::endl;
      std::cerr << "failed to receive message" << std::endl;
      exit(1);
    }

    std::cout << "Line 67" << std::endl;

    // delivery: <- tag
    //->room:sender:message <- data

    if (received_message.tag == TAG_DELIVERY) {
      std::cout << "Line 73" << std::endl;
      size_t room_colon_index  = received_message.data.find(":");
      size_t sender_colon_index = received_message.data.find(":", room_colon_index + 1);
      std::string senderUsername = received_message.data.substr(room_colon_index + 1, sender_colon_index - room_colon_index - 1);
      std::string messageToPrint = received_message.data.substr(sender_colon_index + 1);

      std::cout << senderUsername << ":" << messageToPrint << std::endl;
      
    }
    if (received_message.tag == TAG_ERR) {
      std::cout << "Why are you here?" << std::endl;
      break;
    }

    std::cout << "You didn't hit any if statements" << std::endl;
  }

  conn.close();
  return 0;
}
