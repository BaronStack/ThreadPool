#include <iostream>
#include <unistd.h>
#include <atomic>

#include "threadpool.h"
#include "threadpool_imp.h"

#define THREAD_COUNT 10

using std::cout;
using std::endl;

struct Cxt{
	int thread_id;
	std::atomic<int>* last_id;

	Cxt(std::atomic<int>* p, int i) :
	thread_id(i),last_id(p) {}
};

static void Thread1(void *ptr) {
	Cxt *cxt = reinterpret_cast<Cxt*>(ptr);
	int count = THREAD_COUNT;
	while(count --) {
		printf("------- *%d* running ------- \n", cxt->thread_id);
		sleep(2);
	}
}

static void Thread2(void *ptr) {
	Cxt *cxt = reinterpret_cast<Cxt*>(ptr);
	int count = THREAD_COUNT;
	while(count --) {
		printf("------- *%d* running ------- \n", cxt->thread_id);
		sleep(2);
	}
}

static void finish1(void *ptr) {
	Cxt *cxt = reinterpret_cast<Cxt*>(ptr);
	printf("Finish excute %d\n", cxt->thread_id);
	delete cxt;
}

int main(int argc, char *argv[]) {
	std::atomic<int> last_id(0);

	auto* thread_pool1 = new ThreadPoolImpl();
	auto* thread_pool2 = new ThreadPoolImpl();

	// Set the background threads in threadpool
	// threadpool1 have 3 threads and with lower IO priority
	// threadpool2 have 7 threads and with lower CPU priority
	thread_pool1->SetBackgroundThreads(3);
	thread_pool2->SetBackgroundThreads(7);

	thread_pool1->LowerIOPriority();
	thread_pool2->LowerCPUPriority();

	for (int i = 0, j = 0;i < 10; j++,i ++) {
		Cxt cxt_i(&last_id, i);
		Cxt cxt_j(&last_id, j);
		if (i % 2 == 0 ) {
			thread_pool1->Schedule(&Thread1, &cxt_i, &cxt_i, &finish1);
		} else {
			thread_pool2->Schedule(&Thread2, &cxt_j, &cxt_j, &finish1);
		}
	}

	Cxt cxt_us(&last_id, 1);
	thread_pool1->UnSchedule(&cxt_us);
	thread_pool2->UnSchedule(&cxt_us);

	return 0;
}
