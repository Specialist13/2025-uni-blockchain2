CXX = g++
CXXFLAGS = -std=c++17 -fopenmp -O2
SRC = $(wildcard ./source/*.cpp ./source/block/*.cpp ./source/transaction/*.cpp \
       ./source/utxo/*.cpp ./source/mining/*.cpp ./helpers/*.cpp ./ui/*.cpp main.cpp)
TARGET = main

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
