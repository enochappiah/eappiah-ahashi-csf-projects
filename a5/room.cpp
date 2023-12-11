#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"
#include <pthread.h>

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  //initialize the mutex
  pthread_mutex_init(&lock,NULL);
}

Room::~Room() {
  //destroy the mutex
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  // add User to the room
    Guard guard(lock);
    members.insert(user);
}

void Room::remove_member(User *user) {
  // remove User from the room
  Guard guard(lock);
  members.erase(user);
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
    Guard guard(lock);
    
    UserSet::iterator iterator;
    std::string message_to_send = room_name + ":" + sender_username + ":" + message_text;

    for(iterator = members.begin(); iterator != members.end(); iterator++) {
      Message * message = new Message(TAG_DELIVERY, message_to_send);
      (*iterator)->mqueue.enqueue(message);
    }
}
