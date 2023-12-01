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
 rio_readinitb(&m_fdbuf, m_fd);
}


void Connection::connect(const std::string &hostname, int port) {

 std::string port_string = std::to_string(port);

 m_fd = open_clientfd(hostname.c_str(), port_string.c_str());
 if (m_fd < 0) {
   std::cerr << "Couldn't connect to server\n";
   exit(1);
 }

 rio_readinitb(&m_fdbuf, m_fd);
}


Connection::~Connection() {
 if (is_open()) {
   Close(m_fd);
 }
}


bool Connection::is_open() const {
 return m_fd >= 0;
}


void Connection::close() {
 if (is_open()) {
  m_fd = -1;
 }
}


bool Connection::send(const Message &msg) {
  
 std::string message = msg.tag + ":" + msg.data + "\n";
 if (message.length() > Message::MAX_LEN && msg.tag != TAG_ERR && msg.tag != TAG_OK && msg.tag != TAG_SLOGIN && msg.tag != TAG_RLOGIN && msg.tag != TAG_JOIN && msg.tag != TAG_LEAVE && msg.tag != TAG_SENDALL && msg.tag != TAG_SENDUSER && msg.tag != TAG_QUIT && msg.tag != TAG_DELIVERY && msg.tag != TAG_EMPTY) {
   m_last_result = INVALID_MSG;
   return false;
 }

 Rio_writen(m_fd, message.c_str(), message.length()); 
 m_last_result = SUCCESS;
 return true;
}


bool Connection::receive(Message &msg) {

 char buf[Message::MAX_LEN];


 ssize_t num_bytes = Rio_readlineb(&m_fdbuf, buf, Message::MAX_LEN);

 if (num_bytes <= 0) {
   m_last_result = EOF_OR_ERROR;
   return false;
 }
 
 std::string received_message(buf);
 size_t colon_index = received_message.find(":");
 msg.tag = received_message.substr(0, colon_index);
 msg.data = received_message.substr(colon_index + 1);


 m_last_result = SUCCESS;
 return true;
}
