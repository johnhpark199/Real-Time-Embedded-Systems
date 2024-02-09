CXX = g++
CXXFLAGS = -std=c++17
OPENCV_LIBS = `pkg-config --cflags --libs opencv4`

SRC = main.cpp
EXECUTABLE = Lab3_build

all: $(EXECUTABLE)

$(EXECUTABLE): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OPENCV_LIBS)

.PHONY: clean

clean:
	rm -f $(EXECUTABLE)