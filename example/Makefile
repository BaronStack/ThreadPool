# step 1:
# prepare the compiler
# prepare targe file name
# prepare dependecy libarary
CC = g++
CFLAGS  := -Wall -O2 -fPIC -std=c++11
LDFLAGS = -lpthread

# ifneq ($(USE_RTTI), 1)
# 	CFLAGS += -fno-rtti
# endif

# header file's path
INCLUDE_PATH = -I ../include
LIBPATH = ../libthreadpool.so
STATIC_LIBPATH = ../libthreadpool.a

all : threadpool_test

threadpool_test: threadpool_test.cc
	$(CC) $(CFLAGS) $(INCLUDE_PATH) -lthreadpool $@.cc -o$@  $(LDFLAGS)

# 4. clean the files except source file
clean:
	rm -f ./threadpool_test
