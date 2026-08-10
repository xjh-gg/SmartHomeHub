CXX = g++
CXXFLAGS = -std=c++14 -Wall -Iinclude
TARGET = bin/smart_home
SRCS = src/main.cpp src/SmartDevice.cpp src/SmartHomeHub.cpp src/DeviceFactory.cpp
OBJS = build/main.o build/SmartDevice.o build/SmartHomeHub.o build/DeviceFactory.o

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) smart_home.cfg system.log

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
