CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic
SOURCES := main.cpp Database.cpp MyArray.cpp ForwardList.cpp DoublyList.cpp MyStack.cpp MyQueue.cpp CompleteBinaryTree.cpp
HEADERS := $(wildcard *.hpp)
LEGACY_OBJECTS := $(SOURCES:.cpp=.o)
TARGET := dbms_cpp

.PHONY: all clean
all: ../dbms

../dbms: $(TARGET)
	cp $(TARGET) $@

$(TARGET): $(SOURCES) $(HEADERS) Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(SOURCES) $(LDLIBS) -o $@

clean:
	$(RM) $(LEGACY_OBJECTS) $(LEGACY_OBJECTS:.o=.d) $(TARGET) ../dbms
