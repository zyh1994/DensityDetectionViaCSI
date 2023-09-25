# Compiler and Flags
CC = g++
CFLAGS = `pkg-config --cflags opencv4` -std=c++17
LDFLAGS = `pkg-config --libs opencv4`
INCLUDES =

# Source files, object files and main files
SOURCES = $(wildcard csi/*.cpp) \
         $(wildcard cv/*.cpp) \
         $(wildcard bin/*.cpp) \
         $(wildcard local/*.cpp) \
         $(wildcard network/*.cpp) \
         $(wildcard security/*.cpp) \
         $(wildcard threads/*.cpp) \
		 $(wildcard csv/*.cpp) \
         
OBJECTS = $(SOURCES:.cpp=.o)
BIN2CSV_MAIN = bin2csv.cpp
CSI2BIN_MAIN = csi2bin.cpp

BIN2CSV_OBJECTS = $(BIN2CSV_MAIN:.cpp=.o)
CSI2BIN_OBJECTS = $(CSI2BIN_MAIN:.cpp=.o)

# Target executables
BIN2CSV_EXE = pc_bin2csv
CSI2BIN_EXE = pc_csi2bin

# Default target
all: $(CSI2BIN_EXE) $(BIN2CSV_EXE)

# Individual build targets
bin2csv: $(BIN2CSV_EXE)
csi2bin: $(CSI2BIN_EXE)

# Linking rules
$(BIN2CSV_EXE): $(BIN2CSV_OBJECTS) $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(CSI2BIN_EXE): $(CSI2BIN_OBJECTS) $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compilation rule
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJECTS) $(BIN2CSV_OBJECTS) $(CSI2BIN_OBJECTS) $(CSI2BIN_EXE) $(BIN2CSV_EXE)
