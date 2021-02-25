#include <iostream>
#include <unistd.h>

#include "env.h"
#include "threadpool.h"

#define THREAD_COUNT 300

using std::cout;
using std::endl;

void Thread1(int id) {
	int count = THREAD_COUNT;
	while(count --) {
		printf("------- *%d* running ------- \n");
		sleep(1);
	}
}

void PrintEnvInfo(Env *env) {
	if (evn == nullptr) {
		return;
	}

	int low_thread_nums;
	int high_thread_nums;
	uint64_t time;

	time = env->NowMicros();
	low_thread_nums = env->GetThreadPoolQueueLen(Env::Priority::LOW);
	high_thread_nums = env->GetThreadPoolQueueLen(Env::Priority::HIGH);

	cout << "time : " << env->TimeToString(time) << endl;
	cout << "low thread nums: " << low_thread_nums << endl;
	cout << "high thread nums: " << high_thread_nums << endl;

}

int main(int argc, char *argv[]) {
	Env *env = Env::Default();

	env->SetBackgroundThreads(3, Env::Priority::LOW);
	env->SetBackgroundThreads(7, Env::Priority::HIGH);

	for (int i = 0;i < 10; i ++) {
		if (i % 2 == 0) {
			env->Schedule(Thread1, i, Env::Priority::LOW);
		} else {
			env->Schedule(Thread1, i, Env::Priority::HIGH);
		}

		PrintEnvInfo(env);
	}

	return 0;
}
