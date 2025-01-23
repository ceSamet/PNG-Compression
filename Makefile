# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -O2

# Project files
SOURCES = main.cpp ImageCompressor.cpp PNGImage.cpp PNGStructs.cpp
HEADERS = ImageCompressor.h PNGImage.h PNGStructs.h
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = image_compressor

# Main target
all: $(TARGET)

# Linking
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Compilation
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	del /Q *.o $(TARGET).exe

# Run
run:
	./$(TARGET)

.PHONY: all clean run 