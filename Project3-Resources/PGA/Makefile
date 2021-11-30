VPATH = src:include

OBJS = rayTrace_pga.o
prom = rayTrace
DEP = $(shell find ./include -name "*.h")

CC = g++
CFLAGS = -std=c++14 -c
LFLAGS = -Wall

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(prom)

rayTrace_pga.o: rayTrace_pga.cpp $(DEP)
	$(CC) $(CFLAGS) ./src/rayTrace_pga.cpp

clean:
	rm -rf $(OBJS)
