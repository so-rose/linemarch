#Macros
SHELL = /bin/sh

OBJS = main.o reader.o march.o math.o perlin.o

CXXFLAGS = -Wall -fopenmp -std=gnu++11 -O3
CXX = g++

LIBS = -lOpenImageIO

DATE = $(shell date)

#Main Rules

all: march

clean:
	-rm -f *.o march
	
install:
	@echo "Not Yet Implemented"
	
debug:
	

#Executables

march: ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} ${LIBS} -g -o $@ 
	@echo "Successfully compiled at" ${DATE}
	
#Dependencies

#.cpp.o:
%.o: %.cpp
	${CXX} ${CXXFLAGS} -c $<


