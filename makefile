CXX = g++
CXXFLAGS = -std=c++17
SRC = ${wildcard ./blockchain/*.cpp ./helpers/*.cpp ./ui/*.cpp main.cpp}
TARGET = main

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)
