#ifndef WORKER_H_
#define WORKER_H_

#include <pthread.h>

class Worker;

typedef void* (*WorkerFunction)(Worker*);
void* WorkerAux(void* data);

class Worker {
  public:
    Worker(WorkerFunction f, void* data) : function_(f), data_(data), id_(id_generator++) {
        pthread_attr_init(&attr_);
        pthread_attr_setdetachstate(&attr_, PTHREAD_CREATE_JOINABLE);
    }

    ~Worker() {
        pthread_attr_destroy(&attr_);
    }

    void Run() {
        pthread_create(&thread_, &attr_, WorkerAux, this);
    }

    void Join() {
        pthread_join(thread_, NULL);
    }

    WorkerFunction function() { return function_; }
	void* data() { return data_; }
	size_t id() { return id_; }

  private:
    pthread_t thread_;
    pthread_attr_t attr_;
    WorkerFunction function_;
    void* data_;
    size_t id_;
    static size_t id_generator;
};

void* WorkerAux(void* data) {
	Worker* w = static_cast<Worker*>(data);
	return w->function()(w);
}

size_t Worker::id_generator = 0;

#endif /* WORKER_H_ */
