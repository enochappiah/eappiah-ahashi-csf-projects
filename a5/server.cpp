#include <cstddef>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <cctype>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

struct helper_struct {
  Connection *connection;
  Server *server;
};

void sender_helper(helper_struct *help, User *user) {
  Message message_to_receive;
  Message message_to_send;
  Room *room;

  while (help->connection->is_open()) {
    if (!help->connection->receive(message_to_receive)) {
      //std::cerr << "Could not receive message from sender";
      // message_to_send = Message(TAG_ERR, "Could not receive message from sender\n");
      message_to_send.tag = TAG_ERR;
      message_to_send.data = "Could not receive message from sender";
      //std::cout << message_to_receive.data << "this is it!" << std::endl;
      help->connection->send(message_to_send);
      break;
    }

    if (message_to_receive.tag == TAG_SENDALL) {
      //room = help->server->find_or_create_room(message_to_receive.data);
      std::string trimmedData = message_to_receive.data.substr(0, message_to_receive.data.length() - 1); 
      room->broadcast_message(user->username, trimmedData);
      std::cout << "Sendall " << trimmedData << std::endl;
      // message_to_send = Message(TAG_OK, "Successfully sent message!");
      message_to_send.tag = TAG_OK;
      message_to_send.data = "message sent";
      std::cout << message_to_send.data << std::endl;
      help->connection->send(message_to_send);
    } else if (message_to_receive.tag == TAG_JOIN) {
      std::string trimmedData = message_to_receive.data.substr(0, message_to_receive.data.length() - 1);
      room = help->server->find_or_create_room(trimmedData);
      std::cout << "Join " << trimmedData << std::endl;
      message_to_send.tag = TAG_OK;
      message_to_send.data = "joined room " + room->get_room_name();
      help->connection->send(message_to_send);
    } else if (message_to_receive.tag == TAG_LEAVE || message_to_receive.tag == TAG_QUIT) {
      if (message_to_receive.tag == TAG_LEAVE) {
        message_to_send.tag = TAG_OK;
      message_to_send.data = "Sending ok for leave";
      } else {
        message_to_send.tag = TAG_OK;
      message_to_send.data = "Sending ok for quit";
      }
      help->connection->send(message_to_send);
      room = nullptr;
    }

    if(message_to_send.tag == TAG_ERR) {
      break;
    }

    if (message_to_send.tag == TAG_QUIT) {
      break;
    }
  }

}

void receiver_helper(helper_struct *help, User *user) {
    Message message_to_receive;
    Message message_to_send;
    Room *room;
    // Room *room = nullptr;

    // while (help->connection->is_open()) {
    //     if (!help->connection->receive(message_to_receive)) {
    //         message_to_send.tag = TAG_ERR;
    //         message_to_send.data = "Error receiving message";
    //         help->connection->send(message_to_send);
    //         continue; // Continue listening for new messages instead of returning
    //     }

    //     if (message_to_receive.tag == TAG_JOIN) {
    //         std::string trimmedData = message_to_receive.data.substr(0, message_to_receive.data.length() - 1);
    //         Room *room = help->server->find_or_create_room(trimmedData);
    //         room->add_member(user);
    //         message_to_send.tag = TAG_OK;
    //         message_to_send.data = "welcome";
    //         help->connection->send(message_to_send);
    //     }

    //     // Other message handling code (if needed) goes here
    // }

    // if (room) {
    //     room->remove_member(user);
    // }
    // delete user;

    //bool incoming_result = help->connection->receive(message_to_receive);
    if(!help->connection->receive(message_to_receive) || message_to_receive.tag != TAG_JOIN) {
      message_to_send.tag = TAG_ERR;
      message_to_send.data = "No valid message received";
      help->connection->send(message_to_send);
      return;
    }

    std::string room_name = message_to_receive.data.substr(0,message_to_receive.data.length() - 1);
    room = help->server->find_or_create_room(room_name);
    room->add_member(user);
    message_to_send.tag = TAG_OK;
    message_to_send.data = "welcome";

    help->connection->send(message_to_send);

    while(help->connection->is_open()) {
      Message *loop = user->mqueue.dequeue();

      if(loop == nullptr) {
        continue;
      }

      //bool result = help->connection->send(*loop);
      if (!help->connection->send(*loop)) {
        break;
      }
      delete loop;

      // if (!result) {
      //   break;
      // }
    }
      room->remove_member(user);
      delete user;
    }




////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

void *worker(void *arg) {
  pthread_detach(pthread_self());

  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  helper_struct *help = (helper_struct *) arg;
  //Connection *connection = help->connection;
  //Server *server = help->server;

  Message login_message;
  if (!help->connection->receive(login_message)) {
    std::cerr << "Failed to receive login message";
    return nullptr;
  }

Message message_to_receive;
  if (login_message.tag == TAG_RLOGIN || login_message.tag == TAG_SLOGIN) {
    std::string trimmedData = login_message.data.substr(0, login_message.data.length() - 1);
    User *user = new User(trimmedData);

    message_to_receive.tag = TAG_OK;
    message_to_receive.data = "logged in as " + user->username;
    help->connection->send(message_to_receive);

    if (login_message.tag == TAG_RLOGIN) {
      std::cout << user->username << std::endl;
      receiver_helper(help, user); // Implement
    } else {
      std::cout << "alice? bob?" << std::endl;
      sender_helper(help, user);
    }

  free(help);
  return nullptr;
  }

  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)

  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // TODO: initialize mutex
  pthread_mutex_init(&m_lock, NULL);
}

Server::~Server() {
  // TODO: destroy mutex
  pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
  // TODO: use open_listenfd to create the server socket, return true
  //       if successful, false if not
  std::string port_string = std::to_string(m_port);
  m_ssock = open_listenfd(port_string.c_str());
  
  if (m_ssock < 0) {
    std::cerr << "Failed to create server socket";
    return false;
  }

  return true;
}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
  while (true) {
    int clientfd = Accept(m_ssock, NULL, NULL);
    if (clientfd < 0) {
      std::cerr << "Error in accepting client connection";
    }

    Connection *conn = new Connection(clientfd);
    helper_struct *data = new helper_struct{conn, this};

    // helper_struct *data = new helper_struct();
    // data->connection = new Connection(clientfd);
    // data->server = this;

    pthread_t thr_id;
    pthread_create(&thr_id, nullptr, worker, static_cast<void *>(data));
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  Guard guard(m_lock);

  RoomMap::iterator iterator = m_rooms.find(room_name);
  if (iterator != m_rooms.end()) {
    return iterator->second;
  }

  Room *newRoom = new Room(room_name);
  m_rooms[room_name] = newRoom;

  return newRoom;
}
