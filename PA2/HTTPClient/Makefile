INCLUDE_DIRS = -I .
LIB_DIRS = -L/usr/lib 
CC=g++

CDEFS=
CFLAGS= $(INCLUDE_DIRS) $(CDEFS) -O0 #-g
CPPLIBS= -lpthread 

CPPSRCS= client.cpp 

CPPOBJS= ${CPPSRCS:.cpp=.o}

all: client

clean:
	-rm -f *.o *.d
	-rm -f client
	
distclean:
	-rm -f *.o *.d

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

client: $(CPPOBJS)
	$(CC) $(CFLAGS) $? -o $@ $(CPPLIBS)