INCLUDE_DIRS = -I .
LIB_DIRS = 
CC=gcc

CDEFS= -D_FILE_OFFSET_BITS=64
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= 


CFILES= udp_server.c server_handler.c reliability_server.c

SRCS= ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	server 

clean:
	-rm -f *.o *.d
	-rm -f server 
	

server: $(OBJS)
	$(CC) $(CFLAGS) $? -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c  $^ -o $@
