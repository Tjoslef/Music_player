
CPPUTEST_HOME = ./cpputest
CXX = g++
CXXFLAGS = -I$(CPPUTEST_HOME)/include
LDFLAGS = -L$(CPPUTEST_HOME)/src/CppUTest -L$(CPPUTEST_HOME)/src/CppUTestExt -lCppUTest -lCppUTestExt

SRCS = AllTests.cpp test_playing.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = test_runner

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

my-cleaner:
	rm -f $(OBJS) $(TARGET)

include $(CPPUTEST_HOME)/build/MakefileWorker.mk

