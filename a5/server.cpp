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

void sender_helper(User *user, helper_struct *help) {
  Room *room;
  Message message_to_send;
  Message message_to_receive;

  while (help->connection->is_open()) {
    if (!help->connection->receive(message_to_receive)) {
      help->connection->send(Message(TAG_ERR, "Could not receive message from sender"));
      break;
    }

    if (message_to_receive.tag == TAG_SENDALL) {
      std::string trimmedData = message_to_receive.data.substr(0, message_to_receive.data.length() - 1); 
      room->broadcast_message(user->username, trimmedData);
      help->connection->send(Message(TAG_OK, "message sent"));
    } else if (message_to_receive.tag == TAG_JOIN) {
      std::string trimmedData = message_to_receive.data.substr(0, message_to_receive.data.length() - 1);
      room = help->server->find_or_create_room(trimmedData);
      help->connection->send(Message(TAG_OK, "joined room" + room->get_room_name()));
    } else if (message_to_receive.tag == TAG_LEAVE || message_to_receive.tag == TAG_QUIT) {
      if (message_to_receive.tag == TAG_LEAVE) {
        message_to_send.tag = TAG_OK;
        message_to_send.data = "left";
        
      } else {
        message_to_send.tag = TAG_OK;
        message_to_send.data = "bye!";
      }
      help->connection->send(message_to_send);
      room = nullptr;
    }
  }

}

void receiver_helper(User *user, helper_struct *help) {
    Room *room;
    Message message_to_receive;

    if(!help->connection->receive(message_to_receive) || message_to_receive.tag != TAG_JOIN) {
      help->connection->send(Message(TAG_ERR,"No valid message received"));
      return;
    }

    std::string trimmedData = message_to_receive.data.substr(0,message_to_receive.data.length() - 1);
    room = help->server->find_or_create_room(trimmedData);
    room->add_member(user);
    help->connection->send(Message(TAG_OK, "welcome"));

    while(help->connection->is_open()) {
      Message *messageFromQueue = user->mqueue.dequeue();

      if(!messageFromQueue) {
        continue;
      }

      if (!help->connection->send(*messageFromQueue)) {
        break;
      }
      delete messageFromQueue;
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

  // use a static cast to convert arg from a void* to
  // whatever pointer type describes the object(s) needed
  // to communicate with a client (sender or receiver)
  helper_struct *help = (helper_struct *) arg;
  Message message_to_receive;
  Message login_message;

  if (!help->connection->receive(login_message)) {
    std::cerr << "Failed to receive login message";
    return nullptr;
  }

  // read login message (should be tagged either with
  // TAG_SLOGIN or TAG_RLOGIN), send response

  if (login_message.tag == TAG_RLOGIN || login_message.tag == TAG_SLOGIN) {
    std::string trimmedData = login_message.data.substr(0, login_message.data.length() - 1);
    User *user = new User(trimmedData);

    help->connection->send(Message(TAG_OK, "logged in as " + user->username));

  // depending on whether the client logged in as a sender or
  // receiver, communicate with the client (implementing
  // separate helper functions for each of these possibilities is a good idea)

    if (login_message.tag == TAG_SLOGIN) {
      sender_helper(user, help);
    } else {
      receiver_helper(user, help);
    }

  free(help);
  return nullptr;
  }

  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // initialize mutex
  pthread_mutex_init(&m_lock, NULL);
}

Server::~Server() {
  // destroy mutex
  pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
  // use open_listenfd to create the server socket, return true
  // if successful, false if not
  std::string port_string = std::to_string(m_port);
  m_ssock = open_listenfd(port_string.c_str());
  
  if (m_ssock < 0) {
    std::cerr << "Failed to create server socket";
    return false;
  }

  return true;
}

void Server::handle_client_requests() {
  // infinite loop calling accept or Accept, starting a new
  // pthread for each connected client
  while (true) {
    int clientfd = Accept(m_ssock, NULL, NULL);
    if (clientfd < 0) {
      std::cerr << "Error in accepting client connection";
    }

    Connection *conn = new Connection(clientfd);
    helper_struct *data = new helper_struct{conn, this};

    pthread_t thr_id;
    pthread_create(&thr_id, nullptr, worker, data);
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // return a pointer to the unique Room object representing
  // the named chat room, creating a new one if necessary

  Guard guard(m_lock);
  RoomMap::iterator iterator = m_rooms.find(room_name);
  if (iterator != m_rooms.end()) {
    return iterator->second;
  }

  Room *newRoom = new Room(room_name);
  m_rooms[room_name] = newRoom;
  return newRoom;
}
