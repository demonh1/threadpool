#ifndef WORKER_H
#define WORKER_H

#include "thread_pool.h"

namespace threadpool
{

class Worker
{

public:
    explicit Worker(ThreadPool* pool);

    bool start();

    void join();
    void interrupt();
private:
    void run();
    ThreadPool* pool_;
    boost::thread thread_; // execution thread
};

inline Worker::Worker(ThreadPool* pool)
    : pool_(pool)
{ }

inline void Worker::join()
{
    if (thread_.joinable())
        thread_.join();

}

inline void Worker::interrupt()
{
    thread_.interrupt();
    thread_.join();
}

inline bool Worker::start()
{
    thread_ = boost::thread(boost::bind(&Worker::run,this));
    return thread_.joinable();
}

inline void Worker::run()
{
    try
    {
        while (pool_->runTask(this)) {  }
    }
    catch (...)
    {
        pool_->workerTerminatedUnexpectedly(this);

        throw;
    }
}

} // namespace threadpool

#endif // WORKER_H
