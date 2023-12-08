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

void sender_helper(Connection *connection, User *user, Server *server) {
  Message msg;
  while (true) {
    if (!connection->receive(msg)) {
      std::cerr << "Could not receive message from sender";
      break;
    }

    if (msg.tag == TAG_SENDALL) {
      Room *room = server->find_or_create_room(msg.data);
      room->broadcast_message(user->username, msg.data);
    } else if (msg.tag == TAG_JOIN) {
      Room *room = server->find_or_create_room(msg.data);
      room->add_member(user);
    } else if (msg.tag == TAG_LEAVE) {
      Room *room = server->find_or_create_room(msg.data);
      room->remove_member(user);
    } else if (msg.tag == TAG_QUIT) {
      break;
    }

    Message response(TAG_OK, "Message ok");
    connection->send(response);
  }
}

void receiver_helper(Connection *connection, User *user) {
  Message *msg;

    // Continuously check for new messages and send them to the client
    while (true) {
        msg = user->mqueue.dequeue();
        if (msg == nullptr) {
            // No new messages or an error occurred, continue waiting
            continue;
        }

        // Attempt to send the message
        if (!connection->send(*msg)) {
            std::cerr << "Failed to send message to receiver\n";
            break;
        }
    }
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
  helper_struct *help = static_cast<helper_struct*>(arg);
  Connection *connection = help->connection;
  Server *server = help->server;

  Message login_message;
  if (!connection->receive(login_message)) {
    std::cerr << "Failed to receive login message";
  }

  User *user = nullptr; 
  if (login_message.tag == TAG_RLOGIN || login_message.tag == TAG_SLOGIN) {
    user = new User(login_message.data);

    Message server_response(TAG_OK, "Successful login");
    connection->send(server_response);

    if (login_message.tag == TAG_RLOGIN) {
      receiver_helper(connection, user); // Implement
    } else {
      sender_helper(connection, user, server);
    }


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

    Connection *info = new Connection(clientfd);
    pthread_t thr_id;
    pthread_create(&thr_id,NULL, worker, info);
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
