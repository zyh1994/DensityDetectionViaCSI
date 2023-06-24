# Use GNU C++ compiler in the Ubuntu 16.04
GCC = g++

# Link the OpenCV and the pthread libraries
ifeq ($(shell expr $(UBUNTU_VERSION) \>= 16.04), 1)
	LIBS = $(shell pkg-config --libs opencv4)
else
	LIBS = $(shell pkg-config --libs opencv)
endif

# Detect Ubuntu version
UBUNTU_VERSION := $(shell lsb_release -rs)

# Choose the appropriate pkg-config command based on Ubuntu version
ifeq ($(shell expr $(UBUNTU_VERSION) \>= 16.04), 1)
	INCLUDE = $(shell pkg-config --cflags opencv4)
else
	INCLUDE = $(shell pkg-config --cflags opencv)
endif

# List of source files
SOURCES = $(wildcard *.cpp)

# List of object files, include C++ and C files
OBJECTS = $(SOURCES:.cpp=.o)

# Name of the output executable
OUTPUT = pc_recorder

# Default target
all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(GCC) $(OBJECTS) $(LIBS) -o $@

.cpp.o:
	$(GCC) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(OUTPUT)