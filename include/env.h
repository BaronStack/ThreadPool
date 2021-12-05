// An Env for wrap threadpool's API. The Env was the rocksdb's environment.
// After that you can define your own env class, just inherit the Env class
// and implementation the interface you want to use.

#pragma once

#include <cstdint>
#include <cstdarg>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <vector>

// rocksdb维护了一个Env类，这个类在同一个进程中的若干个rocksdb实例中是能够共享的。所以Rocksdb将这个类作为线程池的入口，
// 从而让Flush/Compaction这样的线程调度过程中，多个db可以只使用同一个线程池

// Env默认实例是PosixEnv，为了保证多个db实例之间共享一个环境变量，PosixEnv仅维护一个单例

class Env {
public:

    Env() = default;

    virtual ~Env();

    static const char *Type() { return "Environment"; }

    // Return a default environment suitable for the current operating
    // system.  Sophisticated users may wish to provide their own Env
    // implementation instead of relying on this default environment.
    //
    // The result of Default() belongs to rocksdb and must never be deleted.
    //
    // 返回适合当前操作系统的默认环境。老练的用户可能希望提供他们自己的 Env 实现，而不是依赖于这个默认环境。
    //
    // Default() 的结果属于rocksdb，绝对不能删除。
    static Env *Default();

    // Priority for scheduling job in thread pool
    enum Priority {
        BOTTOM, LOW, HIGH, USER, TOTAL
    };

    // Priority for requesting bytes in rate limiter scheduler
    enum IOPriority {
        IO_LOW = 0, IO_HIGH = 1, IO_TOTAL = 2
    };

    // Arrange to run "(*function)(arg)" once in a background thread, in
    // the thread pool specified by pri. By default, jobs go to the 'LOW'
    // priority thread pool.

    // "function" may run in an unspecified thread.  Multiple functions
    // added to the same Env may run concurrently in different threads.
    // I.e., the caller may not assume that background work items are
    // serialized.
    // When the UnSchedule function is called, the unschedFunction
    // registered at the time of Schedule is invoked with arg as a parameter.
    //
    // “函数”可以在未指定的线程中运行。添加到同一个 Env 的多个函数可能会在不同的线程中并发运行。
    // 即，调用者可能不会假设后台工作项是序列化的。
    // 调用UnSchedule函数时，以arg为参数调用Schedule时注册的unschedFunction。
    virtual void Schedule(void (*function)(void *arg), void *arg,
                          Priority pri = LOW, void *tag = nullptr,
                          void (*unschedFunction)(void *arg) = nullptr) = 0;

    // Arrange to remove jobs for given arg from the queue_ if they are not
    // already scheduled. Caller is expected to have exclusive lock on arg.
    //
    // 如果尚未安排给定 arg 的作业，则安排从 queue_ 中删除作业。调用者应该对 arg 有独占锁。
    virtual int UnSchedule(void * /*arg*/, Priority /*pri*/) { return 0; }

    // Start a new thread, invoking "function(arg)" within the new thread.
    // When "function(arg)" returns, the thread will be destroyed.
    virtual void StartThread(void (*function)(void *arg), void *arg) = 0;

    // Wait for all threads started by StartThread to terminate.
    virtual void WaitForJoin() {}

    // Get thread pool queue length for specific thread pool.
    virtual unsigned int GetThreadPoolQueueLen(Priority /*pri*/ = LOW) const {
        return 0;
    }

    // The number of background worker threads of a specific thread pool
    // for this environment. 'LOW' is the default pool.
    // default number: 1
    //
    // 此环境的特定线程池的后台工作线程数。 'LOW' 是默认池。默认数量：1
    virtual void SetBackgroundThreads(int number, Priority pri = LOW) = 0;

    virtual int GetBackgroundThreads(Priority pri = LOW) = 0;

    // Enlarge number of background worker threads of a specific thread pool
    // for this environment if it is smaller than specified. 'LOW' is the default
    // pool.
    virtual void IncBackgroundThreadsIfNeeded(int number, Priority pri) = 0;

    // Lower IO priority for threads from the specified pool.
    virtual void LowerThreadPoolIOPriority(Priority /*pool*/ = LOW) {}

    // Lower CPU priority for threads from the specified pool.
    virtual void LowerThreadPoolCPUPriority(Priority /*pool*/ = LOW) {}

    // Converts seconds-since-Jan-01-1970 to a printable string
    virtual std::string TimeToString(uint64_t time) = 0;

    virtual uint64_t NowMicros() = 0;

    virtual uint64_t NowNanos() = 0;

    virtual uint64_t NowCPUNanos() = 0;

    // Returns the ID of the current thread.
    virtual uint64_t GetThreadID() const = 0;

// This seems to clash with a macro on Windows, so #undef it here
#undef GetFreeSpace

private:
    // No copying allowed
    Env(const Env &);

    void operator=(const Env &);
};

// An implementation of Env that forwards all calls to another Env.
// May be useful to clients who wish to override just part of the
// functionality of another Env.
class EnvWrapper : public Env {
public:
    // Initialize an EnvWrapper that delegates all calls to *t
    explicit EnvWrapper(Env *t) : target_(t) {}

    ~EnvWrapper();

    // Return the target to which this Env forwards all calls
    Env *target() const { return target_; }

    void Schedule(void (*f)(void *arg), void *a, Priority pri,
                  void *tag = nullptr, void (*u)(void *arg) = nullptr) override {
        return target_->Schedule(f, a, pri, tag, u);
    }

    int UnSchedule(void *tag, Priority pri) override {
        return target_->UnSchedule(tag, pri);
    }

    void StartThread(void (*f)(void *), void *a) override {
        return target_->StartThread(f, a);
    }

    void WaitForJoin() override { return target_->WaitForJoin(); }

    unsigned int GetThreadPoolQueueLen(Priority pri = LOW) const override {
        return target_->GetThreadPoolQueueLen(pri);
    }

    void SetBackgroundThreads(int num, Priority pri) override {
        return target_->SetBackgroundThreads(num, pri);
    }

    int GetBackgroundThreads(Priority pri) override {
        return target_->GetBackgroundThreads(pri);
    }

    void IncBackgroundThreadsIfNeeded(int num, Priority pri) override {
        return target_->IncBackgroundThreadsIfNeeded(num, pri);
    }

    void LowerThreadPoolIOPriority(Priority pool = LOW) override {
        target_->LowerThreadPoolIOPriority(pool);
    }

    void LowerThreadPoolCPUPriority(Priority pool = LOW) override {
        target_->LowerThreadPoolCPUPriority(pool);
    }

    uint64_t NowMicros() override {
        return target_->NowMicros();
    }

    uint64_t NowNanos() override {
        return target_->NowNanos();
    }

    uint64_t NowCPUNanos() override {
        return target_->NowCPUNanos();
    }

    std::string TimeToString(uint64_t time) override {
        return target_->TimeToString(time);
    }

    uint64_t GetThreadID() const override { return target_->GetThreadID(); }

private:
    Env *target_;
};

