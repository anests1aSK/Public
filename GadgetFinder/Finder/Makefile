CC=gcc
CXX=g++
OBJ=capstone_gadget_finder

.PHONY: all clean

all: $(OBJ)

loader.o: ../inc/loader.cc
	$(CXX) -std=c++11 -c ../inc/loader.cc

capstone_gadget_finder: loader.o capstone_gadget_finder.cc
	$(CXX) -std=c++11 -o capstone_gadget_finder capstone_gadget_finder.cc loader.o -lbfd -lcapstone
	
clean:
	rm -f $(OBJ)
	rm -f *.o