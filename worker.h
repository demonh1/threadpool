#ifndef WORKER_H_
#define WORKER_H_

#include "thread_pool.h"

namespace threadpool{
class Worker {

public:
    explicit Worker(ThreadPool* pool);
    
    bool start();
    
    void join(); // join the thread
    void interrupt();  
private:
    void run(); // run loop
    ThreadPool* pool_; 
    boost::thread thread_; // execution thread
};

inline Worker::Worker(ThreadPool* pool)
    : pool_(pool) { }

inline void Worker::join() {
    if (thread_.joinable()) 
        thread_.join(); 

}
inline void Worker::interrupt() {
	thread_.interrupt();
    thread_.join(); 
}
inline bool Worker::start() {
    //assert(!thread_.joinable());
    thread_ = boost::thread(boost::bind(&Worker::run,this));
    return thread_.joinable();
}

inline void Worker::run() {
    try {
        while (pool_->runTask(this)) {  }
    } catch (...) {
        pool_->workerTerminatedUnexpectedly(this);

        // re-throw
        throw;
    	}
	}	
} // namespace threadpool
#endif
