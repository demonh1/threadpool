#ifndef TASKS_H_
#define TASKS_H_

#include <boost/function.hpp>

namespace threadpool{
namespace tasks{
enum Priority { HIGH,NORMAL,LOW };
struct Task {
	boost::function<void()> task;
	Priority priority;
	Task(boost::function<void()> const& task, Priority priority) : task(task), priority(priority) {}
		
	virtual ~Task() {}
	};

	} // namespace tasks
} // namespace threadpool
#endif
