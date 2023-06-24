# Use GNU C++ compiler in the Ubuntu 16.04
GCC = g++

# Link the OpenCV and the pthread libraries
LIBS = $(shell pkg-config --libs opencv) -lpthread

# Include the necessary header files
INCLUDE = $(shell pkg-config --cflags opencv) -I/usr/local/include/opencv -I/usr/local/include

# List of source files
SOURCES = Main_UDP.cpp VideoHelper.cpp

# List of object files
OBJECTS = $(SOURCES:.cpp=.o)

# Name of the output executable
OUTPUT = csv_recorder

# Default target
all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(GCC) $(OBJECTS) $(LIBS) -o $@

.cpp.o:
	$(GCC) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(OUTPUT)
