vpath %.c src
vpath %.h include

OBJS = rayTrace_pga.o
prom = rayTrace

CC = g++
CFLAGS = -std=c++14 -MM -c
LFLAGS = -Wall

$(prom): $(OBJS) 
  $(CC) $(LFLAGS) $(OBJS) -o $(prom)


clean:
  rm -rf $(OBJS)

