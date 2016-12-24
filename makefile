#Macros
SHELL = /bin/sh

OBJS = main.o reader.o march.o math.o perlin.o
#~ LIBS = openimageio

CXXFLAGS = -Wall -fopenmp -std=gnu++11 -O3
CXX = g++ -I${CURDIR}/libs/include -L${CURDIR}/libs/objs

#~ LIBS = -lOpenImageIO
LIBS = -lOpenImageIO

DATE = $(shell date)

#Main Rules

all: libs march

clean:
	-rm -f *.o march
	
#~ clean-deps:
#~ 	-rm -rf libs
	
#~ clean-all: clean clean-deps
	
install:
	@echo "Not Yet Implemented"
	
debug:
	

#Executables

march: ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} ${LIBS} -g -o $@ 
	@echo "Successfully compiled at" ${DATE}
	
#Dependencies - gets & compiles

#~ openimageio:
#~ 	mkdir libs
#~ 	git clone https://github.com/OpenImageIO/oiio.git libs/oiio
#~ 	cd libs/oiio
#~ 	git checkout Release-1.6.9
#~ 	make -j9
#~ 	cp build/linux64/
	

#.cpp.o:
%.o: %.cpp
	${CXX} ${CXXFLAGS} -c $<


