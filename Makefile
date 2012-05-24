SHELL=/bin/sh

OBJS=idxanalyzer.o main.o 
CXXFLAGS=-Wall 
CXX=g++
INCLUDES=
LIBS=-lm -lrt 
LDFLAGS=-D_FILE_OFFSET_BITS=64  
xmain:${OBJS}
	${CXX} ${CXXFLAGS} ${LDFLAGS} ${INCLUDES} -o $@ ${OBJS} ${LIBS}

clean:
	-rm -f *.o core *.core

.cpp.o:
	${CXX} ${CXXFLAGS} ${INCLUDES} -c  $<

.cc.o: 
	${CXX} ${CXXFLAGS} ${INCLUDES} -c $< 

