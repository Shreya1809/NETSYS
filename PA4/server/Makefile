INCLUDE_DIRS = -I .
LIB_DIRS = -L/usr/lib 
CC=g++

CDEFS=
CFLAGS= $(INCLUDE_DIRS) $(CDEFS) -O0 -std=c++11 #-g
CPPLIBS= -lpthread -lcrypto

CPPSRCS= server.cpp commandhandler.cpp

CPPOBJS= ${CPPSRCS:.cpp=.o}

all:	server

clean:
	-rm -f *.o *.d
	-rm -f server
	

distclean:
	-rm -f *.o *.d

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

server: $(CPPOBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(CPPLIBS)
