#include "thread_pool.h"
#include "worker.h"

using namespace threadpool;

ThreadPool::ThreadPool(size_t minPoolSize, size_t maxPoolSize, size_t keepAlive) :
	minPoolSize_(minPoolSize),
	maxPoolSize_(maxPoolSize),
	keepAlive_(keepAlive),
	poolSize_(0),
	activeWorkers_(0),
	shutdown_(false) {
		init();
   	}
ThreadPool::ThreadPool(size_t poolSize) : 
	minPoolSize_(8),
	maxPoolSize_(poolSize), 
	keepAlive_(10), 
	poolSize_(0), 
	activeWorkers_(0),
 	shutdown_(false) {
		init();
	}	

void ThreadPool::init(){
	boost::unique_lock<Mutex> lock(mutex_);
	for(int i = 0; i < minPoolSize_; ++i)
		addThread();
}

void ThreadPool::drain() {
	boost::unique_lock<Mutex> lock(mutex_);
	while(activeWorkers_ > 0 || !tasks_.empty())
		drainCv_.wait(lock);
}

bool ThreadPool::addThread() {
	Worker* worker = new Worker(this); 
	if(worker->start() ) {
		++poolSize_;
		++activeWorkers_;
		return true;
	}
	delete worker;
   return false;	
}
bool ThreadPool::stop() {
	drain();
	return shutdown_ = true;
}
void ThreadPool::shutdown() {
	boost::unique_lock<Mutex> lock(mutex_);
	shutdown_ = true;
	taskCv_.notify_all();

	while(poolSize_ > 0) 
		drainCv_.wait(lock);

	std::vector<Worker*>::iterator it = terminated_.begin();
	for(; it != terminated_.end(); ++it)
		(*it)->join();

	terminated_.clear();
}

void ThreadPool::shutdownNow() {
	boost::unique_lock<Mutex> lock(mutex_);
	shutdown_ = true;

	while (poolSize_ > 0)
	   	drainCv_.wait(lock);

	std::vector<Worker*>::iterator it = terminated_.begin();
	for (; it != terminated_.end(); ++it)
		(*it)->interrupt();

	terminated_.clear();
}

bool ThreadPool::execute(tasks::Task* task) {
	boost::unique_lock<Mutex> lock(mutex_);

	if(shutdown_) return false;

	if(poolSize_ < minPoolSize_)
		addThread();
	else if(poolSize_ < maxPoolSize_ && (tasks_.size() + activeWorkers_ >= poolSize_))
		// add worker
		addThread();
//
	if(poolSize_ == 0 )
	   	return false;

	//task push
	tasks_.insert(std::make_pair(task->priority, task->task));
	taskCv_.notify_one();

   return true;	
}

void ThreadPool::workerTerminated(Worker* worker, bool expected) {
	--poolSize_;
	--activeWorkers_;

	if(!expected) {
		// add new thread, if necessary
		if(!shutdown_ && poolSize_ < minPoolSize_)
		   addThread();	
	}

	terminated_.push_back(worker);
	drainCv_.notify_all();
}

void ThreadPool::workerTerminatedUnexpectedly(Worker* worker) {
	boost::unique_lock<Mutex> lock(mutex_);
	workerTerminated(worker, false);
}

bool ThreadPool::runTask(Worker* worker) {
	boost::function<void()> task;

	boost::unique_lock<Mutex> lock(mutex_);

	if(!terminated_.empty()) {
		std::vector<Worker*>::iterator it = terminated_.begin();
		for(; it != terminated_.end(); ++it)
			(*it)->join();

		terminated_.clear();

	}
	while(tasks_.empty() && !shutdown_) {
		--activeWorkers_;

		drainCv_.notify_all();
		if(poolSize_ > maxPoolSize_) {
			//wait up to keepAlive
			taskCv_.timed_wait(lock,boost::get_system_time() + boost::posix_time::milliseconds(keepAlive_));
			if(poolSize_ > minPoolSize_ && tasks_.empty()) {
				++activeWorkers_;
				workerTerminated(worker,true);
				return false; // break out of run loop
			}

		} else {
			taskCv_.wait(lock);
	   	}
		++activeWorkers_;
	}
	if(!shutdown_) {
		task = tasks_.begin()->second;
		tasks_.erase(tasks_.begin());
	}
	else {
		workerTerminated(worker,true);
			return false;
	}
		lock.unlock();

		if(task) task();

	return true;
}
