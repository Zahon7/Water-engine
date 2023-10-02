CXX=/bin/g++
override CXXFLAGS+=-lraylib -Iinclude -lGL

all: water run

water: source/water.cpp
	$(CXX) source/water.cpp $(CXXFLAGS) -o binary/water

run: binary/water
	binary/water
