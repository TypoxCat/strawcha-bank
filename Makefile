CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -I. -Iserver -Isrc
SRC_DIR  = src
LIB_SRCS = $(filter-out $(SRC_DIR)/main.cpp, $(wildcard $(SRC_DIR)/*.cpp))
LIB_OBJS = $(LIB_SRCS:.cpp=.o)

all: mbanking

mbanking: $(LIB_OBJS) $(SRC_DIR)/main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Server target
server/Api.o: server/Api.cpp server/Api.h
	$(CXX) $(CXXFLAGS) -c server/Api.cpp -o server/Api.o

server/main.o: server/main.cpp
	$(CXX) $(CXXFLAGS) -c server/main.cpp -o server/main.o

mbanking-server: $(LIB_OBJS) server/Api.o server/main.o
	$(CXX) $(CXXFLAGS) -pthread -o $@ $^

tests/run_tests: $(LIB_OBJS) tests/test_main.cpp
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -o $@ $^

test: tests/run_tests
	./tests/run_tests

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o server/*.o mbanking mbanking-server tests/run_tests

.PHONY: all test clean server