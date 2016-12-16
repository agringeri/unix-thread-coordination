LIB=-lpthread
CC=gcc

all: graph

graph: graph.c
	$(CC) graph.c -o graph $(LIB)

clean: 
	/bin/rm -f *.o core graph
