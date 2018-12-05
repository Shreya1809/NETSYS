INCLUDE_DIRS = -I .
LIB_DIRS = -L/usr/lib 
CC=g++

CDEFS=
CFLAGS= $(INCLUDE_DIRS) $(CDEFS) -O0 -std=c++11 #-g
CPPLIBS= -lpthread -lcrypto

CPPSRCS= library.cpp client.cpp

CPPOBJS= ${CPPSRCS:.cpp=.o}

all:	client

clean:
	-rm -f *.o *.d
	-rm -f client
	

distclean:
	-rm -f *.o *.d

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

client: $(CPPOBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(CPPLIBS)
