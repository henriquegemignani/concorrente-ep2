#ifndef MUTEX_H_
#define MUTEX_H_

#include <pthread.h>

class Mutex {
  public:
    Mutex() { pthread_mutex_init(&mutex_, NULL); }
    Mutex(const Mutex&) { pthread_mutex_init(&mutex_, NULL); }
    ~Mutex() {
        pthread_mutex_destroy(&mutex_);
    }
    void Lock() {
        pthread_mutex_lock(&mutex_);
    }
    void Unlock() {
        pthread_mutex_unlock(&mutex_);
    }

  private:
    pthread_mutex_t mutex_;
};

#endif /* MUTEX_H_ */
