.PHONY: build run clean

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = nes.exe
SRC = main.cpp cartridge.cpp bus.cpp cpu.cpp

build: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
