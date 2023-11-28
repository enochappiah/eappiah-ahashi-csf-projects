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
  connection.send(slogin_message);


  Message server_response;
  if (!connection.receive(server_response) || server_response.tag == TAG_ERR) {
    std::cerr << server_response.data << std::endl;
    exit(1);
  }


 // TODO: loop reading commands from user, sending messages to
 //       server as appropriate

 std::string input;
 while (true) {
   if (!std::getline(std::cin, input)) {
     break;
   }
   input = trim(input);
   std::stringstream ss (input);
   std::string command;
   ss >> command;

   //TODO remove just for testing purposes
   std::cout << ss.str() << std::endl; 
   std::cout << command << std::endl; 


   if (ss.str() == "/quit") {
     Message user_quit(TAG_QUIT, "");
       connection.send(user_quit);
       break;
   }


   if (ss.str() == "/join") {
     //TODO Handle join
     std::getline(ss, input);
     Message joinMessage(TAG_JOIN, input);
     //TODO CHECK MESSAGE/INPUT for correct information

   } 
   if (ss.str() == "/leave") {
    //TODO handle leave
   }
   else {
     // normal message
     Message send_to_everyone(TAG_SENDALL, input);
       connection.send(send_to_everyone);
   }
 }


   connection.close();
 return 0;
}