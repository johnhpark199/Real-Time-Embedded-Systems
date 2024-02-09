CXX = g++
CXXFLAGS = -std=c++17
OPENCV_LIBS = `pkg-config --cflags --libs opencv4`

SRC = Lab2.cpp
EXECUTABLE = Lab2_build

all: $(EXECUTABLE)

$(EXECUTABLE): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OPENCV_LIBS)

.PHONY: clean

clean:
	rm -f $(EXECUTABLE)
