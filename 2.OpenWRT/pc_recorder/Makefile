# Use GNU C++ compiler in the Ubuntu 16.04
GCC = g++

# Link the OpenCV and the pthread libraries
# LIBS = $(shell pkg-config --libs opencv) -lpthread
LIBS = $(shell pkg-config --libs opencv4)

# Include the necessary header files
INCLUDE = $(shell pkg-config --cflags opencv4)

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
