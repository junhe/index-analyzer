SHELL=/bin/sh

OBJS=main.o 
CXXFLAGS=-D_FILE_OFFSET_BITS=64 
CXX=mpicxx
INCLUDES=
LIBS=-lm -lrt -L/home/jh/installs/plfs-dev/lib -lplfs 
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

