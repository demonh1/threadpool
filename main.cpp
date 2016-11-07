#include <iostream>
#include <boost/atomic.hpp>
#include <inttypes.h> 

#include "thread_pool.h"
#include "tasks.h"


using namespace threadpool::tasks;

namespace {
boost::atomic<int64_t> val(0);

void testAdd(bool inc){
	if(inc){
	for (int i = 0; i < 1E6; ++i)
      	++val;
        }
 }

void testReduce(bool dec){
	if(dec){
	for (int i = 0; i < 1E5; ++i)
      	--val;
        }
 }
boost::atomic<int64_t> val1(0);
void testFunc() {
  volatile uint16_t val = 0;
  while (++val < (std::numeric_limits<uint16_t>::max()))
    ;
  ++val1;
}
boost::atomic<int64_t> val2(0);
void test2Add(bool inc){
	if(inc){
	for (int i = 0; i < 1E7; ++i)
      	++val2;
        }
 }
void test2Reduce(bool dec){
	if(dec){
	for (int i = 0; i < 1E6; ++i)
      	--val2;
        }
	}
 
} // namespace
//////////////

int main() {
	
	const size_t kMinSize = 16;
    	const size_t kMaxSize = 256;
    	const size_t kKeepAliveMs = 1000;

    	threadpool::ThreadPool pool(kMinSize, kMaxSize, kKeepAliveMs);

		std::cout << "Test1 ... \n";
	 
    for (int i = 0; i < 1E4; ++i) {
		Task* task = new Task(boost::bind(testFunc), HIGH);
        pool.execute(task);
 	 
    }
    pool.drain();

std::cout << "val = " << val1.load() << "\n";

std::cout << "Test2 >> stop() ... \n";
threadpool::ThreadPool pool1(50);
bool stopped = false;
for (int i = 0; i < 10; ++i) {
        pool1.execute(new Task(boost::bind(testReduce, true),NORMAL));
    } 
		 
    for (int i = 0; i < 10; ++i) {
       pool1.execute(new Task(boost::bind(testAdd, true), HIGH));
 	 
    }
    stopped = pool1.stop();

	if (stopped) std::cout << "val = " << val.load() << "\n"; 

	std::cout << "Test3 >> shutdownNow() ... \n";
	threadpool::ThreadPool pool2(kMinSize, kMaxSize, kKeepAliveMs);
	
    for (int i = 0; i < 10; ++i) {
	   	pool2.execute(new Task(boost::bind(test2Add, true), HIGH));
 	 
    }

	for (int i = 0; i < 10; ++i) {
        pool2.execute(new Task(boost::bind(test2Reduce, true),NORMAL));
    } 
// force stop
pool2.shutdownNow();
std::cout << "val = " << val2.load() << "\n";

}
