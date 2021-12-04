## Description
This is a ThreadPool with C++ language.

Support some features:
- Maintain a pool to automatic create,run,destroy a thread.
- Dynamic increase/descrease the thread pool's thread nums
- Support adjust the thread's CPU and I/O priority
- Expand your environment with inherit `Class Env`

There is a chinese blog to describe the implementation of this threadpool which is written by me.
[A industrial threadpool's implementation](https://blog.csdn.net/Z_Stand/article/details/114156801?spm=1001.2014.3001.5502)

## Compile
> compile 
> os: centos7.3.1611
> gcc: 5.3

- for lib and static lib: `make all`
- for example test: 
    ```c
    cd example
    make all
    ```
    > Before you compile the example, the libthreadpool.so must be produced
    > in the first step.  

## API
### Create ThreadPool
```c++
auto* thread_pool = new ThreadPoolImpl();
```
The interface will produce a PosixEnv to init multi threadpools with LOW/HIGH/USER .etc.

### Schedule thread
```c
void Schedule(void (*function)(void* arg1), void* arg, Priority pri = LOW,
            void* tag = nullptr,
            void (*unschedFunction)(void* arg) = nullptr) override;
```

- `function` is thread func
- `arg` is thread func's args, it's better to choose a struct
- `pri` Env support seperate threads with different tag 
    - `LOW`
    - `HIGH`
    
    With these tags, you can set a convinient cpu or I/O priority to these threads
    
### Priority thread
```c
void LowerThreadPoolIOPriority(Priority pool = LOW);
```

### Set threads' number limits
```c
void SetBackgroundThreads(int num, Priority pri);
```
You can set the threads' num with identity tag, eg: `env->SetBackgroundThreads(10, Env::Priority::LOW)` 
Means that the 'LOW' threadpool has less than 10 threads. 


### Example
For the detail ,you can see the `example/threadpool_test.cc`

```c
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
```
