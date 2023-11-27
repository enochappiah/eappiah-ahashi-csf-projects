#include <cstdio>
#include <cstring>
#include <exception>
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"
#include "cstring"

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
  try {
  connection.connect(server_hostname, server_port);
  } catch (const std::exception &e) {
    //TODO fill later
  }

  // TODO: send slogin message
  Message sloginMessage (TAG_SLOGIN, username);
  


  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate

 

  

  




  // rio_t rio;
  // rio_readinitb(&rio, fd);
  // char buf[1000];
  // ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));
  // if (n < 0) {
  //   return 1; // error reading data from user
  // }
  

  //rio_t rio;
  //rio_readinitb(&rio, fd);

  // while (true) {
  //   std::string input;
  //   //std::cout << "> ";
  //   // std::getline(std::cin, input);

  //   // if (input.empty()) {
  //   //   continue; // Skip empty input
  //   // }
  //   ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));
  //   if (n < 0) {
  //   return 1; // error reading data from user
  //   }

  //   // TODO: Process the user command and send the appropriate message to the server
  //   if (strcmp(buf, const char *s2))
  //   if (buf.substr(0, 5) == "join:") {
  //     // Send join message
  //     rio_writen(fd, input.c_str(), input.length());
  //     rio_writen(fd, "\n", 1);
  //   } else if (input.substr(0, 7) == "slogout") {
  //     // Send slogout message and exit the loop
  //     rio_writen(fd, "slogout\n", 8);
  //     break;
  //   } else {
  //     // Assume the input is a regular message
  //     // Send the message to the server
  //     rio_writen(fd, input.c_str(), input.length());
  //     rio_writen(fd, "\n", 1);
  //   }
  // }

  close(fd); // Close the connection

  return 0;
}
#include <exception>
#include <iostream>
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
  try {
    connection.connect(server_hostname, server_port);
  } catch (const std::exception &e) {
    // Fill in
  }
  
  Message slogin_message(TAG_SLOGIN, username);

  try {
    connection.send(slogin_message);
  } catch (const std::exception &e) {
    // Fill in
  }

  std::string input;
  while (true) {
    if (!std::getline(std::cin, input)) {
      break;
    }

    input = trim(input);

    if (input == "/quit") {
      Message user_quit(TAG_QUIT, "");
      try {
        connection.send(user_quit);
      } catch (const std::exception &e) {
        // Fill in
      }
      break;
    }

    if (input[0] == '/') {

      // Handle join and leave

    } else {
      // normal message
      Message send_to_everyone(TAG_SENDALL, input);
      try {
        connection.send(send_to_everyone);
      } catch (const std::exception &e) {
        // Fill in
      }
    }



  }




  // TODO: send slogin message

  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate

  try {
    connection.close();
  } catch (const std::exception &e) {
    // Fill in
  }

  return 0;
}