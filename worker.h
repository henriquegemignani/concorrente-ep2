#ifndef WORKER_H_
#define WORKER_H_

#include <pthread.h>

typedef void* (*WorkerFunction)(void*);

class Worker {
  public:
    Worker(WorkerFunction f, void* data) : function_(f), data_(data) {
        pthread_attr_init(&attr_);
        pthread_attr_setdetachstate(&attr_, PTHREAD_CREATE_JOINABLE);
    }

    ~Worker() {
        pthread_attr_destroy(&attr_);
    }

    void Run() {
        pthread_create(&thread_, &attr_, function_, data_);
    }

    void Join() {
        pthread_join(thread_, NULL);
    }

  private:
    pthread_t thread_;
    pthread_attr_t attr_;
    WorkerFunction function_;
    void* data_;
};

#endif /* WORKER_H_ */
