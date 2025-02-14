CPPUTEST_HOME = ./cpputest
include $(CPPUTEST_HOME)/build/MakefileWorker.mk

# Project-specific settings
PROJECT_NAME = music_player
TEST_SRC_DIRS = .
SRC_DIRS = .

# CppUTest specific settings
CPPFLAGS += -I$(CPPUTEST_HOME)/include
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

# Your source files
SRC_FILES = AllTests.cpp test_playing.cpp

# This line is important for CppUTest
include $(CPPUTEST_HOME)/build/MakefileWorker.mk

# Custom targets
my-cleaner:
	rm -f $(TARGET) $(OBJS)

.PHONY: my-cleaner
