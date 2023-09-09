# Define the compiler
CC = g++

# Use PKG_CONFIG to get the right compiler flags for OpenCV
CFLAGS = `pkg-config --cflags opencv4`

# Use PKG_CONFIG to get the right linker flags for OpenCV
LDFLAGS = `pkg-config --libs opencv4`

# Include the project header files
INCLUDES =

# Define the source files (excluding main files for now)
SOURCES = $(wildcard csi/*.cpp) $(wildcard cv/*.cpp) $(wildcard bin/*.cpp) $(wildcard local/*.cpp) $(wildcard network/*.cpp) $(wildcard security/*.cpp) $(wildcard threads/*.cpp)

# Define the debug main file
DEBUG_MAIN = DebugMain.cpp

# Define the executable main file
EXEC_MAIN = Main.cpp

# Define the object files
OBJECTS = $(SOURCES:.cpp=.o)

# Define the debug object file
DEBUG_OBJECTS = $(DEBUG_MAIN:.cpp=.o)

# Define the executable object file
EXEC_OBJECTS = $(EXEC_MAIN:.cpp=.o)

# Define the executable file
EXECUTABLE = pc_sampling

# Define the debug executable file
DEBUG_EXECUTABLE = pc_debug

# make all is the default action
all: $(EXECUTABLE) $(DEBUG_EXECUTABLE)

# make debug is compiling the debug version
debug: $(DEBUG_EXECUTABLE)

# make exec is compiling the executable version
exec: $(EXECUTABLE)

$(DEBUG_EXECUTABLE): $(DEBUG_OBJECTS) $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(EXECUTABLE): $(EXEC_OBJECTS) $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compile the source files
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean the project
clean:
	rm -f $(OBJECTS) $(DEBUG_OBJECTS) $(EXEC_OBJECTS) $(EXECUTABLE) $(DEBUG_EXECUTABLE)
