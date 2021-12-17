#include <iostream>
#include <unistd.h>
#include <atomic>

#include "env.h"
#include "threadpool.h"

#define THREAD_COUNT 3

using std::cout;
using std::endl;

struct Cxt {
    int thread_id;
    std::atomic<int> *last_id;

    Cxt(std::atomic<int> *p, int i) :
            thread_id(i), last_id(p) {}
};

static void Thread1(void *ptr) {
    Cxt *cxt = reinterpret_cast<Cxt *>(ptr);
    int count = THREAD_COUNT;
    while (count--) {
        printf("------- *%d* running ------- \n", cxt->thread_id);
        sleep(2);
    }
}

static void Thread2(void *ptr) {
    Cxt *cxt = reinterpret_cast<Cxt *>(ptr);
    int count = THREAD_COUNT;
    while (count--) {
        printf("------- *%d* running ------- \n", cxt->thread_id);
        sleep(2);
    }
}

static void finish1(void *ptr) {
    Cxt *cxt = reinterpret_cast<Cxt *>(ptr);
    printf("Finish excute %d\n", cxt->thread_id);
    delete cxt;
}

void PrintEnvInfo(Env *env) {
    if (env == nullptr) {
        return;
    }

    int low_thread_nums;
    int high_thread_nums;
    uint64_t time;

    time = env->NowMicros();
    low_thread_nums = env->GetThreadPoolQueueLen(Env::Priority::LOW);
    high_thread_nums = env->GetThreadPoolQueueLen(Env::Priority::HIGH);

    cout << "time : " << env->TimeToString(time) << endl
         << "low thread nums: " << low_thread_nums << endl
         << "high thread nums: " << high_thread_nums << endl
         << "thread id: " << env->GetThreadID() << endl
         << endl;

}

int main(int argc, char *argv[]) {
    Env *env = Env::Default();
    std::atomic<int> last_id(0);

    env->SetBackgroundThreads(3, Env::Priority::LOW); // 设置低优先级有3个后台线程
    env->SetBackgroundThreads(7, Env::Priority::HIGH);// 设置高优先级有7个后台线程

    for (int i = 0, j = 0; i < 10; j++, i++) {
        Cxt cxt_i(&last_id, i);
        Cxt cxt_j(&last_id, j);
        if (i % 2 == 0) env->Schedule(&Thread1, &cxt_i, Env::Priority::LOW, &cxt_i, &finish1);
        else env->Schedule(&Thread2, &cxt_j, Env::Priority::HIGH, &cxt_j, &finish1);
//        PrintEnvInfo(env);
    }

    Cxt cxt_us(&last_id, 1);
    env->UnSchedule(&cxt_us, Env::Priority::LOW);
    delete env;
    return 0;
}

