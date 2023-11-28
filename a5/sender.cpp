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
  connection.connect(server_hostname, server_port);

   // TODO: send slogin message
  Message slogin_message(TAG_SLOGIN, username);
  connection.send(slogin_message);


 // TODO: loop reading commands from user, sending messages to
 //       server as appropriate

 std::string input;
 while (true) {
   if (!std::getline(std::cin, input)) {
     break;
   }

   input = trim(input);
   if (input == "/quit") {
     Message user_quit(TAG_QUIT, "");
       connection.send(user_quit);
       break;
   }


   if (input[0] == '/') {
     // Handle join and leave
   } else {
     // normal message
     Message send_to_everyone(TAG_SENDALL, input);
       connection.send(send_to_everyone);
   }
 }


   connection.close();
 return 0;
}