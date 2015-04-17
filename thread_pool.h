#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <cstddef>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <map>
#include "tasks.h"

namespace threadpool {

class Worker;
typedef boost::mutex Mutex;

class ThreadPool {
public:
	
    ThreadPool(size_t minPoolSize, size_t maxPoolSize, size_t keepAlive);
    
	ThreadPool (size_t poolSize);


    ~ThreadPool() {
        shutdown();
    }

    //
    // Public ThreadPool implementation
    //

    bool execute(tasks::Task* task); // execute a task
    
    void drain(); // wait for all tasks to complete
	
    bool stop(); // stop pool	
    void shutdownNow(); // force stop pool

    inline bool empty() { 
	boost::unique_lock<Mutex> lock(mutex_);
    	return tasks_.empty();
	}
    inline size_t size(){  // get  number of executing tasks 
	boost::unique_lock<Mutex> lock(mutex_);
   	return poolSize_;
	}

    inline size_t queueSize() { // get number of pending tasks in the pool
	boost::unique_lock<Mutex> lock(mutex_);
    	return tasks_.size(); 
	}

    /// -------------------------- 
    /// execute an available task
    ///
    /// called by worker threads to execute the next available task,
    /// or to block until a task is available
    ///
    /// @return  true if the worker should continue, false -- to exit
   /// -------------------------- 
    bool runTask(Worker* worker);

    ///-------------------------- 
    // Signal that a woker has terminated unexpectedly (exception)
    ///--------------------------
    void workerTerminatedUnexpectedly(Worker* worker);

private: 
    void init();
    void shutdown();
    bool addThread();
    void workerTerminated(Worker* worker, bool expected);

     size_t minPoolSize_;
     size_t maxPoolSize_;
     size_t keepAlive_;

     size_t poolSize_; // existing workers in the pool
     size_t activeWorkers_; // workers running tasks
     bool shutdown_; // shutdown flag
	    

    std::multimap<tasks::Priority, boost::function<void()> > tasks_; // queue tasks
    std::vector<Worker*> terminated_; // terminated workers to join

    Mutex mutex_;
    boost::condition_variable_any taskCv_;
    boost::condition_variable_any drainCv_;
};

} // threadpool namespace

#endif //THREADPOOL_H_
