# Define the compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wpedantic -std=c++17 -O3 -flto

TARGET = main
SRC = main.cpp
OBJ = main.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -c $(SRC)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
