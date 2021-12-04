cmake_minimum_required(VERSION 3.20)
project(threadPool)

set(CMAKE_CXX_STANDARD 11)
include_directories(include)

add_library(thread_pool STATIC
        src/env.cc
        src/env_posix.cc
        src/threadpool_imp.cc)

add_executable(threadPool example/env_thread_test.cc)
target_link_libraries(threadPool  thread_pool -lpthread)

add_executable(env_thread_test example/threadpool_test.cc)
target_link_libraries(env_thread_test thread_pool -lpthread)