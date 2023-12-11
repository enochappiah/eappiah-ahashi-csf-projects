Sample README.txt

ahashi1 & eappiah4 worked together on lots of debugging and implementation/testing

Eventually your report about how you implemented thread synchronization
in the server should go here

We mainly put our critical sections in:
- enqueue and dequeue in message_queue.cpp
- add_member and remove_member in room.cpp

Similarly, the reason we did so in message_queue.cpp is because the message queue is a shared resource amongst the different threads. Thus, since it can be accessed by different threads, we add critical sections in those methods explained above. 

The reason we did so in room.cpp is because the members set is a shared resource amongst the threads. Thus, because the different threads can access the members set, we add critical sections in the methods explained above.

In terms of why we used the synchronization primitives we used, the mutexes made sure that only a single thread could access a shared resource at a time. For example, the "pthread_mutex_t lock" found in room.h synchronizes the accesses to the members set. Additionally, the "pthread_mutex_t m_lock" in message_queue.h synchronizes the accesses to the message queue. The other primitive, the semaphore, manages and helps threads be aware of when a message was added to the queue. 

Finally, we avoided synchronization hazards using multiple methods. First, to avoid race conditions we made sure to use Guards. These ensure that shared resources are not accessed at the same time by multiple threads. To avoid deadlocks, we also use Guards which are safer than pthread_mutex_lock() due to the block scoping that ensures the release of the lock. 