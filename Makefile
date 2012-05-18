SHELL=/bin/sh

OBJS=idxanalyzer.o main.o index.pb.o
CXXFLAGS=-Wall 
CXX=g++
INCLUDES=
LIBS=-lm -lrt
xmain:${OBJS} idxanalyzer.cpp idxanalyzer.h main.cpp protobuf/index.pb.h protobuf/index.pb.cpp
	${CXX} ${CXXFLAGS} ${INCLUDES} -o $@ ${OBJS} ${LIBS}

clean:
	-rm -f *.o core *.core

.cpp.o:
	${CXX} ${CXXFLAGS} ${INCLUDES} -c  $<

