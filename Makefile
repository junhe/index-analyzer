SHELL=/bin/sh

OBJS=idxanalyzer.o main.o protobuf/index.pb.o
CXXFLAGS=-Wall 
CXX=g++
INCLUDES=-I/home/manio/programs/protobuf-2.4.1/include
LIBS=-lm -lrt -lprotobuf
LDFLAGS=-L/home/manio/programs/protobuf-2.4.1/lib 
xmain:${OBJS}
	${CXX} ${CXXFLAGS} ${LDFLAGS} ${INCLUDES} -o $@ ${OBJS} ${LIBS}

clean:
	-rm -f *.o core *.core

.cpp.o:
	${CXX} ${CXXFLAGS} ${INCLUDES} -c  $<

.cc.o: 
	${CXX} ${CXXFLAGS} ${INCLUDES} -c $< 

