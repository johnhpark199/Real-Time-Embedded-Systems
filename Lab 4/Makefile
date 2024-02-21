CXX = g++
CXXFLAGS = -std=c++17
OPENCV_LIBS = `pkg-config --cflags --libs opencv4` -pthread

SRC = main.cpp
EXECUTABLE = Lab4_build

all: $(EXECUTABLE)

$(EXECUTABLE): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OPENCV_LIBS)

.PHONY: clean

clean:
	rm -f $(EXECUTABLE)