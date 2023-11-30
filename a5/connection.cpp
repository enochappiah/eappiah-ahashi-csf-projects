#include <iostream>
#include <sstream>
#include <cctype>
#include <cassert>
#include <stdio.h>
#include <string>
#include "csapp.h"
#include "message.h"
#include "connection.h"


Connection::Connection()
 : m_fd(-1)
 , m_last_result(SUCCESS) {
}


Connection::Connection(int fd)
 : m_fd(fd)
 , m_last_result(SUCCESS) {
 // TODO: call rio_readinitb to initialize the rio_t object
 rio_readinitb(&m_fdbuf, m_fd);
}


void Connection::connect(const std::string &hostname, int port) {
 // TODO: call open_clientfd to connect to the server
 // TODO: call rio_readinitb to initialize the rio_t object

 std::string port_string = std::to_string(port);

 m_fd = open_clientfd(hostname.c_str(), port_string.c_str());
 if (m_fd < 0) {
   std::cerr << "Couldn't connect to server\n";
 }


 rio_readinitb(&m_fdbuf, m_fd);
}


Connection::~Connection() {
 // TODO: close the socket if it is open
 if (is_open()) {
   Close(m_fd);
 }
}


bool Connection::is_open() const {
 // TODO: return true if the connection is open
 return m_fd >= 0;
}


void Connection::close() {
 // TODO: close the connection if it is open
 if (is_open()) {
  m_fd = -1;
 }
}


bool Connection::send(const Message &msg) {
 // TODO: send a message
 // return true if successful, false if not
 // make sure that m_last_result is set appropriately
 std::string message = msg.tag + ":" + msg.data + "\n";
 //std::cout << message << std::endl;

 if (message.length() > Message::MAX_LEN && msg.tag != TAG_ERR && msg.tag != TAG_OK && msg.tag != TAG_SLOGIN && msg.tag != TAG_RLOGIN && msg.tag != TAG_JOIN && msg.tag != TAG_LEAVE && msg.tag != TAG_SENDALL && msg.tag != TAG_SENDUSER && msg.tag != TAG_QUIT && msg.tag != TAG_DELIVERY && msg.tag != TAG_EMPTY) {
   m_last_result = INVALID_MSG;
   //std::cerr << "ERRROR HERE" << std::endl;
   return false;
 }


 Rio_writen(m_fd, message.c_str(), message.length()); // use a try-catch??
 m_last_result = SUCCESS;
 return true;
}


bool Connection::receive(Message &msg) {
 // TODO: receive a message, storing its tag and data in msg
 // return true if successful, false if not
 // make sure that m_last_result is set appropriately


 char buf[Message::MAX_LEN];


 ssize_t num_bytes = Rio_readlineb(&m_fdbuf, buf, Message::MAX_LEN);

// TESTING PURPOSES
//  if (num_bytes > 0) { 
//     printf("Received from server: %s ", buf);
//   }

 

 if (num_bytes <= 0) {
   m_last_result = EOF_OR_ERROR; // invalid_msg?
   printf("Received from server: %s ", buf);
   return false;
 }

  printf("Outside Received from server: %s ", buf);

 // null terminator?


 std::string received_message(buf);
 size_t colon_index = received_message.find(":");
 msg.tag = received_message.substr(0, colon_index);
 msg.data = received_message.substr(colon_index + 1);


 m_last_result = SUCCESS;
 return true;
}
