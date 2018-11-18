INCLUDE_DIRS = -I .
LIB_DIRS = -L/usr/lib 
CC=g++

CDEFS=
CFLAGS= $(INCLUDE_DIRS) $(CDEFS) -O0 -std=c++11 #-g
CPPLIBS= -lpthread 

CPPSRCS= library.cpp proxy.cpp

CPPOBJS= ${CPPSRCS:.cpp=.o}

all:	proxy

clean:
	-rm -f *.o *.d
	-rm -f proxy
	

distclean:
	-rm -f *.o *.d

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

proxy: $(CPPOBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(CPPLIBS)