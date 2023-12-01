#include <cstdio>
#include <exception>
#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"


int main(int argc, char **argv) {
 if (argc != 4) {
   std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
   return 1;
 }


 std::string server_hostname;
 int server_port;
 std::string username;


 server_hostname = argv[1];
 server_port = std::stoi(argv[2]);
 username = argv[3];


  // TODO: connect to server
  Connection connection;
  connection.connect(server_hostname, server_port);

   // TODO: send slogin message
  Message slogin_message(TAG_SLOGIN, username);
if (!connection.send(slogin_message)) {
  std::cerr << "Error: Failed to send login message." << std::endl;
  exit(1);
}

// Receive server response for login
Message server_response;
if (!connection.receive(server_response)) {
  std::cerr << "Error: Failed to receive response from server.";
  //connection.close();
  exit(1);
}

// Check if the response is an error
if (server_response.tag == TAG_ERR) {
  std::cerr << server_response.data;
  //connection.close();
  exit(1);
}

// Check for successful login
if (server_response.tag != TAG_OK) {
  std::cerr << "Error: Unexpected response tag from server: " << server_response.tag << std::endl;
  exit(1);
}


 // TODO: loop reading commands from user, sending messages to
 //       server as appropriate

 std::string input;
 while (true) {

  std::cout << "> ";
  if (!std::getline(std::cin, input)) {
    break;
  }
  input = trim(input);
  std::stringstream ss (input);
  std::string command;
  ss >> command;

  







  if (command != "/quit" && command != "/join" && command != "/leave") {
    Message send_to_everyone(TAG_SENDALL, input);
    //std::cout << send_to_everyone.data << "test" << std::endl;
    connection.send(send_to_everyone);
    if (!connection.receive(server_response) || server_response.tag == TAG_ERR) {
      std::cerr << server_response.data ;
    }
  } else if (command == "/quit") {
      Message user_quit(TAG_QUIT, "");
      connection.send(user_quit);
      if (!connection.receive(server_response) || server_response.tag == TAG_ERR) {
        std::cerr << server_response.data << std::endl;
        exit(1);
      }
      break;
   } else if (command == "/join") {
std::string room_name;
    if (ss >> room_name) {
        Message join_message(TAG_JOIN, room_name);
        if (connection.send(join_message)) {
            // Successfully sent join message, now wait for a response
            if (connection.receive(server_response)) {
                if (server_response.tag == TAG_ERR) {
                    // Server sent an error response to join request
                    std::cerr << server_response.data ;
                    continue;
                } else if (server_response.tag == TAG_OK) {
                    // Successfully joined the room
                    std::cout << "Joined room: " << room_name << std::endl;
                }
            } else {
                // Failed to receive a response after sending join message
                std::cerr << "No response received after join request." ;
            }
        } else {
            // Failed to send join message
            std::cerr << "Failed to send join message." ;
        }
    } else {
        std::cerr << "Incorrect Syntax: /join [room_name]" ;
    }
   } else if (command == "/leave") {
      std::getline(ss, input, '\n');
      Message leaveMessage(TAG_LEAVE, input);
      connection.send(leaveMessage);
      if (!connection.receive(server_response) || server_response.tag == TAG_ERR) {
        std::cerr << server_response.data << std::endl;
      }
   } 

  }
  connection.close();
 return 0;
}