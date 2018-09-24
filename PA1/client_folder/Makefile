INCLUDE_DIRS = -I .
LIB_DIRS = 
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= 

CFILES= udp_client.c commands.c reliability_client.c

SRCS= ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	client 

clean:
	-rm -f *.o *.d
	-rm -f udp_client 
	
client: $(OBJS)
	$(CC) $(CFLAGS) $? -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c  $^ -o $@

