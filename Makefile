# step 1:
# prepare the compiler
# prepare targe file name
# prepare dependecy libarary
CC = g++
CFLAGS  := -Wall -O2 -fPIC -std=c++11
LDFLAGS = -lpthread

# header file's path
INCLUDE_PATH = -I ./include
SRC_PATH = ./src/

# .o file with the same name of .ccc file
OBJS = threadpool_imp.o env_posix.o env.o
SRCS = $(SRC_PATH)threadpool_imp.cc $(SRC_PATH)env_posix.cc $(SRC_PATH)env.cc
TARGET = libthreadpool.so
STATIC_TARGET = libthreadpool.a

# step 2:
# produce the .o files
$(OBJS):$(SRCS)
	$(CC) $(CFLAGS) $(INCLUDE_PATH) -c $^

# 3. produce the .so file
# 'ar crv' produce static lib
# 'cc - shared' produce shared lib
all : $(OBJS)
	ar crv $(STATIC_TARGET) $(OBJS)
	$(CC) -shared $(CFLAGS) $(INCLUDE_PATH) -o $(TARGET) $(OBJS) $(LDFLAGS)

# 4. clean the files except source file
clean:
	rm -f $(OBJS) $(LIB) $(TARGET) $(STATIC_TARGET)
