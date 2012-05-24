SHELL=/bin/sh

OBJS=idxanalyzer.o main.o 
CXXFLAGS=-D_FILE_OFFSET_BITS=64 
CXX=g++
INCLUDES=
LIBS=-lm -lrt 
#LDFLAGS=
LDFLAGS=
xmain:${OBJS}
	${CXX} ${CXXFLAGS} ${LDFLAGS} ${INCLUDES} -o $@ ${OBJS} ${LIBS}

clean:
	-rm -f *.o core *.core

.cpp.o:
	${CXX} ${CXXFLAGS} ${INCLUDES} -c  $<

.cc.o: 
	${CXX} ${CXXFLAGS} ${INCLUDES} -c $< 

