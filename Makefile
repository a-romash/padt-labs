CC = g++
CFLAGS = -Wall -std=c++17
CFLAGS_TEST = -fsanitize=address,undefined -fno-omit-frame-pointer
GTEST_DIR = ../googletest/googletest
GTEST_FLAGS = -I$(GTEST_DIR)/include -I$(GTEST_DIR)
GTEST_SRC = $(GTEST_DIR)/src/gtest-all.cc
GTEST_TESTS = $(wildcard include/collections/tests/*.cpp)

program: main.cpp
	$(CC) $(CFLAGS) main.cpp -o program

tests: tests.cpp $(GTEST_TESTS)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(GTEST_FLAGS) tests.cpp $(GTEST_TESTS) $(GTEST_SRC) -pthread -o tests

clean:
	rm -f program tests
