#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"
#include <pthread.h>

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  // TODO: initialize the mutex
  pthread_mutex_init(&lock,NULL);
}

Room::~Room() {
  // TODO: destroy the mutex
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  // TODO: add User to the room
    Guard guard(lock);
    members.insert(user);
}

void Room::remove_member(User *user) {
  // TODO: remove User from the room
  Guard guard(lock);
  members.erase(user);
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  // TODO: send a message to every (receiver) User in the room
  std::string message = room_name + ":" + sender_username + ":" + message_text;

  for (User *user : members) {
        // Create a new Message object for each user
        Message *msg = new Message(TAG_DELIVERY, message);

        // Enqueue the message to the user's message queue
        user->mqueue.enqueue(msg);
    }
}
