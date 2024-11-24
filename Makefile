CXX = g++
CXXFlAGS = -g -fsanitize=address -std=c++20

OBJ = cooling.o main.o sat.o

all: cooling.o
	./cooling

%.o: %.cpp
	$(CXX) -c $< $(CXXFlAGS) -o $@